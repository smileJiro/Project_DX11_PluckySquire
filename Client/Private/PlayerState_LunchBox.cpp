#include "stdafx.h"
#include "PlayerState_LunchBox.h"

CPlayerState_LunchBox::CPlayerState_LunchBox(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::LUNCHBOX)
{
}


void CPlayerState_LunchBox::Update(_float _fTimeDelta)
{
}

void CPlayerState_LunchBox::Enter()
{
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_01_LOOP_GT);
}

void CPlayerState_LunchBox::Exit()
{
}

void CPlayerState_LunchBox::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE::COORDINATE_3D == _eCoord)
	{
		switch ((_uint)iAnimIdx)
		{
		case (_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_01_LOOP_GT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_01_LOOP_GT);
			break;
		}
	}
}
