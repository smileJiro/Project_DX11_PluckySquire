#include "stdafx.h"
#include "PlayerState_BackRoll.h"

CPlayerState_BackRoll::CPlayerState_BackRoll(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ELECTRIC)
{
}

void CPlayerState_BackRoll::Update(_float _fTimeDelta)
{
	if (true == m_isRollLoop)
	{
		m_vRollTime.y += _fTimeDelta;
		m_pOwner->Move(XMVectorSet(0.0f, -100.f, 0.0f, 0.0f), _fTimeDelta);
		if (m_vRollTime.x <= m_vRollTime.y)
		{
			m_vRollTime.y = 0.0f;
			m_isRollLoop = false;
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ROLL_OUT);
		}
	}

}

void CPlayerState_BackRoll::Enter()
{
	assert(COORDINATE_2D == m_pOwner->Get_CurCoord());
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ROLL_INTO);
	m_vRollTime.y = 0.0f;
	m_isRollLoop = false;
}

void CPlayerState_BackRoll::Exit()
{
}

void CPlayerState_BackRoll::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{
		switch (CPlayer::ANIM_STATE_2D(iAnimIdx))
		{
		case CPlayer::ANIM_STATE_2D::PLAYER_ROLL_INTO:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ROLL_LOOP);
			m_vRollTime.y = 0.0f;
			m_isRollLoop = true;
			break;
		case CPlayer::ANIM_STATE_2D::PLAYER_ROLL_OUT:
			m_pOwner->Set_State(CPlayer::IDLE);
			break;
		}
	}
}
