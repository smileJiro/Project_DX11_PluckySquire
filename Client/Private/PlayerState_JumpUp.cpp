#include "stdafx.h"
#include "PlayerState_JumpUp.h"
#include "PlayerState_JumpDown.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "Interactable.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "ModelObject.h"


CPlayerState_JumpUp::CPlayerState_JumpUp(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMP_UP)
{
}

void CPlayerState_JumpUp::Update(_float _fTimeDelta)
{

	PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();

	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_MOVE])
		{
			//m_pOwner->Add_Force(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed * _fTimeDelta);
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed, _fTimeDelta);
			m_pOwner->Rotate_To(tKeyResult.vMoveDir, m_fAirRotateSpeed);
		}
		else
			m_pOwner->Stop_Rotate();


	}
	else
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_MOVE])
		{
			E_DIRECTION eNewDir = To_EDirection(tKeyResult.vMoveDir);
			F_DIRECTION eFDir = To_FDirection(eNewDir);

			if (m_eOldFDir != eFDir)
			{
				m_pOwner->Set_2DDirection(To_EDirection(tKeyResult.vDir));
				m_eOldFDir = eFDir;
			}
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed2D, _fTimeDelta);
		}
	}
	if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_ZETPROPEL])
	{
		m_pOwner->ZetPropel(_fTimeDelta);
	}
	_float fUpForce = m_pOwner->Get_UpForce();
	if (0.f > fUpForce)
	{
		m_pOwner->Set_State(CPlayer::JUMP_DOWN);
		return;
	}
	INTERACT_RESULT eResult = m_pOwner->Try_Interact(_fTimeDelta);
	if (INTERACT_RESULT::NO_INPUT == eResult
		|| INTERACT_RESULT::FAIL == eResult)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
		{
			m_pOwner->Set_State(CPlayer::ROLL);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_THROWSWORD])
		{
			m_pOwner->Set_State(CPlayer::THROWSWORD);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
		{
			if (m_pOwner->Is_PlatformerMode())
				m_pOwner->Set_State(CPlayer::ATTACK);
			else
				m_pOwner->Set_State(CPlayer::JUMP_ATTACK);
			return;
		}
		else if (tKeyResult.bInputStates[PLAYER_INPUT_THROWOBJECT])
		{
			m_pOwner->Set_State(CPlayer::THROWOBJECT);
			return;
		}
		return;
	}
	else if (INTERACT_RESULT::SUCCESS == eResult)
	{
		return;
	}

}

void CPlayerState_JumpUp::Enter()
{
	// ÅÂ¿õ Ãß°¡ 03.12
	m_pGameInstance->Erase_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::BLOCKER_JUMPPASS);
	m_pOwner->Set_Upforce(0.0f);
	m_fAirRunSpeed = m_pOwner->Get_AirRunSpeed();
	m_bPlatformerMode = m_pOwner->Is_PlatformerMode();
	m_fAirRotateSpeed = m_pOwner->Get_AirRotationSpeed();
	m_fAirRunSpeed2D = m_pOwner->Get_AirRunSpeed2D();
	m_pOwner->LookDirectionXZ_Dynamic(m_pOwner->Get_3DTargetDirection());
	m_pOwner->Get_ActorDynamic()->Set_LinearDamping(0.5f);
	Switch_JumpAnimation();

	if (m_pOwner->Is_OnGround())
	{

		m_pOwner->Jump();
		m_pOwner->ReFuel();
		m_pGameInstance->Start_SFX(_wstring(L"A_sfx_jot_vocal_jump-") + to_wstring(rand() % 12), 50.f);
		m_pGameInstance->Start_SFX(_wstring(L"A_sfx_jot_jump-") + to_wstring(rand() % 6), 20.f);
	}

	//if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	//	CEffect_Manager::GetInstance()->Active_EffectID(TEXT("Zip5"), true, m_pOwner->Get_Transform()->Get_WorldMatrix_Ptr(), 1);
}

void CPlayerState_JumpUp::Exit()
{
	// ÅÂ¿õÃß°¡ 03.12
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::BLOCKER_JUMPPASS);

	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_pOwner->Get_ActorDynamic()->Set_LinearDamping(0.f);
	if (COORDINATE_2D == eCoord)
	{
	}
	else
	{
		m_pOwner->Stop_Rotate();
		//CEffect_Manager::GetInstance()->Stop_Spawn(TEXT("Zip5"), 1.f);

	}
}

void CPlayerState_JumpUp::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

} 

void CPlayerState_JumpUp::Switch_JumpAnimation()
{

	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_bool bSword = m_pOwner->Is_SwordHandling();
	_bool bCarrying = m_pOwner->Is_CarryingObject();



	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eDir = To_FDirection(m_pOwner->Get_2DDirection());
		if (m_bPlatformerMode)
		{
			if (F_DIRECTION::UP == eDir)
				eDir = F_DIRECTION::RIGHT;
			else if (F_DIRECTION::DOWN == eDir)
				eDir = F_DIRECTION::LEFT;
		}
		switch (eDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_OBJECT_RIGHT);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_SWORD_RIGHT);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_OBJECT_UP);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_SWORD_UP);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_OBJECT_DOWN);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_SWORD_DOWN);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_RISE_DOWN);
			break;
		}

	}
	else	if (COORDINATE_3D == eCoord)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_UP_02_GT);
		CEffect_Manager::GetInstance()->Active_Effect(TEXT("Dust_Jump"), true, m_pOwner->Get_Transform()->Get_WorldMatrix_Ptr());
	}

}
