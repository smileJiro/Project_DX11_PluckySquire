#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_JumpDown :
    public CPlayerState
{
public:
    CPlayerState_JumpDown(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)override;

    void Switch_To_JumpDownAnimation();
private:

	_bool Try_Clamber();
    _bool DetectWall(const _vector& _vPlayerPos);
    _bool Is_OverlapOnClamber(const _float3& _vWallPosition);
    _bool Check_ClamberCondition(const _vector& _vPlayerPos);
    void StartClamber();
private:
    CModelObject* m_pBody = nullptr;
    F_DIRECTION m_eOldFDir = F_DIRECTION::F_DIR_LAST;
    _bool m_bPlatformerMode = false;
    _bool m_bGrounded = false;
    _float m_fAirRunSpeed = 1000.f;
    _float m_fAirRunSpeed2D = 300.f;
    _float m_fAirRotateSpeed = 100.f;
    //벽 기어오르기 관련
    _float m_fWallYPosition = -1;
	_vector m_vClamberEndPosition= { 0,0,0,1 };

    _float m_fArmYPositionBefore = -1;
    _float m_fArmHeight = 0;
    _float m_fHeadHeight = 0;
    _float m_fArmLength = 0;
};

END


//
//_bool CPlayerState_JumpDown::Try_Clamber()
//{
//	_vector vPlayerPos = m_pOwner->Get_FinalPosition();
//
//	//벽이 아직 감지되지 않음 -> 벽 감지
//	if (m_fWallYPosition < 0)
//	{
//		//벽 감지하기
//		_float3 vOrigin;
//		XMStoreFloat3(&vOrigin, vPlayerPos + _vector{ 0,m_fHeadHeight,0 } + m_pOwner->Get_LookDirection() * m_fArmLength);
//		_float3 vRayDir = { 0,-1,0 };
//		list<CActorObject*> hitActors;
//		list<RAYCASTHIT> raycasthits;
//		_float fSlopeThreshold = m_pOwner->Get_StepSlopeThreshold();
//		if (m_pGameInstance->RayCast(vOrigin, vRayDir, m_fHeadHeight, hitActors, raycasthits))
//		{
//			auto& iterHitPoint = raycasthits.begin();
//			for (auto& pActor : hitActors)
//			{
//				if (OBJECT_GROUP::MAPOBJECT == pActor->Get_ObjectGroupID())
//					if (iterHitPoint->vNormal.y < fSlopeThreshold)
//					{
//						iterHitPoint++;
//						continue;
//					}
//				if (m_pOwner != pActor)
//				{
//					if (iterHitPoint->vPosition.y > m_fWallYPosition)
//					{
//						//OverlapTest 벽
//						SHAPE_BOX_DESC tBoxDesc;
//						tBoxDesc.vHalfExtents = { 0.25f,0.5f,0.25f };
//						PxCapsuleGeometry pxGeom;
//						m_pOwner->Get_ActorCom()->Get_Shapes()[0]->getCapsuleGeometry(pxGeom);
//						_vector vTmp = { iterHitPoint->vPosition.x, iterHitPoint->vPosition.y, iterHitPoint->vPosition.z,1 };
//						_vector vCheckPos = XMVectorSetY(vTmp, XMVectorGetY(vTmp) + pxGeom.halfHeight + pxGeom.radius + 0.1f);
//						list<CActorObject*> hitActors2;
//						if (false == m_pGameInstance->Overlap(&pxGeom,
//							vCheckPos, hitActors2))
//						{
//							m_fWallYPosition = iterHitPoint->vPosition.y;
//							m_vClamberEndPosition = vTmp;
//						}
//					}
//				}
//				iterHitPoint++;
//			}
//		}
//	}
//	//벽이 이미 감지됨 -> 팔 높이를 넘어가는지 체크
//	else
//	{
//		//
//		//현재 바닥 높이가 팔 높이보다 높고 이전 바닥 높이는 팔 높이보다 낮으면?
//		//-> 기어오르기
//		_float fArmYPositionCurrent = XMVectorGetY(vPlayerPos) + m_fArmHeight;
//		if (m_fArmYPositionBefore > m_fWallYPosition
//			&& fArmYPositionCurrent <= m_fWallYPosition)
//		{
//			_float3 vOrigin;
//			XMStoreFloat3(&vOrigin, vPlayerPos + _vector{ 0,m_fArmHeight,0 });
//			_float3 vRayDir;
//			_vector vLook = m_pOwner->Get_LookDirection();
//			XMStoreFloat3(&vRayDir, vLook);
//			list<CActorObject*> hitActors;
//			list<RAYCASTHIT> raycasthits;
//			_vector vWallNormal = { 0,0,0,-1 };
//			_vector vNewWallNormal;
//			//WallNormal 찾기
//			if (m_pGameInstance->RayCast(vOrigin, vRayDir, m_fArmLength + 1.f, hitActors, raycasthits))
//			{
//				auto& iterHitPoint = raycasthits.begin();
//				for (auto& pActor : hitActors)
//				{
//					if (pActor == m_pOwner)
//						continue;
//					if (-1 == XMVectorGetW(vWallNormal)) //이전 벽 노말이 저장되지 않았을 때
//					{
//						vWallNormal = { iterHitPoint->vNormal.x,iterHitPoint->vNormal.y,iterHitPoint->vNormal.z,0 };
//						continue;
//					}
//					else
//						vNewWallNormal = { iterHitPoint->vNormal.x,iterHitPoint->vNormal.y,iterHitPoint->vNormal.z,0 };
//					_float fDotBefore = XMVectorGetX(XMVector3Dot(vWallNormal, vLook));
//					_float fDotAfter = XMVectorGetX(XMVector3Dot(vNewWallNormal, vLook));
//					if (fDotBefore < fDotAfter)
//					{
//						vWallNormal = vNewWallNormal;
//					}
//					iterHitPoint++;
//				}
//			}
//			if (0.f == XMVector3Length(vWallNormal).m128_f32[0])
//			{
//				return false;
//			}
//			m_pOwner->Set_ClamberEndPosition(m_vClamberEndPosition);
//			m_pOwner->Set_WallNormal(vWallNormal);
//			m_pOwner->Set_State(CPlayer::CLAMBER);
//			return true;
//		}
//		else
//		{
//			m_fArmYPositionBefore = fArmYPositionCurrent;
//		}
//
//	}
//
//
//	return false;
//}