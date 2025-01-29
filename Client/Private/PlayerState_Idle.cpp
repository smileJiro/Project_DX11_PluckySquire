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
	if (MOUSE_DOWN(MOUSE_KEY::LB))
	{
		m_pOwner->Set_State(CPlayer::ATTACK);
		return;
	}

	/* Test Move Code */
	if (KEY_PRESSING(KEY::SPACE))
	{
		m_pOwner->Set_State(CPlayer::JUMP);
		return;
	}
	if (KEY_PRESSING(KEY::LSHIFT))
	{
		m_pOwner->Set_State(CPlayer::ROLL);
		return;
	}
	_bool bMove = false;
	if (KEY_PRESSING(KEY::W))
		bMove = true;
	else if (KEY_PRESSING(KEY::A))
		bMove = true;
	else if (KEY_PRESSING(KEY::S))
		bMove = true;
	else if (KEY_PRESSING(KEY::D))
		bMove = true;

	if (bMove)
		m_pOwner->Set_State(CPlayer::RUN);

}

void CPlayerState_Idle::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eFDir = EDir_To_FDir( m_pOwner->Get_2DDirection());
		switch (eFDir)
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
