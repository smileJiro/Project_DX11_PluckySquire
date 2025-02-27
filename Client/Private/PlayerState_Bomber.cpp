#include "stdafx.h"
#include "PlayerState_Bomber.h"
#include "Detonator.h"

CPlayerState_Bomber::CPlayerState_Bomber(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::BOMBER)
{
}

void CPlayerState_Bomber::Update(_float _fTimeDelta)
{
}

void CPlayerState_Bomber::Enter()
{
	m_pDetonator = static_cast<CDetonator*>( m_pOwner->Get_PartObject(CPlayer::PLAYER_PART_DETONATOR));
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_REMOTE_PUSH_GT);
}

void CPlayerState_Bomber::Exit()
{
	m_pOwner->UnEquip_Part(CPlayer::PLAYER_PART_DETONATOR);
}

void CPlayerState_Bomber::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pOwner->Set_State(CPlayer::IDLE);
}
