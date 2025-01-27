#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "IdleState.h"
#include "Monster.h"
#include "FSM.h"

CIdleState::CIdleState()
{
}

HRESULT CIdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 3.f;

	return S_OK;
}


void CIdleState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CIdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_fAccTime += _fTimeDelta;

	//몬스터 인식 범위 안에 들어오면 인식상태로 전환
	_float dis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_Position());
	if (dis <= m_fAlertRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::ALERT, m_pFSM);
	}
	
	else if (m_fDelayTime <= m_fAccTime)
	{
		Event_ChangeMonsterState(MONSTER_STATE::PATROL, m_pFSM);
	}

}

void CIdleState::State_Exit()
{
	m_fAccTime = 0.f;
}

CIdleState* CIdleState::Create(void* _pArg)
{
	CIdleState* pInstance = new CIdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIdleState::Free()
{
	__super::Free();
}
