#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossEnergyBallState.h"
#include "Monster.h"
#include "FSM.h"

CBossEnergyBallState::CBossEnergyBallState()
{
}

HRESULT CBossEnergyBallState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.5f;
		
	return S_OK;
}


void CBossEnergyBallState::State_Enter()
{
	Delay_Off();
	m_iAttackCount = 0;
}

void CBossEnergyBallState::State_Update(_float _fTimeDelta)
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

	//5개 뿜고 공격 종료
	if (5 <= m_iAttackCount)
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

void CBossEnergyBallState::State_Exit()
{
	Delay_Off();
	m_iAttackCount = 0;
}

CBossEnergyBallState* CBossEnergyBallState::Create(void* _pArg)
{
	CBossEnergyBallState* pInstance = new CBossEnergyBallState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossEnergyBallState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossEnergyBallState::Free()
{
	__super::Free();
}
