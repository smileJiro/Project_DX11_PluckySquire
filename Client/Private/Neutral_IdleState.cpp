#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Neutral_IdleState.h"
#include "Monster.h"
#include "FSM.h"

CNeutral_IdleState::CNeutral_IdleState()
{
}

HRESULT CNeutral_IdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 3.f;

	return S_OK;
}


void CNeutral_IdleState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CNeutral_IdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	m_fAccTime += _fTimeDelta;
	
	if (m_fDelayTime <= m_fAccTime)
	{
		Event_ChangeMonsterState(MONSTER_STATE::PATROL, m_pFSM);
	}

}

void CNeutral_IdleState::State_Exit()
{
	m_fAccTime = 0.f;
}

CNeutral_IdleState* CNeutral_IdleState::Create(void* _pArg)
{
	CNeutral_IdleState* pInstance = new CNeutral_IdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CNeutral_IdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNeutral_IdleState::Free()
{
	__super::Free();
}
