#include "stdafx.h"
#include "PlayerState_JumpDown.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"

CPlayerState_JumpDown::CPlayerState_JumpDown(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMP_DOWN)
{
}

void CPlayerState_JumpDown::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	//바닥일 때
	if (m_pOwner->Is_OnGround())
	{

		m_pOwner->Stop_Rotate();
		if (tKeyResult.bKeyStates[PLAYER_INPUT::PLAYER_KEY_MOVE])
		{
			m_pOwner->Set_State(CPlayer::RUN);
			return;
		}
		//바닥에 방금 닿음
		if (false == m_bGrounded)
		{
			m_bGrounded = true;

			if (COORDINATE_3D == eCoord)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_LAND_02_GT);
		}
		else
			m_pOwner->Set_State(CPlayer::IDLE);

		return;
	}

	// 이하 공중일 때

	if (tKeyResult.bKeyStates[PLAYER_KEY_ROLL])
	{
		m_pOwner->Set_State(CPlayer::ROLL);
		return;
	}
	else if (tKeyResult.bKeyStates[PLAYER_KEY_THROWSWORD])
	{
		m_pOwner->Set_State(CPlayer::THROWSWORD);
		return;
	}
	else if (tKeyResult.bKeyStates[PLAYER_KEY_ATTACK])
	{
		m_pOwner->Set_State(CPlayer::JUMP_ATTACK);
		return;
	}

	if (tKeyResult.bKeyStates[PLAYER_INPUT::PLAYER_KEY_MOVE])
	{
		//기어오르기 체크
		if (Try_Clamber())
			return;

		//공중 무빙
		m_pOwner->Add_Force(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed);
		m_pOwner->Rotate_To(tKeyResult.vMoveDir, m_fAirRotateSpeed);
	}
	else
		m_pOwner->Stop_Rotate();
}

void CPlayerState_JumpDown::Enter()
{
	Switch_To_JumpDownAnimation();
}

void CPlayerState_JumpDown::Exit()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
	}
	else
	{
		m_pOwner->Stop_Rotate();
	}
}

void CPlayerState_JumpDown::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{

	}
	else
	{
		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_LAND_02_GT == iAnimIdx)
			m_pOwner->Set_State(CPlayer::IDLE);
	}
}

void CPlayerState_JumpDown::Switch_To_JumpDownAnimation()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	_bool bSword = m_pOwner->Is_SwordEquiped();
	_bool bCarrying = m_pOwner->Is_CarryingObject();
	
	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		switch (eDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_OBJECT_RIGHT);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_SWORD_RIGHT);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_OBJECT_UP);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_SWORD_UP);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			if (bCarrying)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_OBJECT_DOWN);
			else if (bSword)
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_SWORD_DOWN);
			else
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_JUMP_FALL_DOWN);
			break;
		}
	}
	else	if (COORDINATE_3D == eCoord)
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_DOWN_02_GT);


}

_bool CPlayerState_JumpDown::Try_Clamber()
{
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
	_float fHeadHeight = m_pOwner->Get_HeadHeight();
	_vector vRayOrigin = vPlayerPos + _vector{ 0,fHeadHeight,0 } + m_pOwner->Get_LookDirection() * m_fArmLength;
	_float3 vOrigin;
	XMStoreFloat3(&vOrigin, vRayOrigin);
	_float3 vRayDir = { 0,-1,0 };
	list<CActorObject*> hitActors;
	list<_float3> hitPositions;

	if (m_pGameInstance->RayCast(vOrigin, vRayDir, 1.5, hitActors, hitPositions))
	{
		_float fClamberHeightCurrent = -1;
		auto& iterPosition = hitPositions.begin();
		for (auto& pActor : hitActors)
		{
			if (m_pOwner != pActor)
			{
				if (iterPosition->y > fClamberHeightCurrent)
				{
					fClamberHeightCurrent = iterPosition->y;
				}
			}
			iterPosition++;
		}

		//바닥이 몸통 범위 안에 있다.
		if (fClamberHeightCurrent > 0)
		{
			_float fClamberHeightBefore = XMVectorGetY(m_vClamberPosition);
			_float fArmHeight = XMVectorGetY(vPlayerPos) + m_fArmHeight;
			//현재 바닥 높이가 팔 높이보다 높고 이전 바닥 높이는 팔 높이보다 낮으면?
			//-> 기어오르기
			if (fArmHeight< fClamberHeightCurrent
				&& fArmHeight > fClamberHeightBefore)
			{
				m_vClamberPosition = { vOrigin.x, fClamberHeightCurrent, vOrigin.z };
				m_pOwner->Set_ClamberPosition(m_vClamberPosition);
				m_pOwner->Set_State(CPlayer::CLAMBER);
				return true;
			}
			else
			{
				m_vClamberPosition = { vOrigin.x, fClamberHeightCurrent, vOrigin.z };
			}
		}
	}
	return false;
}
