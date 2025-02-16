#include "stdafx.h"
#include "PlayerState_Run.h"
#include "PlayerState_Idle.h"
#include "GameInstance.h"
#include "Player.h"
#include "StateMachine.h"


CPlayerState_Run::CPlayerState_Run(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::RUN)
{
}

void CPlayerState_Run::Update(_float _fTimeDelta)
{
	//Foot_On();

	_float fUpForce = m_pOwner->Get_UpForce();
	_bool bOnGround = m_pOwner->Is_OnGround();
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	if (COORDINATE_3D == eCoord)
	{
		_float fUpForce = m_pOwner->Get_UpForce();
		_bool bOnGround = m_pOwner->Is_OnGround();
		if (false == bOnGround && -0.001 > fUpForce)
		{
			m_pOwner->Set_State(CPlayer::JUMP_DOWN);
			return;
		}
	}

	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	_bool bSneak = tKeyResult.bInputStates[PLAYER_INPUT_SNEAK];
	if (tKeyResult.bInputStates[PLAYER_INPUT_MOVE])
	{
		_float fMoveSpeed = m_pOwner->Get_MoveSpeed(eCoord);;
		COORDINATE eCoord = m_pOwner->Get_CurCoord();
		if (COORDINATE_2D == eCoord)
		{
			E_DIRECTION eNewDir = To_EDirection(tKeyResult.vMoveDir);
			F_DIRECTION eFDir = EDir_To_FDir(eNewDir);
			m_pOwner->Set_2DDirection(To_EDirection(tKeyResult.vDir));

			if (m_eOldFDir != eFDir)
			{
				Switch_RunAnimation2D(eFDir);
				m_eOldFDir = eFDir;
			}
		}
		else
		{
			m_pOwner->Rotate_To(XMVector3Normalize(tKeyResult.vMoveDir), m_fRotateSpeed);
			if (bSneak != m_bSneakBefore)
			{
				Switch_RunAnimation3D(bSneak);
				m_bSneakBefore = bSneak;
			}
			fMoveSpeed  = bSneak ? fMoveSpeed * 0.5f : fMoveSpeed;
		}
		m_pOwner->Move(XMVector3Normalize(tKeyResult.vMoveDir)* fMoveSpeed, _fTimeDelta);

		if (tKeyResult.bInputStates[PLAYER_INPUT_ATTACK])
			m_pOwner->Set_State(CPlayer::ATTACK);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_SPINATTACK])
			m_pOwner->Set_State(CPlayer::SPINATTACK);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_JUMP])
			m_pOwner->Set_State(CPlayer::JUMP_UP);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_ROLL])
			m_pOwner->Set_State(CPlayer::ROLL);
		else if (tKeyResult.bInputStates[PLAYER_INPUT_THROWSWORD])
			m_pOwner->Set_State(CPlayer::THROWSWORD);
		else	if (tKeyResult.bInputStates[PLAYER_INPUT_THROWOBJECT])
			m_pOwner->Set_State(CPlayer::THROWOBJECT);
		return;
	}
	else
	{
		m_pOwner->Set_State(CPlayer::IDLE);
		return;
	}
}

void CPlayerState_Run::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	PLAYER_INPUT_RESULT tKeyResult = m_pOwner->Player_KeyInput();
	m_bSneakBefore = tKeyResult.bInputStates[PLAYER_INPUT_SNEAK];
	if (COORDINATE_2D == eCoord)
	{
		m_bPlatformerMode = m_pOwner->Is_PlatformerMode();
	}
	else
	{
		 PLAYER_INPUT_RESULT tKeyResult  = m_pOwner->Player_KeyInput();
		Switch_RunAnimation3D(tKeyResult.bInputStates[PLAYER_INPUT_SNEAK]);
	}

}

void CPlayerState_Run::Exit()
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

void CPlayerState_Run::Switch_RunAnimation2D(F_DIRECTION _eFDir)
{
	_bool bSword = m_pOwner->Is_SwordHandling();
	_bool bCarrying = m_pOwner->Is_CarryingObject();
	if (m_bPlatformerMode)
	{
		if (F_DIRECTION::UP == _eFDir)
			_eFDir = F_DIRECTION::RIGHT;
		else if (F_DIRECTION::DOWN == _eFDir)
			_eFDir = F_DIRECTION::LEFT;
	}
	switch (_eFDir)
	{
	case Client::F_DIRECTION::LEFT:
	case Client::F_DIRECTION::RIGHT:
		if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_OBJECT_RIGHT);
		else if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_RIGHT);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_RIGHT);
		break;
	case Client::F_DIRECTION::UP:
		if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_OBJECT_UP);
		else if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_UP);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_UP);
		break;
	case Client::F_DIRECTION::DOWN:
		if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_OBJECT_DOWN);
		else if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_SWORD_DOWN);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_DOWN);
		break;
	case Client::F_DIRECTION::F_DIR_LAST:
	default:
		break;
	}
}

void CPlayerState_Run::Switch_RunAnimation3D(_bool _bStealth)
{
	if (_bStealth)
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_STEALTH_RUN_GT);
	else
	{
		_bool bSword = m_pOwner->Is_SwordHandling();
		_bool bCarrying = m_pOwner->Is_CarryingObject();
		if (bSword)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_RUN_SWORD_01_GT);
		else if (bCarrying)
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_RUN_01_GT);
		else
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_RUN_01_GT);
	}
}

//_bool CPlayerState_Run::Foot_On()
//{
//	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
//
//	_bool isRaycast = false;
//	_float fFootHitPointY = 0.f;
//	_float3 vOrigin;
//	_float3 vOriginLeft;
//	_float3 vOriginRight;
//	PxCapsuleGeometry capsule;
//	m_pOwner->Get_ActorCom()->Get_Shapes()[CPlayer::SHAPE_FOOT]->getCapsuleGeometry(capsule);
//
//	XMStoreFloat3(&vOrigin, vPlayerPos + capsule.radius * XMVectorSetY(m_pOwner->Get_LookDirection(), 0.f));
//	vOrigin.y += capsule.radius * 2.f;
//	_vector vSide = XMVector3Cross(XMVectorSetY(m_pOwner->Get_LookDirection(), 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
//	XMStoreFloat3(&vOriginLeft, XMLoadFloat3(&vOrigin) - capsule.halfHeight * XMVector3Normalize(vSide));
//	XMStoreFloat3(&vOriginRight, XMLoadFloat3(&vOrigin) + capsule.halfHeight * XMVector3Normalize(vSide));
//	_float3 vRayDir = { 0,-1,0 };
//	list<CActorObject*> hitActors;
//	list<RAYCASTHIT> raycasthits;
//	_float fHeightThreshold = m_pOwner->Get_FootHeightThreshold();
//
//	if (m_pGameInstance->RayCast(vOrigin, vRayDir, capsule.radius * 2.f, hitActors, raycasthits))
//	{
//		
//		auto& iterHitPoint = raycasthits.begin();
//		for (auto& pActor : hitActors)
//		{
//			if (iterHitPoint->vPosition.y <= vPlayerPos.m128_f32[1])
//			{
//				iterHitPoint++;
//				continue;
//			}
//			if (m_pOwner != pActor)
//			{
//				if (iterHitPoint->vPosition.y - fHeightThreshold <= vPlayerPos.m128_f32[1])
//				{
//					if (fFootHitPointY < iterHitPoint->vPosition.y)
//					{
//						fFootHitPointY = iterHitPoint->vPosition.y;
//						isRaycast = true;
//					}
//
//					//m_pOwner->Get_ActorCom()->Set_GlobalPose(_float3(vPlayerPos.m128_f32[0], iterHitPoint->vPosition.y, vPlayerPos.m128_f32[2]));
//				}
//			}
//			iterHitPoint++;
//		}
//	}
//
//	if (m_pGameInstance->RayCast(vOriginLeft, vRayDir, capsule.radius*2.f, hitActors, raycasthits))
//	{
//		
//		auto& iterHitPoint = raycasthits.begin();
//		for (auto& pActor : hitActors)
//		{
//			if (iterHitPoint->vPosition.y <= vPlayerPos.m128_f32[1])
//			{
//				iterHitPoint++;
//				continue;
//			}
//			if (m_pOwner != pActor)
//			{
//				if (iterHitPoint->vPosition.y - fHeightThreshold <= vPlayerPos.m128_f32[1])
//				{
//					if (fFootHitPointY < iterHitPoint->vPosition.y)
//					{
//						fFootHitPointY = iterHitPoint->vPosition.y;
//						isRaycast = true;
//					}
//
//					//m_pOwner->Get_ActorCom()->Set_GlobalPose(_float3(vPlayerPos.m128_f32[0], iterHitPoint->vPosition.y, vPlayerPos.m128_f32[2]));
//				}
//			}
//			iterHitPoint++;
//		}
//	}
//
//	if (m_pGameInstance->RayCast(vOriginRight, vRayDir, capsule.radius*2.f, hitActors, raycasthits))
//	{
//		
//		auto& iterHitPoint = raycasthits.begin();
//		for (auto& pActor : hitActors)
//		{
//			if (iterHitPoint->vPosition.y <= vPlayerPos.m128_f32[1])
//			{
//				iterHitPoint++;
//				continue;
//			}
//			if (m_pOwner != pActor)
//			{
//				if (iterHitPoint->vPosition.y - fHeightThreshold <= vPlayerPos.m128_f32[1])
//				{
//					if (fFootHitPointY < iterHitPoint->vPosition.y)
//					{
//						fFootHitPointY = iterHitPoint->vPosition.y;
//						isRaycast = true;
//					}
//					
//					//m_pOwner->Get_ActorCom()->Set_GlobalPose(_float3(vPlayerPos.m128_f32[0], iterHitPoint->vPosition.y, vPlayerPos.m128_f32[2]));
//				}
//			}
//			iterHitPoint++;
//		}
//	}
//
//	if(isRaycast && fFootHitPointY != vPlayerPos.m128_f32[1])
//	{
//		//m_pOwner->Get_ActorCom()->Set_GlobalPose(_float3(vPlayerPos.m128_f32[0], fFootHitPointY, vPlayerPos.m128_f32[2]));
//		CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom());
//		_vector vVelocity = pDynamicActor->Get_LinearVelocity();
//		vVelocity.m128_f32[1] += fFootHitPointY - vPlayerPos.m128_f32[1];
//		pDynamicActor->Set_LinearVelocity(vVelocity);
//	}
//
//	return true;
//}

