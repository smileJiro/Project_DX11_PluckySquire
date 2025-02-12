#include "stdafx.h"
#include "PlayerState_Roll.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"


CPlayerState_Roll::CPlayerState_Roll(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ROLL)
{
}

void CPlayerState_Roll::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	_float fProgress = m_pOwner->Get_AnimProgress();
	_float fForwardStartProgress = eCoord == COORDINATE_2D ? m_f2DForwardStartProgress : m_f3DForwardStartProgress;
	_float fForwardEndProgress= eCoord == COORDINATE_2D ? m_f2DForwardEndProgress : m_f3DForwardEndProgress;
	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	m_pOwner->Stop_Rotate();
	if (fProgress >= fMotionCancelProgress)
	{
		 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();

		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		{
			m_pOwner->Set_State(CPlayer::RUN);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_KEY_ATTACK])
		{
			m_pOwner->Set_State(CPlayer::ATTACK);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_KEY_JUMP])
		{
			m_pOwner->Set_State(CPlayer::JUMP_UP);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_KEY_ROLL])
		{
			m_pOwner->Set_State(CPlayer::ROLL);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_KEY_THROWSWORD])
		{
			m_pOwner->Set_State(CPlayer::THROWSWORD);
			return;
		}

	}
	if (fProgress >= fForwardStartProgress)
	{
		if (fProgress <= fForwardEndProgress)
		{
			_float fRatio = (fProgress - fForwardStartProgress) / (fForwardEndProgress - fForwardStartProgress);
			_float fMaxSpeed = eCoord == COORDINATE_2D ? m_f2DForwardSpeedMax : m_f3DForwardSpeedMax;
			_float fMinSpeed = eCoord == COORDINATE_2D ? m_f2DForwardSpeedMin : m_f3DForwardSpeedMin;
			_float fSpeed = fMaxSpeed - (fMaxSpeed - fMinSpeed) * fRatio;
			m_pOwner->Move((m_vDirection)*fSpeed, _fTimeDelta);
		}
	}
	
	
}

void CPlayerState_Roll::Enter()
{

    COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_bool bSwrodEquiped = m_pOwner->Is_SwordHandling();

    if (COORDINATE_2D == eCoord)
    {
		F_DIRECTION eFDir = EDir_To_FDir( m_pOwner->Get_2DDirection());
		m_vDirection = EDir_To_Vector(m_pOwner->Get_2DDirection());
        switch (eFDir)
        {
        case Client::F_DIRECTION::LEFT:
        case Client::F_DIRECTION::RIGHT:
            bSwrodEquiped ?
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_RIGHT_SINGLEROLLONLY)
                : m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_RIGHT_NOSWORD);

            break;
        case Client::F_DIRECTION::UP:
            bSwrodEquiped ?
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_UP_SINGLEROLLONLY)
                : m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_UP_NOSWORD);

            break;
        case Client::F_DIRECTION::DOWN:
            bSwrodEquiped ?
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_DOWN_SINGLEROLLONLY)
                : m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_DOWN_NOSWORD);
            break;
        case Client::F_DIRECTION::F_DIR_LAST:
        default:
            break;
        }
    }
    else
    {
		 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();
		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
			m_vDirection = m_pOwner->Get_3DTargetDirection();
		else
			m_vDirection = m_pOwner->Get_LookDirection();
		m_pOwner->LookDirectionXZ_Dynamic(m_vDirection);
		m_pOwner->Stop_Rotate();
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_DODGE_GT);
    }
}

void CPlayerState_Roll::Exit()
{

}

void CPlayerState_Roll::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pOwner->Set_State(CPlayer::IDLE);
}
