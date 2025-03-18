#include "stdafx.h"
#include "PlayerState_EngageBoss.h"

CPlayerState_EngageBoss::CPlayerState_EngageBoss(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ENGAGE_BOSS)
{
}

void CPlayerState_EngageBoss::Update(_float _fTimeDelta)
{
}

void CPlayerState_EngageBoss::Enter()
{
	m_pOwner->Set_Kinematic(true);
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_CINE_DO9_LB_ENGAGE_SH01);
}

void CPlayerState_EngageBoss::Exit()
{
}

void CPlayerState_EngageBoss::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if ((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_CINE_DO9_LB_ENGAGE_SH01 == iAnimIdx)
	{

		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_CINE_DO9_LB_ENGAGE_SH02);
	}
}
