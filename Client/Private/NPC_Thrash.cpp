#include "stdafx.h"
#include "NPC_Thrash.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"



CNPC_Thrash::CNPC_Thrash(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CNPC_Companion(_pDevice, _pContext)
{
}

CNPC_Thrash::CNPC_Thrash(const CNPC_Thrash& _Prototype)
	:CNPC_Companion(_Prototype)
{
}



HRESULT CNPC_Thrash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Thrash::Initialize(void* _pArg)
{
	CNPC_Thrash::NPC_DESC* pDesc = static_cast<CNPC_Thrash::NPC_DESC*>(_pArg);
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = true;
	pDesc->iNumPartObjects = PART_END;

	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	m_f2DSpeed = pDesc->tTransform2DDesc.fSpeedPerSec = 190.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	m_f3DSpeed = pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	m_iMainIndex = pDesc->iMainIndex;
	m_iSubIndex = pDesc->iSubIndex;
	m_fDelayTime = 0.1f;

	m_eActionType = ACTION_WAIT;
	m_eMoving = MOVING_PRE;


	//if (FAILED(Ready_ActorDesc(pDesc)))
	//	return E_FAIL;

	wsprintf(m_strDialogueIndex, pDesc->strDialogueIndex);
	wsprintf(m_strCurSecion, TEXT("Chapter2_P0102"));

	m_strName = TEXT("Thrash");

	if (FAILED(__super::Child_Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter2_P0102"), this, SECTION_2D_PLAYMAP_OBJECT);

	CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
	pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, Thrash_C06_Idle01, true);
	pModelObject->Set_Animation(ANIM_2D::Thrash_C06_Idle01);

	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
	m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Thrash"), &tAnimEventDesc));
	Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CNPC_Thrash::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(226.3f, -2313.1f, 0.f, 1.f));
	
	return S_OK;
}

void CNPC_Thrash::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CNPC_Thrash::Child_Update(_float _fTimeDelta)
{
	if (m_isTrace)
		Trace(_fTimeDelta);
	else
		Welcome_Jot(_fTimeDelta);

	__super::Child_Update(_fTimeDelta);
}

void CNPC_Thrash::Child_LateUpdate(_float _fTimeDelta)
{
	__super::Child_LateUpdate(_fTimeDelta);

	if (TEXT("Chapter2_P0102") != CSection_Manager::GetInstance()->Get_Cur_Section_Key())
	{
		m_isRender = true;
		//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CSection_Manager::GetInstance()->Get_Cur_Section_Key(), this, SECTION_2D_PLAYMAP_OBJECT);
	}
}


HRESULT CNPC_Thrash::Render()
{

	return S_OK;
}

void CNPC_Thrash::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

}

void CNPC_Thrash::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	int a = 0;
}

void CNPC_Thrash::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	m_isColPlayer = false;
}



void CNPC_Thrash::Trace(_float _fTimeDelta)
{

	// 안에 있는 개수가 0개냐?
	// 너가 1번에 있냐?
	// 1번에 있으면 값을 가져와야한다.
	// 무슨 값을? 이 전 NPC의 값을

	_float2 vTargetObjectPos = _float2(
		m_pTargetObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0],
		m_pTargetObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1]);

	// 너 밖에없어? 그러면 PlayerPos로 기준점을 잡는다.
	//_float2 _vTargetPos = _float2(
	//	m_pTarget->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0],
	//	m_pTarget->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1]);

	_float2 _NPCPos = _float2(
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0],
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1]);

	Cal_PlayerDistance();

	if (COORDINATE_2D == m_pTarget->Get_CurCoord())
	{
		m_isMove = Trace_Player(vTargetObjectPos, _NPCPos);
	}

	// TODO :: 테스트 코드
	For_MoveAnimationChange(_fTimeDelta, _NPCPos);
	//if (m_isMove)
	//{
	//	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_run_right);
	//	Delay_On();
	//}
}

void CNPC_Thrash::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

}

void CNPC_Thrash::ChangeState_Panel(_float _fTimeDelta, _float2 _vNpcPos)
{
	
}

void CNPC_Thrash::For_MoveAnimationChange(_float _fTimeDelta, _float2 _vNpcPos)
{
	_vector CurPos = XMVectorSet(_vNpcPos.x, _vNpcPos.y, 0.f, 1.f);
	_vector PrePos = XMVectorSet(m_vPreNPCPos.x, m_vPreNPCPos.y, 0.f, 1.f);

	// 어? 너 지금 이전 거리가 달라졌네  그러면 이동한거야 / 애니메이션 변경시키자구
	if (0.f < XMVectorGetX(XMVector3Length(CurPos - PrePos)))
	{
		CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

		if (0.f < fabs(m_vPreNPCPos.x - _vNpcPos.x) && 0.f < fabs(m_vPreNPCPos.y - _vNpcPos.y))
		{
			if (5.f < fabs(m_vPreNPCPos.y - _vNpcPos.y))
			{
				//위로 올라간다.

				//(1.f > m_vPreNPCPos.y - _NPCPos.y)


				if (1.f > m_vPreNPCPos.y - _vNpcPos.y)
				{
					// 위로 간다.
					if (Thrash_run_up != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
						static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_run_up);
					m_eAnimationType = ANIM_UP;
					m_fIdleWaitTime = 0.f;
				}
				else if (1.f < m_vPreNPCPos.y - _vNpcPos.y)
				{
					// 아래로간다.
					if (Thrash_run_down != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
						static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_run_down);
					m_eAnimationType = ANIM_DOWN;
					m_fIdleWaitTime = 0.f;
				}


			}
			else if (0.f <= fabs(m_vPreNPCPos.x - _vNpcPos.x))
			{
				// 양옆으로 이동한다.

				if (1.f < m_vPreNPCPos.x - _vNpcPos.x)
				{
					// 좌측으로 이동한다.
					if (Thrash_run_right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
					{
						_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
						m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
						static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_run_right);
						m_eAnimationType = ANIM_LEFT;
					}


					m_fIdleWaitTime = 0.f;
				}
				else if (-1.f >= m_vPreNPCPos.x - _vNpcPos.x)
				{
					// 우측으로 이동한다.
					if (Thrash_run_right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
					{
						_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
						m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
						static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_run_right);
						m_eAnimationType = ANIM_RIGHT;
					}

					m_fIdleWaitTime = 0.f;
				}

			}
		}

		//y로만 이동한거야?
		m_vPreNPCPos = _vNpcPos;
	}
	else
	{
		CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

		if (ANIM_UP != m_eAnimationType && ANIM_DOWN != m_eAnimationType)
			m_fIdleWaitTime += _fTimeDelta;

		if (ANIM_IDLE != m_eAnimationType && 3.f <= m_fIdleWaitTime)
		{
			m_eAnimationType = ANIM_IDLE;
		}

		switch (m_eAnimationType)
		{
		case ANIM_UP:
		{
			if (Thrash_idle_up != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_idle_up);

		}
		break;

		case ANIM_DOWN:
		{
			if (Thrash_idle_down != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_idle_down);
		}
		break;

		case ANIM_LEFT:
		{
			if (Thrash_idle_right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
			{
				_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_idle_right);
			}

		}
		break;

		case ANIM_RIGHT:
		{
			if (Thrash_idle_right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
			{
				_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_idle_right);
			}
		}
		break;

		case ANIM_IDLE:
		{
			static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_idle_down);
		}
		break;
		}
	}
}

void CNPC_Thrash::Welcome_Jot(_float _fTimeDelta)
{
	// 쫓고 있는 중인가요?
	if (m_isTrace)
		return;

	m_isLookOut = true;

	// 안쫓고있어요 만나게하자.
	if (TEXT("Chapter2_P0102") == CSection_Manager::GetInstance()->Get_Cur_Section_Key())
	{
		if (ACTION_WAIT == m_eActionType)
		{
			if (Thrash_C06_Idle01 != static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
			{
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_C06_Idle01);
			}

			m_fWelcomeWaitTime += _fTimeDelta;

			if (5.f <= m_fWelcomeWaitTime)
			{
				m_eActionType = ACTION_MOVE;
			}
			else
				return;
		}
		else if (ACTION_MOVE == m_eActionType)
		{
			if (Thrash_idle_right != static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
			{
				_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_idle_right);
				m_eAnimationType = ANIM_LEFT;
			}

			_float4 vPlayerPos = _float4(Uimgr->Get_Player()->Get_FinalPosition().m128_f32[0], Uimgr->Get_Player()->Get_FinalPosition().m128_f32[1], 0.f, 1.f);
			_float4 vThrashPos = _float4(m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0], m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1], 0.f, 1.f);

			_float fCheckY = fabs(vPlayerPos.y - vThrashPos.y);

			// 플레이어와 쓰레시의 높이가 1 이하 이면 다이얼로그를 시작한다.
			if (15.f >= fCheckY)
			{
				m_eActionType = ACTION_DIALOG;

				if (Thrash_talk01_right != static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				{
					_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
					m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
					static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Thrash_talk01_right);
					m_eAnimationType = ANIM_LEFT;
				}

				Throw_Dialogue();
			}
		}
		else if (ACTION_DIALOG == m_eActionType)
		{
			if (MOVING_PRE == m_eMoving && false == Uimgr->Get_DisplayDialogue())
			{

				_float3 vNextPos = _float3(-783.f, 161.f, 0.f);
				Event_Book_Main_Section_Change_Start(1, &vNextPos);
				m_eMoving = MOVING_DIA;
				
				


				// 다이얼로그 한개 더띄운다.
				// 다이얼로그가 끝났으면 trace를 이제 true로해서 따라 다니게 한다.
				//m_eActionType = ACTION_TRACE;
				//
				//
				//m_isTrace = true;
			}
			else if (MOVING_DIA == m_eMoving)
			{
				m_eMoving = MOVING_CUR;
			}
			else if (MOVING_CUR == m_eMoving)
			{
				if (false == Uimgr->Get_DisplayDialogue())
				{
					m_eActionType = ACTION_TRACE;
					m_isTrace = true;
				}
			}
			
		}
	}
}



void CNPC_Thrash::Interact(CPlayer* _pUser)
{
	
}

_bool CNPC_Thrash::Is_Interactable(CPlayer* _pUser)
{
	return true;
}

_float CNPC_Thrash::Get_Distance(COORDINATE _eCOord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION) - _pUser->Get_FinalPosition()).m128_f32[0];
}






HRESULT CNPC_Thrash::Ready_ActorDesc(void* _pArg)
{
	CNPC_Thrash::NPC_DESC* pDesc = static_cast<CNPC_Thrash::NPC_DESC*>(_pArg);

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

HRESULT CNPC_Thrash::Ready_Components()
{
	m_p2DColliderComs.resize(1);

	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { 70.f, 70.f };
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 0.f, AABBDesc.vExtents.y * 0.5f };
	AABBDesc.isBlock = false;
	AABBDesc.isTrigger = false;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider2D"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;

	m_p2DNpcCollider = m_p2DColliderComs[0];
	Safe_AddRef(m_p2DNpcCollider);

	return S_OK;
}

HRESULT CNPC_Thrash::Ready_PartObjects()
{

	CModelObject::MODELOBJECT_DESC NPCBodyDesc{};

	NPCBodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	NPCBodyDesc.iCurLevelID = m_iCurLevelID;
	NPCBodyDesc.isCoordChangeEnable = false;
	NPCBodyDesc.strModelPrototypeTag_2D = TEXT("Thrash");
	//NPCBodyDesc.i2DModelPrototypeLevelID
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



CNPC_Thrash* CNPC_Thrash::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Thrash* pInstance = new CNPC_Thrash(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_Thrash::Clone(void* _pArg)
{
	CNPC_Thrash* pInstance = new CNPC_Thrash(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_Thrash Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_Thrash::Free()
{
	
	__super::Free();
}

HRESULT CNPC_Thrash::Cleanup_DeadReferences()
{
	return S_OK;
}
