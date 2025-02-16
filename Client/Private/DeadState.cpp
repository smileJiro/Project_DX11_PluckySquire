#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "DeadState.h"
#include "Monster.h"
#include "FSM.h"

CDeadState::CDeadState()
{
}

HRESULT CDeadState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 3.f;

	return S_OK;
}


void CDeadState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CDeadState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (true == m_pOwner->Get_AnimChangeable())
	{
		Event_DeleteObject(m_pOwner);
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
}

void CDeadState::State_Exit()
{
}

CDeadState* CDeadState::Create(void* _pArg)
{
	CDeadState* pInstance = new CDeadState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CDeadState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeadState::Free()
{
	__super::Free();
}
