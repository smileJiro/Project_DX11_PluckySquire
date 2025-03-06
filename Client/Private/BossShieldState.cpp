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

	if (m_pOwner->Get_AnimChangeable())
	{
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	}

	
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
