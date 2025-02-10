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
	m_fAttackRange = pDesc->fAttackRange;

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
	
	if(m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
	{
		_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
		if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
		{
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_ATTACK, m_pFSM);
			return;
		}
		if (fDis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
		{
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_CHASE, m_pFSM);
		}
		//구역 밖으로 나가면 못 쫓음
		else
		{
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}
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
