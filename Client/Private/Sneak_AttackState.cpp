#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_AttackState.h"
#include "Monster.h"
#include "FSM.h"

#include "Camera_Manager.h"
#include "PlayerData_Manager.h"
#include "PlayerBody.h"

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

	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	pPlayer->Get_Body()->Register_OnAnimEndCallBack(bind(&CSneak_AttackState::On_Player_AnimEnd, this, std::placeholders::_1, std::placeholders::_2));
	
	return S_OK;
}


void CSneak_AttackState::State_Enter()
{
	Beetle_CutScene();
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
	if(m_pOwner->Get_AnimChangeable())
	{
		if(false == m_pOwner->Is_FormationMode())
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		else
		{
			m_pOwner->Add_To_Formation();
			Event_ChangeMonsterState(MONSTER_STATE::FORMATION_BACK, m_pFSM);
		}
	}

	// FadeInOut 시간 계산
	FadeInOut(_fTimeDelta);
}

void CSneak_AttackState::State_Exit()
{

}

void CSneak_AttackState::After_Attack()
{
	_float3 vPlayerPos = { };
	_float3 vMonsterPos = { };

	switch (m_eWayIndex)
	{
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_1:
		vPlayerPos = { -31.f, 6.56f, 22.5f };
		vMonsterPos = { -16.5f, 6.56f, 22.6f };
		break;
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2:
		vPlayerPos = { 40.f, 0.35f, -7.f };
		vMonsterPos = { 32.15f, 0.35f, 1.66f };
		break;
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2_2:
		vPlayerPos = { 36.6f, 0.35f, 5.5f };
		vMonsterPos = { 39.5f, 0.35f, 10.5f };
		break;
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_3:
		vPlayerPos = { 43.f, 0.35f, -0.5f };
		vMonsterPos = { 47.f, 0.35f, -0.5f };
		break;

	default:
		return;
	}
	Event_Sneak_BeetleCaught(static_cast<CActorObject*>(m_pTarget), static_cast<CActorObject*>(m_pOwner), &vPlayerPos, &vMonsterPos);
	
	m_pOwner->Set_AnimChangeable(true);

	Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
}

void CSneak_AttackState::Beetle_CutScene()
{
	// 1. Change Camera
	CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);
	CCamera_Manager::GetInstance()->Start_Changing_ArmLength(CCamera_Manager::TARGET, 0.f, 5.4f, EASE_IN_OUT);
	CCamera_Manager::GetInstance()->Start_Turn_ArmVector(CCamera_Manager::TARGET, 0.f, XMVectorSet(0.336f, 0.7632f, -0.552f, 0.f), EASE_IN_OUT);
	CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 0.f, CCamera::LEVEL_6, EASE_IN_OUT);

	// 2. Change Player Animation
	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	pPlayer->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_KNOCKED_DOWN_AND_EATEN_FROM_BEHIND_LATCH);
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

		m_isStartFade = false;
	}
}

void CSneak_AttackState::On_Player_AnimEnd(COORDINATE _eCoordinate, _uint _iAnimIndex)
{
	if (COORDINATE_3D == _eCoordinate && (_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_01 == _iAnimIndex) {
		// Player Anim 바꾸기
		CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
		pPlayer->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_01);
	}
}

void CSneak_AttackState::On_Beetle_AnimEnd(COORDINATE _eCoordinate, _uint _iAnimIndex)
{
	m_pOwner->
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
	__super::Free();
}
