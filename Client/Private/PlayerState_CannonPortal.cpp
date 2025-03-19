#include "stdafx.h"
#include "PlayerState_CannonPortal.h"
#include "Portal_Cannon.h" 
#include "Player.h"
#include "Actor_Dynamic.h"
#include "GameInstance.h"


CPlayerState_CannonPortal::CPlayerState_CannonPortal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::START_CANNON_PORTAL)
{

}

//대포포탈은 기본적으로 2D에서만 탈 수 있음. 3D에서는 안쪽 별이 없어지고 비활성화됨.
//E를 누르면 대포 포탈 중앙으로 빨려들어가고, 
//E 누르는 동안 빙글빙글 돌면서 날아갈 방향을 가리킴. 포탈이 하얗게 됨
// E 뗄 때 날아감. 날아갈 때 빠르게 빙글 도는 애니메이션 나옴
//
// 
//포탈 위치까지 쬐그려서 이동.
//도착하면 빙그빙글
//버튼 뗴면 발사
void CPlayerState_CannonPortal::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	INTERACT_RESULT eResult = m_pOwner->Try_Interact(_fTimeDelta);
	if (INTERACT_RESULT::CHARGE_CANCEL == eResult)
	{
		if(false == Is_ChargeCompleted())
		{
			m_pPortal->Set_PortalState(CPortal_Cannon::IDLE);
			m_pOwner->Set_State(CPlayer::IDLE);
		}
		else
			Set_SubState(SUB_SHOOT_READY);
		return;
	}
	if (SUB_INTO == m_eSubState)
	{
		//CHARGING인 동안에는 포탈을 향해 이동,
		//도착하면 SUB_SPIN_CHARGE 상태로 전환
		if (INTERACT_RESULT::CHARGING == eResult
			|| INTERACT_RESULT::INTERACT_START == eResult)
		{
			if (m_pOwner->Move_To(m_vPortalPos, _fTimeDelta))
				Set_SubState(SUB_SPIN_CHARGE);
		}
	}
	else if (SUB_SPIN_CHARGE == m_eSubState)
	{
		if (INTERACT_RESULT::SUCCESS == eResult)
		{
			Set_SubState(SUB_SPIN_CHARGE_COMPLETE);
			return;
		}

	}
	else if (SUB_SPIN_CHARGE_COMPLETE == m_eSubState)
	{
		//CHARGING 완료면? 방향을 정할 수 있다.
		PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		{
			swap(tKeyResult.vDir.m128_f32[1], tKeyResult.vDir.m128_f32[2]);
			m_pPortal->Set_ShootDirection(tKeyResult.vDir);
			CPortal_Cannon* pTmpTarget = m_pPortal->Find_TargetPortal(tKeyResult.vDir);
			if (pTmpTarget)
				pTmpTarget->Set_PortalState(CPortal_Cannon::TARGETEED);
			if(m_pTargetPortal && m_pTargetPortal != pTmpTarget)
				m_pTargetPortal->Set_PortalState(CPortal_Cannon::IDLE);
			m_pTargetPortal = pTmpTarget;
		}
		else
		{
			m_pPortal->Set_ShootDirection(_vector{0.f,0.f,0.f});
			if (m_pTargetPortal)
				m_pTargetPortal->Set_PortalState(CPortal_Cannon::IDLE);
			m_pTargetPortal = m_pPortal;
		}
	}
	else if (SUB_SHOOT_READY == m_eSubState)
	{
		if (m_bShootedFrame)
			m_bShootedFrame = false;
		else
			Set_SubState(SUB_SHOOT);
	}
	else if (SUB_SHOOT == m_eSubState)
	{

		if (m_pTargetPortal)
		{
			_float fUpForce = m_pOwner->Get_UpForce();
			_float fDistance = XMVector3Length(m_pTargetPortal->Get_FinalPosition() - m_pOwner->Get_FinalPosition()).m128_f32[0];
			cout << fDistance << endl;
			if (fUpForce < 0.f && fDistance < 0.3f)
			{
				Set_SubState(SUB_GOAL_IN_READY);

				return;
			}
		}
		else
		{
			_float fUpForce = m_pOwner->Get_UpForce();
			if (fUpForce < 0.f)
			{
				m_pOwner->Get_ActorDynamic()->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, true);
				if (m_pOwner->Is_OnGround())
				{
					Set_SubState(SUB_GOAL);
				}
			}

		}
	}
	else if (SUB_GOAL == m_eSubState)
	{

	}
	else if (SUB_GOAL_IN_READY == m_eSubState)
	{
		if (m_bGoalInFrame)
			m_bGoalInFrame = false;
		else
			Set_SubState(SUB_GOAL_IN);
	}
	else if (SUB_GOAL_IN == m_eSubState)
	{
	}

}

void CPlayerState_CannonPortal::Enter()
{
	m_pPortal = dynamic_cast<CPortal_Cannon*>(m_pOwner->Get_InteractableObject());
	assert(nullptr != m_pPortal);
	m_vPortalPos = m_pPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);
	m_vPortalPos3D = m_pPortal->Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_State(CTransform::STATE_POSITION);
	Set_SubState(SUB_INTO);
	m_pPortal->Set_PortalState(CPortal_Cannon::CHARGE);
}

void CPlayerState_CannonPortal::Exit()
{
	int a = 0;
}

void CPlayerState_CannonPortal::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{
		if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CANNON_CURL_INTO == iAnimIdx)
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CANNON_CURL_SPIN);
		}
		if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_RIGHT == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_UP == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_DOWN == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}

	}
	else
	{
		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_CANON_FLY_LAND_GT == iAnimIdx)
			m_pOwner->Set_State(CPlayer::IDLE);
	}
}

void CPlayerState_CannonPortal::Set_SubState(SUB_STATE _eState)
{
	if (m_eSubState == _eState)
		return;
	//cout << "State Change : " << m_eSubState << " -> " << _eState << endl;
	m_eSubState = _eState;
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
	switch (m_eSubState)
	{
	case Client::CPlayerState_CannonPortal::SUB_INTO:
	{
		_vector vDir = XMVector3Normalize(XMVectorSetW(m_vPortalPos - vPlayerPos, 0.f));
		F_DIRECTION eFDir = To_FDirection(vDir);
		E_DIRECTION eEDIr = To_EDirection(eFDir);
		m_pOwner->Set_2DDirection(eEDIr);
		m_e2DLookDir = eEDIr;
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CANNONCHARGE_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CANNONCHARGE_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CANNONCHARGE_DOWN);
			break;
		default:
			break;
		}
		break;
	}
	case Client::CPlayerState_CannonPortal::SUB_SPIN_CHARGE:
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_CANNON_CURL_INTO);
		break;
	}
	case Client::CPlayerState_CannonPortal::SUB_SPIN_CHARGE_COMPLETE:
	{
		//방향 화살표? 나오게하기

		break;
	}
	case Client::CPlayerState_CannonPortal::SUB_SHOOT_READY:
	{
		m_pPortal->Use_Portal(m_pOwner);
		m_pPortal->Set_PortalState(CPortal_Cannon::INACTIVE);
		m_bShootedFrame = true;
		break;
	}
	case Client::CPlayerState_CannonPortal::SUB_SHOOT:
	{
		m_pOwner->Get_ActorDynamic()->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, false);
		m_pOwner->Set_Kinematic(false);
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_CANNON);
		if(m_pTargetPortal)
			m_pPortal->Shoot_Target(m_pOwner, m_pTargetPortal);
		else
			m_pPortal->Shoot_Ground(m_pOwner, m_e2DLookDir);
		break;
	}
	case Client::CPlayerState_CannonPortal::SUB_GOAL:
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_CANON_FLY_LAND_GT);
		break;
	case Client::CPlayerState_CannonPortal::SUB_GOAL_IN_READY:
		m_pTargetPortal->Use_Portal(m_pOwner);
		m_bGoalInFrame = true;
		break;
	case Client::CPlayerState_CannonPortal::SUB_GOAL_IN:
	{
		m_pTargetPortal->Set_PortalState(CPortal_Cannon::GOAL_IN);
		_vector vDir = m_pPortal->Get_ShootDirection();
		swap(vDir.m128_f32[1], vDir.m128_f32[2]);
		F_DIRECTION eFDir = To_FDirection(vDir);
		m_pOwner->Set_2DDirection(To_EDirection(eFDir));
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_DOWN);
			break;
		case Client::F_DIRECTION::F_DIR_LAST:
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}
