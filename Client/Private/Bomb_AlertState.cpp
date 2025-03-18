#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Bomb_AlertState.h"
#include "Monster.h"
#include "FSM.h"

CBomb_AlertState::CBomb_AlertState()
{
}

HRESULT CBomb_AlertState::Initialize(void* _pArg)
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


void CBomb_AlertState::State_Enter()
{
	if(true == m_pOwner->Has_StateAnim(MONSTER_STATE::ALERT))
	{
		m_pOwner->Set_AnimChangeable(false);
	}

	if (nullptr != m_pOwner && nullptr != m_pTarget)
	{
		if (COORDINATE_2D == m_pOwner->Get_CurCoord())
			m_pOwner->Change_Dir();
	}
}

void CBomb_AlertState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	
	if (true == m_pOwner->Get_AnimChangeable())
	{
		Event_ChangeMonsterState(MONSTER_STATE::PANIC, m_pFSM);
	}
}

void CBomb_AlertState::State_Exit()
{
}

CBomb_AlertState* CBomb_AlertState::Create(void* _pArg)
{
	CBomb_AlertState* pInstance = new CBomb_AlertState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBomb_AlertState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBomb_AlertState::Free()
{
	__super::Free();
}
