#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossIdleState.h"
#include "Monster.h"
#include "FSM.h"

CBossIdleState::CBossIdleState()
{
}

HRESULT CBossIdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fCoolTime = 2.f;

	return S_OK;
}


void CBossIdleState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CBossIdleState::State_Update(_float _fTimeDelta)
{
	m_fAccTime += _fTimeDelta;
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	
	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

	if (m_fAccTime >= m_fCoolTime)
		Event_ChangeBossState(BOSS_STATE::ATTACK, m_pFSM);
}

void CBossIdleState::State_Exit()
{
	m_fAccTime = 0.f;
}

CBossIdleState* CBossIdleState::Create(void* _pArg)
{
	CBossIdleState* pInstance = new CBossIdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossIdleState::Free()
{
	__super::Free();
}
