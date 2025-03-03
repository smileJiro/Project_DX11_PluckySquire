#include "stdafx.h"
#include "Key.h"
#include "GameInstance.h"

CKey::CKey(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarriableObject(_pDevice, _pContext)
{
}
CKey::CKey(const CKey& _Prototype)
	: CCarriableObject(_Prototype)
{
}

HRESULT CKey::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	CARRIABLE_DESC* pDesc = static_cast<CARRIABLE_DESC*>(_pArg);

	pDesc->eCrriableObjId = CARRIABLE_OBJ_ID::KEY;
	pDesc->vHeadUpRoolPitchYaw3D = { 0.f, 0.f, 0.f };
	pDesc->vHeadUpOffset3D = { 0.f, 1.3f, 0.f };
	pDesc->isCoordChangeEnable = true;
	pDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
	pDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
	pDesc->strModelPrototypeTag_2D = TEXT("Key2D");
	pDesc->strModelPrototypeTag_3D = TEXT("Key_01");
	pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	//pDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.9f, 0.f);
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	pDesc->iRenderGroupID_3D = RG_3D;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;

	CActor::ACTOR_DESC ActorDesc = {};
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { 0.3f, 0.1f, 0.75f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	SHAPE_SPHERE_DESC ShapeDesc2 = {};
	ShapeDesc2.fRadius = 0.25f;
	SHAPE_DATA ShapeData2;
	ShapeData2.pShapeDesc = &ShapeDesc2;
	ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData2.isTrigger = false;
	ShapeData2.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER_TRIGGER;
	XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData2);


	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER;
	pDesc->pActorDesc = &ActorDesc;
	pDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	// 2D 충돌용 콜라이더 추가.
	m_p2DColliderComs.resize(2);

	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 0.2f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::MAP_GIMMICK;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
		return E_FAIL;

	Set_AnimationLoop(COORDINATE_2D, IDLE, true);

	// TEMP
	m_pControllerModel->Get_Model(COORDINATE_2D)->Set_Animation(IDLE);

	return S_OK;
}

void CKey::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CKey::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CKey::Render()
{
//#ifdef _DEBUG
//	m_p2DColliderComs[0]->Render();
//	if (COORDINATE_2D == Get_CurCoord() && 2 <= m_p2DColliderComs.size() && nullptr != m_p2DColliderComs[1])
//		m_p2DColliderComs[1]->Render();
//#endif
	return __super::Render();
}

CKey* CKey::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CKey* pInstance = new CKey(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKey::Clone(void* _pArg)
{
	CKey* pInstance = new CKey(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKey::Free()
{
	__super::Free();
}
