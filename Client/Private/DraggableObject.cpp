#include "stdafx.h"
#include "DraggableObject.h"
#include "Player.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Actor_Dynamic.h"

CDraggableObject::CDraggableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CDraggableObject::CDraggableObject(const CDraggableObject& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CDraggableObject::Initialize(void* _pArg)
{
	DRAGGABLE_DESC* pDesc = static_cast<DRAGGABLE_DESC*>(_pArg);

	pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->iRenderGroupID_3D = RG_3D;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	m_eInteractKey = KEY::E;
	m_eInteractType = INTERACT_TYPE::HOLDING;
	
	

	if (COORDINATE_3D == pDesc->eStartCoord)
	{
		CActor::ACTOR_DESC ActorDesc = {};
		ActorDesc.pOwner = this;
		ActorDesc.FreezeRotation_XYZ[0] = false;
		ActorDesc.FreezeRotation_XYZ[1] = false;
		ActorDesc.FreezeRotation_XYZ[2] = false;
		ActorDesc.FreezePosition_XYZ[0] = false;
		ActorDesc.FreezePosition_XYZ[1] = false;
		ActorDesc.FreezePosition_XYZ[2] = false;
		ActorDesc.isAddActorToScene = true;

		SHAPE_BOX_DESC ShapeDesc = {};
		ShapeDesc.vHalfExtents = pDesc->vBoxHalfExtents;
		SHAPE_DATA ShapeData;
		ShapeData.pShapeDesc = &ShapeDesc;
		ShapeData.eShapeType = SHAPE_TYPE::BOX;
		ShapeData.eMaterial = ACTOR_MATERIAL::STICKY;
		ShapeData.isTrigger = false;
		ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
		ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
		ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
		XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(pDesc->vBoxOffset.x, pDesc->vBoxOffset.y, pDesc->vBoxOffset.z));
		ActorDesc.ShapeDatas.push_back(ShapeData);

		SHAPE_SPHERE_DESC ShapeDesc2 = {};
		ShapeDesc2.fRadius = 0.25f;
		SHAPE_DATA ShapeData2;
		ShapeData2.pShapeDesc = &ShapeDesc2;
		ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
		ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
		ShapeData2.isTrigger = false;
		ShapeData2.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
		ShapeData2.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
		ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER_TRIGGER;

		XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, pDesc->vBoxHalfExtents.y, 0.f));
		ActorDesc.ShapeDatas.push_back(ShapeData2);

		ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
		ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
		pDesc->pActorDesc = &ActorDesc;
		pDesc->eActorType = ACTOR_TYPE::DYNAMIC;

		if (FAILED(__super::Initialize(pDesc)))
			return E_FAIL;

		m_pActorCom->Set_MassLocalPos(pDesc->vBoxOffset);
		m_pActorCom->Set_Mass(15.5f);
		return S_OK;
	}
	else
	{
		m_p2DColliderComs.resize(2);

		CCollider_AABB::COLLIDER_AABB_DESC tBoxDesc = {};
		tBoxDesc.pOwner = this;
		tBoxDesc.vExtents = { pDesc->vBoxHalfExtents.x *2.f, pDesc->vBoxHalfExtents.y*2.f};
		tBoxDesc.vScale = { 1.0f, 1.0f };
		tBoxDesc.vOffsetPosition = { pDesc->vBoxOffset.x,pDesc->vBoxOffset.y };
		tBoxDesc.isBlock = true;
		tBoxDesc.isTrigger = false;
		tBoxDesc.iCollisionGroupID = OBJECT_GROUP::BLOCKER;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AAABB"),
			TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &tBoxDesc)))
			return E_FAIL;

		CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
		CircleDesc.pOwner = this;
		CircleDesc.fRadius = 0.33f;
		CircleDesc.vScale = { 1.0f, 1.0f };
		CircleDesc.vOffsetPosition = { 0.f, 0.f };
		CircleDesc.isBlock = false;
		CircleDesc.isTrigger = true;
		CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
			TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
			return E_FAIL;


		if (FAILED(__super::Initialize(pDesc)))
			return E_FAIL;
	}





	return S_OK;
}

void CDraggableObject::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CDraggableObject::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

void CDraggableObject::OnContact_Modify(const COLL_INFO& _My, const COLL_INFO& _Other, CModifiableContacts& _ModifiableContacts)
{
	SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
	switch (eShapeUse)
	{
	case Client::SHAPE_USE::SHAPE_BODY:
	{
		if (m_pDragger 
			&& OBJECT_GROUP::MAPOBJECT == _Other.pActorUserData->iObjectGroup
			|| OBJECT_GROUP::DYNAMIC_OBJECT == _Other.pActorUserData->iObjectGroup
			|| OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup)
		{
			_uint iContactCount = _ModifiableContacts.Get_ContactCount();
			for (_uint i = 0; i < iContactCount; i++)
			{

				_ModifiableContacts.Set_StaticFriction(i, 0);
				_ModifiableContacts.Set_DynamicFriction(i, 0);
			}
		}

		break;
	}
	}
}

void CDraggableObject::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup)
	{
		m_bUserContact = true;
	}
}

void CDraggableObject::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup)
	{
		int a = 0;
	}
}

void CDraggableObject::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup)
	{
		m_bUserContact = false;
	}
}

void CDraggableObject::Interact(CPlayer* _pUser)
{

}

_bool CDraggableObject::Is_Interactable(CPlayer* _pUser)
{
	if (_pUser->Is_CarryingObject())
		return false;
	//최초 붙잡기는 접촉해 있어야 함.
	//최초 붙잡기인지 판단 -> m_pDragger가 없는지?
	//접촉했는지 판단 -> m_bUserCOntact가 true?
	//이미 붙잡혀있었으면 m_bUserCOntact는 소용 없음. 
	//-> m_pDragger가 있으면 무적권 가능. 
	//플레이어가 이제 관심없어지면 m_pDragger를 nullptr로 만들어줘야 함.

	if (m_pDragger)
		return true;

	return m_bUserContact;
}

_float CDraggableObject::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

}

void CDraggableObject::On_InteractionStart(CPlayer* _pPlayer)
{
	Set_Dragger(_pPlayer);
	_pPlayer->Set_State(CPlayer::DRAG);
}

void CDraggableObject::On_InteractionEnd(CPlayer* _pPlayer)
{
	Set_Dragger(nullptr);
}

void CDraggableObject::Move(_fvector _vForce, _float _fTimeDelta)
{
	ACTOR_TYPE eActorType = Get_ActorType();

	if (COORDINATE_3D == Get_CurCoord())
	{
		CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
		_vector vVeclocity = _vForce /** m_tStat[COORDINATE_3D].fMoveSpeed*/  /** fDot*/;

		vVeclocity = XMVectorSetY(vVeclocity, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));

		if (pDynamicActor->Is_Dynamic())
		{
			pDynamicActor->Set_LinearVelocity(vVeclocity);
		}
		else
		{
			m_pControllerTransform->Go_Direction(_vForce, _fTimeDelta);
		}
	}
	else
	{
		m_pControllerTransform->Go_Direction(_vForce, XMVectorGetX(XMVector3Length(_vForce)), _fTimeDelta);
	}
}


CDraggableObject* CDraggableObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDraggableObject* pInstance = new CDraggableObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DraggableObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDraggableObject::Clone(void* _pArg)
{
	CDraggableObject* pInstance = new CDraggableObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DraggableObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDraggableObject::Free()
{
	__super::Free();	
}
