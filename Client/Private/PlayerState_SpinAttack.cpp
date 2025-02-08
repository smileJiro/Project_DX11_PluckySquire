#include "stdafx.h"
#include "PlayerState_SpinAttack.h"

CPlayerState_SpinAttack::CPlayerState_SpinAttack(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::SPINATTACK)
{
}

void CPlayerState_SpinAttack::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	//기모으기
	if (SPIN_CHARGING == m_eSpinState)
	{
		if(false == m_bLaunchInput)
			m_bLaunchInput = tKeyResult.bInputStates[PLAYER_KEY_SPINLAUNCH];
		_float fProgress = m_pOwner->Get_AnimProgress();
		_float fLaunchableProgress = eCoord == COORDINATE_2D ? m_fLaunchableProgress2D : m_fLaunchableProgress3D;
		//기 다 모았으면
		if (fProgress >= fLaunchableProgress)
		{
			if (m_bLaunchInput)
			{
				Set_SpinState(SPIN_SPINNING);
				return;
			}
		}
		if (tKeyResult.bInputStates[PLAYER_KEY_ATTACK])
			m_pOwner->Set_State(CPlayer::ATTACK);
		else if (tKeyResult.bInputStates[PLAYER_KEY_JUMP])
			m_pOwner->Set_State(CPlayer::JUMP_UP);
		else if (tKeyResult.bInputStates[PLAYER_KEY_ROLL])
			m_pOwner->Set_State(CPlayer::ROLL);
		else if (tKeyResult.bInputStates[PLAYER_KEY_THROWSWORD])
			m_pOwner->Set_State(CPlayer::THROWSWORD);
	}
	//돌기
	else if(SPIN_SPINNING == m_eSpinState)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fSpinMoveSpeed, _fTimeDelta);
	}
	//다 돌고 난 후
	else if (SPIN_SPINEND == m_eSpinState)
	{

	}

}

void CPlayerState_SpinAttack::Enter()
{
	m_eCoord= m_pOwner->Get_CurCoord();
	m_fSpinMoveSpeed = m_pOwner->Get_MoveSpeed(m_eCoord);
	m_iSpinAttackLevel = (_int)m_pOwner->Get_SpinAttackLevel();
	m_eFDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
	Set_SpinState(SPIN_CHARGING);

}

void CPlayerState_SpinAttack::Exit()
{
}

void CPlayerState_SpinAttack::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_3D == eCoord)
	{
		if((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_SPIN_LOOP_GT == iAnimIdx)
			On_RoundEnd();
		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_OUT_GT == iAnimIdx)
			m_pOwner->Set_State(CPlayer::IDLE);
	}
	else
	{
		if (1 == m_iSpinAttackLevel)
		{
			if((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL1 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL1 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL1 == iAnimIdx)
				On_RoundEnd();
		}  
		else if (2 == m_iSpinAttackLevel)
		{
			if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL2 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL2 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL2 == iAnimIdx)
				On_RoundEnd();
		}
		else if (3 == m_iSpinAttackLevel)
		{
			if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL3 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL3 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL3 == iAnimIdx)
				On_RoundEnd();
		}
		else if (4 == m_iSpinAttackLevel)
		{
			if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL4 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL4 == iAnimIdx
				|| (_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL4 == iAnimIdx)
				On_RoundEnd();
		}

	}
}

void CPlayerState_SpinAttack::Set_SpinState(SPIN_STATE _eNewState)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_eSpinState = _eNewState;
	switch (m_eSpinState)
	{
	case Client::CPlayerState_SpinAttack::SPIN_CHARGING:
	{
		if (COORDINATE_3D == eCoord)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_INTO_GT);
		else
		{
			F_DIRECTION m_eFDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
			switch (m_eFDir)
			{
			case Client::F_DIRECTION::LEFT:
			case Client::F_DIRECTION::RIGHT:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_INTO_RIGHT);
				break;
			case Client::F_DIRECTION::UP:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_INTO_UP);
				break;
			case Client::F_DIRECTION::DOWN:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_INTO_DOWN);
				break;
			default:
				break;
			}
		}
		break;
	}
	case Client::CPlayerState_SpinAttack::SPIN_SPINNING:
	{
		if (COORDINATE_3D == eCoord)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_SPIN_LOOP_GT);
		else
		{

			switch (m_eFDir)
			{
			case Client::F_DIRECTION::LEFT:
			case Client::F_DIRECTION::RIGHT:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL1);
				break;
			case Client::F_DIRECTION::UP:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_INTO_UP);
				break;
			case Client::F_DIRECTION::DOWN:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_INTO_DOWN);
				break;
			default:
				break;
			}
		}
		break;
	}
	case Client::CPlayerState_SpinAttack::SPIN_SPINEND:
	{
		if (COORDINATE_3D == eCoord)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_OUT_GT);
		else
		{
			switch (m_eFDir)
			{
			case Client::F_DIRECTION::LEFT:
			case Client::F_DIRECTION::RIGHT:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL1);
				break;
			case Client::F_DIRECTION::UP:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_INTO_UP);
				break;
			case Client::F_DIRECTION::DOWN:
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_INTO_DOWN);
				break;
			default:
				break;
			}
		}
		break;
	}
	default:
			break;
	}
}

void CPlayerState_SpinAttack::On_RoundEnd()
{
	if (m_iSpinAttackLevel >= m_iSpinCount)
	{
		m_iSpinCount++;
		Set_SpinState(SPIN_SPINNING);
	}
	else
	{
		Set_SpinState(SPIN_SPINEND);
	}
}
