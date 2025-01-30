#include "stdafx.h"
#include "PlayerState_ThrowSword.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"

CPlayerState_ThrowSword::CPlayerState_ThrowSword(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::THROWSWORD)
{
}

void CPlayerState_ThrowSword::Update(_float _fTimeDelta)
{
    if(MOUSE_UP(MOUSE_KEY::RB))
    {
        m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SWORDTHROW_THROW_NEWRIG);

 
        return;
    }
}

void CPlayerState_ThrowSword::Enter()
{
    m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SWORDTHROW_THROW_GT);
}

void CPlayerState_ThrowSword::Exit()
{
}

void CPlayerState_ThrowSword::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (COORDINATE_3D == _eCoord)
    {
        if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SWORDTHROW_THROW_GT == iAnimIdx)
        {
            m_pOwner->Set_State(CPlayer::IDLE);
        }
    }
}
