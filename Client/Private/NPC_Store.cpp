#include "stdafx.h"
#include "NPC_Store.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Collision_Manager.h"
#include "UI_Manager.h"

CNPC_Store::CNPC_Store(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CNPC(_pDevice, _pContext)
{
}

CNPC_Store::CNPC_Store(const CNPC_Store& _Prototype)
	:CNPC(_Prototype)
{
}

HRESULT CNPC_Store::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Store::Initialize(void* _pArg)
{
	CNPC_Store::NPC_DESC* pDesc = static_cast<CNPC_Store::NPC_DESC*>(_pArg);
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = true;
	pDesc->iNumPartObjects = PART_END;

	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 300.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;
	m_iMainIndex = pDesc->iMainIndex;
	m_iMainIndex = pDesc->iMainIndex;
	

	//if (FAILED(Ready_ActorDesc(pDesc)))
	//	return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);


	CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

	pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, Martina_idle01, true);
	pModelObject->Set_Animation(ANIM_2D::Martina_idle01);
	pModelObject->Register_OnAnimEndCallBack(bind(&CNPC_Store::Animation_End, this, placeholders::_1, placeholders::_2));
	//Bind_AnimEventFunc("Idle", bind(&CNPC_Store::IDLE, this));
	/* Com_AnimEventGenerator */
	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
	m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_GAMEPLAY, TEXT("Prototype_Component_NPC_SHOP_2DAnimation"), &tAnimEventDesc));
	Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);


	
	//CActor::ACTOR_DESC ActorDesc;
	//
	///* Actor의 주인 오브젝트 포인터 */
	//ActorDesc.pOwner = this;
	//
	///* Actor의 회전축을 고정하는 파라미터 */
	//ActorDesc.FreezeRotation_XYZ[0] = false;
	//ActorDesc.FreezeRotation_XYZ[1] = false;
	//ActorDesc.FreezeRotation_XYZ[2] = false;
	//
	///* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
	//ActorDesc.FreezePosition_XYZ[0] = false;
	//ActorDesc.FreezePosition_XYZ[1] = false;
	//ActorDesc.FreezePosition_XYZ[2] = false;
	//
	///* 사용하려는 Shape의 형태를 정의 */
	//SHAPE_CAPSULE_DESC ShapeDesc = {};
	//ShapeDesc.fHalfHeight = 0.25f;
	//ShapeDesc.fRadius = 0.25f;
	//
	///* 해당 Shape의 Flag에 대한 Data 정의 */
	//SHAPE_DATA ShapeData;
	//ShapeData.pShapeDesc = &ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
	//ShapeData.eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
	//ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
	//ShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	//XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
	//
	///* 최종으로 결정 된 ShapeData를 PushBack */
	//ActorDesc.ShapeDatas.push_back(ShapeData);
	//
	///* 만약, Shape을 여러개 사용하고싶다면, 아래와 같이 별도의 Shape에 대한 정보를 추가하여 push_back() */
	//ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	//ShapeData.isTrigger = true;                     // Trigger로 사용하겠다.
	//XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0, 0.5, 0)); //여기임
	//SHAPE_SPHERE_DESC SphereDesc = {};
	//SphereDesc.fRadius = 1.f;
	//ShapeData.pShapeDesc = &SphereDesc;
	//ActorDesc.ShapeDatas.push_back(ShapeData);
	//
	///* 충돌 필터에 대한 세팅 ()*/
	//ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER;
	//ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::MONSTER | OBJECT_GROUP::INTERACTION_OBEJCT | OBJECT_GROUP::MONSTER_PROJECTILE;
	//
	//pDesc->pActorDesc = &ActorDesc;
	//
	//
	//__super::Initialize(pDesc);
	//Ready_PartObjects();
	


	return S_OK;
}

void CNPC_Store::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CNPC_Store::Update(_float _fTimeDelta)
{
	CCollision_Manager::GetInstance()->Add_Collider(m_strSectionName, OBJECT_GROUP::INTERACTION_OBEJCT, m_pColliderCom);
	
	__super::Update(_fTimeDelta);
}

void CNPC_Store::Late_Update(_float _fTimeDelta)
{

	if (true == OnCOllsion2D_Enter())
	{
		// 애니메이션 변경
	}
	

	__super::Late_Update(_fTimeDelta);

	//CGameObject* pPlayer;

	/* 테스트용 코드*/
	if (KEY_DOWN(KEY::J))
	{
		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Martina_MenuOpen);
	}


}

HRESULT CNPC_Store::Render()
{
	//__super::Render();
	m_pColliderCom->Render();
	return S_OK;
}

void CNPC_Store::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	int a = 0;
}

void CNPC_Store::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	int a = 0;
}

void CNPC_Store::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	int a = 0;
}

_bool CNPC_Store::OnCOllsion2D_Enter()
{
	// 이전에 false 였고 현재 true 인경우
	//m_isPreCollision2D 가 false
	//m_isCollision2D 가 true
	// 인터렉션창을 띄운다.
	

	m_isPreCollision2D = m_isCollision2D;

	return false;
}

_bool CNPC_Store::OnCOllsion2D_Stay()
{
	// 이전에 true 였고 지금도 true면 들어오는건 false, // 애니메이션 유지
		//m_isPreCollision2D 가 true
	//m_isCollision2D 가 true
	// 인터렉션창을 띄운다.
	// E를 누르면 아이템 상점을 띄운다.


	return false;
}

_bool CNPC_Store::OnCOllsion2D_Exit()
{

	// 이전에 true였고 현재 false 인경우
			//m_isPreCollision2D 가 true
	//m_isCollision2D 가 false
	// 인터렉션창을 끈다.


	m_isPreCollision2D = m_isCollision2D;

	return false;
}

void CNPC_Store::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
	CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

	switch ((CNPC_Store::ANIM_2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
	{

	case Martina_idle01:
		if (false == m_isDelay)
		{
			Set_AnimChangeable(true);
		}
		break;
	case Martina_MenuOpen:
		if (false == m_isDelay)
		{

			Set_AnimChangeable(true);
		}
		break;

	default:
		break;
	}
}

HRESULT CNPC_Store::Ready_ActorDesc(void* _pArg)
{
	CNPC_Store::NPC_DESC* pDesc = static_cast<CNPC_Store::NPC_DESC*>(_pArg);

	pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
	CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;

	/* Actor의 주인 오브젝트 포인터 */
	ActorDesc->pOwner = this;

	/* Actor의 회전축을 고정하는 파라미터 */
	ActorDesc->FreezeRotation_XYZ[0] = true;
	ActorDesc->FreezeRotation_XYZ[1] = false;
	ActorDesc->FreezeRotation_XYZ[2] = true;

	/* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
	ActorDesc->FreezePosition_XYZ[0] = false;
	ActorDesc->FreezePosition_XYZ[1] = false;
	ActorDesc->FreezePosition_XYZ[2] = false;


	/* 사용하려는 Shape의 형태를 정의 */
	SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
	ShapeDesc->fHalfHeight = 0.5f;
	ShapeDesc->fRadius = 0.5f;

	/* 해당 Shape의 Flag에 대한 Data 정의 */
	SHAPE_DATA* ShapeData = new SHAPE_DATA;
	ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
	ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
	ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
	ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
	XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

	/* 최종으로 결정 된 ShapeData를 PushBack */
	ActorDesc->ShapeDatas.push_back(*ShapeData);

	/* 충돌 필터에 대한 세팅 ()*/
	ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER;

	/* Actor Component Finished */
	pDesc->pActorDesc = ActorDesc;

	/* Shapedata 할당해제 */
	for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
	{
		Safe_Delete(ShapeData);
	}

	return S_OK;
}

HRESULT CNPC_Store::Ready_Components()
{
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { 65.f, 65.f };
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 0.f, AABBDesc.vExtents.y * 0.7f };
	AABBDesc.isBlock = true;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Test"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Store::Ready_PartObjects()
{

	CModelObject::MODELOBJECT_DESC NPCBodyDesc{};

	NPCBodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	NPCBodyDesc.iCurLevelID = m_iCurLevelID;
	NPCBodyDesc.isCoordChangeEnable = false;
	NPCBodyDesc.strModelPrototypeTag_2D = TEXT("NPC_Store");
	//NPCBodyDesc.strModelPrototypeTag_3D = TEXT("barfBug_Rig");
	NPCBodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	//NPCBodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

	NPCBodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	NPCBodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

	NPCBodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	NPCBodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	NPCBodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
	NPCBodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();

	//NPCBodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	//NPCBodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	//NPCBodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
	//NPCBodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

	m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_NPC_Body"), &NPCBodyDesc));
	if (nullptr == m_PartObjects[PART_BODY])
		return E_FAIL;

	return S_OK;

}



CNPC_Store* CNPC_Store::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Store* pInstance = new CNPC_Store(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_Store::Clone(void* _pArg)
{
	CNPC_Store* pInstance = new CNPC_Store(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_Store Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_Store::Free()
{
	__super::Free();
}

HRESULT CNPC_Store::Cleanup_DeadReferences()
{
	return S_OK;
}
