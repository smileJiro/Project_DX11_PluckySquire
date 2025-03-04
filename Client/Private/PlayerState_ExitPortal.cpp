#include "stdafx.h"
#include "PlayerState_ExitPortal.h"
#include "Portal.h"
#include "Actor_Dynamic.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

CPlayerState_ExitPortal::CPlayerState_ExitPortal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::EXIT_PORTAL)
{
}

void CPlayerState_ExitPortal::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_3D == eCoord)
	{
		if(false == m_bWaitFirstFrame)
		{
			m_bWaitFirstFrame = true;
			return;
		}
		if (m_pOwner->Is_OnGround() && -0.f > m_pOwner->Get_UpForce())
			m_pOwner->Set_State(CPlayer::IDLE);
	}

}

void CPlayerState_ExitPortal::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	_vector vPlayerPos = m_pOwner->Get_FinalPosition();

	if (COORDINATE_3D == eCoord)
	{
		static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, true);
		m_ePortalNormal = m_pOwner->Get_PortalNormal();
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_BOOK_JUMP_FALL_FRONT_NEWRIG);

		_vector vImpulse = { 0.f,0.f,0.f,0.f };

		switch (m_ePortalNormal)
		{
		case Engine::NORMAL_DIRECTION::POSITIVE_X:
			vImpulse = { 1.f,0.f,0.f ,0.f };
			break;
		case Engine::NORMAL_DIRECTION::NEGATIVE_X:
			vImpulse = { -1.f,0.f,0.f ,0.f };
			break;
		case Engine::NORMAL_DIRECTION::POSITIVE_Y:
			vImpulse = { 0.f,1.f,0.f ,0.f };

			break;
		case Engine::NORMAL_DIRECTION::NEGATIVE_Y:
			vImpulse = { 0.f,-1.f,0.f ,0.f };

			break;
		case Engine::NORMAL_DIRECTION::POSITIVE_Z:
			vImpulse = { 0.f,0.f,1.f,0.f };
			break;
		case Engine::NORMAL_DIRECTION::NEGATIVE_Z:
			vImpulse = { 0.f,0.f,-1.f ,0.f };
			break;
		default:

			break;
		}
		m_pOwner->Add_Impuls(vImpulse * m_f3DJumpDistance);
		_vector vNewLookDir = vImpulse;
		if (abs(vNewLookDir.m128_f32[1]) > abs(vNewLookDir.m128_f32[0])
			&& abs(vNewLookDir.m128_f32[1]) > abs(vNewLookDir.m128_f32[2]))
		{
			vNewLookDir = { 0.f,0.f,-1.f };
		}
		m_pOwner->LookDirectionXZ_Dynamic(vNewLookDir);
		//_float3 vPos;
		//XMStoreFloat3(&vPos, vPlayerPos + vImpulse * 0.5f);
		//m_pOwner->Get_ActorCom()->Set_GlobalPose(vPos);


		////2D상태일 때 카메라의 방향 구하기
		//_vector vCamLook = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);
		//vCamLook = -XMVectorSetY(vCamLook, 0.f);
		//if (abs(vCamLook.m128_f32[0]) > abs(vCamLook.m128_f32[2]))
		//{
		//	vCamLook.m128_f32[2] = 0.f;
		//	vCamLook.m128_f32[0] /= vCamLook.m128_f32[0];
		//}
		//else
		//{
		//	vCamLook.m128_f32[0] = 0.f;
		//	vCamLook.m128_f32[2] /= vCamLook.m128_f32[2];
		//}
		////2D상태일 때 플레이어의 3D좌표계방향 구하기
		//_vector vNewLookDIr = FDir_To_Vector(EDir_To_FDir( m_pOwner->Get_2DDirection()));
		//vNewLookDIr = XMVectorSetZ(vNewLookDIr, XMVectorGetY(vNewLookDIr));
		//vNewLookDIr = XMVectorSetY(vNewLookDIr, 0.f);

		////2D 상태일 때 카메라의 방향이 z+가 아니라면?
		////카메라 방향 :100 -> 001
		////플레이어 방향 : 
		//vNewLookDIr += (vCamLook - _vector{ 0.f,0.f,1.f });
		//static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Start_ParabolicTo(vTargetPos, XMConvertToRadians(45.f));
	}
	else
	{
		E_DIRECTION eEDIr = To_EDirection(m_pOwner->Get_LookDirection());
		m_pOwner->Set_2DDirection(eEDIr);
		F_DIRECTION eFDir = EDir_To_FDir(eEDIr);
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
		default:
			break;
		}
	}

}
//플레이어 죽으면 떨어짐

void CPlayerState_ExitPortal::Exit()
{
}

void CPlayerState_ExitPortal::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{
		if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_RIGHT == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_UP == iAnimIdx
			|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKENTER_DOWN == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
	else
	{
		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_BOOK_JUMP_FALL_FRONT_NEWRIG == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
}
