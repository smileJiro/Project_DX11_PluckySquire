#include "stdafx.h"
#include "PlayerState_JumpDown.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Interactable.h"
#include "Effect_Manager.h"
#include "ModelObject.h"
#include "PlayerBody.h"

#define CLAMBER_OVERLAP_OFFSET 0.1f

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
	if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_ZETPROPEL])
	{
		m_pOwner->ZetPropel(_fTimeDelta);
	}
	_float fUpForce = m_pOwner->Get_UpForce();
	_bool bReverse = m_pBody->Is_ReverseAnimation();
	if (false == bReverse && 0.f < fUpForce)
	{
		m_pBody->Set_ReverseAnimation(true);
	}
	else if (bReverse && 0.f > fUpForce)
	{
		m_pBody->Set_ReverseAnimation(false);
	}

		// 이하 공중일 때
	_bool bCarrying = m_pOwner->Is_CarryingObject();

	if (COORDINATE_3D == eCoord)
	{


		if (tKeyResult.bInputStates[PLAYER_INPUT::PLAYER_INPUT_MOVE])
		{
			//기어오르기 체크
			if (false == bCarrying && Try_Clamber())
				return;

			//공중 무빙
			//m_pOwner->Add_Force(XMVector3Normalize(tKeyResult.vMoveDir) * m_fAirRunSpeed * _fTimeDelta );
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
			return;
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

}

void CPlayerState_JumpDown::Enter()
{
	m_bPlatformerMode = m_pOwner->Is_PlatformerMode();
	m_fAirRunSpeed = m_pOwner->Get_AirRunSpeed();
	m_fAirRotateSpeed = m_pOwner->Get_AirRotationSpeed();
	m_fAirRunSpeed2D = m_pOwner->Get_AirRunSpeed2D();
	Switch_To_JumpDownAnimation();

	m_pOwner->Get_ActorDynamic()->Set_LinearDamping(0.5f);
	m_fArmYPositionBefore = XMVectorGetY(m_pOwner->Get_FinalPosition()) + m_pOwner->Get_ArmHeight();
	m_fWallYPosition = -1;
	m_pBody = m_pOwner->Get_Body();
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
	m_pOwner->Get_ActorDynamic()->Set_LinearDamping(0.f);
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

	if (m_fWallYPosition < 0.f)
	{
		if (DetectWall(vPlayerPos))
		{
			return false; // 벽만 감지했을 뿐, 바로 Clamber 시작은 아님
		}
	}
	else
	{
		if (Check_ClamberCondition(vPlayerPos))
		{
			StartClamber();
			return true;
		}
	}

	return false;
}
_bool CPlayerState_JumpDown::DetectWall(const _vector& vPlayerPos)
{
	_float3 vStart;
	XMStoreFloat3(&vStart, vPlayerPos + _vector{ 0.f, m_fHeadHeight, 0.f } + m_pOwner->Get_LookDirection() * m_fArmLength);
	_float3 vRayDir = { 0.f, -1.f, 0.f };

	list<CActorObject*> hitActors;
	list<RAYCASTHIT> hitInfos;

	if (!m_pGameInstance->RayCast(vStart, vRayDir, m_fHeadHeight, hitActors, hitInfos))
		return false;

	auto hitInfoIter = hitInfos.begin();
	for (auto& pActor : hitActors)
	{
		if (pActor->Get_ObjectGroupID() != OBJECT_GROUP::MAPOBJECT)
		{
			++hitInfoIter;
			continue;
		}

		if (hitInfoIter->vNormal.y < m_pOwner->Get_StepSlopeThreshold())
		{
			++hitInfoIter;
			continue;
		}

		if (pActor == m_pOwner)
		{
			++hitInfoIter;
			continue;
		}

		if (hitInfoIter->vPosition.y > m_fWallYPosition)
		{
			if (false == Is_OverlapOnClamber(hitInfoIter->vPosition))
			{
				m_fWallYPosition = hitInfoIter->vPosition.y;
				m_vClamberEndPosition = XMVectorSetW( XMLoadFloat3(&hitInfoIter->vPosition),1.f);
			}
		}

		++hitInfoIter;
	}

	return true;
}

//벽 위에 플레이어가 올라갈 공간이 있는지 체크
_bool CPlayerState_JumpDown::Is_OverlapOnClamber(const _float3& _vWallPosition)
{
	PxCapsuleGeometry capsuleGeom;
	m_pOwner->Get_ActorCom()->Get_Shapes()[0]->getCapsuleGeometry(capsuleGeom);

	_vector vCheckPos = XMVectorSetW(XMLoadFloat3( &_vWallPosition), 1.f);
	vCheckPos = XMVectorSetY(vCheckPos, XMVectorGetY(vCheckPos) + capsuleGeom.halfHeight + capsuleGeom.radius + CLAMBER_OVERLAP_OFFSET);

	list<CActorObject*> overlapActors;
	return m_pGameInstance->Overlap(&capsuleGeom, vCheckPos, overlapActors);
}

_bool CPlayerState_JumpDown::Check_ClamberCondition(const _vector& _vPlayerPos)
{
	_float fCurrentArmY = XMVectorGetY(_vPlayerPos) + m_fArmHeight;

	if (m_fArmYPositionBefore > m_fWallYPosition && fCurrentArmY <= m_fWallYPosition)
	{
		m_fArmYPositionBefore = fCurrentArmY;
		return true;
	}

	m_fArmYPositionBefore = fCurrentArmY;
	return false;
}

void CPlayerState_JumpDown::StartClamber()
{
	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
	_vector vLookDir = m_pOwner->Get_LookDirection();

	_float3 vRayOrigin, vRayDir;
	XMStoreFloat3(&vRayOrigin, vPlayerPos + _vector{ 0.f, m_fArmHeight, 0.f });
	XMStoreFloat3(&vRayDir, vLookDir);
	list<CActorObject*> hitActors;
	list<RAYCASTHIT> hitInfos;
	if (!m_pGameInstance->RayCast(vRayOrigin, vRayDir, m_fArmLength + 1.f, hitActors, hitInfos))
		return;

	_vector vBestNormal = { 0.f, 0.f, 0.f, -1.f }; // W 속성은 아직 WallNormal을 못 찾았다는 표시
	_vector vTempNormal = { 0.f, 0.f, 0.f, 0.f };

	auto hitInfoIter = hitInfos.begin();
	for (auto& pActor : hitActors)
	{
		if (pActor == m_pOwner)
		{
			++hitInfoIter;
			continue;
		}

		if (-1.f == XMVectorGetW(vBestNormal))//이전 벽 노말이 저장되지 않았을 때
		{
			vBestNormal = XMLoadFloat3(&hitInfoIter->vNormal);
			++hitInfoIter;
			continue;
		}

		vTempNormal = XMLoadFloat3(&hitInfoIter->vNormal);

		_float fDotBefore = XMVectorGetX(XMVector3Dot(vBestNormal, vLookDir));
		_float fDotAfter = XMVectorGetX(XMVector3Dot(vTempNormal, vLookDir));
		//바라보는 방향과 더 가까운 벽 찾기
		if (fDotBefore < fDotAfter)
		{
			vBestNormal = vTempNormal;
		}

		++hitInfoIter;
	}

	if (0.f == XMVectorGetX(XMVector3Length(vBestNormal)))
		return;

	//벽타기가 끝나면 위치할 지점 저장
	m_pOwner->Set_ClamberEndPosition(m_vClamberEndPosition);
	//벽의 Normal(플레이어가 바라봐야 할 방향) 저장
	m_pOwner->Set_WallNormal(XMVectorSetW(vBestNormal, 0.f));
	m_pOwner->Set_State(CPlayer::CLAMBER);
}