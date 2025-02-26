#include "stdafx.h"
#include "PlayerState_Stamp.h"

CPlayerState_Stamp::CPlayerState_Stamp(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::STAMP)
{
}

void CPlayerState_Stamp::Update(_float _fTimeDelta)
{
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput_Stamp();

	switch (m_eStampState)
	{
	case Client::CPlayerState_Stamp::STAMP_START:
		m_pOwner->Stop_Rotate();
		break;
	case Client::CPlayerState_Stamp::STAMP_KEEP:
	{
		_vector vSmashPosition = m_pOwner->Get_FinalPosition() + XMVector3Normalize( XMVectorSetY(m_pOwner->Get_LookDirection(), 0.f)) * m_fStampingRange;
		if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
			m_pOwner->Set_State(CPlayer::ROLL);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_CANCEL_STAMP])
			m_pOwner->Set_State(CPlayer::IDLE);
		else if (false == tKeyResult.bInputStates[PLAYER_INPUT_KEEP_STAMP])
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STAMP_ATTACK_EDIT_NEWRIG);
			m_eStampState = STAMP_SMASH;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		{
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fStampSpeed, _fTimeDelta);
			m_pOwner->Rotate_To(XMVector3Normalize(tKeyResult.vMoveDir), m_fRotateSpeed);
		}
		else
		{
			m_pOwner->Stop_Rotate();
		}
		break;
	}
	case Client::CPlayerState_Stamp::STAMP_SMASH:
		m_pOwner->Stop_Rotate();
		break;
	default:
		break;
	}



}

void CPlayerState_Stamp::Enter()
{
	m_pOwner->Equip_Part(CPlayer::PLAYER_PART_STOP_STMAP);
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STAMP_PULLOUT_SHORT_NEWRIG);
	m_fStampSpeed = m_pOwner->Get_MoveSpeed(COORDINATE_3D)*0.5f;
	m_eStampState = STAMP_START;
}

void CPlayerState_Stamp::Exit()
{
	if(m_pOwner->Is_SwordHandling())
		m_pOwner->Equip_Part(CPlayer::PLAYER_PART_SWORD);
	else
		m_pOwner->UnEquip_Part(CPlayer::PLAYER_PART_STOP_STMAP);


}

void CPlayerState_Stamp::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STAMP_PULLOUT_SHORT_NEWRIG == iAnimIdx)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STAMP_HOLD_GT);
		m_eStampState = STAMP_KEEP;
	}
	else if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STAMP_ATTACK_EDIT_NEWRIG == iAnimIdx)
	{
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}
