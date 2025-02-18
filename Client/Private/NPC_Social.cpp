#include "stdafx.h"
#include "NPC_Social.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"
#include "Npc_OnlySocial.h"



CNPC_Social::CNPC_Social(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CNPC(_pDevice, _pContext)
{
}

CNPC_Social::CNPC_Social(const CNPC_Social& _Prototype)
	:CNPC(_Prototype)
{
}



HRESULT CNPC_Social::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Social::Initialize(void* _pArg)
{
	CNPC_OnlySocial::NPC_ONLYSOCIAL* pDesc = static_cast<CNPC_OnlySocial::NPC_ONLYSOCIAL*>(_pArg);





	m_strAnimaionName = pDesc->strAnimationName;
	m_iCreateSection = pDesc->strCreateSection;
	m_strDialogueID = pDesc->strDialogueId;
	m_iStartAnimation = pDesc->iStartAnimation;
	m_vPosition = _float3(pDesc->vPositionX, pDesc->vPositionY, pDesc->vPositionZ);
	m_vCollsionScale = _float2(pDesc->CollsionScaleX, pDesc->CollsionScaleY);
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = true;
	pDesc->iNumPartObjects = PART_END;

	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 300.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;
	m_iMainIndex = pDesc->iMainIndex;
	m_iSubIndex = pDesc->iSubIndex;

	wsprintf(m_strCurSecion, pDesc->strSectionid.c_str());
	

	//if (FAILED(Ready_ActorDesc(pDesc)))
	//	return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pDesc->strSectionid, this, SECTION_2D_PLAYMAP_OBJECT);

	wsprintf(m_strCurSecion, pDesc->strSectionid.c_str());

	//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);


	CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

	pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, m_iStartAnimation, true);
	pModelObject->Set_Animation(m_iStartAnimation);

	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
	m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, m_strAnimaionName, &tAnimEventDesc));
	Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CNPC_Social::On_AnimEnd, this , placeholders::_1, placeholders::_2));
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(m_vPosition.x, m_vPosition.y, m_vPosition.z, 1.f));

	
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

void CNPC_Social::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CNPC_Social::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
	
	
}

void CNPC_Social::Late_Update(_float _fTimeDelta)
{
	if (false == m_isThrow && true == m_isColPlayer)
	{
		Throw_Dialogue();
		m_isThrow = true;
	}
	
	if (false == m_isColPlayer && true == m_isThrow)
	{
		m_isThrow = false;
	}


	__super::Late_Update(_fTimeDelta);

	
}

HRESULT CNPC_Social::Render()
{
#ifdef _DEBUG
	//if (COORDINATE_2D == Get_CurCoord())
	//	m_p2DNpcCollider->Render();
#endif // _DEBUG

	return S_OK;
}

void CNPC_Social::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CNPC_Social::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CNPC_Social::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	m_isColPlayer = false;
}


void CNPC_Social::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	
}


HRESULT CNPC_Social::Ready_ActorDesc(void* _pArg)
{
	CNPC_Social::NPC_DESC* pDesc = static_cast<CNPC_Social::NPC_DESC*>(_pArg);

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
	Safe_Delete(ShapeData);

	return S_OK;
}

HRESULT CNPC_Social::Ready_Components()
{
	m_p2DColliderComs.resize(1);
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { m_vCollsionScale.x, m_vCollsionScale.y };
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 0.f, AABBDesc.vExtents.y * 0.5f };
	AABBDesc.isBlock = true;
	AABBDesc.isTrigger = false;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;
	
	m_p2DNpcCollider = m_p2DColliderComs[0];
	Safe_AddRef(m_p2DNpcCollider);


	//CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	//AABBDesc.pOwner = this;
	//AABBDesc.vExtents = { m_vCollsionScale.x, m_vCollsionScale.y };
	//AABBDesc.vScale = { 1.0f, 1.0f };
	//AABBDesc.vOffsetPosition = { 0.f, AABBDesc.vExtents.y * 0.5f };
	//AABBDesc.isBlock = false;
	//AABBDesc.isTrigger = false;
	////AABBDesc.vOffsetPosition = { 0.f, 0.f };
	//if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_SocialNPC2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Social::Ready_PartObjects()
{

	CModelObject::MODELOBJECT_DESC NPCBodyDesc{};

	NPCBodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	NPCBodyDesc.iCurLevelID = m_iCurLevelID;
	NPCBodyDesc.isCoordChangeEnable = false;
	NPCBodyDesc.strModelPrototypeTag_2D = m_strAnimaionName;
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

void CNPC_Social::Interact(CPlayer* _pUser)
{
	m_isColPlayer = true;
}

_bool CNPC_Social::Is_Interactable(CPlayer* _pUser)
{
	return true;
}

_float CNPC_Social::Get_Distance(COORDINATE _eCOord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCOord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCOord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}


CNPC_Social* CNPC_Social::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Social* pInstance = new CNPC_Social(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_Social::Clone(void* _pArg)
{
	CNPC_Social* pInstance = new CNPC_Social(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_Social Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_Social::Free()
{

	__super::Free();
}

HRESULT CNPC_Social::Cleanup_DeadReferences()
{
	return S_OK;
}
