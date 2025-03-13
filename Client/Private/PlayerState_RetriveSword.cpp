#include "stdafx.h"
#include "PlayerState_RetriveSword.h"

CPlayerState_RetriveSword::CPlayerState_RetriveSword(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::RETRIVE_SWORD)
{
}

void CPlayerState_RetriveSword::Update(_float _fTimeDelta)
{
}

void CPlayerState_RetriveSword::Enter()
{
	m_pOwner->Set_Mode(CPlayer::PLAYER_MODE_SWORD);
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORD_RETRIEVE);
	m_pOwner->Set_2DDirection(F_DIRECTION::DOWN);
}

void CPlayerState_RetriveSword::Exit()
{
}

void CPlayerState_RetriveSword::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_SWORD_RETRIEVE)
	{
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}