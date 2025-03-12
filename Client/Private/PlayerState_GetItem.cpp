#include "stdafx.h"
#include "PlayerState_GetItem.h"

CPlayerState_GetItem::CPlayerState_GetItem(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::GET_ITEM)
{
}

void CPlayerState_GetItem::Update(_float _fTimeDelta)
{
}

void CPlayerState_GetItem::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	switch (eCoord)
	{
	case Engine::COORDINATE_2D:
		m_pOwner->Set_2DDirection(E_DIRECTION::LEFT);
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ITEM_RETRIEVE);
		break;
	case Engine::COORDINATE_3D:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ITEM_GET_NEWRIG);
		break;

	default:
		break;
	}
}

void CPlayerState_GetItem::Exit()
{

}

void CPlayerState_GetItem::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	switch (_eCoord)
	{
	case Engine::COORDINATE_2D:
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ITEM_RETRIEVE)
		{
			m_pOwner->Set_2DDirection(E_DIRECTION::RIGHT);
			m_pOwner->Set_State(CPlayer::IDLE);
		}
		break;
	default:
		break;
	}
}
