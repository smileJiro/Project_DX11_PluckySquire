#include "stdafx.h"
#include "PlayerState_Attack.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"

CPlayerState_Attack::CPlayerState_Attack(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ATTACK)
{
}

void CPlayerState_Attack::Update(_float _fTimeDelta)
{
	if (MOUSE_DOWN(MOUSE_KEY::LB))
	{
		m_bCombo = true;
	}
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	//0.3이상일 때 공격버튼이 눌린 적 있었으면?
	_float fProgress =m_pOwner->Get_AnimProgress();
	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	_float fForwardingProgress = eCoord == COORDINATE_2D ? m_f2DForwardingProgress : m_f3DForwardingProgress;
	if (fProgress >= fMotionCancelProgress)
	{
		if (m_bCombo)
		{
			m_iComboCount++;
			if (2 >= m_iComboCount)
			{
				m_pOwner->Attack(m_iComboCount);
			}

			m_bCombo = false;
		}

	}
	else if(fProgress >= fForwardingProgress)
	{
		m_pOwner->Stop_Move();
	}
	else
	{
		m_pOwner->Move_Forward(m_fForwardSpeed, _fTimeDelta);

	}
	
}

void CPlayerState_Attack::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_pOwner->Attack(m_iComboCount);
}

void CPlayerState_Attack::Exit()
{
}

void CPlayerState_Attack::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_iComboCount = 0;
	m_bCombo = false;
	m_pOwner->Set_State(CPlayer::IDLE);
}
