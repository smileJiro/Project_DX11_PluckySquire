#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ChaseWalkState.h"
#include "Monster.h"
#include "FSM.h"

CChaseWalkState::CChaseWalkState()
{
}

HRESULT CChaseWalkState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CChaseWalkState::State_Enter()
{
}

void CChaseWalkState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	_vector dir = m_pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
	_float dis = XMVectorGetX(XMVector3Length((dir)));
	if (dis <= m_fAttackRange)
	{
		//공격 전환
		Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);
		return;
	}

	//추적 범위 벗어나면 IDLE 전환
	if (dis > m_fChaseRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
	else
	{
		//추적
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION));
		m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(dir);
		m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta*2.f);
		m_pOwner->Get_ControllerTransform()->Go_Straight(_fTimeDelta);
	}
}

void CChaseWalkState::State_Exit()
{
}

CChaseWalkState* CChaseWalkState::Create(void* _pArg)
{
	CChaseWalkState* pInstance = new CChaseWalkState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CChaseWalkState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaseWalkState::Free()
{
	__super::Free();
}
