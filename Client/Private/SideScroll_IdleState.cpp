#include "stdafx.h"
#include "SideScroll_IdleState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CSideScroll_IdleState::CSideScroll_IdleState()
{
}

HRESULT CSideScroll_IdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (0.f <= pDesc->fAttack2DRange)
		m_fAttack2DRange = pDesc->fAttack2DRange;
	else
	{
		m_fAttack2DRange = -1.f;
	}
		
	return S_OK;
}

void CSideScroll_IdleState::State_Enter()
{
}

void CSideScroll_IdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (COORDINATE_2D == m_pOwner->Get_CurCoord() && m_pOwner->Get_Include_Section_Name() == m_pTarget->Get_Include_Section_Name())
	{
		_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
		if (fDis <= m_fAttack2DRange && false == m_pOwner->IsDelay())
		{
			Event_ChangeMonsterState(MONSTER_STATE::SIDESCROLL_ATTACK, m_pFSM);
			return;
		}
	}
}

void CSideScroll_IdleState::State_Exit()
{
}

CSideScroll_IdleState* CSideScroll_IdleState::Create(void* _pArg)
{
	CSideScroll_IdleState* pInstance = new CSideScroll_IdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSideScroll_IdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSideScroll_IdleState::Free()
{
	__super::Free();
}
