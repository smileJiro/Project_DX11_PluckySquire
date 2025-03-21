#include "stdafx.h"
#include "PlayerState_Roll.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"
#include "Effect_Manager.h"
#include "ModelObject.h"


CPlayerState_Roll::CPlayerState_Roll(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ROLL)
{
}

void CPlayerState_Roll::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	_float fProgress = m_pOwner->Get_AnimProgress();
	_float fForwardStartProgress = eCoord == COORDINATE_2D ? m_f2DForwardStartProgress : m_f3DForwardStartProgress;
	_float fForwardEndProgress= eCoord == COORDINATE_2D ? m_f2DForwardEndProgress : m_f3DForwardEndProgress;
	_float fMotionCancelProgress = eCoord == COORDINATE_2D ? m_f2DMotionCancelProgress : m_f3DMotionCancelProgress;
	m_pOwner->Stop_Rotate();
	if (fProgress >= fMotionCancelProgress)
	{
		 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();

		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
		{
			m_pOwner->Set_State(CPlayer::RUN);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
		{
			m_pOwner->Set_State(CPlayer::ATTACK);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_JUMP])
		{
			if(false == m_pOwner->Is_PlatformerMode())
				m_pOwner->Set_State(CPlayer::JUMP_UP);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
		{
			m_pOwner->Set_State(CPlayer::ROLL);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_THROWSWORD])
		{
			m_pOwner->Set_State(CPlayer::THROWSWORD);
			return;
		}

	}
	if (fProgress >= fForwardStartProgress)
	{
		if (fProgress <= fForwardEndProgress)
		{
			_float fRatio = (fProgress - fForwardStartProgress) / (fForwardEndProgress - fForwardStartProgress);
			_float fMaxSpeed = eCoord == COORDINATE_2D ? m_f2DForwardSpeedMax : m_f3DForwardSpeedMax;
			_float fMinSpeed = eCoord == COORDINATE_2D ? m_f2DForwardSpeedMin : m_f3DForwardSpeedMin;
			_float fSpeed = fMaxSpeed - (fMaxSpeed - fMinSpeed) * fRatio;
			//m_pOwner->Set_Upforce(0.f);
			m_pOwner->Move((m_vDirection)*fSpeed, _fTimeDelta);
		}
		else
		{
			m_pOwner->Stop_Move();
		}
	}
	
	
}

void CPlayerState_Roll::Enter()
{

    COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_bool bSwrodEquiped = m_pOwner->Is_SwordHandling();

    if (COORDINATE_2D == eCoord)
    {
		F_DIRECTION eFDir = To_FDirection( m_pOwner->Get_2DDirection());
		m_vDirection = EDir_To_Vector(m_pOwner->Get_2DDirection());
		if (m_pOwner->Is_PlatformerMode())
		{
			if (F_DIRECTION::UP == eFDir)
			{
				eFDir = F_DIRECTION::RIGHT;
				m_vDirection = _vector{ 1,0,0 };
			}
			else if (F_DIRECTION::DOWN == eFDir)
			{
				eFDir = F_DIRECTION::LEFT;
				m_vDirection = _vector{ -1,0,0 };
			}

		}
        switch (eFDir)
        {
        case Client::F_DIRECTION::LEFT:
        case Client::F_DIRECTION::RIGHT:
            bSwrodEquiped ?
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_RIGHT_SINGLEROLLONLY)
                : m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_RIGHT_NOSWORD);

            break;
        case Client::F_DIRECTION::UP:
            bSwrodEquiped ?
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_UP_SINGLEROLLONLY)
                : m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_UP_NOSWORD);

            break;
        case Client::F_DIRECTION::DOWN:
            bSwrodEquiped ?
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_DOWN_SINGLEROLLONLY)
                : m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_DODGEROLL_DOWN_NOSWORD);
            break;
        case Client::F_DIRECTION::F_DIR_LAST:
        default:
            break;
        }
    }
    else
    {
		 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();
		if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
			m_vDirection = m_pOwner->Get_3DTargetDirection();
		else
			m_vDirection = m_pOwner->Get_LookDirection();
		m_pOwner->LookDirectionXZ_Dynamic(m_vDirection);
		m_pOwner->Stop_Rotate();
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_DODGE_GT);

		CEffect_Manager::GetInstance()->Active_Effect(TEXT("Dodge"), true,m_pOwner->Get_Transform()->Get_WorldMatrix_Ptr());
		//CEffect_Manager::GetInstance()->Active_EffectID(TEXT("Zip5"), false, m_pOwner->Get_Transform()->Get_WorldMatrix_Ptr(), 1);

    }

	//if (static_cast<CPlayer*>(m_pOwner)->Is_OnGround())
	m_pGameInstance->Start_SFX(_wstring(L"A_sfx_jot_roll-") + to_wstring(rand() % 7), 20.f);
}

void CPlayerState_Roll::Exit()
{
	//if (COORDINATE_2D == m_pOwner->Get_CurCoord())
	//	CEffect_Manager::GetInstance()->Stop_Spawn(TEXT("Zip5"), 1.f);

	static_cast<CModelObject*>(m_pOwner->Get_PartObject(CPlayer::PART::PART_BODY))->Off_Trail();
}

void CPlayerState_Roll::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (m_pOwner->Is_OnGround())
		m_pOwner->Set_State(CPlayer::IDLE);
	else
		m_pOwner->Set_State(CPlayer::JUMP_DOWN);
}
