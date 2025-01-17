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
	m_pOwner->Set_AnimChangeable(false);
}

void CAlertState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	//CHASE ÀüÈ¯
	_float dis = XMVectorGetX(XMVector3Length((m_pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION))));
	if (dis <= m_fChaseRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
		return;
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
