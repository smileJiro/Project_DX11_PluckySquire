#include "stdafx.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"
#include "PlayerState_Run.h"

CPlayerState_Idle::CPlayerState_Idle(CPlayer* _pOwner, CStateMachine* _pContext)
	:CPlayerState(_pOwner, _pContext)
{
}

void CPlayerState_Idle::Update(_float _fTimeDelta)
{
	_bool bMove = false;
	/* Test Move Code */
	if (KEY_PRESSING(KEY::W))
	{
		bMove = true;
	}
	if (KEY_PRESSING(KEY::A))
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
	if (bMove)
	{
		m_pOwner->Switch_Animation(CPlayer::LATCH_ANIM_RUN_01_GT);
		m_pStateMachine->Transition_To(new CPlayerState_Run(m_pOwner, m_pStateMachine));
	}
}
