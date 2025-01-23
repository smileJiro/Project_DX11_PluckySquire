#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossHomingBallState.h"
#include "Monster.h"
#include "FSM.h"

CBossHomingBallState::CBossHomingBallState()
{
}

HRESULT CBossHomingBallState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.5f;
		
	return S_OK;
}


void CBossHomingBallState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CBossHomingBallState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	m_fAccTime += _fTimeDelta;

	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_Position() - m_pOwner->Get_Position());
	//АјАн
	//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_Position());
	m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);
	m_pOwner->Attack(_fTimeDelta);
	Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	
}

void CBossHomingBallState::State_Exit()
{
	m_fAccTime = 0.f;
}

CBossHomingBallState* CBossHomingBallState::Create(void* _pArg)
{
	CBossHomingBallState* pInstance = new CBossHomingBallState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossHomingBallState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHomingBallState::Free()
{
	__super::Free();
}
