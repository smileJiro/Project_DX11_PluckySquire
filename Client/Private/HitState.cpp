#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "HitState.h"
#include "Monster.h"
#include "FSM.h"

CHitState::CHitState()
{
}

HRESULT CHitState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CHitState::State_Enter()
{
	if (0 >= m_pOwner->Get_Stat().iHP)
	{
		m_pFSM->Change_State((_uint)MONSTER_STATE::DEAD);
		return;
	}
	m_pOwner->Set_AnimChangeable(false);
}

void CHitState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if(true == m_pOwner->Get_AnimChangeable())
	{
		if(m_pFSM->Has_State(MONSTER_STATE::CHASE))
		{
			Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
		}
		else
		{
			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		}
	}
}

void CHitState::State_Exit()
{
}

CHitState* CHitState::Create(void* _pArg)
{
	CHitState* pInstance = new CHitState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CHitState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHitState::Free()
{
	__super::Free();
}
