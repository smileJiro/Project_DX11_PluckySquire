#include "stdafx.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"
#include "PlayerState_Run.h"


CPlayerState_Idle::CPlayerState_Idle(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::IDLE)
{
}

void CPlayerState_Idle::Update(_float _fTimeDelta)
{
	_bool bMove = false;
	_bool bJump = false;
	/* Test Move Code */
	if (KEY_PRESSING(KEY::SPACE))
	{
		bJump = true;
	}
	else if (KEY_PRESSING(KEY::W))
	{
		bMove = true;
	}
	else if (KEY_PRESSING(KEY::A))
	{
		bMove = true;
	}
	else if (KEY_PRESSING(KEY::S))
	{
		bMove = true;
	}
	else if (KEY_PRESSING(KEY::D))
	{
		bMove = true;
	} 
	if (bJump)
	{
		m_pOwner->Set_State(CPlayer::JUMP);
	}
	else if (bMove)
	{
		m_pOwner->Set_State(CPlayer::RUN);
	}
}
