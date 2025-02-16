#include "stdafx.h"
#include "PlayerState_Die.h"

CPlayerState_Die::CPlayerState_Die(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::DIE)
{
}

void CPlayerState_Die::Update(_float _fTimeDelta)
{
}

void CPlayerState_Die::Enter()
{
}

void CPlayerState_Die::Exit()
{
}

void CPlayerState_Die::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}
