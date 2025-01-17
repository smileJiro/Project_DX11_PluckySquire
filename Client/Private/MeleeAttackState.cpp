#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MeleeAttackState.h"
#include "Monster.h"
#include "FSM.h"

CMeleeAttackState::CMeleeAttackState()
{
}

HRESULT CMeleeAttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CMeleeAttackState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CMeleeAttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	_float dis = XMVectorGetX(XMVector3Length((m_pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION))));
	//공격 범위 벗어나고 추적 범위 내면 Chase 전환
	if (dis > m_fAttackRange && dis <= m_fChaseRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
		return;
	}
	//범위 전부를 벗어나면 Idle 전환
	if (dis > m_fChaseRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
	else
	{
		//공격

	}
}

void CMeleeAttackState::State_Exit()
{
}

CMeleeAttackState* CMeleeAttackState::Create(void* _pArg)
{
	CMeleeAttackState* pInstance = new CMeleeAttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CMeleeAttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeleeAttackState::Free()
{
	__super::Free();
}
