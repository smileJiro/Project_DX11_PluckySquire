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
	if (m_bCombo)
	{
		m_iComboCount++;
		if (2 >= m_iComboCount )
		{
			m_pOwner->Attack(m_iComboCount);
		}

		m_bCombo = false;
	}
	else
	{
		m_iComboCount = 0;
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}
