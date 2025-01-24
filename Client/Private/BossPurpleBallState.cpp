#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossPurpleBallState.h"
#include "Monster.h"
#include "FSM.h"

CBossPurpleBallState::CBossPurpleBallState()
{
}

HRESULT CBossPurpleBallState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.3f;
	
	return S_OK;
}


void CBossPurpleBallState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
	Delay_Off();
	m_iAttackCount = 0;
}

void CBossPurpleBallState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (true == m_isDelay)
	{
		m_fAccTime += _fTimeDelta;
		if (m_fDelayTime <= m_fAccTime)
		{
			Delay_Off();
		}
	}

	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_Position() - m_pOwner->Get_Position());
	//공격
	//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_Position());
	m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

	//3번 뿜고 공격 종료
	if (m_iNumAttack <= m_iAttackCount)
	{
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	}
	else
	{
		if (false == m_isDelay)
		{
			m_pOwner->Attack(_fTimeDelta);
			++m_iAttackCount;
			Delay_On();
		}
	}
	
}

void CBossPurpleBallState::State_Exit()
{
	Delay_Off();
	m_iAttackCount = 0;
}

CBossPurpleBallState* CBossPurpleBallState::Create(void* _pArg)
{
	CBossPurpleBallState* pInstance = new CBossPurpleBallState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossPurpleBallState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossPurpleBallState::Free()
{
	__super::Free();
}
