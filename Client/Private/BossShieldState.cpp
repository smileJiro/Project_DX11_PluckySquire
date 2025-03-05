#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossShieldState.h"
#include "Monster.h"
#include "FSM.h"

CBossShieldState::CBossShieldState()
{
}

HRESULT CBossShieldState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.3f;
	m_iNumAttack = 10;
		
	return S_OK;
}


void CBossShieldState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CBossShieldState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	//m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	//공격
	//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_FinalPosition());
	//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

	//10개 뿜고 공격 종료

	if (m_pOwner->Get_AnimChangeable())
	{
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	}

	//if (m_iNumAttack <= m_iAttackCount)
	//{
	//	Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	//}
	//else
	//{
	//	if (false == m_isDelay)
	//	{
	//		m_pOwner->Attack();
	//		++m_iAttackCount;
	//		Delay_On();
	//	}
	//}
	
}

void CBossShieldState::State_Exit()
{
}

CBossShieldState* CBossShieldState::Create(void* _pArg)
{
	CBossShieldState* pInstance = new CBossShieldState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossShieldState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossShieldState::Free()
{
	__super::Free();
}
