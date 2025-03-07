#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossYellowBallState.h"
#include "Monster.h"
#include "FSM.h"

CBossYellowBallState::CBossYellowBallState()
{
}

HRESULT CBossYellowBallState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.3f;
	m_iNumAttack = 3;
	
	return S_OK;
}


void CBossYellowBallState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
	Delay_Off();
	m_iAttackCount = 0;
}

void CBossYellowBallState::State_Update(_float _fTimeDelta)
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

void CBossYellowBallState::State_Exit()
{
	Delay_Off();
	m_iAttackCount = 0;
}

CBossYellowBallState* CBossYellowBallState::Create(void* _pArg)
{
	CBossYellowBallState* pInstance = new CBossYellowBallState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossYellowBallState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossYellowBallState::Free()
{
	__super::Free();
}
