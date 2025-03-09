#include "stdafx.h"
#include "PlayerState_CyberIdle.h"
#include "GameInstance.h"
#include "Actor_Dynamic.h"
#include "PlayerBody.h"

CPlayerState_CyberIdle::CPlayerState_CyberIdle(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CYBER_IDLE)
{
}


void CPlayerState_CyberIdle::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();

	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		m_pOwner->Shoot_Rifle();
	}
	if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
	{

		F_DIRECTION eFDirection = To_FDirection(tKeyResult.vMoveDir);


		if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
		{
			m_pOwner->Set_State(CPlayer::CYBER_DASH);
			return;
		}
		else
		{

			m_pDynamicActor->Set_LinearVelocity(tKeyResult.vMoveDir * m_f3DCyberFlySpeed);
		}

	}
	_vector vVelocity = m_pDynamicActor->Get_LinearVelocity();

	//cout << "Velocity : " << vVelocity << endl;
	Set_VeloState(vVelocity);

	
}

void CPlayerState_CyberIdle::Enter()
{
	m_pDynamicActor = m_pOwner->Get_ActorDynamic();
	//PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();
	//if (false == m_bRifleTriggered && tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	//{
	//	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_SHOOT);
	//}
	//else if (m_bRifleTriggered && false == tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	//{
	//	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_IDLE);
	//}

}

void CPlayerState_CyberIdle::Exit()
{
}

void CPlayerState_CyberIdle::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if ((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_RIGHT_DASH == iAnimIdx
		|| (_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_UP_DASH == iAnimIdx
		|| (_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_LEFT_DASH == iAnimIdx
		|| (_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_DOWN_DASH== iAnimIdx)
	{
		Set_VeloState(m_pDynamicActor->Get_LinearVelocity());
	}

}

void CPlayerState_CyberIdle::Set_VeloState(_fvector _vVelocity)
{
	_float fVelocity = XMVectorGetX(XMVector2Length(_vVelocity));
	VELOCITY_STATE eNewVeloState = VELOCITY_STATE::VELOCITY_IDLE;
	if (fVelocity >= 6.f)
	{
		eNewVeloState = VELOCITY_DASH;
	}
	else if (fVelocity >= m_fRunVelocityThreshold)
	{
		F_DIRECTION eFDir = To_FDirection(_vVelocity);
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
			eNewVeloState = VELOCITY_STATE::VELOCITY_RUN_LEFT;
			break;
		case Client::F_DIRECTION::RIGHT:
			eNewVeloState = VELOCITY_STATE::VELOCITY_RUN_RIGHT;
			break;
		case Client::F_DIRECTION::UP:
			eNewVeloState = VELOCITY_STATE::VELOCITY_RUN_UP;
			break;
		case Client::F_DIRECTION::DOWN:
			eNewVeloState = VELOCITY_STATE::VELOCITY_RUN_DOWN;
			break;
		default:
			break;
		}
	}


	if (m_eVelocityState == eNewVeloState)
		return;
	if(m_pOwner->Get_Body()->Is_AnimTransition())
		return;
	m_eVelocityState = eNewVeloState;
	switch (m_eVelocityState)
	{
	case Client::CPlayerState_CyberIdle::VELOCITY_IDLE:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_IDLE);
		break;
	case Client::CPlayerState_CyberIdle::VELOCITY_RUN_RIGHT:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_RIGHT);
		break;
	case Client::CPlayerState_CyberIdle::VELOCITY_RUN_LEFT:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_LEFT);
		break;
	case Client::CPlayerState_CyberIdle::VELOCITY_RUN_UP:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_UP);
		break;
	case Client::CPlayerState_CyberIdle::VELOCITY_RUN_DOWN:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_DOWN);
		break;

	default:
		break;
	}

}


//==============================================================================================================
//CyberDash
//==============================================================================================================


CPlayerState_CyberDash::CPlayerState_CyberDash(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CYBER_DASH)
{
}

void CPlayerState_CyberDash::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();

	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		m_pOwner->Shoot_Rifle();
	}
	_vector vVelocity = m_pDynamicActor->Get_LinearVelocity();
	_float fVelocity = XMVectorGetX(XMVector2Length(vVelocity));
	if(fVelocity <= m_fEndDashVelocityThreshold)
		m_pOwner->Set_State(CPlayer::CYBER_IDLE);
}

void CPlayerState_CyberDash::Enter()
{
	m_pDynamicActor = m_pOwner->Get_ActorDynamic();
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();
	_float3 vForce;
	XMStoreFloat3(&vForce, tKeyResult.vMoveDir * m_f3DCyberDashForce);
	F_DIRECTION eFDirection = To_FDirection(tKeyResult.vMoveDir);
	m_pDynamicActor->Add_Impulse(vForce);
	switch (eFDirection)
	{
	case Client::F_DIRECTION::LEFT:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_LEFT_DASH);
		break;
	case Client::F_DIRECTION::RIGHT:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_RIGHT_DASH);
		break;
	case Client::F_DIRECTION::UP:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_UP_DASH);
		break;
	case Client::F_DIRECTION::DOWN:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_DOWN_DASH);
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
}
