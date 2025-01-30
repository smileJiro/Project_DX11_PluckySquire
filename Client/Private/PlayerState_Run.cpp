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

	if (m_pOwner->Is_SwordEquiped() && MOUSE_DOWN(MOUSE_KEY::LB))
	{
		m_pOwner->Set_State(CPlayer::ATTACK);
		return;
	}
	if (m_pOwner->Is_SwordEquiped() && MOUSE_DOWN(MOUSE_KEY::RB))
	{
		m_pOwner->Set_State(CPlayer::THROWSWORD);
		
		return;
	}

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
		m_pOwner->Move(XMVector3Normalize(vMoveDir), _fTimeDelta);
		if (KEY_PRESSING(KEY::SPACE))
		{
			m_pOwner->Set_State(CPlayer::JUMP);
			return;
		}
		if (KEY_PRESSING(KEY::LSHIFT))
		{
			m_pOwner->Set_State(CPlayer::ROLL);
			return;
		}

		if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			E_DIRECTION eNewDir =  To_EDirection(vMoveDir);
			F_DIRECTION eFDir = EDir_To_FDir(eNewDir);
			m_pOwner->Set_2DDirection(eNewDir);
			Switch_RunAnimation2D(eFDir);
		}
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
	m_pOwner->Stop_Move();
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

