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

		if (m_pOwner->Is_OnGround())
		{
			m_pOwner->Set_State(CPlayer::RUN);
			return;
		}
	}
	else
	{
		m_pOwner->Stop_Rotate();
		if (m_pOwner->Is_OnGround())
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_DOWN_02_GT);
			m_pOwner->Set_State(CPlayer::IDLE);
			return;
		}
	}
	_float fNewUpForce = m_pOwner->Get_UpForce();
	if (m_fUpForceBefore >= 0 && fNewUpForce < 0)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_DOWN_02_GT);
	}
	m_fUpForceBefore = fNewUpForce;
}

void CPlayerState_Jump::Enter()
{
	if (COORDINATE_2D == m_pOwner->Get_CurCoord())
	{
	}
	else
	{
		m_pOwner->Jump();
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_UP_02);
	}

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
		if((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_DOWN_02_GT ==  iAnimIdx)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_LAND_02);
		else if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_LAND_02 == iAnimIdx)
			m_pOwner->Set_State(CPlayer::IDLE);
	}
}
