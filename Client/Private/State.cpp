#include "stdafx.h"
#include "GameInstance.h"
#include "State.h"
#include "Monster.h"
#include "FSM.h"

CState::CState()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CState::Initialize(void* _pArg)
{
	m_iCurLevel = static_cast<STATEDESC*>(_pArg)->iCurLevel;
	//플레이어 위치 가져오기
	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevel, TEXT("Layer_Player"), 0);
	if (nullptr == m_pTarget)
	{
	#ifdef _DEBUG
		cout << "STATE : NO PLAYER" << endl;
	#endif // _DEBUG
	}
	else
	{
		Safe_AddRef(m_pTarget);
	}

	return S_OK;
}

HRESULT CState::CleanUp()
{
	//몬스터 active false 일때 작업 필요

	if (nullptr == m_pTarget)
		return S_OK;

	if (true == m_pTarget->Is_Dead())
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}


	return S_OK;
}

_float CState::Get_CurCoordRange(MONSTER_STATE _eState)
{
	_float fRange = { 0.f };

	switch (_eState)
	{
	case Client::MONSTER_STATE::ALERT:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fAlertRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fAlert2DRange;
		break;
	case Client::MONSTER_STATE::CHASE:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fChaseRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fChase2DRange;
		break;
	case Client::MONSTER_STATE::ATTACK:
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			fRange = m_fAttackRange;
		else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			fRange = m_fAttack2DRange;
		break;
	case Client::MONSTER_STATE::LAST:
		break;
	default:
		break;
	}

	return fRange;
}

_bool CState::Check_Target3D(_bool _isSneak)
{
	if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		return false;

	MONSTER_STATE eState = MONSTER_STATE::ALERT;
	if (true == _isSneak)
		eState = MONSTER_STATE::SNEAK_ALERT;

	if (m_pOwner->IsTarget_In_Detection())
	{
		//------테스트
		_float3 vPos; XMStoreFloat3(&vPos, XMVectorSetY(m_pOwner->Get_FinalPosition(), m_pOwner->Get_FinalPosition().m128_f32[1] + m_pOwner->Get_RayOffset().y));
		_vector vTargetDir = m_pTarget->Get_FinalPosition() - XMLoadFloat3(&vPos);
		_float3 vDir; XMStoreFloat3(&vDir, XMVector3Normalize(XMVectorSetY(vTargetDir, 0.f)));
		_float3 vOutPos;
		CActorObject* pActor = nullptr;

		if (m_pGameInstance->RayCast_Nearest(vPos, vDir, Get_CurCoordRange(MONSTER_STATE::ALERT), &vOutPos, &pActor))
		{
			if (OBJECT_GROUP::MAPOBJECT & static_cast<ACTOR_USERDATA*>(pActor->Get_ActorCom()->Get_RigidActor()->userData)->iObjectGroup)
			{
				//플레이어가 레이 오브젝트보다 가까우면 인식(임시로 맵오브젝트 써봄)
				if (2 == m_pGameInstance->Compare_VectorLength(vTargetDir, XMLoadFloat3(&vOutPos) - m_pOwner->Get_FinalPosition()))
				{
					Event_ChangeMonsterState(eState, m_pFSM);
					return true;
				}
			}
		}
		//레이 충돌 안했을 때(장애물이 없었을 때)
		else
		{
			Event_ChangeMonsterState(eState, m_pFSM);
			return true;
		}
		//---------
	}

	return false;
}

void CState::Determine_AvoidDirection(_fvector& _vDestination, _float3* _vDirection)
{
	if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		return;

	//이동할 때마다 타겟 방향쪽으로 꺾어서 막혔나 체크하고 안 막혔으면 회전
	_float3 vOffset = m_pOwner->Get_RayOffset();
	_float3 vPos; XMStoreFloat3(&vPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));

	//임시코드
	vOffset.x -= m_fAlertRange * tanf(45.f) * 0.5f;
	_float3 vLeftPos; XMStoreFloat3(&vLeftPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	vOffset.x += m_fAlertRange * tanf(45.f);
	_float3 vRightPos; XMStoreFloat3(&vRightPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	//

	_vector vRayDir = XMVector3Normalize(XMVectorSetY(_vDestination - XMLoadFloat3(&vPos), 0.f));
	_float3 vRayDirection; XMStoreFloat3(&vRayDirection, vRayDir);
	_vector vLook = XMVector3Normalize(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
	_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
	_vector vResult = XMVectorZero();
	_float3 vOutPos;
	_float fRepulse;
	_float fRepulsePower=1.f;
	//레이 사거리 임시

	//우선 룩 방향
	if (false == m_pGameInstance->RayCast_Nearest(vPos, vLookDir, 2.f))
	{
		vResult = vLook;
	}
	else
	{
		//레이를 정면 3방향으로 쏨. 장애물과의 거리를 통해 가중치를 설정하고 가중치만큼의 벡터로 회피벡터 설정 후 이동 벡터에 더해 결과 벡터
		if (m_pGameInstance->RayCast_Nearest(vPos, vLookDir, m_fRayDis, &vOutPos))
		{
			fRepulse = 1 - (XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPos) - XMLoadFloat3(&vOutPos))) / m_fRayDis);
			if (fRepulse < 0.f)
				fRepulse = 0.f;
			vResult += -1.f * (vLook)*fRepulse * fRepulsePower;
		}

		_vector AxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -40.f)));
		if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLookDiag, m_fRayDis, &vOutPos))
		{
			fRepulse = 1 - (XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPos) - XMLoadFloat3(&vOutPos))) / m_fRayDis);
			if (fRepulse < 0.f)
				fRepulse = 0.f;
			vResult += -1.f * (XMLoadFloat3(&vDirLeftLookDiag)) * fRepulse * fRepulsePower;
		}

		_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 40.f)));
		if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLookDiag, m_fRayDis, &vOutPos))
		{
			fRepulse = 1 - (XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPos) - XMLoadFloat3(&vOutPos))) / m_fRayDis);
			if (fRepulse < 0.f)
				fRepulse = 0.f;
			vResult += -1.f * (XMLoadFloat3(&vDirRightLookDiag)) * fRepulse * fRepulsePower;
		}

		_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
		if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLook, m_fRayDis, &vOutPos))
		{
			fRepulse = 1 - (XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPos) - XMLoadFloat3(&vOutPos))) / m_fRayDis);
			if (fRepulse < 0.f)
				fRepulse = 0.f;
			vResult += -1.f * (XMLoadFloat3(&vDirLeftLook)) * fRepulse * fRepulsePower;
		}

		_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
		if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLook, m_fRayDis, &vOutPos))
		{
			fRepulse = 1 - (XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPos) - XMLoadFloat3(&vOutPos))) / m_fRayDis);
			if (fRepulse < 0.f)
				fRepulse = 0.f;
			vResult += -1.f * (XMLoadFloat3(&vDirRightLook)) * fRepulse * fRepulsePower;
		}
		vResult += vRayDir * 1.5f;
	}

	// 
	////가운데 레이
	//if (m_pGameInstance->RayCast_Nearest(vPos, vRayDirection, m_fRayDis))
	//{
	//	_vector AxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//	//룩 벡터에서 타겟 방향과 가까운 방향으로 꺾은 방향이 막혔나 체크
	//	_vector v = XMVector3Cross(XMVectorSetY(vLook, 0.f), vRayDir);
	//	//룩 기준 왼쪽 대각선
	//	if (0.f >= XMVectorGetY(XMVector3Cross(XMVectorSetY(vLook, 0.f), vRayDir)))
	//	{
	//		_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
	//		if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLookDiag, m_fRayDis))
	//		{
	//			_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
	//			//현재 룩 벡터 막혔나 체크
	//			if (m_pGameInstance->RayCast_Nearest(vPos, vLookDir, m_fRayDis))
	//			{
	//				//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
	//				_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
	//				if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLook, m_fRayDis))
	//				{
	//					_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
	//					if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLookDiag, m_fRayDis))
	//					{
	//						_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
	//						if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLook, m_fRayDis))
	//						{
	//							//뒤 쪽은 일단 처리 안함
	//							cout << "뒤쪽" << endl;
	//						}
	//						else
	//						{
	//							vResult += XMLoadFloat3(&vDirRightLook);
	//							cout << " 룩 오른쪽" << endl;
	//						}
	//					}
	//					else
	//					{
	//						vResult += XMLoadFloat3(&vDirRightLookDiag);
	//						cout << " 룩 오른쪽 대각선" << endl;
	//					}
	//				}
	//				else
	//				{
	//					vResult += XMLoadFloat3(&vDirLeftLook);
	//					cout << " 룩 왼쪽" << endl;
	//				}
	//			}
	//			else
	//			{
	//				//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
	//				vResult += vLook;
	//				cout << " 룩" << endl;
	//			}
	//		}
	//		else
	//		{
	//			vResult += XMLoadFloat3(&vDirLeftLookDiag);
	//			cout << " 룩 왼쪽 대각선" << endl;
	//		}
	//	}

	//	//룩 기준 오른쪽 대각선
	//	else
	//	{
	//		_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
	//		if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLookDiag, m_fRayDis))
	//		{
	//			_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
	//			//현재 룩 벡터 막혔나 체크
	//			if (m_pGameInstance->RayCast_Nearest(vPos, vLookDir, m_fRayDis))
	//			{
	//				//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
	//				_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
	//				if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightLook, m_fRayDis))
	//				{
	//					_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
	//					if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLookDiag, m_fRayDis))
	//					{
	//						_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
	//						if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftLook, m_fRayDis))
	//						{
	//							//뒤 쪽은 일단 처리 안함
	//							cout << "뒤쪽" << endl;
	//						}
	//						else
	//						{
	//							vResult += XMLoadFloat3(&vDirLeftLook);
	//							cout << " 룩 왼쪽" << endl;
	//						}
	//					}
	//					else
	//					{
	//						vResult += XMLoadFloat3(&vDirLeftLookDiag);
	//						cout << " 룩 왼쪽 대각선" << endl;
	//					}
	//				}
	//				else
	//				{
	//					vResult += XMLoadFloat3(&vDirRightLook);
	//					cout << " 룩 오른쪽" << endl;
	//				}
	//			}
	//			else
	//			{
	//				//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
	//				vResult += vLook;
	//				cout << " 룩" << endl;
	//			}
	//		}
	//		else
	//		{
	//			vResult += XMLoadFloat3(&vDirRightLookDiag);
	//			cout << " 룩 오른쪽 대각선" << endl;
	//		}
	//	}






	//	/*

	//	//레이에 막혔으면 왼쪽, 오른쪽 대각선 순으로 레이 쏨
	//	_float3 vDirLeftDiag; XMStoreFloat3(&vDirLeftDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, -45.f)));
	//	_float3 vDirRightDiag; XMStoreFloat3(&vDirRightDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, 45.f)));
	//	if (m_pGameInstance->RayCast_Nearest(vPos, vDirLeftDiag, m_fRayDis))
	//	{
	//		if (m_pGameInstance->RayCast_Nearest(vPos, vDirRightDiag, m_fRayDis))
	//		{
	//			//왼쪽, 오른쪽으로 쏘는데 더하지 않고 우선순위대로 하나만 씀
	//			_float3 vDirLeft; XMStoreFloat3(&vDirLeft, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, -90.f)));
	//			_float3 vDirRight; XMStoreFloat3(&vDirRight, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vRayDir, 90.f)));

	//			if (false == m_pGameInstance->RayCast_Nearest(vPos, vDirLeft, m_fRayDis))
	//			{
	//				vResult += XMLoadFloat3(&vDirLeft);
	//				cout << "왼쪽" << endl;
	//			}
	//			else if (false == m_pGameInstance->RayCast_Nearest(vPos, vDirRight, m_fRayDis))
	//			{
	//				vResult += XMLoadFloat3(&vDirRight);
	//				cout << "오른쪽" << endl;
	//			}

	//			//전부 다 막혔을 때는 뒤로감 (임시처리)
	//			else
	//			{
	//				vResult = -1.f * vRayDir;
	//				cout << "뒤" << endl;
	//			}

	//		}
	//		else
	//		{
	//			vResult += XMLoadFloat3(&vDirRightDiag);
	//			cout << "오른쪽 대각선" << endl;
	//		}
	//	}
	//	else
	//	{
	//		vResult += XMLoadFloat3(&vDirLeftDiag);
	//		cout << "왼쪽 대각선" << endl;
	//	}

	//	*/
	//}
	////막히지 않은 쪽의 방향 벡터 더함
	//else
	//{
	//	vResult += vRayDir;
	//	cout << "가운데" << endl;
	//}

	////왼 레이
	//if (m_pGameInstance->RayCast_Nearest(vLeftPos, vRayDirection, m_fRayDis))
	//{
	//	_vector AxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//	//룩 벡터에서 타겟 방향과 가까운 방향으로 꺾은 방향이 막혔나 체크
	//	_vector v = XMVector3Cross(XMVectorSetY(vLook, 0.f), vRayDir);
	//	//룩 기준 왼쪽 대각선
	//	if (0.f >= XMVectorGetY(XMVector3Cross(XMVectorSetY(vLook, 0.f), vRayDir)))
	//	{
	//		_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
	//		if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirLeftLookDiag, m_fRayDis))
	//		{
	//			_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
	//			//현재 룩 벡터 막혔나 체크
	//			if (m_pGameInstance->RayCast_Nearest(vLeftPos, vLookDir, m_fRayDis))
	//			{
	//				//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
	//				_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
	//				if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirLeftLook, m_fRayDis))
	//				{
	//					_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
	//					if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirRightLookDiag, m_fRayDis))
	//					{
	//						_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
	//						if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirRightLook, m_fRayDis))
	//						{
	//							//뒤 쪽은 일단 처리 안함
	//							cout << "뒤쪽" << endl;
	//						}
	//						else
	//						{
	//							vResult += XMLoadFloat3(&vDirRightLook);
	//							cout << " 룩 오른쪽" << endl;
	//						}
	//					}
	//					else
	//					{
	//						vResult += XMLoadFloat3(&vDirRightLookDiag);
	//						cout << " 룩 오른쪽 대각선" << endl;
	//					}
	//				}
	//				else
	//				{
	//					vResult += XMLoadFloat3(&vDirLeftLook);
	//					cout << " 룩 왼쪽" << endl;
	//				}
	//			}
	//			else
	//			{
	//				//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
	//				vResult += vLook;
	//				cout << " 룩" << endl;
	//			}
	//		}
	//		else
	//		{
	//			vResult += XMLoadFloat3(&vDirLeftLookDiag);
	//			cout << " 룩 왼쪽 대각선" << endl;
	//		}
	//	}

	//	//룩 기준 오른쪽 대각선
	//	else
	//	{
	//		_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
	//		if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirRightLookDiag, m_fRayDis))
	//		{
	//			_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
	//			//현재 룩 벡터 막혔나 체크
	//			if (m_pGameInstance->RayCast_Nearest(vLeftPos, vLookDir, m_fRayDis))
	//			{
	//				//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
	//				_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
	//				if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirRightLook, m_fRayDis))
	//				{
	//					_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
	//					if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirLeftLookDiag, m_fRayDis))
	//					{
	//						_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
	//						if (m_pGameInstance->RayCast_Nearest(vLeftPos, vDirLeftLook, m_fRayDis))
	//						{
	//							//뒤 쪽은 일단 처리 안함
	//							cout << "뒤쪽" << endl;
	//						}
	//						else
	//						{
	//							vResult += XMLoadFloat3(&vDirLeftLook);
	//							cout << " 룩 왼쪽" << endl;
	//						}
	//					}
	//					else
	//					{
	//						vResult += XMLoadFloat3(&vDirLeftLookDiag);
	//						cout << " 룩 왼쪽 대각선" << endl;
	//					}
	//				}
	//				else
	//				{
	//					vResult += XMLoadFloat3(&vDirRightLook);
	//					cout << " 룩 오른쪽" << endl;
	//				}
	//			}
	//			else
	//			{
	//				//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
	//				vResult += vLook;
	//				cout << " 룩" << endl;
	//			}
	//		}
	//		else
	//		{
	//			vResult += XMLoadFloat3(&vDirRightLookDiag);
	//			cout << " 룩 오른쪽 대각선" << endl;
	//		}
	//	}
	//}
	////막히지 않은 쪽의 방향 벡터 더함
	//else
	//{
	//	vResult += vRayDir;
	//	cout << "가운데" << endl;
	//}

	////오른 레이
	//if (m_pGameInstance->RayCast_Nearest(vRightPos, vRayDirection, m_fRayDis))
	//{
	//	_vector AxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//	//룩 벡터에서 타겟 방향과 가까운 방향으로 꺾은 방향이 막혔나 체크
	//	_vector v = XMVector3Cross(XMVectorSetY(vLook, 0.f), vRayDir);
	//	//룩 기준 왼쪽 대각선
	//	if (0.f >= XMVectorGetY(XMVector3Cross(XMVectorSetY(vLook, 0.f), vRayDir)))
	//	{
	//		_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
	//		if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirLeftLookDiag, m_fRayDis))
	//		{
	//			_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
	//			//현재 룩 벡터 막혔나 체크
	//			if (m_pGameInstance->RayCast_Nearest(vRightPos, vLookDir, m_fRayDis))
	//			{
	//				//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
	//				_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
	//				if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirLeftLook, m_fRayDis))
	//				{
	//					_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
	//					if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirRightLookDiag, m_fRayDis))
	//					{
	//						_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
	//						if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirRightLook, m_fRayDis))
	//						{
	//							//뒤 쪽은 일단 처리 안함
	//							cout << "뒤쪽" << endl;
	//						}
	//						else
	//						{
	//							vResult += XMLoadFloat3(&vDirRightLook);
	//							cout << " 룩 오른쪽" << endl;
	//						}
	//					}
	//					else
	//					{
	//						vResult += XMLoadFloat3(&vDirRightLookDiag);
	//						cout << " 룩 오른쪽 대각선" << endl;
	//					}
	//				}
	//				else
	//				{
	//					vResult += XMLoadFloat3(&vDirLeftLook);
	//					cout << " 룩 왼쪽" << endl;
	//				}
	//			}
	//			else
	//			{
	//				//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
	//				vResult += vLook;
	//				cout << " 룩" << endl;
	//			}
	//		}
	//		else
	//		{
	//			vResult += XMLoadFloat3(&vDirLeftLookDiag);
	//			cout << " 룩 왼쪽 대각선" << endl;
	//		}
	//	}

	//	//룩 기준 오른쪽 대각선
	//	else
	//	{
	//		_float3 vDirRightLookDiag; XMStoreFloat3(&vDirRightLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 45.f)));
	//		if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirRightLookDiag, m_fRayDis))
	//		{
	//			_float3 vLookDir; XMStoreFloat3(&vLookDir, vLook);
	//			//현재 룩 벡터 막혔나 체크
	//			if (m_pGameInstance->RayCast_Nearest(vRightPos, vLookDir, m_fRayDis))
	//			{
	//				//타겟과 가까운 방향, 룩 방향 모두 막혀있으면 우선순위대로 체크
	//				_float3 vDirRightLook; XMStoreFloat3(&vDirRightLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, 90.f)));
	//				if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirRightLook, m_fRayDis))
	//				{
	//					_float3 vDirLeftLookDiag; XMStoreFloat3(&vDirLeftLookDiag, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -45.f)));
	//					if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirLeftLookDiag, m_fRayDis))
	//					{
	//						_float3 vDirLeftLook; XMStoreFloat3(&vDirLeftLook, XMVector3Normalize(m_pGameInstance->Rotate_Vector(AxisY, vLook, -90.f)));
	//						if (m_pGameInstance->RayCast_Nearest(vRightPos, vDirLeftLook, m_fRayDis))
	//						{
	//							//뒤 쪽은 일단 처리 안함
	//							cout << "뒤쪽" << endl;
	//						}
	//						else
	//						{
	//							vResult += XMLoadFloat3(&vDirLeftLook);
	//							cout << " 룩 왼쪽" << endl;
	//						}
	//					}
	//					else
	//					{
	//						vResult += XMLoadFloat3(&vDirLeftLookDiag);
	//						cout << " 룩 왼쪽 대각선" << endl;
	//					}
	//				}
	//				else
	//				{
	//					vResult += XMLoadFloat3(&vDirRightLook);
	//					cout << " 룩 오른쪽" << endl;
	//				}
	//			}
	//			else
	//			{
	//				//현재 룩벡터라서 바꿀 필요없을거 같긴 한데 일단 헷갈리니까 명시적으로
	//				vResult += vLook;
	//				cout << " 룩" << endl;
	//			}
	//		}
	//		else
	//		{
	//			vResult += XMLoadFloat3(&vDirRightLookDiag);
	//			cout << " 룩 오른쪽 대각선" << endl;
	//		}
	//	}
	//}
	////막히지 않은 쪽의 방향 벡터 더함
	//else
	//{
	//	vResult += vRayDir;
	//	cout << "가운데" << endl;
	//}


	if (0.1f < XMVectorGetX(XMVector3Length(vResult)))
	{
		XMStoreFloat3(_vDirection, XMVector3Normalize(vResult));
	}
}


void CState::Set_Owner(CMonster* _pOwner)
{
	m_pOwner = _pOwner;
}

void CState::Free()
{
	Safe_Release(m_pGameInstance);

	if (nullptr != m_pTarget)
		Safe_Release(m_pTarget);
	m_pOwner = nullptr;
	__super::Free();
}
