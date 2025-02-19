#include "stdafx.h"
#include "PlayerState_ExitPortal.h"
#include "Portal.h"
#include "Actor_Dynamic.h"
#include "GameInstance.h"

CPlayerState_ExitPortal::CPlayerState_ExitPortal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::EXIT_PORTAL)
{
}

void CPlayerState_ExitPortal::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_3D == eCoord)
	{
		if (m_pOwner->Is_OnGround() && -0.f > m_pOwner->Get_UpForce())
			m_pOwner->Set_State(CPlayer::IDLE);
	}

}

void CPlayerState_ExitPortal::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_pPortal = m_pOwner->Get_CurrentPortal();

	assert(m_pPortal);
	_vector vPortalPos = m_pPortal->Get_ControllerTransform()->Get_Transform(eCoord)->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();

	if (COORDINATE_3D == eCoord)
	{
		static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_FOOT, true);
		m_ePortalNormal = m_pOwner->Get_PortalNormal();
		_vector vDir = m_pOwner->Get_LookDirection(COORDINATE_2D);
		vDir = XMVectorSetZ(vDir, XMVectorGetY(vDir));
		vDir = XMVectorSetY(vDir, 0);
		m_pOwner->LookDirectionXZ_Dynamic(vDir);
		_vector vTargetPos = vPortalPos + vDir * m_f3DJumpDistance;
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
		m_pOwner->LookDirectionXZ_Dynamic(vImpulse);
		m_pOwner->Add_Impuls(vImpulse * m_f3DJumpDistance);
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
//ÇÃ·¹ÀÌ¾î Á×À¸¸é ¶³¾îÁü

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
