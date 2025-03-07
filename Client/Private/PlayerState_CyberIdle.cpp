#include "stdafx.h"
#include "PlayerState_CyberIdle.h"
#include "GameInstance.h"



CPlayerState_CyberIdle::CPlayerState_CyberIdle(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CYBER_IDLE)
{
}

void CPlayerState_CyberIdle::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();

	if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
	{
		m_pOwner->Set_State(CPlayer::CYBER_FLY);
		return;
	}
	//기본공격
	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		if (false == m_bRifleTriggered)
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_SHOOT);
		}
	}
	else if(m_bRifleTriggered)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_IDLE);
	}

	

	m_bRifleTriggered = tKeyResult.bInputStates[PLAYER_INPUT_ATTACK];
	if (m_bRifleTriggered)
	{
		m_pOwner->Shoot_Rifle();
	}
}

void CPlayerState_CyberIdle::Enter()
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();
	if (false == m_bRifleTriggered && tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_SHOOT);
	}
	else if (m_bRifleTriggered && false == tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_IDLE);
	}

}

void CPlayerState_CyberIdle::Exit()
{
}

void CPlayerState_CyberIdle::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_SHOOT == iAnimIdx)
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_IDLE);
}

//========================FLY===========================================================================


CPlayerState_CyberFly::CPlayerState_CyberFly(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CYBER_FLY)
{
}

void CPlayerState_CyberFly::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();


	if (false == tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
	{
		m_pOwner->Set_State(CPlayer::CYBER_IDLE);
		return;
	}

	m_pOwner->Move(tKeyResult.vMoveDir * m_f3DCyberFlySpeed, _fTimeDelta);
	F_DIRECTION eNewFDir = To_FDirection(tKeyResult.vMoveDir);
	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		m_pOwner->Shoot_Rifle();
	}
	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		if (false == m_bRifleTriggered)
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_SHOOT);
		}
	}
	else if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
	{
		m_pOwner->Set_State(CPlayer::CYBER_DASH);
	}
	else  if (m_bRifleTriggered /*|| (m_eFDir != eNewFDir)*/)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_IDLE);
		m_eFDir = eNewFDir;

		//switch (m_eFDir)
		//{
		//case Client::F_DIRECTION::LEFT:
		//	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_LEFT);
		//	break;
		//case Client::F_DIRECTION::RIGHT:
		//	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_RIGHT);
		//	break;
		//case Client::F_DIRECTION::UP:
		//	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_UP);
		//	break;
		//case Client::F_DIRECTION::DOWN:
		//	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_DOWN);
		//	break;
		//default:
		//	return;
		//}
	}


	m_bRifleTriggered = tKeyResult.bInputStates[PLAYER_INPUT_ATTACK];
	return;


}

void CPlayerState_CyberFly::Enter()
{

}

void CPlayerState_CyberFly::Exit()
{

}

void CPlayerState_CyberFly::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}

//=============================================DASH==================================================
CPlayerState_CyberDash::CPlayerState_CyberDash(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CYBER_DASH)
{
}

void CPlayerState_CyberDash::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();

	m_pOwner->Move(tKeyResult.vMoveDir * m_f3DCyberDashSpeed, _fTimeDelta);
	F_DIRECTION eNewFDir = To_FDirection(tKeyResult.vMoveDir);
	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		m_pOwner->Shoot_Rifle();
	}
	m_pOwner->Move(m_vDashDirection* m_f3DCyberDashSpeed,_fTimeDelta);
}

void CPlayerState_CyberDash::Enter()
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();
	m_vDashDirection = tKeyResult.vMoveDir;
	F_DIRECTION eFDIr = To_FDirection(m_vDashDirection);
	switch (eFDIr)
	{
	case Client::F_DIRECTION::LEFT:
		m_pOwner->Set_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_LEFT_DASH);
		break;
	case Client::F_DIRECTION::RIGHT:
		m_pOwner->Set_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_RIGHT_DASH);
		break;
	case Client::F_DIRECTION::UP:
		m_pOwner->Set_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_UP_DASH);
		break;
	case Client::F_DIRECTION::DOWN:
		m_pOwner->Set_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_DOWN_DASH);
		break;
	default:
		break;
	}

}

void CPlayerState_CyberDash::Exit()
{
}

void CPlayerState_CyberDash::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pOwner->Set_State(CPlayer::CYBER_IDLE);
}
