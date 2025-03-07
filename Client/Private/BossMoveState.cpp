#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossMoveState.h"
#include "ButterGrump.h"
#include "FSM.h"

CBossMoveState::CBossMoveState()
{
}

HRESULT CBossMoveState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CBossMoveState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CBossMoveState::State_Update(_float _fTimeDelta)
{
	m_fAccTime += _fTimeDelta;
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);


	if (m_fAccTime >= m_fCoolTime)
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
}

void CBossMoveState::State_Exit()
{
	m_fAccTime = 0.f;
}

CBossMoveState* CBossMoveState::Create(void* _pArg)
{
	CBossMoveState* pInstance = new CBossMoveState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossMoveState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossMoveState::Free()
{
	__super::Free();
}
