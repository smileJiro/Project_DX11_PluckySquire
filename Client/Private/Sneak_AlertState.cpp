#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_AlertState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_AlertState::CSneak_AlertState()
{
}

HRESULT CSneak_AlertState::Initialize(void* _pArg)
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


void CSneak_AlertState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CSneak_AlertState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	
	_float dis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
	if (dis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	{
		Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);
		return;
	}
	if (dis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	}
	else
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
}

void CSneak_AlertState::State_Exit()
{
}

CSneak_AlertState* CSneak_AlertState::Create(void* _pArg)
{
	CSneak_AlertState* pInstance = new CSneak_AlertState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_AlertState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_AlertState::Free()
{
	__super::Free();
}
