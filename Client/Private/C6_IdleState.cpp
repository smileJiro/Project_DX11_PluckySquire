#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "C6_IdleState.h"
#include "Monster.h"
#include "FSM.h"

CC6_IdleState::CC6_IdleState()
{
}

HRESULT CC6_IdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChase2DRange = pDesc->fChase2DRange;

	if (0 <= pDesc->fDelayTime)
	{
		m_fDelayTime = pDesc->fDelayTime;
	}
	else
	{
		m_fDelayTime = 1.f;
	}

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CC6_IdleState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CC6_IdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (false == m_pOwner->Is_Stay())
		m_fAccTime += _fTimeDelta;

	if (nullptr != m_pTarget)
	{
		if (m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
		{
			_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
			if (fDis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
			{
				Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
			}
		}
	}

}

void CC6_IdleState::State_Exit()
{
	m_fAccTime = 0.f;
}

CC6_IdleState* CC6_IdleState::Create(void* _pArg)
{
	CC6_IdleState* pInstance = new CC6_IdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CC6_IdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CC6_IdleState::Free()
{
	__super::Free();
}
