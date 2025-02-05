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
		//떨어지기 시작
		if (m_bRising && 0 > fUpForce)
		{
			m_bRising = false;
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_FALL);
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
		m_pOwner->Add_Impuls(_vector{0,m_fJumpAttackRisingForce ,0});
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_RISE);
	}
	
}

void CPlayerState_JumpAttack::Exit()
{
}

void CPlayerState_JumpAttack::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
	}
	else
	{
		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_FALL == iAnimIdx)
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_LAND_GT_EDIT);
		}
		else if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMPATTACK_LAND_GT_EDIT == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
			return;
		}
	}
}
