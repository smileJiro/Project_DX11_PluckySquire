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

	m_fDelayTime = 3.f;

	return S_OK;
}


void CHitState::State_Enter()
{
	//m_fAccTime = 0.f;
	m_pOwner->Set_AnimChangeable(false);
}

void CHitState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	/*m_fAccTime += _fTimeDelta;
	
	if (m_fDelayTime <= m_fAccTime)
	{
		Event_ChangeMonsterState(MONSTER_STATE::PATROL, m_pFSM);
	}*/

	Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
}

void CHitState::State_Exit()
{
	m_fAccTime = 0.f;
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
