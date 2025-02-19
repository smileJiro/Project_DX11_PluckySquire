#include "stdafx.h"
#include "PlayerState_LunchBox.h"
#include "LunchBox.h"

CPlayerState_LunchBox::CPlayerState_LunchBox(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::LUNCHBOX)
{
}


void CPlayerState_LunchBox::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK]
		|| tKeyResult.bInputStates[PLAYER_INPUT_THROWSWORD]
		|| tKeyResult.bInputStates[PLAYER_INPUT_JUMP]
		|| tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
	{
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}

	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
	if (false == m_bArrival )
	{
		if (false == m_pOwner->Check_Arrival(m_vPosition, 0.5f))
			m_pOwner->Move(XMVector3Normalize(m_vPosition - vPlayerPos) * m_fMoveSpeed, _fTimeDelta);
		else
			m_bArrival = true;
	}


	if (LUNCHBOX_STATE_IDLE == m_eCurState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_INTERACT])
		{
			m_eCurState = LUNCHBOX_STATE_CHARGE;
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_02_LOOP_GT);
		}
	}
	else if (LUNCHBOX_STATE_CHARGE == m_eCurState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_INTERACT])
		{

			if (m_pOwner->Try_Interact(m_pLunchBox, _fTimeDelta))
			{
				m_eCurState = LUNCHBOX_STATE_LAST;
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_BACKFLIP_GT);
				m_pLunchBox->Set_PlayingAnim(true);
				Event_KnockBack(m_pOwner, -m_vLook * m_fKnockBackPow);
				return;
			}
		}
		else
		{
			m_eCurState = LUNCHBOX_STATE_IDLE;
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_01_LOOP_GT);

		}
	}
	else
	{


	}
}

void CPlayerState_LunchBox::Enter()
{
	m_pLunchBox = dynamic_cast<CLunchBox*>( m_pOwner->Get_InteractableObject());
	m_vPosition = XMLoadFloat3(&m_pLunchBox->Get_InteractionPoint()) + m_pLunchBox->Get_FinalPosition();
	m_vPosition =XMVectorSetY( m_vPosition, 0.35f);
 	m_vLook = {0.f,0.f,1.f,0.f};
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_01_LOOP_GT);
	m_pOwner->LookDirectionXZ_Dynamic(m_vLook);
	m_eCurState = LUNCHBOX_STATE_IDLE;
}

void CPlayerState_LunchBox::Exit()
{
}

void CPlayerState_LunchBox::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE::COORDINATE_3D == _eCoord)
	{
		switch ((_uint)iAnimIdx)
		{
		case (_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_01_LOOP_GT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_POSE_02_LOOP_GT);
			break;
		case (_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_LUNCHBOX_BACKFLIP_GT:
			m_pOwner->Set_State(CPlayer::IDLE);
			break;
		}
	}
}
