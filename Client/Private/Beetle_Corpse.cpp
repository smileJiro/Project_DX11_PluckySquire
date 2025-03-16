#include "stdafx.h"
#include "Beetle_Corpse.h"
#include "Section_Manager.h"
#include "GameInstance.h"

CBeetle_Corpse::CBeetle_Corpse(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarriableObject(_pDevice, _pContext)
{
}


CBeetle_Corpse::CBeetle_Corpse(const CBeetle_Corpse& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CBeetle_Corpse::Initialize(void* _pArg)
{
	CARRIABLE_DESC* Beetle_CorpseModelDesc = static_cast<CARRIABLE_DESC*>(_pArg);
	Beetle_CorpseModelDesc->eStartCoord = COORDINATE_3D;
	Beetle_CorpseModelDesc->eCrriableObjId = CARRIABLE_OBJ_ID::BEETLE_CORPSE;
	Beetle_CorpseModelDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,0.f };
	Beetle_CorpseModelDesc->vHeadUpOffset3D = { 0.f,1.2f,0.f };
	Beetle_CorpseModelDesc->isCoordChangeEnable = false;
	Beetle_CorpseModelDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
	Beetle_CorpseModelDesc->strModelPrototypeTag_3D = TEXT("beetle_01");
	Beetle_CorpseModelDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
	Beetle_CorpseModelDesc->iRenderGroupID_3D = RG_3D;
	Beetle_CorpseModelDesc->iPriorityID_3D = PR3D_GEOMETRY;
	//Beetle_CorpseModelDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 1.0f, -10.f);
	//Beetle_CorpseModelDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	
	CActor::ACTOR_DESC ActorDesc = {};
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { 0.5f,0.4f ,0.5f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc.vHalfExtents.y, 0.f));
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
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER;
	Beetle_CorpseModelDesc->pActorDesc = &ActorDesc;
	Beetle_CorpseModelDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(Beetle_CorpseModelDesc)))
		return E_FAIL;

	//// 2D 충돌용 콜라이더 추가.
	//m_p2DColliderComs.resize(2);
	////static_cast<CCollider_Circle*> (m_p2DColliderComs[1])->Set_Radius();
	//CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

	//CircleDesc.pOwner = this;
	//CircleDesc.fRadius = 20.f;
	//_float3 vScale = m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_Scale();
	//CircleDesc.vScale = { 1.f / vScale.x, 1.f / vScale.y };
	//CircleDesc.vOffsetPosition = { 0.f, 0.f };
	//CircleDesc.isBlock = false;
	//CircleDesc.isTrigger = true;
	//CircleDesc.iCollisionGroupID = OBJECT_GROUP::GIMMICK_OBJECT;

	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
	//	TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
	//	return E_FAIL;


	Set_Animation(7);
	Set_Progress(COORDINATE_3D, 7, 1.f);
	//m_pControllerModel->Play_Animation(0.f);
	Set_PlayingAnim(false);


	m_pActorCom->Set_Mass(5.f);

    return S_OK;
}

void CBeetle_Corpse::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CBeetle_Corpse::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CBeetle_Corpse::Render()
{
#ifdef _DEBUG
	if (COORDINATE_2D == Get_CurCoord() && 2 <= m_p2DColliderComs.size() && nullptr != m_p2DColliderComs[1])
		m_p2DColliderComs[1]->Render();
#endif
	return __super::Render();
}

void CBeetle_Corpse::Active_OnEnable()
{
	__super::Active_OnEnable();
}

void CBeetle_Corpse::Active_OnDisable()
{
	__super::Active_OnDisable();
}


CBeetle_Corpse* CBeetle_Corpse::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBeetle_Corpse* pInstance = new CBeetle_Corpse(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Beetle_Corpse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBeetle_Corpse::Clone(void* _pArg)
{
	CBeetle_Corpse* pInstance = new CBeetle_Corpse(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Beetle_Corpse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeetle_Corpse::Free()
{
	__super::Free();
}
