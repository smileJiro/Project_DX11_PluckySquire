#include "stdafx.h"
#include "PlayerState_Jump.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"


CPlayerState_Jump::CPlayerState_Jump(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMP)
{
}

void CPlayerState_Jump::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_vector vMoveDir = XMVectorZero();
	_bool bMove = false;

	if (m_pOwner->Is_SwordEquiped() && MOUSE_DOWN(MOUSE_KEY::LB))
	{
		m_pOwner->Set_State(CPlayer::ATTACK);
		return;
	}

	if (KEY_PRESSING(KEY::LSHIFT))
	{
		m_pOwner->Set_State(CPlayer::ROLL);
		return;
	}
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

	_float fUpForce = m_pOwner->Get_UpForce();
	//바닥일 때
	if (m_pOwner->Is_OnGround())
	{
		m_pOwner->Stop_Rotate();
		if (bMove)
		{
			m_pOwner->Set_State(CPlayer::RUN);
			return;
		}
		//바닥에 방금 닿음
		if (m_bGrounded == false)
		{
			m_bGrounded = true;
			Switch_JumpAnimation(LAND);
		}
		else
			m_pOwner->Set_State(CPlayer::IDLE);

	}
	//공중일때
	else
	{
		if (bMove)
			m_pOwner->Move(XMVector3Normalize(vMoveDir), _fTimeDelta);
		else
			m_pOwner->Stop_Rotate();
		//Upforce가 0이상일 때, 
		if (fUpForce > 0)
		{

			m_bRising = true;
		}
		//UpForce가 0이하일 때,
		else if (fUpForce <= 0)
		{
			//방금 떨어지기 시작했을 때
			if (m_bRising)
			{
				Switch_JumpAnimation(DOWN);
				m_bRising = false;
			}
		}
	}

}

void CPlayerState_Jump::Enter()
{
	m_pOwner->Jump();

	Switch_JumpAnimation(UP);
}

void CPlayerState_Jump::Exit()
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

void CPlayerState_Jump::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

	if (COORDINATE_2D == _eCoord)
	{

	}
	else
	{
 		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_LAND_02_GT == iAnimIdx)
			m_pOwner->Set_State(CPlayer::IDLE);
	}
}

void CPlayerState_Jump::Switch_JumpAnimation(JUMP_STATE _eJMPSTATE)
{
	F_DIRECTION eDir = EDir_To_FDir( m_pOwner->Get_2DDirection());
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_bool bSword = m_pOwner->Is_SwordEquiped();
	_bool bCarrying = m_pOwner->Is_CarryingObject();
	switch (_eJMPSTATE)
	{
	case Client::CPlayerState_Jump::UP:
		if (COORDINATE_2D == eCoord)
		{
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
		break;
	case Client::CPlayerState_Jump::DOWN:
		if (COORDINATE_2D == eCoord)
		{
			switch (eDir)
			{
			case Client::F_DIRECTION::LEFT:
			case Client::F_DIRECTION::RIGHT:
				if (bCarrying)
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_OBJECT_RIGHT);
				else if (bSword)
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_SWORD_RIGHT);
				else
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_RIGHT);
				break;
			case Client::F_DIRECTION::UP:
				if (bCarrying)
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_OBJECT_UP);
				else if (bSword)
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_SWORD_UP);
				else
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_UP);
				break;
			case Client::F_DIRECTION::DOWN:
				if (bCarrying)
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_OBJECT_DOWN);
				else if (bSword)
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_SWORD_DOWN);
				else
					m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_DOWN);
				break;
			}
		}
		else	if (COORDINATE_3D == eCoord)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_DOWN_02_GT);
		break;
	case Client::CPlayerState_Jump::LAND:
		if (COORDINATE_3D == eCoord)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_LAND_02_GT);
		break;
	case Client::CPlayerState_Jump::LAST:
		break;
	default:
		break;
	}


}
