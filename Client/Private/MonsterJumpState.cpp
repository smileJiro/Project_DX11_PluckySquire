#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterJumpState.h"
#include "Monster.h"
#include "FSM.h"

CMonsterJumpState::CMonsterJumpState()
{
}

HRESULT CMonsterJumpState::Initialize(void* _pArg)
{
	MONSTERMOVE_DESC* pDesc = static_cast<MONSTERMOVE_DESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CMonsterJumpState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CMonsterJumpState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Go_Straight_F_Dir(_fTimeDelta);

	//점프가 끝나면 다음 상태로 전환
	if (true == m_pOwner->Get_AnimChangeable())
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	}
}

void CMonsterJumpState::State_Exit()
{
}

CMonsterJumpState* CMonsterJumpState::Create(void* _pArg)
{
	CMonsterJumpState* pInstance = new CMonsterJumpState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CMonsterJumpState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterJumpState::Free()
{
	__super::Free();
}
