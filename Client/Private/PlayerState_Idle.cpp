#include "stdafx.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"
#include "PlayerState_Run.h"


CPlayerState_Idle::CPlayerState_Idle(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::IDLE)
{
}

void CPlayerState_Idle::Update(_float _fTimeDelta)
{
	_bool bMove = false;
	_bool bJump = false;
	/* Test Move Code */
	if (KEY_PRESSING(KEY::SPACE))
	{
		bJump = true;
	}
	else if (KEY_PRESSING(KEY::W))
	{
		bMove = true;
	}
	else if (KEY_PRESSING(KEY::A))
	{
		bMove = true;
	}
	else if (KEY_PRESSING(KEY::S))
	{
		bMove = true;
	}
	else if (KEY_PRESSING(KEY::D))
	{
		bMove = true;
	} 
	if (bJump)
	{
		m_pOwner->Set_State(CPlayer::JUMP);
	}
	else if (bMove)
	{
		m_pOwner->Set_State(CPlayer::RUN);
	}

}

void CPlayerState_Idle::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eOldDir = m_pOwner->Get_2DDirection();
		switch (eOldDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_IDLE_DOWN);
			break;
		case Client::F_DIRECTION::F_DIR_LAST:
		default:
			break;
		}
	}

	else
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_01_GT);


}

void CPlayerState_Idle::Exit()
{
}
