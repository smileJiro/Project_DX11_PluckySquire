#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_AttackState.h"
#include "Monster.h"
#include "FSM.h"

#include "Camera_Manager.h"
#include "PlayerData_Manager.h"
#include "PlayerBody.h"

#include "Beetle.h"
#include "Formation_Manager.h"
#include "Layer.h"

CSneak_AttackState::CSneak_AttackState()
{
}

HRESULT CSneak_AttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CSneak_AttackState::State_Enter()
{
	_uint iPlayerAnim = static_cast<CModelObject*>(CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Get_Body())->Get_CurrentAnimIndex();
	if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_KNOCKED_DOWN_AND_EATEN_FROM_BEHIND_LATCH == iPlayerAnim
		|| (_uint)CPlayer::ANIM_STATE_3D::LATCH_KNOCKED_DOWN_AND_EATEN_FROM_BEHIND_LOOP_LATCH == iPlayerAnim)
	{
		Beetle_CutScene();
	}
	m_pOwner->Set_AnimChangeable(false);
}

void CSneak_AttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	//cout << "Attack" << endl;
	//컷씬으로 들어가며 초기화
	//Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);
	
	//if(m_pOwner->Get_AnimChangeable())
	//{
	//	if(false == m_pOwner->Is_FormationMode())
	//		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
	//	else
	//	{
	//		m_pOwner->Add_To_Formation();
	//		Event_ChangeMonsterState(MONSTER_STATE::FORMATION_BACK, m_pFSM);
	//	}
	//}

	// FadeInOut 시간 계산
	FadeInOut(_fTimeDelta);

	Check_Animation_End();
}

void CSneak_AttackState::State_Exit()
{

}

void CSneak_AttackState::After_Attack()
{
	_float3 vPlayerPos = { };
	_float3 vMonsterPos = { };

	if (true == m_pOwner->Is_FormationMode())
	{
		vPlayerPos = { 49.f, 24.01f, -1.f };

		CFormation_Manager::GetInstance()->Reset_Formation();

		static_cast<CActorObject*>(m_pTarget)->Get_ActorCom()->Set_GlobalPose(vPlayerPos);
		m_pTarget->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z, 1.f));
	}

	else
	{
		switch (m_eWayIndex)
		{
		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_1:
		{
			vPlayerPos = { -31.f, 6.56f, 22.5f };
			vMonsterPos = { -16.5f, 6.56f, 22.6f };

			Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(m_pOwner), &vPlayerPos, &vMonsterPos);

			m_pOwner->Set_AnimChangeable(true);

			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}

		break;

		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2:
		{
			vPlayerPos = { 40.f, 0.35f, -7.f };

			Reset_Chapter2_2(vPlayerPos);
		}

		break;
		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2_2:
		{
			vPlayerPos = { 36.6f, 0.35f, 5.5f };
			Reset_Chapter2_2(vPlayerPos);
		}
		break;
		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_3:
		{
			vPlayerPos = { 43.f, 0.35f, -0.5f };
			Reset_Chapter2_2(vPlayerPos);
		}
		break;

		//레이어에 있는 모든 몬스터 전부 돌리기
		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_1:
		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_2:
		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_3:
		case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_4:
		{
			vPlayerPos = { 24.f, 13.1f, 24.4f };

			auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevel, TEXT("Layer_Bridge_Beetle"));

			if (nullptr != pLayer)
			{
				const auto& Objects = pLayer->Get_GameObjects();

				for_each(Objects.begin(), Objects.end(), [&](CGameObject* pGameObject) {
					auto pObject = dynamic_cast<CMonster*>(pGameObject);

					if (nullptr != pObject)
					{
						switch (pObject->Get_WayIndex())
						{
						case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_1:
							vMonsterPos = _float3(26.5f, 8.58f, 25.f);
							break;

						case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_2:
							vMonsterPos = _float3(29.5f, 8.6f, 31.5f);
							break;

						case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_3:
							vMonsterPos = _float3(35.5f, 8.58f, 31.5f);
							break;

						case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_4:
							vMonsterPos = _float3(31.8f, 8.6f, 24.4f);
							break;
						}

						Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(pObject), &vPlayerPos, &vMonsterPos);
						pObject->Set_AnimChangeable(true);

						Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, pObject->Get_FSM());
					}
					});
			}
		}

		break;

		case Client::SNEAKWAYPOINTINDEX::CHAPTER8_1:
		{
			vPlayerPos = { 22.2f, 24.76f, -1.3f };

			Reset_Chapter8_Soldier(vPlayerPos);
		}

		break;
		case Client::SNEAKWAYPOINTINDEX::CHAPTER8_2:
		case Client::SNEAKWAYPOINTINDEX::CHAPTER8_3:
		{
			vPlayerPos = { -9.3f, 21.58f, 0.32f };
			Reset_Chapter8_Soldier(vPlayerPos);
		}
		break;


		case Client::SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE1:
		{
			vPlayerPos = { -10.5f, 14.1f, -3.3f };
			vMonsterPos = { 15.f, 11.1f, 3.4f };

			Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(m_pOwner), &vPlayerPos, &vMonsterPos);

			m_pOwner->Set_AnimChangeable(true);

			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}

		break;

		case Client::SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE2:
		{
			vPlayerPos = { 26.5f, 13.72f, -3.3f };
			vMonsterPos = { 36.5f, 11.11f, 4.2f };

			Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(m_pOwner), &vPlayerPos, &vMonsterPos);

			m_pOwner->Set_AnimChangeable(true);

			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}

		break;

		case Client::SNEAKWAYPOINTINDEX::CHAPTER8_FORMATION:
		{
			//Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(m_pOwner), &vPlayerPos, &vMonsterPos);

			//m_pOwner->Set_AnimChangeable(true);

			//Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}

		break;


		default:
			return;
		}
	}
	//Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(m_pOwner), &vPlayerPos, &vMonsterPos);
	//
	//m_pOwner->Set_AnimChangeable(true);

	//Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
}

void CSneak_AttackState::Beetle_CutScene()
{
	// 1. Change Camera
	CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);
	CCamera_Manager::GetInstance()->Start_Changing_ArmLength(CCamera_Manager::TARGET, 0.f, 4.4f, EASE_IN_OUT);
	CCamera_Manager::GetInstance()->Start_Turn_ArmVector(CCamera_Manager::TARGET, 0.f, XMVectorSet(0.336f, 0.7632f, -0.552f, 0.f), EASE_IN_OUT);
	CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 0.f, CCamera::LEVEL_6, EASE_IN_OUT);

	// 2. Change Player Animation
	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	pPlayer->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_KNOCKED_DOWN_AND_EATEN_FROM_BEHIND_LATCH);

	// 3. Player Input 막기
	pPlayer->Set_BlockPlayerInput(true);
}

void CSneak_AttackState::FadeInOut(_float _fTimeDelta)
{
	if (false == m_isStartFade) {
		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fSneakFadeTime, _fTimeDelta, LERP);

		if (fRatio >= (1.f - EPSILON)) {
			m_fSneakFadeTime.y = 0.f;
			m_isStartFade = true;
			CCamera_Manager::GetInstance()->Start_FadeOut(0.7f);
		}

		return;
	}
	
	// FadeOut이 시작됐다면?
	_float fFadeRatio = CCamera_Manager::GetInstance()->Get_DofBufferData().fFadeRatio;

	if (0.f == fFadeRatio) {
		// Player 위치, Monster의 상태 전환
		After_Attack();

		// FadeIn 시작
		CCamera_Manager::GetInstance()->Start_FadeIn(0.7f);
		CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 0.f);
		
		// Player Animation 바꾸기
		CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
		pPlayer->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_01);

		// KeyInput 막은 거 풀기
		pPlayer->Set_BlockPlayerInput(false);

		m_isStartFade = false;
	}
}

void CSneak_AttackState::Check_Animation_End()
{
	// Player Anim 바꾸기
	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	if (false == pPlayer->Get_Body()->Is_AnimTransition() && false == pPlayer->Get_Body()->Is_DuringAnimation()) {
		pPlayer->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_KNOCKED_DOWN_AND_EATEN_FROM_BEHIND_LOOP_LATCH);
	}
}

void CSneak_AttackState::Reset_Chapter2_2(_float3 _vPlayerPos)
{
	_float3 vMonsterPos;

	auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevel, TEXT("Layer_Sneak_Beetle2"));

	if (nullptr != pLayer)
	{
		const auto& Objects = pLayer->Get_GameObjects();

		for_each(Objects.begin(), Objects.end(), [&](CGameObject* pGameObject) {
			auto pObject = dynamic_cast<CMonster*>(pGameObject);

			if (nullptr != pObject)
			{
				switch (pObject->Get_WayIndex())
				{
				case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2:
					vMonsterPos = _float3(32.15f, 0.35f, 1.66f);
					break;

				case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2_2:
					vMonsterPos = _float3(39.5f, 0.35f, 10.5f);
					break;

				case Client::SNEAKWAYPOINTINDEX::CHAPTER2_3:
					vMonsterPos = _float3(47.f, 0.35f, -0.5f);
					break;
				}

				Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(pObject), &_vPlayerPos, &vMonsterPos);
				pObject->Set_AnimChangeable(true);

				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, pObject->Get_FSM());
			}
			});
	}
}

void CSneak_AttackState::Reset_Chapter8_Soldier(_float3 _vPlayerPos)
{
	_float3 vMonsterPos;

	auto pLayer = m_pGameInstance->Find_Layer(m_iCurLevel, TEXT("Layer_Sneak_Soldier"));

	if (nullptr != pLayer)
	{
		const auto& Objects = pLayer->Get_GameObjects();

		for_each(Objects.begin(), Objects.end(), [&](CGameObject* pGameObject) {
			CMonster* pObject = static_cast<CMonster*>(pGameObject);

			if (nullptr != pObject)
			{
				switch (pObject->Get_WayIndex())
				{
				case Client::SNEAKWAYPOINTINDEX::CHAPTER8_1:
					vMonsterPos = _float3(13.f, 21.58f, 5.5f);
					break;

				case Client::SNEAKWAYPOINTINDEX::CHAPTER8_2:
					vMonsterPos = _float3(-3.55f, 21.58f, 6.15f);
					break;

				case Client::SNEAKWAYPOINTINDEX::CHAPTER8_3:
					vMonsterPos = _float3(-3.f, 21.58f, 13.f);
					break;
				}

				Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(pObject), &_vPlayerPos, &vMonsterPos);
				pObject->Set_AnimChangeable(true);

				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, pObject->Get_FSM());
			}
			});
	}
}

CSneak_AttackState* CSneak_AttackState::Create(void* _pArg)
{
	CSneak_AttackState* pInstance = new CSneak_AttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_AttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_AttackState::Free()
{
	//CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr()->Get_Body()->Remove
	__super::Free();
}
