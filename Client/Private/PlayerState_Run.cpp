#include "stdafx.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"


CPlayerState_Run::CPlayerState_Run(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::RUN)
{
}

void CPlayerState_Run::Update(_float _fTimeDelta)
{
	COORDINATE eCoord =  m_pOwner->Get_CurCoord();
	_vector vMoveDir = XMVectorZero();
	_bool bMove = false;

	if (KEY_PRESSING(KEY::W))
	{
		if (eCoord == COORDINATE_3D)
			vMoveDir += _vector{ 0.f, 0.f, 1.f,0.f };
		else
			vMoveDir += _vector{ 0.f, 1.f, 0.f,0.f };
		bMove = true;
	}
	if (KEY_PRESSING(KEY::A))
	{
		vMoveDir += _vector{ -1.f, 0.f, 0.f,0.f };
		bMove = true;
	}
	if (KEY_PRESSING(KEY::S))
	{
		if (eCoord == COORDINATE_3D)
			vMoveDir += _vector{ 0.f, 0.f, -1.f,0.f };
		else
			vMoveDir += _vector{ 0.f, -1.f, 0.f,0.f };

		bMove = true;
	}
	if (KEY_PRESSING(KEY::D))
	{
		vMoveDir += _vector{ 1.f, 0.f, 0.f,0.f };
		bMove = true;
	}
	if (bMove)
	{
		m_pOwner->Move(vMoveDir, _fTimeDelta);
		if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			F_DIRECTION eNewDir =  To_FDirection(vMoveDir);
			F_DIRECTION eOldDir = m_pOwner->Get_2DDirection();
			m_pOwner->Set_2DDirection(eNewDir);
			if (eNewDir != eOldDir)
			{
			
				switch (eNewDir)
				{
				case Client::F_DIRECTION::LEFT:
				case Client::F_DIRECTION::RIGHT:
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_RIGHT);
					break;
				case Client::F_DIRECTION::UP:
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP);
					break;
				case Client::F_DIRECTION::DOWN:
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_DOWN);
					break;
				case Client::F_DIRECTION::F_DIR_LAST:
				default:
					break;
				}
			}

		}
	}
	else
	{
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}

void CPlayerState_Run::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	
	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eOldDir = m_pOwner->Get_2DDirection();
		switch (eOldDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_DOWN);
			break;
		case Client::F_DIRECTION::F_DIR_LAST:
		default:
			break;
		}
	}
	
	else
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_RUN_01_GT);

}

void CPlayerState_Run::Exit()
{
}
