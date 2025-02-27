#include "stdafx.h"
#include "PlayerState_Clamber.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"
#include "PlayerState_Run.h"

CPlayerState_Clamber::CPlayerState_Clamber(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::CLAMBER)
{
	m_fArmHeight = m_pOwner->Get_ArmHeight();
	m_fHeadHeight = m_pOwner->Get_HeadHeight();
	m_fArmLength = m_pOwner->Get_ArmLength();
}

void CPlayerState_Clamber::Update(_float _fTimeDelta)
{
	m_fAnimRatio = m_pOwner->Get_AnimProgress();
	_float3 vEndPosition, vStartPosition, vCurPosition;

	_float fLerpRatio = (m_fAnimRatio - m_fLerpStartProgress) / (m_fLerpEndProgress - m_fLerpStartProgress);
	fLerpRatio = max(0.f, min(1.f, fLerpRatio));
	XMStoreFloat3(&vEndPosition, m_vClamberEndPosition);
	XMStoreFloat3(&vStartPosition, m_vClamberStartPosition);
	XMStoreFloat3(&vCurPosition, XMVectorLerp(XMLoadFloat3(&vStartPosition), XMLoadFloat3(&vEndPosition), fLerpRatio));
	m_pOwner->Set_Position(XMLoadFloat3( &vCurPosition));
}

//벽에 닿아야 SceneQueryFlag가 켜지고, RayCast가 돼야 Clamber상태가 되므로 여기선 항상 벽에 붙어있음.
//->WallNormal과 CamberEndPosition은 항상 유효한 값이 들어있음.
void CPlayerState_Clamber::Enter()
{
	assert(COORDINATE_3D == m_pOwner->Get_CurCoord());
	static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, false);
	m_pOwner->Stop_Move();
	m_pOwner->Set_Kinematic(true);

	m_pOwner->UnEquip_Part(CPlayer::PLAYER_PART_SWORD);
	m_vClamberNormal = m_pOwner->Get_WallNormal();
	m_vClamberEndPosition = m_pOwner->Get_ClamberEndPosition();
	m_vClamberEndPosition = XMVectorSetW(m_vClamberEndPosition, 1);
	m_vClamberStartPosition = m_vClamberEndPosition + m_vClamberNormal * m_fArmLength + _vector{0,-m_fArmHeight,0};
	m_vClamberStartPosition = XMVectorSetW(m_vClamberStartPosition,1);
	m_pOwner->Set_Position(m_vClamberStartPosition);
	m_pOwner->LookDirectionXZ_Kinematic(-m_vClamberNormal);
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_CLAMBER_01_EDIT_NEWRIG);
}

void CPlayerState_Clamber::Exit()
{
	static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, true);
}

void CPlayerState_Clamber::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	assert(COORDINATE_3D == _eCoord);

	if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_CLAMBER_01_EDIT_NEWRIG == iAnimIdx)
	{
		m_pOwner->Set_Position(m_vClamberEndPosition);
		m_pOwner->LookDirectionXZ_Kinematic(-m_vClamberNormal);
		m_pOwner->Set_Kinematic(false);
		m_pOwner->Equip_Part(CPlayer::PLAYER_PART_SWORD);
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}
