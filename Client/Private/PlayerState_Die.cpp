#include "stdafx.h"
#include "PlayerState_Die.h"
#include "Player.h"

CPlayerState_Die::CPlayerState_Die(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::DIE)
{
}

void CPlayerState_Die::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	if (tKeyResult.bInputStates[PLAYER_INPUT_REVIVE])
	{
		m_pOwner->Revive();
	}
}

void CPlayerState_Die::Enter()
{
	COORDINATE eCurCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCurCoord)
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DEATH_DOWN);
	else
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_DIE_GT);
}

void CPlayerState_Die::Exit()
{
}

void CPlayerState_Die::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}
