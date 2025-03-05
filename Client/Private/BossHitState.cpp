#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossHitState.h"
#include "Monster.h"
#include "FSM.h"

CBossHitState::CBossHitState()
{
}

HRESULT CBossHitState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CBossHitState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CBossHitState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (true == m_pOwner->Get_AnimChangeable())
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
}

void CBossHitState::State_Exit()
{
}

CBossHitState* CBossHitState::Create(void* _pArg)
{
	CBossHitState* pInstance = new CBossHitState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossHitState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHitState::Free()
{
	__super::Free();
}
