#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossTransitionState.h"
#include "Monster.h"
#include "FSM.h"

CBossTransitionState::CBossTransitionState()
{
}

HRESULT CBossTransitionState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CBossTransitionState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CBossTransitionState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (true == m_pOwner->Get_AnimChangeable())
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
}

void CBossTransitionState::State_Exit()
{
}

CBossTransitionState* CBossTransitionState::Create(void* _pArg)
{
	CBossTransitionState* pInstance = new CBossTransitionState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossTransitionState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossTransitionState::Free()
{
	__super::Free();
}
