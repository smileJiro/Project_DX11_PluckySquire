#include "stdafx.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"


CPlayerState_Run::CPlayerState_Run(CPlayer* _pOwner, CStateMachine* _pContext)
	:CPlayerState(_pOwner, _pContext)
{
}

void CPlayerState_Run::Update(_float _fTimeDelta)
{
	_vector vMoveDir = XMVectorZero();
	_bool bMove = false;
	/* Test Move Code */
	if (KEY_PRESSING(KEY::W))
	{
		vMoveDir += _vector{ 0.f, 0.f, 1.f,0.f };
		bMove = true;
	}
	if (KEY_PRESSING(KEY::A))
	{
		vMoveDir += _vector{ -1.f, 0.f, 0.f,0.f };
		bMove = true;
	}
	if (KEY_PRESSING(KEY::S))
	{
		vMoveDir += _vector{ 0.f, 0.f, -1.f,0.f };
		bMove = true;
	}
	if (KEY_PRESSING(KEY::D))
	{
		vMoveDir += _vector{ 1.f, 0.f, 0.f,0.f };
		bMove = true;
	}
	if (bMove)
	{
		m_pOwner->Move(vMoveDir, _fTimeDelta);
	}
	else
	{
		m_pOwner->Switch_Animation(CPlayer::LATCH_ANIM_IDLE_01_GT);
		m_pStateMachine->Transition_To(new CPlayerState_Idle(m_pOwner, m_pStateMachine));
	}
}
