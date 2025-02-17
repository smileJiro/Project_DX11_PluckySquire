#include "stdafx.h"
#include "PlayerState_LaydownObject.h"

CPlayerState_LaydownObject::CPlayerState_LaydownObject(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::LAYDOWNOBJECT)
{
}

void CPlayerState_LaydownObject::Update(_float _fTimeDelta)
{
}

void CPlayerState_LaydownObject::Enter()
{
}

void CPlayerState_LaydownObject::Exit()
{
}

void CPlayerState_LaydownObject::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}

void CPlayerState_LaydownObject::Align()
{
}
