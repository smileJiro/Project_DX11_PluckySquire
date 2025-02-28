#include "stdafx.h"
#include "PlayerState_ErasePalmDecal.h"

CPlayerState_ErasePalmDecal::CPlayerState_ErasePalmDecal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ERASE_PALM)
{
}

void CPlayerState_ErasePalmDecal::Update(_float _fTimeDelta)
{
}

void CPlayerState_ErasePalmDecal::Enter()
{
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STAMP_ERASE_GT);
}

void CPlayerState_ErasePalmDecal::Exit()
{
}

void CPlayerState_ErasePalmDecal::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_3D == _eCoord && (_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STAMP_ERASE_GT)
		m_pOwner->Set_State(CPlayer::IDLE);
}
