#include "stdafx.h"
#include "PlayerState_ThrowSword.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"

CPlayerState_ThrowSword::CPlayerState_ThrowSword(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::THROWSWORD)
{
}

void CPlayerState_ThrowSword::Update(_float _fTimeDelta)
{
	if (m_pOwner->Is_SwordHandling())
		return;
	 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();

	if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		m_pOwner->Set_State(CPlayer::RUN);
	else if (tKeyResult.bInputStates[PLAYER_INPUT_JUMP])
		m_pOwner->Set_State(CPlayer::JUMP_UP);
	else if (tKeyResult.bInputStates[PLAYER_KEY_ROLL])
		m_pOwner->Set_State(CPlayer::ROLL);

}

void CPlayerState_ThrowSword::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	switch (eCoord)
	{
	case Engine::COORDINATE_2D:
	{
		F_DIRECTION eDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		switch (eDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_INTO_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_INTO_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_INTO_DOWN);
			break;
		default:
			break;
		}
		break;
	}
	case Engine::COORDINATE_3D:
	    m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SWORDTHROW_THROW_GT);
		break;
	default:
		break;
	}
}

void CPlayerState_ThrowSword::Exit()
{
}

void CPlayerState_ThrowSword::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (COORDINATE_3D == _eCoord)
    {
        if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SWORDTHROW_THROW_GT == iAnimIdx)
        {
            m_pOwner->Set_State(CPlayer::IDLE);
        }
    }
	else
	{
		F_DIRECTION eDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_INTO_RIGHT == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_INTO_UP == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_INTO_DOWN== iAnimIdx)
		{

			switch (eDir)
			{
			case Client::F_DIRECTION::LEFT:
			case Client::F_DIRECTION::RIGHT:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_OUT_RIGHT);
				break;
			case Client::F_DIRECTION::UP:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_OUT_UP);
				break;
			case Client::F_DIRECTION::DOWN:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_OUT_DOWN);
				break;
			default:
				break;
			}
		}
		else if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_OUT_RIGHT == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_OUT_UP == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORDTHROW_OUT_DOWN == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
}
