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
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	if (COORDINATE_3D == eCoord)
	{
		_float fUpForce = m_pOwner->Get_UpForce();
		_bool bOnGround = m_pOwner->Is_OnGround();
		if (false == bOnGround && -0.001 > fUpForce)
		{
			m_pOwner->Set_State(CPlayer::JUMP_DOWN);
			return;
		}


		_bool bSneak = tKeyResult.bKeyStates[PLAYER_KEY_SNEAK];
		if (bSneak != m_bSneakBefore)
		{
			Switch_IdleAnimation3D(bSneak);
			m_bSneakBefore = bSneak;
		}
		
	}


	if (tKeyResult.bKeyStates[PLAYER_KEY_MOVE])
		m_pOwner->Set_State(CPlayer::RUN);
	else if (tKeyResult.bKeyStates[PLAYER_KEY_ATTACK])
		m_pOwner->Set_State(CPlayer::ATTACK);
	else if (tKeyResult.bKeyStates[PLAYER_KEY_JUMP])
		m_pOwner->Set_State(CPlayer::JUMP_UP);
	else if (tKeyResult.bKeyStates[PLAYER_KEY_ROLL])
		m_pOwner->Set_State(CPlayer::ROLL);
	else if (tKeyResult.bKeyStates[PLAYER_KEY_THROWSWORD])
		m_pOwner->Set_State(CPlayer::THROWSWORD);

}

void CPlayerState_Idle::Enter()
{

	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eFDir = EDir_To_FDir( m_pOwner->Get_2DDirection());
		Switch_IdleAnimation2D(eFDir);
	}
	else
	{
		m_pOwner->Stop_Move();
		PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
		Switch_IdleAnimation3D(tKeyResult.bKeyStates[PLAYER_KEY_SNEAK]);

	}



}

void CPlayerState_Idle::Exit()
{
}

void CPlayerState_Idle::Switch_IdleAnimation2D(F_DIRECTION _eFDir)
{
	switch (_eFDir)
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

void CPlayerState_Idle::Switch_IdleAnimation3D(_bool _bStealth)
{
	if (_bStealth)
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_STEALTH_IDLE_GT);
	else
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_01_GT);
}
