#include "stdafx.h"
#include "PlayerState_ExitPortal.h"
#include "Portal.h"
#include "Actor_Dynamic.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "Camera_Target.h"
#include "Camera_2D.h"
#include "Friend_Controller.h"
#include "TriggerObject.h"
#include "Section_Manager.h"
#include "Trigger_Manager.h"

CPlayerState_ExitPortal::CPlayerState_ExitPortal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::EXIT_PORTAL)
{
}

void CPlayerState_ExitPortal::Update(_float _fTimeDelta)
{
	if (m_bFirstFrame)
	{
		m_bFirstFrame = false;
		m_bSecondFrame = true;
		return;
	}
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (m_bSecondFrame)
	{
		m_bSecondFrame = false;

		if (COORDINATE_3D == eCoord)
		{
			Shoot_Player3D();
		}
		else
		{
			E_DIRECTION eEDIr = To_EDirection(m_pOwner->Get_LookDirection());
			m_pOwner->Set_2DDirection(eEDIr);
			F_DIRECTION eFDir = To_FDirection(eEDIr);
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

		return;
	}
	if (COORDINATE_3D == eCoord)
	{
		if (-0.f > m_pOwner->Get_UpForce())
		{
			static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, true);
			if(m_pOwner->Is_OnGround())
				m_pOwner->Set_State(CPlayer::IDLE);
		}


	}
}

void CPlayerState_ExitPortal::Enter()
{
	m_pPortal = dynamic_cast<CPortal*>(m_pOwner->Get_InteractableObject());
	assert(nullptr != m_pPortal);
	m_ePortalNormal = m_pPortal->Get_PortalNormal();
	m_pPortal->Use_Portal(m_pOwner);
	static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, false);



}
//플레이어 죽으면 떨어짐

void CPlayerState_ExitPortal::Exit()
{
	static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, true);

	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		if (true == CFriend_Controller::GetInstance()->Is_Train())
		{
			CTriggerObject::TRIGGEROBJECT_DESC DescA = {};
			DescA.vHalfExtents = { 25.f, 25.f, 0.f };
			DescA.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
			DescA.szEventTag = TEXT("Start_Train");
			DescA.eConditionType = CTriggerObject::TRIGGER_ENTER;
			DescA.isReusable = false; // 한 번 하고 삭제할 때
			DescA.eStartCoord = COORDINATE_2D;
			_vector v2DPos = m_pOwner->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);
			DescA.tTransform2DDesc.vInitialPosition = _float3(XMVectorGetX(v2DPos), XMVectorGetY(v2DPos), 0.0f);
			
			_wstring wsSectionKey = CSection_Manager::GetInstance()->Get_Cur_Section_Key();
			CSection* pSection = CSection_Manager::GetInstance()->Find_Section(wsSectionKey);
			CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, (LEVEL_ID)m_pOwner->Get_CurLevelID(), &DescA, pSection);

			CFriend_Controller::GetInstance()->End_Train();
		}
		
	}
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

void CPlayerState_ExitPortal::Shoot_Player3D()
{
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
	//static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, true);
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
	//_vector vNewLookDir = vImpulse;
	//if (abs(vNewLookDir.m128_f32[1]) > abs(vNewLookDir.m128_f32[0])
	//	&& abs(vNewLookDir.m128_f32[1]) > abs(vNewLookDir.m128_f32[2]))
	//{
	//	vNewLookDir = { 0.f,0.f,-1.f };
	//}

	_float3 vPos;
	XMStoreFloat3(&vPos, vPlayerPos + vImpulse * 0.5f);
	m_pOwner->Get_ActorCom()->Set_GlobalPose(vPos);

	if(NORMAL_DIRECTION::NEGATIVE_Y == m_ePortalNormal
		|| NORMAL_DIRECTION::POSITIVE_Y == m_ePortalNormal)
	{

		//2D상태일 때 카메라의 방향 구하기
		_vector vCamLook = static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_CurrentCamera())->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);
		vCamLook = -XMVectorSetY(vCamLook, 0.f);
		if (abs(vCamLook.m128_f32[0]) > abs(vCamLook.m128_f32[2]))
		{
			vCamLook.m128_f32[2] = 0.f;
			vCamLook.m128_f32[0] /= vCamLook.m128_f32[0];
		}
		else
		{
			vCamLook.m128_f32[0] = 0.f;
			vCamLook.m128_f32[2] /= vCamLook.m128_f32[2];
		}
		//2D상태일 때 플레이어의 3D좌표계방향 구하기
		F_DIRECTION eFDir= To_FDirection(m_pOwner->Get_2DDirection());
		_vector vNewLookDIr = { 0.f,0.f,0.f,0.f };
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
			vNewLookDIr = XMVector3Rotate(vCamLook, XMQuaternionRotationAxis({ 0.f,1.f,0.f }, XMConvertToRadians(-90.f)));
			break;
		case Client::F_DIRECTION::RIGHT:
			vNewLookDIr = XMVector3Rotate(vCamLook, XMQuaternionRotationAxis({ 0.f,1.f,0.f }, XMConvertToRadians(90.f)));
			break;
		case Client::F_DIRECTION::UP:
			vNewLookDIr = vCamLook;
			break;
		case Client::F_DIRECTION::DOWN:
			vNewLookDIr = -1* vCamLook;
			break;

		default:
			break;
		}


		m_pOwner->LookDirectionXZ_Dynamic(vNewLookDIr);
	}
	else
		m_pOwner->LookDirectionXZ_Dynamic(vImpulse);

}
