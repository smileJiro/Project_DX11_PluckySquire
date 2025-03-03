#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossRockVolleyState.h"
#include "Monster.h"
#include "FSM.h"

CBossRockVolleyState::CBossRockVolleyState()
{
}

HRESULT CBossRockVolleyState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.3f;
	m_iNumAttack = 10;
		
	return S_OK;
}


void CBossRockVolleyState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
	Delay_Off();
	m_iAttackCount = 0;
}

void CBossRockVolleyState::State_Update(_float _fTimeDelta)
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

	//m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	//공격
	//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_FinalPosition());
	//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

	//10개 뿜고 공격 종료
	if (m_iNumAttack <= m_iAttackCount)
	{
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	}
	else
	{
		if (false == m_isDelay)
		{
			//m_pOwner->Attack();
			++m_iAttackCount;
			Delay_On();
		}
	}
	
}

void CBossRockVolleyState::State_Exit()
{
	Delay_Off();
	m_iAttackCount = 0;
}

CBossRockVolleyState* CBossRockVolleyState::Create(void* _pArg)
{
	CBossRockVolleyState* pInstance = new CBossRockVolleyState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossRockVolleyState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossRockVolleyState::Free()
{
	__super::Free();
}
