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

}

void CPlayerState_EngageBoss::Exit()
{
}

void CPlayerState_EngageBoss::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}
