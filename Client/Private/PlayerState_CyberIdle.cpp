#include "stdafx.h"
#include "PlayerState_CyberIdle.h"
#include "GameInstance.h"
#include "Actor_Dynamic.h"
#include "PlayerBody.h"
#include "Camera_Manager.h"
#include "Camera_Target.h"

CPlayerState_CyberIdle::CPlayerState_CyberIdle(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CYBER_IDLE)
{
}


void CPlayerState_CyberIdle::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();

	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
	{
		m_pOwner->Shoot_Rifle(_vector{0.f,0.f,0.f});
	}
	_vector vVelocity = { 0.f,0.f,0.f };
	if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
	{

		if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
		{
			m_pOwner->Set_State(CPlayer::CYBER_DASH);
			return;
		}
		else
		{
			vVelocity = tKeyResult.vMoveDir * m_pOwner->Get_CyberFlySpeed();
		}

	}

	m_pOwner->Set_CyberVelocity(vVelocity);
	m_pOwner->Update_CyberJot(_fTimeDelta);
	Set_VeloState(m_pOwner->Get_CyberVelocity());
}

void CPlayerState_CyberIdle::Enter()
{
	//m_pDynamicActor = m_pOwner->Get_ActorDynamic();
	//m_pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
	//m_pCameraTargetWorldMatrix = m_pTargetCamera->Get_TargetMatrix();
	int a = 0;
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
		//Set_VeloState(m_pDynamicActor->Get_LinearVelocity());
	}

}

void CPlayerState_CyberIdle::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{	
	m_pOwner->Set_State(CPlayer::CYBER_HIT);
}

void CPlayerState_CyberIdle::Set_VeloState(_fvector _vVelocity)
{
	_float fVelocity = XMVectorGetX(XMVector2Length(_vVelocity));
	VELOCITY_STATE eNewVeloState = VELOCITY_STATE::VELOCITY_IDLE;
	if (fVelocity >= m_fFlyMotionThreshold)
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
		m_pOwner->Shoot_Rifle(_vector{0.f,0.f,0.f});
	}

	if (m_fDashEndSpeedThreshold >= m_pOwner->Get_CyberCurrentSpeed())
	{
		m_pOwner->Set_State(CPlayer::CYBER_IDLE);
		return;
	}

	m_pOwner->Update_CyberJot(_fTimeDelta);
}

void CPlayerState_CyberDash::Enter()
{
	m_pDynamicActor = m_pOwner->Get_ActorDynamic();
	m_pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());

	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();
	m_vDashVelocity = tKeyResult.vMoveDir * m_pOwner->Get_CyberDashSpeed();
	m_pOwner->Set_CyberVelocity(m_vDashVelocity);
	F_DIRECTION eFDirection = To_FDirection(tKeyResult.vMoveDir);
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

	_float fG = m_pGameInstance->Compute_Random(0.6f, 0.909f);
	static_cast<CModelObject*>(m_pOwner->Get_PartObject(CPlayer::PART::PART_BODY))->On_Trail(0.025f, 0.5f, _float4(0.0f, fG, 1.0f, 0.7f), _float4(1.f, 1.f, 1.f, 1.f), 0.f, 1.5f);/*,_float4(242.f / 255.f, 44.f / 255.f, 103.f / 255.f, 0.9f)*/
//	static_cast<CModelObject*>(m_pOwner->Get_PartObject(CPlayer::PLAYER_PART::PLAYER_PART_ZETPACK))->On_Trail(0.05f, 0.5f, _float4(0.0f, fG, 1.0f, 0.7f)/*,_float4(242.f / 255.f, 44.f / 255.f, 103.f / 255.f, 0.9f)*/);

}

void CPlayerState_CyberDash::Exit()
{
	static_cast<CModelObject*>(m_pOwner->Get_PartObject(CPlayer::PART::PART_BODY))->Off_Trail();
	//static_cast<CModelObject*>(m_pOwner->Get_PartObject(CPlayer::PLAYER_PART::PLAYER_PART_ZETPACK))->Off_Trail();
	m_pDynamicActor->Set_LinearDamping(5.f);
}

void CPlayerState_CyberDash::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}

void CPlayerState_CyberDash::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
	m_pOwner->Set_State(CPlayer::CYBER_HIT);
}


//=================================HIT=============================================================================
CPlayerState_CyberHit::CPlayerState_CyberHit(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CYBER_HIT)
{
}

void CPlayerState_CyberHit::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_CyberJot();
	_vector vVelocity = { 0.f,0.f,0.f };
	if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
	{
		vVelocity = tKeyResult.vMoveDir * m_pOwner->Get_CyberFlySpeed();
	}

	m_pOwner->Set_CyberVelocity(vVelocity);
	m_pOwner->Update_CyberJot(_fTimeDelta);
}

void CPlayerState_CyberHit::Enter()
{
	m_pDynamicActor = m_pOwner->Get_ActorDynamic();
	m_pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
	m_pCameraTargetWorldMatrix = m_pTargetCamera->Get_TargetMatrix();
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_HIT_EDIT);
}

void CPlayerState_CyberHit::Exit()
{
}

void CPlayerState_CyberHit::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if ((_uint)CPlayer::ANIM_STATE_3D::CYBERJOT_ANIM_HIT_EDIT == iAnimIdx)
	{
		m_pOwner->Set_State(CPlayer::CYBER_IDLE);
	}
}
