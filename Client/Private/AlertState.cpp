#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "AlertState.h"
#include "Monster.h"
#include "FSM.h"

CAlertState::CAlertState()
{
}

HRESULT CAlertState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CAlertState::State_Enter()
{
}

void CAlertState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	//추적 범위 벗어나면 IDLE 전환
	_float dis = XMVectorGetX(XMVector3Length((m_pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION))));
	if (dis <= m_fAttackRange)
	{
		//공격 전환
		Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);
		return;
	}
	if (dis > m_fChaseRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
	else
	{
		//추적
		m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION));
		m_pOwner->Get_ControllerTransform()->Go_Straight(_fTimeDelta);
	}
}

void CAlertState::State_Exit()
{
}

CAlertState* CAlertState::Create(void* _pArg)
{
	CAlertState* pInstance = new CAlertState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CAlertState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlertState::Free()
{
	__super::Free();
}
