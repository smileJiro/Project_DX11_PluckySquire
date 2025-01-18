#include "stdafx.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"


CPlayerState_Run::CPlayerState_Run(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::RUN)
{
}

void CPlayerState_Run::Update(_float _fTimeDelta)
{
	COORDINATE eCoord =  m_pOwner->Get_CurCoord();
	_vector vMoveDir = XMVectorZero();
	_bool bMove = false;

	if (KEY_PRESSING(KEY::W))
	{
		if (eCoord == COORDINATE_3D)
			vMoveDir += _vector{ 0.f, 0.f, 1.f,0.f };
		else
			vMoveDir += _vector{ 0.f, 1.f, 0.f,0.f };
		bMove = true;
	}
	if (KEY_PRESSING(KEY::A))
	{
		vMoveDir += _vector{ -1.f, 0.f, 0.f,0.f };
		bMove = true;
	}
	if (KEY_PRESSING(KEY::S))
	{
		if (eCoord == COORDINATE_3D)
			vMoveDir += _vector{ 0.f, 0.f, -1.f,0.f };
		else
			vMoveDir += _vector{ 0.f, -1.f, 0.f,0.f };

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
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}
