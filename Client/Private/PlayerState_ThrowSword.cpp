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
	if (m_pOwner->Is_SwordHandling())
		return;
	 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();

	if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		m_pOwner->Set_State(CPlayer::RUN);
	else if (tKeyResult.bInputStates[PLAYER_KEY_JUMP])
		m_pOwner->Set_State(CPlayer::JUMP_UP);
	else if (tKeyResult.bInputStates[PLAYER_KEY_ROLL])
		m_pOwner->Set_State(CPlayer::ROLL);

}

void CPlayerState_ThrowSword::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	switch (eCoord)
	{
	case Engine::COORDINATE_2D:
		break;
	case Engine::COORDINATE_3D:
	    m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SWORDTHROW_THROW_GT);
		break;
	default:
		break;
	}
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
