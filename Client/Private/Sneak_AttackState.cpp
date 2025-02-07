#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_AttackState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_AttackState::CSneak_AttackState()
{
}

HRESULT CSneak_AttackState::Initialize(void* _pArg)
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


void CSneak_AttackState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CSneak_AttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
	//if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	//{
	//	//m_pOwner->Attack();
	//}
	//else
	//{
	//	Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);
	//}

	Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);

	////공격 범위 벗어나고 추적 범위 내면 Chase 전환
	//if (fDis > Get_CurCoordRange(MONSTER_STATE::ATTACK) && fDis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
	//{
	//	Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	//	return;
	//}
	////범위 전부를 벗어나면 Idle 전환
	//if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	//{
	//	Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	//}
	//else
	//{
	//	//타겟 방향으로 회전 후 공격
	//	/*if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
	//	{
	//		m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_FinalPosition());
	//	}
	//	else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
	//	{
	//		m_pOwner->Change_Dir();
	//	}*/
	//	m_pOwner->Attack();
	//}
}

void CSneak_AttackState::State_Exit()
{
}

CSneak_AttackState* CSneak_AttackState::Create(void* _pArg)
{
	CSneak_AttackState* pInstance = new CSneak_AttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_AttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_AttackState::Free()
{
	__super::Free();
}
