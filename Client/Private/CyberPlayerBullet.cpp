#include "stdafx.h"
#include "CyberPlayerBullet.h"
#include "Character.h"
#include "3DModel.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Effect_Manager.h"

CCyberPlayerBullet::CCyberPlayerBullet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CCyberPlayerBullet::CCyberPlayerBullet(const CCyberPlayerBullet& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CCyberPlayerBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCyberPlayerBullet::Initialize(void* _pArg)
{
	CYBERPLAYER_PROJECTILE_DESC* pDesc = static_cast<CYBERPLAYER_PROJECTILE_DESC*>(_pArg);
	pDesc->iNumPartObjects = 1;
	pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.vInitialScaling = _float3(1, 1, 1);
	pDesc->tTransform3DDesc.fSpeedPerSec = m_fSpeed;

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    CActor::ACTOR_DESC ActorDesc;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc.pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc.FreezeRotation_XYZ[0] = true;
    ActorDesc.FreezeRotation_XYZ[1] = true;
    ActorDesc.FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc.FreezePosition_XYZ[0] = false;
    ActorDesc.FreezePosition_XYZ[1] = false;
    ActorDesc.FreezePosition_XYZ[2] = false;

    /* 사용하려는 Shape의 형태를 정의 */
    ActorDesc.ShapeDatas.resize(1);

	SHAPE_SPHERE_DESC tBodyShapeDesc = {};
     tBodyShapeDesc.fRadius = 0.25f;
	SHAPE_DATA tBodyShapeData = {};
    tBodyShapeData.pShapeDesc = &tBodyShapeDesc;       
    tBodyShapeData.eShapeType = SHAPE_TYPE::SPHERE;   
    tBodyShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;  
    tBodyShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    tBodyShapeData.isTrigger = false;                  
    tBodyShapeData.FilterData.MyGroup = OBJECT_GROUP::PLAYER_PROJECTILE;
	tBodyShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE;
	XMStoreFloat4x4(&tBodyShapeData.LocalOffsetMatrix, XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.f, 0.0f));

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas[tBodyShapeData.iShapeUse] = tBodyShapeData;

    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER_PROJECTILE;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE ;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	CModelObject::MODELOBJECT_DESC tModelDesc{};
	tModelDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
	tModelDesc.strModelPrototypeTag_3D = TEXT("CyberPlayerBullet");
	tModelDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	tModelDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	tModelDesc.iRenderGroupID_3D = RG_3D;
	tModelDesc.iPriorityID_3D = PR3D_GEOMETRY;
	tModelDesc.eStartCoord = COORDINATE_3D;
	tModelDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
	m_PartObjects[PART_BODY] = m_pModel= static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	if (nullptr == m_PartObjects[PART_BODY])
	{
		MSG_BOX("CPlayer VISOR Creation Failed");
		return E_FAIL;
	}
	Safe_AddRef(m_pModel);
	m_PartObjects[PART_BODY]->Set_Position({ 0.f,0.f,0.f });
	m_PartObjects[PART_BODY]->Get_ControllerTransform()->Rotation(XMConvertToRadians(90.f), _vector{ 0.f,1.f,0.f,0.f });
	Set_PartActive(PART_BODY, true);

	m_vLookDIr = XMVector3Normalize( m_pControllerTransform->Get_WorldMatrix().r[2]);
	static_cast<C3DModel*>(m_pModel->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_UseAlbedoMap(0, false, true);
	static_cast<C3DModel*>(m_pModel->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(0.f, 1.f, 1.f, 1.f), true);
	return S_OK;
}

void CCyberPlayerBullet::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CCyberPlayerBullet::Update(_float _fTimeDelta)
{
	m_fLifeTimeAcc += _fTimeDelta;
	if (m_fLifeTimeAcc >= m_fLifeTime)
	{
		Event_DeleteObject(this);
		m_fLifeTimeAcc = 0.f;
	}
	m_pControllerTransform->Go_Straight(_fTimeDelta);
	__super::Update(_fTimeDelta);
}

void CCyberPlayerBullet::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	//여긴 MONSTER 아니면 MONSTER_PROJECTILE만 충돌됨.
	Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), m_iDamg, _vector{ 0.f,0.f,0.f });
	Event_DeleteObject(this);

	CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("CyberBulletHit"), true, Get_FinalPosition());
}



CCyberPlayerBullet* CCyberPlayerBullet::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCyberPlayerBullet* pInstance = new CCyberPlayerBullet(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create CyberPlayerBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCyberPlayerBullet::Clone(void* _pArg)
{
	CGameObject* pInstance = new CCyberPlayerBullet(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Clone CyberPlayerBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCyberPlayerBullet::Free()
{
	Safe_Release(m_pModel);
	__super::Free();
}
