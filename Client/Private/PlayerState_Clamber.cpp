#include "stdafx.h"
#include "PlayerState_Clamber.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"
#include "PlayerState_Run.h"

CPlayerState_Clamber::CPlayerState_Clamber(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CLAMBER)
{
}

void CPlayerState_Clamber::Update(_float _fTimeDelta)
{
}

void CPlayerState_Clamber::Enter()
{

	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_2D == eCoord)
	{
	}
	else
	{
		m_pOwner->Stop_Move();
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_CLAMBER_01_EDIT_NEWRIG);
	}
}

void CPlayerState_Clamber::Exit()
{
}
