#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MonsterMoveState.h"
#include "Monster.h"
#include "FSM.h"

CMonsterMoveState::CMonsterMoveState()
{
}

HRESULT CMonsterMoveState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;

	m_eMoveNextState = pDesc->eMoveNextState;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CMonsterMoveState::State_Enter()
{
	//m_pOwner->Set_AnimChangeable(false);
}

void CMonsterMoveState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Go_Straight_F_Dir(_fTimeDelta);

	//이동 후 도착하면 다음 상태로 전환
	if(true == m_pOwner->IsContactToBlock())
	{
		if (MONSTER_STATE::LAST != m_eMoveNextState)
		{
			Event_ChangeMonsterState(m_eMoveNextState, m_pFSM);
		}
		else
		{
			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		}
	}
}

void CMonsterMoveState::State_Exit()
{
}

CMonsterMoveState* CMonsterMoveState::Create(void* _pArg)
{
	CMonsterMoveState* pInstance = new CMonsterMoveState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CMonsterMoveState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterMoveState::Free()
{
	__super::Free();
}
