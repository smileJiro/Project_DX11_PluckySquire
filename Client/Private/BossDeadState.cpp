#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossDeadState.h"
#include "Monster.h"
#include "FSM.h"

CBossDeadState::CBossDeadState()
{
}

HRESULT CBossDeadState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CBossDeadState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CBossDeadState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	//if (true == m_pOwner->Get_AnimChangeable())
	//	Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
}

void CBossDeadState::State_Exit()
{
}

CBossDeadState* CBossDeadState::Create(void* _pArg)
{
	CBossDeadState* pInstance = new CBossDeadState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossDeadState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossDeadState::Free()
{
	__super::Free();
}
