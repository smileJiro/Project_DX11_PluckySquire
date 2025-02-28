#include "stdafx.h"
#include "Bomb.h"
#include "Section_Manager.h"
#include "Collider_Circle.h"
#include "Character.h"
#include "Actor_Dynamic.h"
#include "Effect2D_Manager.h"

CBomb::CBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarriableObject(_pDevice, _pContext)
{
}


CBomb::CBomb(const CBomb& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CBomb::Initialize(void* _pArg)
{
	CARRIABLE_DESC* BombModelDesc = static_cast<CARRIABLE_DESC*>(_pArg);
	BombModelDesc->eCrriableObjId = CARRIABLE_OBJ_ID::BOMB;
	BombModelDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,0.f };
	BombModelDesc->vHeadUpOffset3D = { 0.f,1.3f,0.f };
	BombModelDesc->isCoordChangeEnable = true;
	BombModelDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
	BombModelDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
	BombModelDesc->strModelPrototypeTag_2D = TEXT("Bomb2D");
	BombModelDesc->strModelPrototypeTag_3D = TEXT("Bomb");
	BombModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	BombModelDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	//BombModelDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 1.0f, 0.f);
	BombModelDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	BombModelDesc->iRenderGroupID_3D = RG_3D;
	BombModelDesc->iPriorityID_3D = PR3D_GEOMETRY;
	//BombModelDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 1.0f, -10.f);
	BombModelDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	
	CActor::ACTOR_DESC ActorDesc = {};
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = true;
	ActorDesc.FreezeRotation_XYZ[1] = true;
	ActorDesc.FreezeRotation_XYZ[2] = true;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_SPHERE_DESC ShapeDesc = {};
	ShapeDesc.fRadius = 0.5f;
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc.fRadius, 0.f));
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
	XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc2.fRadius, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData2);

	//폭발 Shape - 폭발시 켜기 위해 ShapeUse 트리거로 지정
	SHAPE_SPHERE_DESC ExplosionShapeDesc = {};
	ExplosionShapeDesc.fRadius = 3.f;
	SHAPE_DATA ExplosionShapeData;
	ExplosionShapeData.pShapeDesc = &ExplosionShapeDesc;
	ExplosionShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	ExplosionShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ExplosionShapeData.isTrigger = true;
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
	ExplosionShapeData.FilterData.MyGroup = OBJECT_GROUP::EXPLOSION;
	ExplosionShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER;
	XMStoreFloat4x4(&ExplosionShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ExplosionShapeData);

	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER;
	BombModelDesc->pActorDesc = &ActorDesc;
	BombModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;

	if (FAILED(__super::Initialize(BombModelDesc)))
		return E_FAIL;

	m_p2DColliderComs.resize(3);
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 30.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = true;
	CircleDesc.isTrigger = false;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
	CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_2DBombCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
		return E_FAIL;


	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 80.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::EXPLOSION;
	CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_TRIGGER;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_2DExplisionCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[2]), &CircleDesc)))
		return E_FAIL;


	Get_ActorCom()->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_TRIGER, false);
	m_p2DColliderComs[2]->Set_Active(false);

	Set_AnimationLoop(COORDINATE_2D, 0, true);

	m_fLifeTime = 5.f;
	m_fExplodeTime = 1.f;

    return S_OK;
}

void CBomb::Priority_Update(_float _fTimeDelta)
{
	if (true == m_isOn)
	{
		m_fAccTime += _fTimeDelta;

		if (false == m_isExplode)
		{
			if (m_fLifeTime <= m_fAccTime)
			{
				Explode();
			}
		}
	}

	else if (true == m_isExplode)
	{
		m_fAccTime += _fTimeDelta;

		if (m_fExplodeTime <= m_fAccTime)
		{
			if (false == m_isDead)
			{
				Event_DeleteObject(this);
			}
		}
	}

	__super::Priority_Update(_fTimeDelta);
}

void CBomb::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CBomb::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CBomb::Render()
{

#ifdef _DEBUG
	if (COORDINATE_2D == Get_CurCoord())
	{
		for (_uint i = 0; i < m_p2DColliderComs.size(); i++)
		{
			if (true == m_p2DColliderComs[i]->Is_Active())
			{
				m_p2DColliderComs[i]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(Get_Include_Section_Name()));
			}
		}
	}
#endif

	__super::Render();

	return S_OK;
}

void CBomb::Set_Time_On()
{
	m_isOn = true;
}

void CBomb::Set_Time_Off()
{
	m_isOn = false;
}

void CBomb::Bomb_Shape_Enable(_bool _isEnable)
{
	Get_ActorCom()->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, _isEnable);
}

void CBomb::Explode()
{
	Set_Time_Off();

	Set_Render(false);

	//기존 콜라이더를 끄고 폭발용 콜라이더 켬

	if (COORDINATE_2D == Get_CurCoord())
	{
		m_p2DColliderComs[0]->Set_Active(false);
		m_p2DColliderComs[(_uint)COLLIDER2D_USE::COLLIDER2D_BODY+1]->Set_Active(false);
		m_p2DColliderComs[(_uint)COLLIDER2D_USE::COLLIDER2D_TRIGGER+1]->Set_Active(true);

		CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Generic_Explosion"), Get_Include_Section_Name(), Get_ControllerTransform()->Get_WorldMatrix());
	}
	else if (COORDINATE_3D == Get_CurCoord())
	{
		static_cast<CActor_Dynamic*>(Get_ActorCom())->Set_Kinematic();
		Get_ActorCom()->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, false);
		Get_ActorCom()->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_TRIGER, true);
	}

	m_fAccTime = 0.f;
	m_isExplode = true;
}

HRESULT CBomb::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;

	if (COORDINATE_2D == _eCoordinate)
	{
		Switch_Animation(0);
	}

	return S_OK;
}

void CBomb::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CBomb::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CBomb::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CBomb::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	if ((OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup || OBJECT_GROUP::MONSTER & _Other.pActorUserData->iObjectGroup)
		&& (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
	{
		//_vector vRepulse = 10.f * XMVector3Normalize(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition());
		_vector vDir = _Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition();
		_float fDistance = XMVectorGetX(XMVector3Length(vDir));
		_vector vRepulse = 30.f / fDistance * XMVector3Normalize(vDir);
		Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), 1, vRepulse);
	}
}

void CBomb::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CBomb::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CBomb::OnContact_Modify(const COLL_INFO& _0, const COLL_INFO& _1, CModifiableContacts& _ModifiableContacts, _bool _bIm0)
{
}

void CBomb::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if ((_uint)COLLIDER2D_USE::COLLIDER2D_TRIGGER== _pMyCollider->Get_ColliderUse())
	{
		if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID() || OBJECT_GROUP::MONSTER & _pOtherCollider->Get_CollisionGroupID())
		{
			Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVector3Normalize(_pOtherObject->Get_FinalPosition() - Get_FinalPosition()), 500.f);
		}
	}
}

void CBomb::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CBomb::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CBomb::Active_OnEnable()
{
	__super::Active_OnEnable();
	Set_Render(true);
	Set_Time_On();
	//CActor_Dynamic* pDynamic = static_cast<CActor_Dynamic*>(Get_ActorCom());
	//pDynamic->Update(0.f);
	//pDynamic->Set_Dynamic();

	m_p2DColliderComs[0]->Set_Active(true);
	m_p2DColliderComs[2]->Set_Active(false);
}

void CBomb::Active_OnDisable()
{
	m_fAccTime = 0.f;
	m_isExplode = false;

	if (COORDINATE_2D == Get_CurCoord())
	{
		m_p2DColliderComs[(_uint)COLLIDER2D_USE::COLLIDER2D_BODY+1]->Set_Active(true);
	}
	else if (COORDINATE_3D == Get_CurCoord())
	{
		Get_ActorCom()->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, true);
		Get_ActorCom()->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_TRIGER, false);
	}

	__super::Active_OnDisable();
}


CBomb* CBomb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBomb* pInstance = new CBomb(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Bomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBomb::Clone(void* _pArg)
{
	CBomb* pInstance = new CBomb(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Bomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBomb::Free()
{
	__super::Free();
}
