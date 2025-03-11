#include "stdafx.h"
#include "SideScroll_AttackState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CSideScroll_AttackState::CSideScroll_AttackState()
{
}

HRESULT CSideScroll_AttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (0.f <= pDesc->fAttack2DRange)
		m_fAttack2DRange = pDesc->fAttack2DRange;
	else
	{
		m_fAttack2DRange = -1.f;
	}
		
	return S_OK;
}

void CSideScroll_AttackState::State_Enter()
{
}

void CSideScroll_AttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Attack();

	if (true == m_pOwner->Get_AnimChangeable())
		Event_ChangeMonsterState(MONSTER_STATE::SIDESCROLL_IDLE, m_pFSM);
}

void CSideScroll_AttackState::State_Exit()
{
}

CSideScroll_AttackState* CSideScroll_AttackState::Create(void* _pArg)
{
	CSideScroll_AttackState* pInstance = new CSideScroll_AttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSideScroll_AttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSideScroll_AttackState::Free()
{
	__super::Free();
}
