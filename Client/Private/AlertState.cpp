#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "AlertState.h"
#include "Monster.h"
#include "FSM.h"

CAlertState::CAlertState()
{
}

HRESULT CAlertState::Initialize(void* _pArg)
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


void CAlertState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CAlertState::State_Update(_float _fTimeDelta)
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

void CAlertState::State_Exit()
{
}

CAlertState* CAlertState::Create(void* _pArg)
{
	CAlertState* pInstance = new CAlertState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CAlertState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlertState::Free()
{
	__super::Free();
}
