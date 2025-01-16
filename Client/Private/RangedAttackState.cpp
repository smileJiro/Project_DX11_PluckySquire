#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "RangedAttackState.h"
#include "Monster.h"
#include "FSM.h"

CRangedAttackState::CRangedAttackState()
{
}

HRESULT CRangedAttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CRangedAttackState::State_Enter()
{
}

void CRangedAttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
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

void CRangedAttackState::State_Exit()
{
}

CRangedAttackState* CRangedAttackState::Create(void* _pArg)
{
	CRangedAttackState* pInstance = new CRangedAttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CRangedAttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRangedAttackState::Free()
{
	__super::Free();
}
