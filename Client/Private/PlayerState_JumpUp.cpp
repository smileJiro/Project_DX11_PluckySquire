#include "stdafx.h"
#include "PlayerState_JumpUp.h"
#include "PlayerState_JumpDown.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"


CPlayerState_JumpUp::CPlayerState_JumpUp(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMP_UP)
{
}

void CPlayerState_JumpUp::Update(_float _fTimeDelta)
{
	_float fUpForce = m_pOwner->Get_UpForce();
	if (0 > fUpForce)
	{
		m_pOwner->Set_State(CPlayer::JUMP_DOWN);
		return;
	}

	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	 
	if (tKeyResult.bKeyStates[PLAYER_KEY_ROLL])
	{
		m_pOwner->Set_State(CPlayer::ROLL);
		return;
	}
	else if (tKeyResult.bKeyStates[PLAYER_KEY_THROWSWORD])
	{
		m_pOwner->Set_State(CPlayer::THROWSWORD);
		return;
	}
	else if (tKeyResult.bKeyStates[PLAYER_KEY_ATTACK])
	{
		m_pOwner->Set_State(CPlayer::JUMP_ATTACK);
		return;
	}
	if (tKeyResult.bKeyStates[PLAYER_INPUT::PLAYER_KEY_MOVE])
	{
		m_pOwner->Add_Force(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed);
		m_pOwner->Rotate_To(tKeyResult.vMoveDir, m_fAirRotateSpeed);
	}
	else
		m_pOwner->Stop_Rotate();
}

void CPlayerState_JumpUp::Enter()
{
	m_pOwner->Jump();
	Switch_JumpAnimation();
}

void CPlayerState_JumpUp::Exit()
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

void CPlayerState_JumpUp::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

} 

void CPlayerState_JumpUp::Switch_JumpAnimation()
{

	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_bool bSword = m_pOwner->Is_SwordEquiped();
	_bool bCarrying = m_pOwner->Is_CarryingObject();

	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		switch (eDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_OBJECT_RIGHT);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_SWORD_RIGHT);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_OBJECT_UP);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_SWORD_UP);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_OBJECT_DOWN);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_SWORD_DOWN);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_DOWN);
			break;
		}
	}
	else	if (COORDINATE_3D == eCoord)
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_UP_02_GT);

}
