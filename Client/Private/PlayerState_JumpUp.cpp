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

	 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();
	 
	if (tKeyResult.bInputStates[PLAYER_INPUT_INTERACT])
	{
		m_pOwner->Try_Interact(m_pOwner->Get_InteractableObject(), _fTimeDelta);
		return;
	}
	else
	{
		m_pOwner->End_Interact();
		if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
		{
			m_pOwner->Set_State(CPlayer::ROLL);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_THROWSWORD])
		{
			m_pOwner->Set_State(CPlayer::THROWSWORD);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
		{
			if (m_pOwner->Is_PlatformerMode())
				m_pOwner->Set_State(CPlayer::ATTACK);
			else
				m_pOwner->Set_State(CPlayer::JUMP_ATTACK);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_THROWOBJECT])
		{
			m_pOwner->Set_State(CPlayer::THROWOBJECT);
			return;
		}
	}
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_MOVE])
		{
			m_pOwner->Add_Force(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed);
			m_pOwner->Rotate_To(tKeyResult.vMoveDir, m_fAirRotateSpeed);
		}
		else
			m_pOwner->Stop_Rotate();
	}
	else
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_MOVE])
		{
			E_DIRECTION eNewDir = To_EDirection(tKeyResult.vMoveDir);
			F_DIRECTION eFDir = EDir_To_FDir(eNewDir);

			if (m_eOldFDir != eFDir)
			{
				m_pOwner->Set_2DDirection(To_EDirection(tKeyResult.vDir));
				m_eOldFDir = eFDir;
			}
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed2D, _fTimeDelta);
		}
	}
}

void CPlayerState_JumpUp::Enter()
{
	m_fAirRunSpeed = m_pOwner->Get_AirRunSpeed();
	m_bPlatformerMode = m_pOwner->Is_PlatformerMode();
	m_fAirRotateSpeed = m_pOwner->Get_AirRotationSpeed();
	m_fAirRunSpeed2D = m_pOwner->Get_AirRunSpeed2D();
	m_pOwner->LookDirectionXZ_Dynamic(m_pOwner->Get_3DTargetDirection());
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
	_bool bSword = m_pOwner->Is_SwordHandling();
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
