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
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CIdleState::State_Enter()
{
}

void CIdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	//몬스터 범위 안에 들어오면 추적으로 전환
	_float dis = XMVectorGetX(XMVector3Length((m_pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION))));
	if (dis < m_fChaseRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	}

}

void CIdleState::State_Exit()
{
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
