#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossIdleState.h"
#include "ButterGrump.h"
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

	//m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);
	_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());

	m_pOwner->Get_ControllerTransform()->Turn_To_DesireDir(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec() * _fTimeDelta);

	CButterGrump* pBoss = static_cast<CButterGrump*>(m_pOwner);

	//공격이 이어진다면 이전 공격 상태로 전환
	if (true == pBoss->Is_AttackChained())
	{
		if (m_fAccTime >= pBoss->Get_ChainDelayTime())
		{
			Event_ChangeBossState((BOSS_STATE)m_pOwner->Get_PreState(), m_pFSM);
		}
	}
	else if (true == pBoss->Is_Converse())
	{
		Event_ChangeBossState(BOSS_STATE::SHIELD, m_pFSM);
	}
	else if (true == pBoss->Is_Move())
	{
		Event_ChangeBossState(BOSS_STATE::MOVE, m_pFSM);
	}
	else
	{
		if (m_fAccTime >= m_fCoolTime)
			Event_ChangeBossState(BOSS_STATE::ATTACK, m_pFSM);
	}
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
