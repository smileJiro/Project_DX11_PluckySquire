#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "SideScroll_HitState.h"
#include "Monster.h"
#include "FSM.h"

CSideScroll_HitState::CSideScroll_HitState()
{
}

HRESULT CSideScroll_HitState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CSideScroll_HitState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CSideScroll_HitState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if(true == m_pOwner->Get_AnimChangeable())
		Event_ChangeMonsterState(MONSTER_STATE::PATROL, m_pFSM);
}

void CSideScroll_HitState::State_Exit()
{
}

CSideScroll_HitState* CSideScroll_HitState::Create(void* _pArg)
{
	CSideScroll_HitState* pInstance = new CSideScroll_HitState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSideScroll_HitState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSideScroll_HitState::Free()
{
	__super::Free();
}
