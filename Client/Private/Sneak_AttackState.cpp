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

	//컷씬으로 들어가며 초기화
	//Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);
	Event_ChangeMonsterState(MONSTER_STATE::SNEAK_BACK, m_pFSM);
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
