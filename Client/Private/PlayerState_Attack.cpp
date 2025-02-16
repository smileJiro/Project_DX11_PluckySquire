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
    m_pOwner->Stop_Rotate();
    PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
    if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
    {
        m_bCombo = true;
    }
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	_float fProgress =m_pOwner->Get_AnimProgress();
	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	if (COORDINATE_2D == eCoord &&
        fProgress >= m_f2DForwardStartProgress && fProgress <= m_f2DForwardEndProgress)
	{

        if (m_pOwner->Is_PlatformerMode() )
        {
            if (false == m_bRised && false == m_pOwner->Is_OnGround())
            {
                m_pOwner->Add_Upforce(m_f2DRisingForce);
                m_bRised = true;
            }
        }
        else
            m_pOwner->Move(EDir_To_Vector( m_pOwner->Get_2DDirection())  *m_f2DForwardSpeed, _fTimeDelta);
	}
    //cout << "combo : "<< m_iComboCount << " / Progress : " << fProgress << endl;
	if (fProgress >= fMotionCancelProgress)
	{
        if(m_pOwner->Is_AttackTriggerActive())
        {
            m_pOwner->End_Attack();
        }
        if (m_bCombo)
        {
            m_iComboCount++;
            if (2 >= m_iComboCount)
            {
                if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
                {
                    if (COORDINATE_2D == eCoord)
                    {
 
                        E_DIRECTION eNewDir = To_EDirection(tKeyResult.vMoveDir);
                        F_DIRECTION eFDir = EDir_To_FDir(eNewDir);
                        m_pOwner->Set_2DDirection(To_EDirection(tKeyResult.vDir));
                    }

                }
                Switch_To_AttackAnimation(m_iComboCount);
                m_pOwner->Start_Attack((CPlayer::ATTACK_TYPE)(CPlayer::ATTACK_TYPE_NORMAL1 + m_iComboCount));

            }
            m_bCombo = false;
        }
        else
        {
            if (tKeyResult.bInputStates[PLAYER_INPUT_JUMP])
                m_pOwner->Set_State(CPlayer::JUMP_UP);
            else if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
                m_pOwner->Set_State(CPlayer::ROLL);
            else if (tKeyResult.bInputStates[PLAYER_INPUT_THROWSWORD])
                m_pOwner->Set_State(CPlayer::THROWSWORD);

        }
	}
	
}

void CPlayerState_Attack::Enter()
{
     PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();
    COORDINATE eCoord = m_pOwner->Get_CurCoord();
    if (COORDINATE_3D == eCoord)
    {
        _vector vDir;
        if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
            vDir = m_pOwner->Get_3DTargetDirection();
        else
            vDir = m_pOwner->Get_LookDirection();
        m_pOwner->LookDirectionXZ_Dynamic(vDir);
        m_pOwner->Stop_Rotate();
    }
    else
    {
	    m_f2DForwardSpeed = m_pOwner->Get_2DAttackForwardingSpeed();

    }

	Switch_To_AttackAnimation(m_iComboCount);
    m_pOwner->Start_Attack(CPlayer:: ATTACK_TYPE_NORMAL1);
}

void CPlayerState_Attack::Exit()
{
}

void CPlayerState_Attack::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_iComboCount = 0;
	m_bCombo = false;
    m_bRised = false;
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
