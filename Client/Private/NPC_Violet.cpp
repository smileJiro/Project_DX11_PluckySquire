#include "stdafx.h"
#include "NPC_Violet.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"



CNPC_Violet::CNPC_Violet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CNPC_Companion(_pDevice, _pContext)
{
}

CNPC_Violet::CNPC_Violet(const CNPC_Violet& _Prototype)
	:CNPC_Companion(_Prototype)
{
}



HRESULT CNPC_Violet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Violet::Initialize(void* _pArg)
{
	CNPC_Violet::NPC_DESC* pDesc = static_cast<CNPC_Violet::NPC_DESC*>(_pArg);
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = true;
	pDesc->iNumPartObjects = PART_END;

	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	m_f2DSpeed = pDesc->tTransform2DDesc.fSpeedPerSec = 200.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	m_f3DSpeed = pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	m_iMainIndex = pDesc->iMainIndex;
	m_iSubIndex = pDesc->iSubIndex;
	m_fDelayTime = 0.1f;

	m_eActionType = ACTION_WAIT;
	m_eRenderType = VIOLET_RENDER;

	//m_strCurSecion = pDesc->cursection
	
	 

	wsprintf(m_strDialogueIndex, pDesc->strDialogueIndex);
	wsprintf(m_strCurSecion, TEXT("Chapter1_P0708"));

	m_strName = TEXT("Violet");
	 


	//if (FAILED(Ready_ActorDesc(pDesc)))
	//	return E_FAIL;

	if (FAILED(__super::Child_Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter1_P0708"), this, SECTION_2D_PLAYMAP_OBJECT);

	CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
	pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, Violet_idle_down, true);

	pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, Violet_Run_Down, true);
	pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, Violet_Run_Right, true);
	pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, Violet_Run_Up, true);
	//pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, Violet_idle_down, true);
	pModelObject->Set_Animation(ANIM_2D::Violet_idle_down);

	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
	m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Violet"), &tAnimEventDesc));
	Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CNPC_Violet::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(990.f, -36.5f, 0.f, 1.f));
	
	return S_OK;
}

void CNPC_Violet::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CNPC_Violet::Child_Update(_float _fTimeDelta)
{
	m_isLookOut = Uimgr->Get_VioletMeet();

	if (true == m_isLookOut)
	{
		if (m_isTrace)
		{
			Trace(_fTimeDelta);
		}
		else
		{
			if (true == Uimgr->Get_VioletMeet())
				Welcome_Jot(_fTimeDelta);
		}

		Rock_Dialog(_fTimeDelta);
		__super::Child_Update(_fTimeDelta);
	}
	
}

void CNPC_Violet::Child_LateUpdate(_float _fTimeDelta)
{
	if (m_isLookOut == true)
	{
		__super::Child_LateUpdate(_fTimeDelta);

		if (VIOLET_RENDER == m_eRenderType && TEXT("Chapter2_P0102") == CSection_Manager::GetInstance()->Get_Cur_Section_Key())
		{
			m_eRenderType = VIOLET_NOTRENDER;
			
			m_isRender = false;

			CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);

		}
		else if (VIOLET_NOTRENDER == m_eRenderType && TEXT("Chapter2_P0102") != CSection_Manager::GetInstance()->Get_Cur_Section_Key())
		{
			m_eRenderType = VIOLET_RENDER;
			
			m_isRender = true;
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CSection_Manager::GetInstance()->Get_Cur_Section_Key(), this, SECTION_2D_PLAYMAP_OBJECT);
		}
	}
		


		
}


HRESULT CNPC_Violet::Render()
{

	return S_OK;
}

void CNPC_Violet::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	
}

void CNPC_Violet::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	int a = 0;
}

void CNPC_Violet::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	m_isColPlayer = false;
}



void CNPC_Violet::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (iAnimIdx != m_iPreState)
	{
		switch (ANIM_2D(iAnimIdx))
		{
		case ANIM_2D::Violet_idle_down:
		{
			static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_idle_down);
		}

		break;
		}
	}
}

void CNPC_Violet::ChangeState_Panel()
{

}

void CNPC_Violet::For_MoveAnimationChange(_float _fTimeDelta, _float2 _vNpcPos)
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
					if ( Violet_Run_Up != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
					static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_Run_Up);
					m_eAnimationType = ANIM_UP;
					m_fIdleWaitTime = 0.f;
				}
				else if (1.f < m_vPreNPCPos.y - _vNpcPos.y)
				{
					// 아래로간다.
					if (Violet_Run_Down != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
						static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_Run_Down);
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
					if (Violet_Run_Right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
					{
						_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
						m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
						static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_Run_Right);
						m_eAnimationType = ANIM_LEFT;
					}


					m_fIdleWaitTime = 0.f;
				}
				else if (-1.f >= m_vPreNPCPos.x - _vNpcPos.x)
				{
					// 우측으로 이동한다.
					if (Violet_Run_Right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
					{
						_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
						m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
						static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_Run_Right);
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
			if (Violet_idle_up != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_idle_up);

		}
		break;

		case ANIM_DOWN:
		{
			if (Violet_idle_down != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_idle_down);
		}
		break;

		case ANIM_LEFT:
		{
			if (Violet_idle_right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
			{
				_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_idle_right);
			}
			
		}
		break;

		case ANIM_RIGHT:
		{
			if (Violet_idle_right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
			{
				_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_idle_right);
			}
		}
		break;

		case ANIM_IDLE:
		{
			if (Violet_idle_down != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_idle_down);
		}
		break;
		}
	}
}

void CNPC_Violet::Welcome_Jot(_float _fTimeDelta)
{
	if (true == m_isTrace)
		return;

	m_isLookOut = true;
	// 트레이스 상태가 아니다.

	// 플레이어의 섹션하고 나와의 섹션이 동일하면 시작하자.
	//if (CSection_Manager::GetInstance()->SetActive_Section(TEXT("Chapter1_P0708")))
	if (TEXT("Chapter1_P0708") == CSection_Manager::GetInstance()->Get_Cur_Section_Key())
	{
		// 캐릭터 입장 후 잠시 대기!
		if (m_eActionType == ACTION_WAIT)
		{
			if (Violet_idle_down != static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_idle_down);

			m_fWelcomeWaitTime += _fTimeDelta;

			if (5.f < m_fWelcomeWaitTime)
			{
				m_eActionType = ACTION_MOVE;
			}
			else
				return;
		}

		// 캐릭터로 향해 뚜벅뚜벅 걸어온다.
		//m_eActionType = ACTION_MOVE;

		if (ACTION_MOVE == m_eActionType)
		{
			if (Violet_Run_Right != static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
			{
				_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_Run_Right);
				m_eAnimationType = ANIM_LEFT;
			}


			_float4 vPlayerPos = _float4(Uimgr->Get_Player()->Get_FinalPosition().m128_f32[0], Uimgr->Get_Player()->Get_FinalPosition().m128_f32[1], 0.f, 1.f);
			_float4 vVioletPos = _float4(m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0], m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1], 0.f, 1.f);
			_float4 vCalPos = _float4(vVioletPos.x - vPlayerPos.x, vVioletPos.y - vPlayerPos.y, 0.f, 1.f);
			_float	fDistance = XMVectorGetX(XMVector3Length(XMVectorSet(vCalPos.x, vCalPos.y, 0.f, 1.f)));

			if (200.f <= fDistance)
			{
				// 걸어간다.
				vVioletPos.x -= m_f2DSpeed * _fTimeDelta;
				m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_State(CTransform::STATE_POSITION, _float4(vVioletPos.x, vVioletPos.y, 0.f, 1.f));
			}
			else
			{

				// 일정 거리까지 오면 다이얼로그 시작한다.
				m_eActionType = ACTION_DIALOG;

				if (Violet_Talk01_Right != static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				{
					_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
					m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
					static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_Talk01_Right);
					m_eAnimationType = ANIM_LEFT;
				}

				Throw_Dialogue();

				// 멈추고 다이얼로그를 시작한다.
			}
		}
		else if (ACTION_DIALOG == m_eActionType)
		{

			// 다이얼로그가 끝났나?
			// 다이얼로그가 끝났는지 계속 체크한다.
			if (false == Uimgr->Get_DisplayDialogue())
			{
				// 다이얼로그가 끝났으면 trace를 이제 true로해서 따라 다니게 한다.
				m_eActionType = ACTION_TRACE;
				m_isTrace = true;
			}
		}
	}
}

void CNPC_Violet::Rock_Dialog(_float _fTimeDelta)
{
	if (TEXT("Chapter2_P0304") == CSection_Manager::GetInstance()->Get_Cur_Section_Key() && 
		2.f > fabs(m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0] + 400) &&
		m_isDialogRock == false)
	{
		wsprintf(m_strDialogueIndex, TEXT("Violet_Rock_01"));
		Throw_Dialogue();
		m_isDialogRock = true;
	}

	if (m_isDialogRock == true)
	{
		if (true == Uimgr->Get_DisplayDialogue())
		{
			CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

			if (Violet_Talk01_Right != pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
				static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Violet_Talk01_Right);
		}
	}
		


}

void CNPC_Violet::Interact(CPlayer* _pUser)
{
	
}

_bool CNPC_Violet::Is_Interactable(CPlayer* _pUser)
{
	return true;
}

_float CNPC_Violet::Get_Distance(COORDINATE _eCOord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION) - _pUser->Get_FinalPosition()).m128_f32[0];
}

void CNPC_Violet::Trace(_float _fTimeDelta)
{
	

	_float2 vTargetObjectPos = _float2(
		m_pTargetObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0],
		m_pTargetObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1]);

	_float2 _NPCPos = _float2(
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0],
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1]);

	Cal_PlayerDistance();

	if (COORDINATE_2D == m_pTarget->Get_CurCoord())
	{
		m_isMove = Trace_Player(vTargetObjectPos, _NPCPos);
	}

	// TODO :: 테스트 코드
	
	//if (true == m_isMove && )

	//_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
	//m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
	

	///////////// 애니메이션 변경 관련 ////////////////
	For_MoveAnimationChange(_fTimeDelta, _NPCPos);
	

	
	

}

HRESULT CNPC_Violet::Ready_ActorDesc(void* _pArg)
{
	CNPC_Violet::NPC_DESC* pDesc = static_cast<CNPC_Violet::NPC_DESC*>(_pArg);

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

HRESULT CNPC_Violet::Ready_Components()
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

HRESULT CNPC_Violet::Ready_PartObjects()
{

	CModelObject::MODELOBJECT_DESC NPCBodyDesc{};

	NPCBodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	NPCBodyDesc.iCurLevelID = m_iCurLevelID;
	NPCBodyDesc.isCoordChangeEnable = false;
	NPCBodyDesc.strModelPrototypeTag_2D = TEXT("Violet");
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



CNPC_Violet* CNPC_Violet::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Violet* pInstance = new CNPC_Violet(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_Violet::Clone(void* _pArg)
{
	CNPC_Violet* pInstance = new CNPC_Violet(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_Violet Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_Violet::Free()
{
	
	__super::Free();
}

HRESULT CNPC_Violet::Cleanup_DeadReferences()
{
	return S_OK;
}

