#include "stdafx.h"
#include "PlayerState_JumpDown.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Interactable.h"
#include "Effect_Manager.h"

CPlayerState_JumpDown::CPlayerState_JumpDown(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMP_DOWN)
{
	 m_fArmHeight = m_pOwner->Get_ArmHeight();
	 m_fHeadHeight = m_pOwner->Get_HeadHeight();
	 m_fArmLength = m_pOwner->Get_ArmLength();
}

void CPlayerState_JumpDown::Update(_float _fTimeDelta)
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();
	//바닥일 때
	if (m_pOwner->Is_OnGround())
	{

		m_pOwner->Stop_Rotate();
		if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_MOVE])
		{
			m_pOwner->Set_State(CPlayer::RUN);
			return;
		}
		//바닥에 방금 닿음
		if (false == m_bGrounded)
		{
			m_bGrounded = true;

			if (COORDINATE_3D == eCoord)
			{
				m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_LAND_02_GT);
				CEffect_Manager::GetInstance()->Active_Effect(TEXT("Dust_Jump"), true, m_pOwner->Get_Transform()->Get_WorldMatrix_Ptr());

			}

			m_pGameInstance->Start_SFX(_wstring(L"A_sfx_jot_land-") + to_wstring(rand() % 10), 30.f);
		}
		else
			m_pOwner->Set_State(CPlayer::IDLE);

		return;
	}
	// 이하 공중일 때
	_bool bCarrying = m_pOwner->Is_CarryingObject();
	//물건 들고있지 않으면?
	if (false == bCarrying)
	{
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
		}
		else if(INTERACT_RESULT::SUCCESS == eResult)
		{
			return;
		}

	}
	else
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT_THROWOBJECT])
		{
			m_pOwner->Set_State(CPlayer::THROWOBJECT);
			return;
		}
	}
	if (COORDINATE_3D == eCoord)
	{
		if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_MOVE])
		{
			//기어오르기 체크
			if (false == bCarrying && Try_Clamber())
				return;

			//공중 무빙
			m_pOwner->Add_Force(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed * _fTimeDelta );
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
			F_DIRECTION eFDir = EDir_To_FDir(eNewDir);

			if (m_eOldFDir != eFDir)
			{
				m_pOwner->Set_2DDirection(To_EDirection(tKeyResult.vDir));
				m_eOldFDir = eFDir;
			}
			m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed2D, _fTimeDelta);
		}
	}

}

void CPlayerState_JumpDown::Enter()
{
	m_bPlatformerMode = m_pOwner->Is_PlatformerMode();
	m_fAirRunSpeed = m_pOwner->Get_AirRunSpeed();
	m_fAirRotateSpeed = m_pOwner->Get_AirRotationSpeed();
	m_fAirRunSpeed2D = m_pOwner->Get_AirRunSpeed2D();
	Switch_To_JumpDownAnimation();

	m_fArmYPositionBefore = XMVectorGetY(m_pOwner->Get_FinalPosition()) + m_pOwner->Get_ArmHeight();
	m_fWallYPosition = -1;
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
	_bool bSword = m_pOwner->Is_SwordHandling();
	_bool bCarrying = m_pOwner->Is_CarryingObject();
	
	if (COORDINATE_2D == eCoord)
	{
		F_DIRECTION eDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
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
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_DOWN_02_GT);
	}


}

_bool CPlayerState_JumpDown::Try_Clamber()
{
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();

	//벽이 아직 감지되지 않음 -> 벽 감지
	if (m_fWallYPosition < 0)
	{
		//벽 감지하기
		_float3 vOrigin;
		XMStoreFloat3(&vOrigin, vPlayerPos + _vector{ 0,m_fHeadHeight,0 } + m_pOwner->Get_LookDirection() * m_fArmLength);
		_float3 vRayDir = { 0,-1,0 };
		list<CActorObject*> hitActors;
		list<RAYCASTHIT> raycasthits;
		_float fSlopeThreshold = m_pOwner->Get_StepSlopeThreshold();
		if (m_pGameInstance->RayCast(vOrigin, vRayDir, m_fHeadHeight, hitActors, raycasthits))
		{
			auto& iterHitPoint = raycasthits.begin();
			for (auto& pActor : hitActors)
			{
				if(OBJECT_GROUP::MAPOBJECT ==  pActor->Get_ObjectGroupID())
				if (iterHitPoint->vNormal.y < fSlopeThreshold)
				{
					iterHitPoint++;
					continue;
				}
				if (m_pOwner != pActor)
				{
					if (iterHitPoint->vPosition.y > m_fWallYPosition)
					{
						//OverlapTest 벽
						SHAPE_BOX_DESC tBoxDesc;
						tBoxDesc.vHalfExtents = { 0.25f,0.5f,0.25f };
						PxCapsuleGeometry pxGeom;
						m_pOwner->Get_ActorCom()->Get_Shapes()[0]->getCapsuleGeometry(pxGeom);
						_vector vTmp = { iterHitPoint->vPosition.x, iterHitPoint->vPosition.y, iterHitPoint->vPosition.z,1 };
						_vector vCheckPos =  XMVectorSetY(vTmp, XMVectorGetY(vTmp) + pxGeom.halfHeight + pxGeom.radius + 0.1f);
						list<CActorObject*> hitActors2;
						if (false == m_pGameInstance->Overlap(&pxGeom,
							vCheckPos, hitActors2))
						{
  							m_fWallYPosition = iterHitPoint->vPosition.y;
							m_vClamberEndPosition = vTmp;
						}
						else
						{
    							int a = 0;
						}
					}
				}
				iterHitPoint++;
			}
		}
	}
	//벽이 이미 감지됨 -> 팔 높이를 넘어가는지 체크
	else
	{
		//
		//현재 바닥 높이가 팔 높이보다 높고 이전 바닥 높이는 팔 높이보다 낮으면?
		//-> 기어오르기
		_float fArmYPositionCurrent = XMVectorGetY(vPlayerPos) + m_fArmHeight;
		if (m_fArmYPositionBefore> m_fWallYPosition
			&& fArmYPositionCurrent <= m_fWallYPosition)
		{
			_float3 vOrigin;
			XMStoreFloat3(&vOrigin, vPlayerPos + _vector{ 0,m_fArmHeight,0 } );
			_float3 vRayDir; 
			_vector vLook = m_pOwner->Get_LookDirection();
			XMStoreFloat3(&vRayDir, vLook);
			list<CActorObject*> hitActors;
			list<RAYCASTHIT> raycasthits;
			_vector vWallNormal = { 0,0,0,-1 };
			_vector vNewWallNormal;
			//WallNormal 찾기
			if (m_pGameInstance->RayCast(vOrigin, vRayDir, m_fArmLength + 1.f, hitActors, raycasthits))
			{
				auto& iterHitPoint = raycasthits.begin();
				for (auto& pActor : hitActors)
				{
					if (pActor == m_pOwner)
						continue;
					if (-1 == XMVectorGetW(vWallNormal)) //이전 벽 노말이 저장되지 않았을 때
					{
						vWallNormal = { iterHitPoint->vNormal.x,iterHitPoint->vNormal.y,iterHitPoint->vNormal.z,0 };
						continue;
					}
					else
						vNewWallNormal = { iterHitPoint->vNormal.x,iterHitPoint->vNormal.y,iterHitPoint->vNormal.z,0 };
					_float fDotBefore = XMVectorGetX(XMVector3Dot(vWallNormal, vLook));
					_float fDotAfter = XMVectorGetX(XMVector3Dot(vNewWallNormal, vLook));
					if (fDotBefore < fDotAfter)
					{
						vWallNormal = vNewWallNormal;
					}
					iterHitPoint++;
				}
			}
			if (0.f == XMVector3Length(vWallNormal).m128_f32[0])
			{
				return false;
			}
			m_pOwner->Set_ClamberEndPosition(m_vClamberEndPosition);
			m_pOwner->Set_WallNormal(vWallNormal);
			m_pOwner->Set_State(CPlayer::CLAMBER);
			return true;
		}
		else
		{
			m_fArmYPositionBefore = fArmYPositionCurrent;
		}
		
	}


	return false;
}
