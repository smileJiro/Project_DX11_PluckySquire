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
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CRangedAttackState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CRangedAttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_Position());
	//공격 범위 벗어나고 추적 범위 내면 Chase 전환
	if (fDis > Get_CurCoordRange(MONSTER_STATE::ATTACK) && fDis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
		return;
	}
	//범위 전부를 벗어나면 Idle 전환
	if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
	else
	{
		//공격
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_Position());
		}
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			m_pOwner->Change_Dir();
		}
		m_pOwner->Attack(_fTimeDelta);
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
