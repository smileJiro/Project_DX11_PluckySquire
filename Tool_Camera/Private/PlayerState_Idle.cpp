#include "stdafx.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Test_Player.h"
#include "StateMachine.h"
#include "PlayerState_Run.h"


CPlayerState_Idle::CPlayerState_Idle(CTest_Player* _pOwner)
	:CPlayerState(_pOwner, CTest_Player::IDLE)
{
}

void CPlayerState_Idle::Update(_float _fTimeDelta)
{
	/*if (MOUSE_DOWN(MOUSE_KEY::LB))
	{
		m_pOwner->Set_State(CTest_Player::ATTACK);
		return;
	}*/

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
		m_pOwner->Set_State(CTest_Player::JUMP);
	}
	else if (bMove)
	{
		m_pOwner->Set_State(CTest_Player::RUN);
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
		case Camera_Tool::F_DIRECTION::LEFT:
		case Camera_Tool::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_IDLE_RIGHT);
			break;
		case Camera_Tool::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_IDLE_UP);
			break;
		case Camera_Tool::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CTest_Player::ANIM_STATE_2D::PLAYER_IDLE_DOWN);
			break;
		case Camera_Tool::F_DIRECTION::F_DIR_LAST:
		default:
			break;
		}
	}

	else
		m_pOwner->Switch_Animation((_uint)CTest_Player::ANIM_STATE_3D::LATCH_ANIM_IDLE_01_GT);


}

void CPlayerState_Idle::Exit()
{
}
