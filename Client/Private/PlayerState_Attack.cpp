#include "stdafx.h"
#include "PlayerState_Attack.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"

CPlayerState_Attack::CPlayerState_Attack(CPlayer* _pOwner, E_DIRECTION _eDirection)
	:CPlayerState(_pOwner, CPlayer::ATTACK)
{
	m_eDirection = _eDirection;
}

void CPlayerState_Attack::Update(_float _fTimeDelta)
{
	if (MOUSE_DOWN(MOUSE_KEY::LB))
	{
		m_bCombo = true;
	}

    if (KEY_PRESSING(KEY::LSHIFT))
    {
        m_pOwner->Set_State(CPlayer::ROLL);
        return;
    }
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	//0.3이상일 때 공격버튼이 눌린 적 있었으면?
	_float fProgress =m_pOwner->Get_AnimProgress();
	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	_float fForwardingProgress = eCoord == COORDINATE_2D ? m_f2DForwardingProgress : m_f3DForwardingProgress;
	if (fProgress >= fMotionCancelProgress)
	{
        if (KEY_PRESSING(KEY::SPACE))
        {
            m_pOwner->Set_State(CPlayer::JUMP);
            return;
        }
		if (m_bCombo)
		{
			m_iComboCount++;
			if (2 >= m_iComboCount)
			{
                Switch_To_AttackAnimation(m_iComboCount);
			}
			m_bCombo = false;
		}

	}
	else if(fProgress >= fForwardingProgress)
	{
		m_pOwner->Stop_Move();
	}
	else
	{
        switch (eCoord)
        {
        case Engine::COORDINATE_2D:
            m_pOwner->Move(EDir_To_Vector(m_eDirection), _fTimeDelta);
            break;
        case Engine::COORDINATE_3D:
            m_pOwner->Move_Forward(m_f3DForwardSpeed, _fTimeDelta);
            break;
        }
	}
	
}

void CPlayerState_Attack::Enter()
{
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
