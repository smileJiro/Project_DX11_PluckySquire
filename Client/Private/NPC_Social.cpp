#include "stdafx.h"
#include "NPC_Social.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"
#include "Npc_OnlySocial.h"
#include "Dialog_Manager.h"



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




	m_is2D = pDesc->is2D;
	m_strAnimaionName = pDesc->strAnimationName;
	//m_iCreateSection = pDesc->strCreateSection;
	m_strDialogueID = pDesc->strDialogueId;


	m_fNPCCollsionHalfHeight = pDesc->fCollisionHalfHeight;
	m_fNPCCollisionRadius = pDesc->fCollisionRadius;
	m_fNPCTriggerRadius = pDesc->fTriggerRadius;


	
	if (false == m_is2D)
	{
		m_iCreateSection = pDesc->strCreateSection = 1;
	}
	else
	{
		m_iCreateSection = pDesc->strCreateSection;
	}

	m_iStartAnimation = pDesc->iStartAnimation;
	m_isHaveDialog = pDesc->isDialog;
	m_vPosition = _float3(pDesc->vPositionX, pDesc->vPositionY, pDesc->vPositionZ);
	m_vCollsionScale = _float2(pDesc->CollsionScaleX, pDesc->CollsionScaleY);
	m_isInteractable = pDesc->isInteractable;
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;


	true == pDesc->is2D ? pDesc->eStartCoord = COORDINATE_2D : pDesc->eStartCoord = COORDINATE_3D;

	//if (true == pDesc->is2D)
	//	pDesc->eStartCoord = COORDINATE_2D;
	
	pDesc->isCoordChangeEnable = true;
	pDesc->iNumPartObjects = PART_END;

	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 300.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;
	m_iMainIndex = pDesc->iMainIndex;
	m_iSubIndex = pDesc->iSubIndex;

	m_iPreState = 3;

	wsprintf(m_strCurSecion, pDesc->strSectionid.c_str());
	
	if (!pDesc->is2D)
	{
		if (FAILED(Ready_ActorDesc(pDesc)))
			return E_FAIL;
	}

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (m_is2D)
	{
		if (FAILED(Ready_Components()))
			return E_FAIL;
	}


	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	
	if (true == pDesc->is2D)
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pDesc->strSectionid, this, SECTION_2D_PLAYMAP_OBJECT);
		wsprintf(m_strCurSecion, pDesc->strSectionid.c_str());

		CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

		pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, m_iStartAnimation, true);
		pModelObject->Set_Animation(m_iStartAnimation);

		CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
		tAnimEventDesc.pReceiver = this;
		tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
		m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, m_strAnimaionName, &tAnimEventDesc));
		Safe_AddRef(m_pAnimEventGenerator);
		Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CNPC_Social::On_AnimEnd, this, placeholders::_1, placeholders::_2));
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(m_vPosition.x, m_vPosition.y, m_vPosition.z, 1.f));

		m_eInteractID = INTERACT_ID::NPC;
	}

	if (!m_is2D)
	{
		for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
		{
			Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
		}
		Safe_Delete(pDesc->pActorDesc);


		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(pDesc->vPositionX, pDesc->vPositionY, pDesc->vPositionZ, 1.f));
		m_pControllerTransform->Set_Scale(COORDINATE_3D, _float3(1.5f, 1.5f, 1.5f));
		m_pControllerTransform->Rotation(XMConvertToRadians(pDesc->fRotateAngle), XMVectorSet(0.f, 1.f, 0.f, 0.f));


		CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
		pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, m_iStartAnimation, true);
		pModelObject->Set_Animation(m_iStartAnimation);
		pModelObject->Set_PlayingAnim(false);
	}

	return S_OK;
}

void CNPC_Social::Priority_Update(_float _fTimeDelta)
{
     	__super::Priority_Update(_fTimeDelta);
}

void CNPC_Social::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);


	if (m_iPreSocialNpcAnimationindex != m_iSocialNpcAnimationIndex)
	{
		if (-1 == m_iSocialNpcAnimationIndex)
			m_iSocialNpcAnimationIndex = 0;

		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(m_iSocialNpcAnimationIndex);
		m_iPreSocialNpcAnimationindex = m_iSocialNpcAnimationIndex;
	}

	if (true == m_isPlayDisplay && false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
	{
		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(m_iStartAnimation);

		m_iSocialNpcAnimationIndex = m_iStartAnimation;
		m_isPlayDisplay = false;
		
	}


	

}

void CNPC_Social::Late_Update(_float _fTimeDelta)
{
	if (false == m_isThrow && true == m_isColPlayer && true == m_isHaveDialog && true == m_is2D)
	{
		Throw_Dialogue();
		m_isThrow = true;
		m_isPlayDisplay = true;
	}
	
	if (false == m_isColPlayer && true == m_isThrow && true == m_is2D)
	{
		m_isThrow = false;
	}

	if (!m_is2D)
	{
		m_pGameInstance->Add_RenderObject_New(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_GEOMETRY, this);

		CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

		if (true == pModelObject->Is_PlayingAnim() && false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
			pModelObject->Set_PlayingAnim(false);
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
	//if (true == m_isThrow && false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
	//{
	//	m_isThrow = false;
	//}
}

void CNPC_Social::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	m_isColPlayer = false;
}


void CNPC_Social::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (m_strDialogueID == TEXT("C01_Humgrump_01"))
	{
		if (iAnimIdx != m_iPreState)
		{
			switch (iAnimIdx)
			{
			case 0:
			{
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(20);
			}
			break;

			case 20:
			{

				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(21);
			}
			break;
			}

		}
	}
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
	ShapeDesc->fHalfHeight = m_fNPCCollsionHalfHeight;
	ShapeDesc->fRadius = m_fNPCCollisionRadius;

	/* 해당 Shape의 Flag에 대한 Data 정의 */
	SHAPE_DATA* ShapeData = new SHAPE_DATA;
	ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
	ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
	ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
	ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

	/* 최종으로 결정 된 ShapeData를 PushBack */
	ActorDesc->ShapeDatas.push_back(*ShapeData);

	SHAPE_CAPSULE_DESC* ShapeDesc2 = new SHAPE_CAPSULE_DESC;
	ShapeDesc2->fHalfHeight = m_fNPCCollsionHalfHeight;
	ShapeDesc2->fRadius = m_fNPCTriggerRadius;
	ShapeData->pShapeDesc = ShapeDesc2;              // 위에서 정의한 ShapeDesc의 주소를 저장.
	ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
	ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
	ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
	XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

	/* 최종으로 결정 된 ShapeData를 PushBack */
	ActorDesc->ShapeDatas.push_back(*ShapeData);


	/* 충돌 필터에 대한 세팅 ()*/
	ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
	ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::EXPLOSION | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER;

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

	if (true == m_isInteractable)
		AABBDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	else
		AABBDesc.iCollisionGroupID = OBJECT_GROUP::NONE;

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
	NPCBodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	
	if (false == m_is2D)
	{
		NPCBodyDesc.strModelPrototypeTag_3D = m_strAnimaionName;
		NPCBodyDesc.iModelPrototypeLevelID_3D = 1;
	}
	
	NPCBodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	NPCBodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

	NPCBodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	NPCBodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	NPCBodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
	NPCBodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();

	
	if (false == m_is2D)
	{
		NPCBodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
		NPCBodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
		NPCBodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
		NPCBodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();
	}


	m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_NPC_Body"), &NPCBodyDesc));
	if (nullptr == m_PartObjects[PART_BODY])
		return E_FAIL;

	return S_OK;

}

void CNPC_Social::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	int a = 0;
}

void CNPC_Social::Interact(CPlayer* _pUser)
{
	if(m_is2D)
	m_isColPlayer = true;

	else if (!m_is2D)
	{
		CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

		CDialog_Manager::GetInstance()->Set_DialogId(m_strDialogueIndex);
		pModelObject->Set_PlayingAnim(true);
		//pModelObject->Set_AnimationLoop(COORDINATE_3D, 0, true);
	}
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
