#include "stdafx.h"
#include "PlayerState_Attack.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"

CPlayerState_Attack::CPlayerState_Attack(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ATTACK)
{

}

void CPlayerState_Attack::Update(_float _fTimeDelta)
{
    PLAYER_KEY_RESULT tKeyResult = m_pOwner->Player_KeyInput();

    if (tKeyResult.bKeyStates[PLAYER_KEY_ATTACK])
    {
        m_bCombo = true;
    }
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	_float fProgress =m_pOwner->Get_AnimProgress();
	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	_float fForwardingProgress = eCoord == COORDINATE_2D ? m_f2DForwardingProgress : m_f3DForwardingProgress;
    if (COORDINATE_3D == eCoord)
        m_pOwner->Rotate_To(XMVector3Normalize(m_vDirection), 1080);
	if (fProgress >= fMotionCancelProgress)
	{

        if (m_bCombo)
        {
            m_iComboCount++;
            if (2 >= m_iComboCount)
            {
                Switch_To_AttackAnimation(m_iComboCount);

            }
            m_bCombo = false;
        }
        else
        {
            if (tKeyResult.bKeyStates[PLAYER_KEY_JUMP])
                m_pOwner->Set_State(CPlayer::JUMP);
            else if (tKeyResult.bKeyStates[PLAYER_KEY_ROLL])
                m_pOwner->Set_State(CPlayer::ROLL);
            else if (tKeyResult.bKeyStates[PLAYER_KEY_THROWSWORD])
                m_pOwner->Set_State(CPlayer::THROWSWORD);
            else if (tKeyResult.bKeyStates[PLAYER_KEY_MOVE])
                m_pOwner->Set_State(CPlayer::RUN);
        }
	}
	
}

void CPlayerState_Attack::Enter()
{
    COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_3D == eCoord)
	    m_vDirection = m_pOwner->Get_3DTargetDirection();
    else
		m_vDirection = EDir_To_Vector(m_pOwner->Get_2DDirection());
	Switch_To_AttackAnimation(m_iComboCount);
}

void CPlayerState_Attack::Exit()
{
}

void CPlayerState_Attack::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_iComboCount = 0;
	m_bCombo = false;
	m_pOwner->Set_State(CPlayer::IDLE);
}

void CPlayerState_Attack::Switch_To_AttackAnimation(_uint iComboCount)
{
    COORDINATE eCoord = m_pOwner->Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        F_DIRECTION eFDIr = EDir_To_FDir( m_pOwner->Get_2DDirection());
        switch (eFDIr)
        {
        case Client::F_DIRECTION::LEFT:
        case Client::F_DIRECTION::RIGHT:
            switch (m_iComboCount)
            {
            case 0:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACK_RIGHT);
                break;
            case 1:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_RIGHT);
                break;
            case 2:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_RIGHT);
                break;
            default:
                break;
            }
            break;
        case Client::F_DIRECTION::UP:
            switch (m_iComboCount)
            {
            case 0:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACK_UP);
                break;
            case 1:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_UP);
                break;
            case 2:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_UP);
                break;
            default:
                break;
            }
            break;
        case Client::F_DIRECTION::DOWN:
            switch (m_iComboCount)
            {
            case 0:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACK_DOWN);
                break;
            case 1:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_01_DOWN);
                break;
            case 2:
                m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ATTACKCOMBO_03_DOWN);
                break;
            default:
                break;
            }
            break;
        case Client::F_DIRECTION::F_DIR_LAST:
        default:
            break;
        }
    }
    else
    {
        switch (m_iComboCount)
        {
        case 0:
            m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ATTACK_01_GT_EDIT);
            break;
        case 1:
            m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ATTACK_02_GT);
            break;
        case 2:
            m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_ATTACK_03_GT);
            break;
        default:
            break;
        }
    }
}
