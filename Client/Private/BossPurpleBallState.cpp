#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossPurpleBallState.h"
#include "Monster.h"
#include "FSM.h"

CBossPurpleBallState::CBossPurpleBallState()
{
}

HRESULT CBossPurpleBallState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.3f;
	
	return S_OK;
}


void CBossPurpleBallState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
	Delay_Off();
	m_iAttackCount = 0;
}

void CBossPurpleBallState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;


	_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());

	m_pOwner->Get_ControllerTransform()->Turn_To_DesireDir(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec() * _fTimeDelta);
	
	if (m_pOwner->Get_AnimChangeable())
	{
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	}
}

void CBossPurpleBallState::State_Exit()
{
	Delay_Off();
	m_iAttackCount = 0;
}

CBossPurpleBallState* CBossPurpleBallState::Create(void* _pArg)
{
	CBossPurpleBallState* pInstance = new CBossPurpleBallState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossPurpleBallState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossPurpleBallState::Free()
{
	__super::Free();
}
