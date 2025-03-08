#include "stdafx.h"
#include "PlayerState_CyberIdle.h"
#include "GameInstance.h"
#include "Actor_Dynamic.h"


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
		_float3 vForce;
		XMStoreFloat3(&vForce, tKeyResult.vMoveDir * m_f3DCyberFlySpeed * _fTimeDelta);
		m_pDynamicActor->Add_Force(vForce);

		m_eDirection = To_FDirection(tKeyResult.vMoveDir);


		if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
		{
			_float3 vForce;
			XMStoreFloat3(&vForce, tKeyResult.vMoveDir * m_f3DCyberDashForce);
			m_pDynamicActor->Add_Impulse(vForce);
			switch (m_eDirection)
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
		else
		{

		}

	}
	_float vVelocity = XMVector3Length(m_pDynamicActor->Get_LinearVelocity()).m128_f32[0];
	//cout << "Velocity : " << vVelocity << endl;

	if (vVelocity >= 10.f)
	{

	}
	else if (vVelocity >= m_fRunVelocityThreshold)
		Set_VeloState(VELOCITY_RUN);
	else
		Set_VeloState(VELOCITY_IDLE);

	
}

void CPlayerState_CyberIdle::Enter()
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();
	m_pDynamicActor = m_pOwner->Get_ActorDynamic();
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

}

void CPlayerState_CyberIdle::Set_VeloState(VELOCITY_STATE _eState)
{
	if (m_eVelocityState == _eState)
		return;
	cout << "m_eVelocityState" << m_eVelocityState << endl;
	m_eVelocityState = _eState;
	if (VELOCITY_STATE::VELOCITY_IDLE == m_eVelocityState)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_IDLE);
	}
	else if (VELOCITY_STATE::VELOCITY_RUN == m_eVelocityState)
	{
		switch (m_eDirection)
		{
		case Client::F_DIRECTION::LEFT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_LEFT);
			break;
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_FLYING_DOWN);
			break;
		default:
			break;
		}
	}

}
