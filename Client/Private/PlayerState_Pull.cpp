#include "stdafx.h"
#include "PlayerState_Pull.h"
#include "GameInstance.h"

CPlayerState_Pull::CPlayerState_Pull(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::ELECTRIC)
{
}

void CPlayerState_Pull::Update(_float _fTimeDelta)
{
	INTERACT_RESULT eResult = m_pOwner->Try_Interact(_fTimeDelta);

	if (eResult == INTERACT_RESULT::CHARGE_CANCEL)
	{
		m_pOwner->Set_State(CPlayer::IDLE);
	}
}

void CPlayerState_Pull::Enter()
{
	assert(COORDINATE_2D == m_pOwner->Get_CurCoord());
	START_SFX_DELAY(TEXT("A_sfx_JotLeverPulled"), 0.0f, g_SFXVolume, true);
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_PULL_LOOP);
}

void CPlayerState_Pull::Exit()
{
	END_SFX(TEXT("A_sfx_JotLeverPulled"));
}

void CPlayerState_Pull::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
	END_SFX(TEXT("A_sfx_JotLeverPulled"));
	m_pOwner->Get_InteractableObject()->Cancel_Interact(m_pOwner);
	m_pOwner->Set_State(CPlayer::IDLE);
}
