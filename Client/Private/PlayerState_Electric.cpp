#include "stdafx.h"
#include "PlayerState_Electric.h"

CPlayerState_Electric::CPlayerState_Electric(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ELECTRIC)
{
}

void CPlayerState_Electric::Update(_float _fTimeDelta)
{
}

void CPlayerState_Electric::Enter()
{
	assert(COORDINATE_2D == m_pOwner->Get_CurCoord());
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ELECTROCUTE);
}

void CPlayerState_Electric::Exit()
{
}

void CPlayerState_Electric::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{
		switch (CPlayer::ANIM_STATE_2D(iAnimIdx))
		{
		case CPlayer::ANIM_STATE_2D::PLAYER_ELECTROCUTE:
			m_pOwner->Set_State(CPlayer::IDLE);
			break;
		}
	}
}
