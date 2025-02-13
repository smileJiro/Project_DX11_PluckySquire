#include "stdafx.h"
#include "PlayerState_JumpAttack.h"
#include "Player.h"

CPlayerState_JumpAttack::CPlayerState_JumpAttack(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMP_ATTACK)
{
}

void CPlayerState_JumpAttack::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_float fUpForce = m_pOwner->Get_UpForce();
	F_DIRECTION eDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	_bool bGround = m_pOwner->Is_OnGround();
	//¶³¾îÁö±â ½ÃÀÛ
	if (m_bRising && 0 > fUpForce)
	{
		if (COORDINATE_2D == eCoord)
		{
			switch (eDir)
			{
			case Client::F_DIRECTION::LEFT:
			case Client::F_DIRECTION::RIGHT:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_FALL_RIGHT);
				break;
			case Client::F_DIRECTION::UP:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_FALL_UP);
				break;
			case Client::F_DIRECTION::DOWN:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_FALL_DOWN);
				break;
			}
		}
		else
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_FALL);
		}
		m_bRising = false;
	}
	//LAND ¸ð¼Ç Äµ½½
	_float fProgress = m_pOwner->Get_AnimProgress();

	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	_float fFloorDistance = eCoord == COORDINATE_2D ?m_pOwner->Get_2DFloorDistance() :m_pOwner->Get_3DFloorDistance();
	_float fLandAnimHeightThreshold = eCoord == COORDINATE_2D ? m_f2DLandAnimHeightThreshold : m_f3DLandAnimHeightThreshold;
	if (COORDINATE_3D == eCoord)
	{
		if (0 > fUpForce && fFloorDistance >= 0 && fFloorDistance <= fLandAnimHeightThreshold)
		{
			if (false == m_fLandAnimed)
			{
				m_fLandAnimed = true;
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_LAND_GT_EDIT);
				return;
			}
		}
	}
	else
	{
		if (bGround)
		{
			if (false == m_fLandAnimed)
			{
				m_fLandAnimed = true;
				switch (eDir)
				{
				case Client::F_DIRECTION::LEFT:
				case Client::F_DIRECTION::RIGHT:
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_OUT_RIGHT);
					break;
				case Client::F_DIRECTION::UP:
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_OUT_UP);
					break;
				case Client::F_DIRECTION::DOWN:
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_OUT_DOWN);
					break;
				}
			}
		}
	}
	if (bGround)
	{
		if (COORDINATE_3D == eCoord && fProgress >= fMotionCancelProgress)
		{
			if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
				m_pOwner->Set_State(CPlayer::RUN);
			else if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
				m_pOwner->Set_State(CPlayer::ATTACK);
			else if (tKeyResult.bInputStates[PLAYER_INPUT_JUMP])
				m_pOwner->Set_State(CPlayer::JUMP_UP);
			else if (tKeyResult.bInputStates[PLAYER_KEY_ROLL])
				m_pOwner->Set_State(CPlayer::ROLL);
			else if (tKeyResult.bInputStates[PLAYER_KEY_THROWSWORD])
				m_pOwner->Set_State(CPlayer::THROWSWORD);
		}
	}
	else
	{
		if (COORDINATE_2D == eCoord)
		{
			if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
			{
				m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed2D, _fTimeDelta);

				E_DIRECTION eNewDir = To_EDirection(tKeyResult.vMoveDir);
				F_DIRECTION eFDir = EDir_To_FDir(eNewDir);
				if (m_eOldFDir != eFDir)
				{
					m_pOwner->Set_2DDirection(eNewDir);
					m_eOldFDir = eFDir;
				}
			}
		}
	}
}

void CPlayerState_JumpAttack::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCoord)
	{
		m_fAirRunSpeed2D = m_pOwner->Get_AirRunSpeed2D();
		F_DIRECTION eDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		switch (eDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_RISE_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_RISE_UP);
			break;
		case Client::F_DIRECTION::DOWN:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_RISE_DOWN);
			break;
		}
	}
	else
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_RISE);
		m_pOwner->Set_SwordGrip(false);
		m_pOwner->Add_Impuls(_vector{0,m_fJumpAttackRisingForce ,0});
	}
	
}

void CPlayerState_JumpAttack::Exit()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCoord)
	{
	}
	else
	{
		m_pOwner->Set_SwordGrip(true);
	}
}

void CPlayerState_JumpAttack::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
		if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_OUT_RIGHT == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_OUT_DOWN== iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_OUT_UP == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
			return;
		}
	}
	else
	{
		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_LAND_GT_EDIT == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
			return;
		}
	}
}
