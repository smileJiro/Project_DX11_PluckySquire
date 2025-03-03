#include "stdafx.h"
#include "Key.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "Door_Blue.h"
#include "Player.h"

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

	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};

	AABBDesc.pOwner = this;
	AABBDesc.vExtents = _float2(1.f, 1.f);
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 0.f, 0.f };
	AABBDesc.isBlock = false;
	AABBDesc.isTrigger = true;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::GIMMICK_OBJECT;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &AABBDesc)))
		return E_FAIL;

	m_p2DColliderComs[1]->Set_Active(true);

	Register_OnAnimEndCallBack(bind(&CKey::On_AnimEnd, this, placeholders::_1, placeholders::_2));


	// TEMP
	Set_AnimationLoop(COORDINATE_2D, IDLE, true);
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
#ifdef _DEBUG
	for (auto& p2DCollider : m_p2DColliderComs)
	{
		p2DCollider->Render(CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(m_strSectionName));
	}
#endif
	return __super::Render();
}

void CKey::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();

	// 문이 열리네요.
	if (OBJECT_GROUP::DOOR == eGroup)
	{
		CDoor_Blue* pDoor = dynamic_cast<CDoor_Blue*>(_pOtherObject);

		if (nullptr == pDoor)			
			return;
		else
		{
			// 문의 위치로 설정,
			_vector vPosition = pDoor->Get_FinalPosition();
			pDoor->Open_Door();

			// 플레이어를 상태 = IDLE로 설정, 열쇠의 위치 설정,
			if (nullptr != m_pCarrier)
			{
				m_pCarrier->Set_CarryingObject(nullptr);
				m_pCarrier->Set_State(CPlayer::IDLE);

				Set_ParentMatrix(COORDINATE_2D, nullptr);
				Set_ParentMatrix(COORDINATE_3D, nullptr);
				Set_SocketMatrix(COORDINATE_3D, nullptr);
				Set_SocketMatrix(COORDINATE_2D, nullptr);
				Set_ParentBodyMatrix(COORDINATE_3D, nullptr);
				Set_ParentBodyMatrix(COORDINATE_2D, nullptr);

				Set_Position(vPosition + XMVectorSet(0.f, -10.f, 0.f, 0.f));
				Switch_Animation(OPEN);

				Set_Carrier(nullptr);
				for (auto& iter : m_p2DColliderComs)
				{
					Event_SetActive(iter, false);
				}
			}

		}
	}

}

void CKey::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (OPEN == iAnimIdx)
	{
		Event_DeleteObject(this);
	}
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
