#include "stdafx.h"
#include "PlayerState_StartPortal.h"
#include "Portal.h"
#include "GameInstance.h"

CPlayerState_StartPortal::CPlayerState_StartPortal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::START_PORTAL)
{
}

void CPlayerState_StartPortal::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	//Interact 키가 안눌리면 취소해야 함.
	INTERACT_RESULT eResult =  m_pOwner->Try_Interact(_fTimeDelta);
	if (INTERACT_RESULT::CHARGING == eResult)
	{
		if (COORDINATE_2D == eCoord)
		{
			_vector vPlayerPos = m_pOwner->Get_FinalPosition();
			if (false == m_pOwner->Check_Arrival(m_vTargetPos, COORDINATE_2D == eCoord ? 10.f : 0.5f))
				m_pOwner->Move(XMVector3Normalize(m_vTargetPos - vPlayerPos)
					* m_f2DMoveSpeed, _fTimeDelta);

		}
	}
	else if (INTERACT_RESULT::FAIL == eResult
		|| INTERACT_RESULT::NO_INPUT == eResult)
	{
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}
	else if(INTERACT_RESULT::SUCCESS == eResult)
	{
		return;
	}
	if (m_pOwner->Is_PlayingAnim())
	{
		_float fProgress = m_pOwner->Get_AnimProgress();
		if (m_fChargeAnimProgress <= fProgress)
		{
			m_pOwner->Set_PlayingAnim(false);
		}
	}

}

void CPlayerState_StartPortal::Enter()
{
	m_pPortal = dynamic_cast<CPortal*>(m_pOwner->Get_InteractableObject());
	assert(nullptr != m_pPortal);
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_vTargetPos = m_pPortal->Get_ControllerTransform()->Get_Transform(eCoord)->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
	_vector vDir = XMVector3Normalize(m_vTargetPos - vPlayerPos);
	
	if (COORDINATE_3D == eCoord)
	{
		m_pOwner->LookDirectionXZ_Dynamic(vDir);
		m_vTargetPos = XMVectorSetY(m_vTargetPos, XMVectorGetY(vPlayerPos));
		m_vTargetPos += -vDir * m_f3DJumpDistance;
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_BOOK_JUMP_INTO_FRONT_GT);
	}
	else
	{
		F_DIRECTION eFDir = To_FDirection(vDir);
		E_DIRECTION eEDIr = FDir_To_EDir(eFDir);
		m_pOwner->Set_2DDirection(eEDIr);
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKEXIT_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKEXIT_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKEXIT_DOWN);
			break;
		default:
			break;
		}
	}

	m_pGameInstance->Start_SFX(_wstring(L"A_sfx_start_entering_portal-") + to_wstring(rand() % 5), 50.f);

}

void CPlayerState_StartPortal::Exit()
{
	m_pOwner->Set_PlayingAnim(true);
}

void CPlayerState_StartPortal::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}
