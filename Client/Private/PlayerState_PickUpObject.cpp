#include "stdafx.h"
#include "PlayerState_PickUpObject.h"

CPlayerState_PickUpObject::CPlayerState_PickUpObject(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::PICKUPOBJECT)
{
}

void CPlayerState_PickUpObject::Update(_float _fTimeDelta)
{
}

void CPlayerState_PickUpObject::Enter()
{
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_PICKUP_NEWRIG);
	}
	else
	{
		F_DIRECTION eFDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_DOWN);
			break;
		default:
			break;
		}
	}

}

void CPlayerState_PickUpObject::Exit()
{
}

void CPlayerState_PickUpObject::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_3D == _eCoord)
	{
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_3D::LATCH_PICKUP_NEWRIG)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
	else
	{
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_RIGHT
			|| iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_UP
			|| iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_DOWN)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
}
