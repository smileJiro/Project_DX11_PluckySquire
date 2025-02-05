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
	_float fUpForce = m_pOwner->Get_UpForce();
	_bool bOnGround = m_pOwner->Is_OnGround();
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_3D == eCoord)
	{
		_float fUpForce = m_pOwner->Get_UpForce();
		_bool bOnGround = m_pOwner->Is_OnGround();
		if (false == bOnGround && 0 > fUpForce)
		{
			m_pOwner->Set_State(CPlayer::JUMP_DOWN);
			return;
		}
	}

	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	_bool bSneak = tKeyResult.bKeyStates[PLAYER_KEY_SNEAK];
	if (tKeyResult.bKeyStates[PLAYER_KEY_MOVE])
	{
		_float fMoveSpeed = bSneak ? m_fSneakSpeed : m_fSpeed;
		m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir)* fMoveSpeed, _fTimeDelta);

		COORDINATE eCoord = m_pOwner->Get_CurCoord();
		if (COORDINATE_2D == eCoord)
		{
			E_DIRECTION eNewDir = To_EDirection(tKeyResult.vMoveDir);
			F_DIRECTION eFDir = EDir_To_FDir(eNewDir);
			m_pOwner->Set_2DDirection(eNewDir);
			Switch_RunAnimation2D(eFDir);
		}
		else
		{
			m_pOwner->Rotate_To(XMVector3Normalize(tKeyResult.vMoveDir), m_fRotateSpeed);
			if (bSneak != m_bSneakBefore)
			{
				Switch_RunAnimation3D(bSneak);
				m_bSneakBefore = bSneak;
			}
		}

		if (tKeyResult.bKeyStates[PLAYER_KEY_ATTACK])
			m_pOwner->Set_State(CPlayer::ATTACK);
		else if (tKeyResult.bKeyStates[PLAYER_KEY_JUMP])
			m_pOwner->Set_State(CPlayer::JUMP_UP);
		else if (tKeyResult.bKeyStates[PLAYER_KEY_ROLL])
			m_pOwner->Set_State(CPlayer::ROLL);
		else if (tKeyResult.bKeyStates[PLAYER_KEY_THROWSWORD])
			m_pOwner->Set_State(CPlayer::THROWSWORD);
		return;
	}
	else
	{
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}
}

void CPlayerState_Run::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eFDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		Switch_RunAnimation2D(eFDir);
	}
	else
	{
		PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
		Switch_RunAnimation3D(tKeyResult.bKeyStates[PLAYER_KEY_SNEAK]);
	}

}

void CPlayerState_Run::Exit()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
	}
	else
	{
		m_pOwner->Stop_Rotate();
	}

}

void CPlayerState_Run::Switch_RunAnimation2D(F_DIRECTION _eFDir)
{
	_bool bSword = m_pOwner->Is_SwordEquiped();
	_bool bCarrying = m_pOwner->Is_CarryingObject();
	switch (_eFDir)
	{
	case Client::F_DIRECTION::LEFT:
	case Client::F_DIRECTION::RIGHT:
		if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_OBJECT_RIGHT);
		else if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_RIGHT);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_RIGHT);
		break;
	case Client::F_DIRECTION::UP:
		if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_OBJECT_UP);
		else if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_UP);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP);
		break;
	case Client::F_DIRECTION::DOWN:
		if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_OBJECT_DOWN);
		else if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_DOWN);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_DOWN);
		break;
	case Client::F_DIRECTION::F_DIR_LAST:
	default:
		break;
	}
}

void CPlayerState_Run::Switch_RunAnimation3D(_bool _bStealth)
{
	if (_bStealth)
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STEALTH_RUN_GT);
	else
	{
		_bool bSword = m_pOwner->Is_SwordEquiped();
		_bool bCarrying = m_pOwner->Is_CarryingObject();
		if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_RUN_SWORD_01_GT);
		else if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_RUN_01_GT);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_RUN_01_GT);
	}
}

