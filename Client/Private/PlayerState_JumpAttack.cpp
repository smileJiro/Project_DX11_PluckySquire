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


	if (COORDINATE_2D == eCoord)
	{
	}
	else
	{
		//¶³¾îÁö±â ½ÃÀÛ
		if (m_bRising && 0 > fUpForce)
		{
			m_bRising = false;
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_FALL);
		}

	}
	//LAND ¸ð¼Ç Äµ½½
	_float fProgress = m_pOwner->Get_AnimProgress();

	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	if (m_pOwner->Is_OnGround())
	{
		if (false == m_bGrounded)
		{
			m_bGrounded = true;
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_LAND_GT_EDIT);
			return;
		}
	}
	if (m_bGrounded)
	{
		if (fProgress >= fMotionCancelProgress)
		{
			PLAYER_KEY_RESULT tKeyResult = m_pOwner->Player_KeyInput();

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
	}
}

void CPlayerState_JumpAttack::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCoord)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMPINGATTACK_RISE_DOWN);
	}
	else
	{
		m_pOwner->Set_SwordGrip(false);
		m_pOwner->Add_Impuls(_vector{0,m_fJumpAttackRisingForce ,0});
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_RISE);
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
