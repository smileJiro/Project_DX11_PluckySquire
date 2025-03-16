#include "stdafx.h"
#include "Back_WayState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CBack_WayState::CBack_WayState()
{
}

HRESULT CBack_WayState::Initialize(void* _pArg)
{
	SNEAKSTATEDESC* pDesc = static_cast<SNEAKSTATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_iPrevDir = -1;
	m_iDir = -1;
	//m_fDelayTime = 1.f;

	//Initialize_PatrolPoints(pDesc->eWayIndex);
	//Initialize_WayPoints(pDesc->eWayIndex);
		
	return S_OK;
}

void CBack_WayState::Set_Bound(_float3& _vPosition)
{
}

void CBack_WayState::Set_WayPoint(SNEAKWAYPOINTINDEX _eWayIndex)
{
	Initialize_WayPoints(_eWayIndex);
	Initialize_PatrolPoints(_eWayIndex);
}


void CBack_WayState::State_Enter()
{
	m_fAccTime = 0.f;
	m_isOnWay = false;
	m_isTurn = false;
	m_isMove = false;
	m_isPathFind = true;
	cout << "Back_Way" << endl;
}

void CBack_WayState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (true == m_isMove)
	m_fAccTime += _fTimeDelta;

	if (nullptr != m_pTarget)
	{
		if (m_pOwner->Get_CurCoord() == COORDINATE_LAST)
			return;
		
		if (m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
		{	
			//적 발견 시 ALERT 전환
			if (Check_Target3D())
			{
				m_pOwner->Stop_Rotate();
				m_pOwner->Stop_Move();

				_vector vDir = XMVectorSetY(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition(), 0.f);
				_float fDis = XMVectorGetX(XMVector3Length((vDir)));
				//공격 범위 안일 경우 바로 공격으로 전환
				if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
					Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);

				return;
			}
		}
	}

	//다음 웨이포인트 설정
	if (false == m_isTurn && false == m_isMove)
	{
		//타겟 위치에 가까운 웨이포인트 찾아 목표 위치로 지정 

		Determine_BackDirection(&m_vDir);
		//예외처리
		if (XMVector3Equal(XMLoadFloat3(&m_vDir), XMVectorZero()))
		{
			m_pOwner->Stop_Rotate();
			m_pOwner->Stop_Move();
			Event_ChangeMonsterState(MONSTER_STATE::PATROL, m_pFSM);
			return;
		}

		m_isTurn = true;
	}
	
	//이동
	Back_WayMove(_fTimeDelta, m_iDir);
}

void CBack_WayState::State_Exit()
{
	m_fAccTime = 0.f;
	m_fAccDistance = 0.f;
	m_isTurn = false;
	m_isMove = false;
}

void CBack_WayState::Back_WayMove(_float _fTimeDelta, _int _iDir)
{
	//회전
	if (true == m_isTurn && false == m_isMove)
	{
		if (m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
		{
			m_isMove = true;

			m_pOwner->Change_Animation();
		}
		else
		{
			_bool isCW = true;
			_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vDir)));
			if (fResult < 0)
				isCW = false;

			m_pOwner->Turn_Animation(isCW);
		}
	}

	//이동
	if (true == m_isMove)
	{
		//m_pOwner->Get_ActorCom()->Set_LinearVelocity(vDir, m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
		//웨이포인트 도달 했는지 체크 후 도달 했으면 idle로 전환
		
		//Determine_AvoidDirection(XMLoadFloat3(&m_PatrolWaypoints[m_iCurWayIndex]), &m_vDir);
		//static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_LinearVelocity(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());

		//static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_LinearVelocity(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
		m_pOwner->Move(XMLoadFloat3(&m_vDir) * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec(), _fTimeDelta);

		if(true == m_isOnWay)
		{
			if (m_pOwner->Check_Arrival(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition), 0.3f))
			{
				m_pOwner->Stop_Rotate();
				m_pOwner->Stop_Move();
				++m_iCurWayIndex;

				m_isTurn = false;
				m_isMove = false;
				//m_isOnWay = false;

				if (m_Ways.size() <= m_iCurWayIndex)
				{
					m_pFSM->Set_Patrol_Way_Index(m_iDestPatrolIndex);
					Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
				}
			}
		}
		else
		{
			if (m_pOwner->Check_Arrival(XMLoadFloat3(&m_WayPoints[m_iCurWayIndex].vPosition), 0.3f))
			{
				m_pOwner->Stop_Rotate();
				m_pOwner->Stop_Move();
				m_isTurn = false;
				m_isMove = false;
				m_isOnWay = false;
				m_pFSM->Set_Patrol_Way_Index(m_iDestPatrolIndex);
				Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
			}
		}
	}

	//웨이포인트로 이동하다가 장애물 피해야하므로 
}

void CBack_WayState::Determine_Direction()
{
	if (COORDINATE::COORDINATE_LAST == m_pOwner->Get_CurCoord())
		return;


	//다음 웨이 포인트로 넘어감.
	if (false == m_isBack)
	{
		++m_iCurWayIndex;

		if (m_PatrolWays.size() - 1 == m_iCurWayIndex)
			m_isBack = true;

		//예외처리
		if (m_PatrolWays.size()-1 < m_iCurWayIndex)
		{
			m_iCurWayIndex = (_int)m_PatrolWays.size() - 1;
			m_isBack = true;
		}
	}
	else
	{
		--m_iCurWayIndex;
		
		if (0 == m_iCurWayIndex)
			m_isBack = false;

		//예외처리
		if (0 > m_iCurWayIndex)
		{
			m_iCurWayIndex = 0;
			m_isBack = false;
		}
	}

	//시간 랜덤으로 지정 (양 끝 지점만 최솟값을 크게 놓음)
	if (0 == m_iCurWayIndex || m_PatrolWays.size() - 1 == m_iCurWayIndex)
		m_pFSM->Set_Sneak_StopTime(m_pGameInstance->Compute_Random(1.f, 3.f));
	else
	{
		m_pFSM->Set_Sneak_StopTime(m_pGameInstance->Compute_Random(0.f, 3.f));
	}

	XMStoreFloat3(&m_vDir, XMLoadFloat3(&m_WayPoints[m_PatrolWays[m_iCurWayIndex]].vPosition) - m_pOwner->Get_FinalPosition());

}

_vector CBack_WayState::Set_Sneak_PatrolDirection(_int _iDir)
{
	_vector vDir = {};
	switch (_iDir)
	{
	case 0:
		vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "상" << endl;
		break;
	case 1:
		vDir = XMVectorSet(1.f, 0.f, 1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "우상" << endl;
		break;
	case 2:
		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "우" << endl;
		break;
	case 3:
		vDir = XMVectorSet(1.f, 0.f, -1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "우하" << endl;
		break;
	case 4:
		vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "하" << endl;
		break;
	case 5:
		vDir = XMVectorSet(-1.f, 0.f, -1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "좌하" << endl;
		break;
	case 6:
		vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "좌" << endl;
		break;
	case 7:
		vDir = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_FinalPosition() + vDir);
		//cout << "좌상" << endl;
		break;
	default:
		break;
	}
	
	return vDir;
}

void CBack_WayState::Determine_BackDirection(_float3* _vDirection)
{
	if (true == m_isOnWay)
	{
		XMStoreFloat3(_vDirection, XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - m_pOwner->Get_FinalPosition(), 0.f)));
		return;
	}


	_float3 vOffset = m_pOwner->Get_RayOffset();
	_float3 vRayPos; XMStoreFloat3(&vRayPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	_float3 vPos; XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
	vOffset.x -= m_pOwner->Get_RayHalfWidth();
	_float3 vLeftPos; XMStoreFloat3(&vLeftPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	vOffset.x += m_pOwner->Get_RayHalfWidth() * 2.f;
	_float3 vRightPos; XMStoreFloat3(&vRightPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	_vector vResult = XMVectorZero();

	
	_float3 vDest = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
	_float3 vPoint = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
	_uint iDestIndex = -1;
	_uint iStartIndex = -1;


	for (_uint Index = 0; Index < m_PatrolWays.size(); ++Index)
	{
		_vector vPositionToPointDis = XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_PatrolWays[Index]].vPosition) - m_pOwner->Get_FinalPosition(), 0.f);

		if (1 == m_pGameInstance->Compare_VectorLength(XMLoadFloat3(&vDest), vPositionToPointDis))
		{
			XMStoreFloat3(&vDest, vPositionToPointDis);
			iDestIndex = m_PatrolWays[Index];
			m_iDestPatrolIndex = Index;
		}
	}

	if (-1 == iDestIndex)
		return;

	//_vector vDestDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - m_pOwner->Get_FinalPosition(), 0.f);
	_vector vTargetDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vPos), 0.f);
	_vector vDestDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vRayPos), 0.f);
	_vector vDestLeftDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vLeftPos), 0.f);
	_vector vDestRightDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vRightPos), 0.f);
	XMStoreFloat3(&vDest, XMVector3Normalize(vDestDir));
	_float3 vDestTarget; XMStoreFloat3(&vDestTarget, XMVector3Normalize(vTargetDir));
	_float3 vDestLeft; XMStoreFloat3(&vDestLeft, XMVector3Normalize(vDestLeftDir));
	_float3 vDestRight; XMStoreFloat3(&vDestRight, XMVector3Normalize(vDestRightDir));
	//포인트 안 찍고 갈 수 있으면 바로 저장
	if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vPos, vDestTarget, XMVectorGetX(XMVector3Length(vTargetDir)), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
	{
		if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vRayPos, vDest, XMVectorGetX(XMVector3Length(vDestDir)), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
		{
			if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vLeftPos, vDestLeft, XMVectorGetX(XMVector3Length(vDestLeftDir)), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
			{
				if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vRightPos, vDestRight, XMVectorGetX(XMVector3Length(vDestRightDir)), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
				{
					XMStoreFloat3(_vDirection, XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vPos), 0.f)));
					m_iCurWayIndex = iDestIndex;
					return;
				}
			}
		}
	}

	vTargetDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vPos), 0.f);
	vDestDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vRayPos), 0.f);
	vDestLeftDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vLeftPos), 0.f);
	vDestRightDir = XMVectorSetY(XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition) - XMLoadFloat3(&vRightPos), 0.f);
	XMStoreFloat3(&vDest, XMVector3Normalize(vDestDir));
	vDestTarget; XMStoreFloat3(&vDestTarget, XMVector3Normalize(vTargetDir));
	vDestLeft; XMStoreFloat3(&vDestLeft, XMVector3Normalize(vDestLeftDir));
	vDestRight; XMStoreFloat3(&vDestRight, XMVector3Normalize(vDestRightDir));


	for (_uint Index = 0; Index < m_WayPoints.size(); ++Index)
	{
		_vector vPositionToPointDis = XMVectorSetY(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vPos), 0.f);

		//시작점 찾는데 시작 점을 여러개로 쓸지 보고 판단
		if (1 == m_pGameInstance->Compare_VectorLength(XMLoadFloat3(&vPoint), vPositionToPointDis))
		{
			_float3 vPosTo; XMStoreFloat3(&vPosTo, XMVector3Normalize(vPositionToPointDis));

			//추가
			vPos.y += 0.1f;

			//가는길에 장애물 없으면
			if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vPos, vPosTo, XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vPos))),
				OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
			{
				_vector vPositionToLeftPointDis = XMVectorSetY(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vLeftPos), 0.f);
				_float3 vLeftPosTo; XMStoreFloat3(&vLeftPosTo, XMVector3Normalize(vPositionToLeftPointDis));
				if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vLeftPos, vLeftPosTo, XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vLeftPosTo))),
					OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
				{
					_vector vPositionToRightPointDis = XMVectorSetY(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vRightPos), 0.f);
					_float3 vRightPosTo; XMStoreFloat3(&vRightPosTo, XMVector3Normalize(vPositionToRightPointDis));
					if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vRightPos, vRightPosTo, XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vRightPosTo))),
						OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
					{
						XMStoreFloat3(&vPoint, vPositionToPointDis);
						iStartIndex = Index;
					}
				}
				
			}
		}
	}

	if (iStartIndex == iDestIndex)
	{
		for (_uint i = 0; i < m_PatrolWays.size(); ++i)
		{
			if (iStartIndex != m_PatrolWays[i])
			{
				iDestIndex = m_PatrolWays[i];
				m_iDestPatrolIndex = i;
				break;
			}
		}
	}
	if (-1 == iStartIndex || -1 == iDestIndex)
	{
		cout << "복귀 경로 없음" << endl;
		return;
	}

	//목표 위치로 가는 웨이포인트 경로 찾기
	priority_queue <pair<_float, pair<_uint, _uint>>, vector<pair<_float, pair<_uint, _uint>>>, compare> PriorityQueue;	//비용, 부모 인덱스, 자기 인덱스
	map<_uint, _float> OpenMap; //자기 인덱스, 비용
	map<_uint, _uint> ClosedMap;	//자기 인덱스, 부모 인덱스
	_float fCostFromStart = 0.f;
	_vector StartPos = XMLoadFloat3(&m_WayPoints[iStartIndex].vPosition);
	_vector DestPos = XMLoadFloat3(&m_WayPoints[iDestIndex].vPosition);
	_float fTargetDis = XMVectorGetX(XMVector3Length(XMVectorSetY(DestPos - StartPos, 0.f))); //heuristic
	PriorityQueue.push({ fCostFromStart + fTargetDis, {iStartIndex, iStartIndex} }); //시작 노드 부모 자신으로 설정
	OpenMap.insert({ iStartIndex, fCostFromStart + fTargetDis });

	while (!PriorityQueue.empty())
	{
		_float fCost = PriorityQueue.top().first;
		_uint iParentIndex = PriorityQueue.top().second.first;
		_uint iIndex = PriorityQueue.top().second.second;
		PriorityQueue.pop();
		ClosedMap.emplace(iIndex, iParentIndex);
		if (iDestIndex == iIndex)
			break;

		for (_uint i = 0; i < m_WayPoints[iIndex].Neighbors.size(); ++i)
		{
			_uint Neighbor = m_WayPoints[iIndex].Neighbors[i];
			//닫힌 목록에 없을 때
			if (ClosedMap.end() == ClosedMap.find(Neighbor))
			{
				_vector NodePos = XMLoadFloat3(&m_WayPoints[Neighbor].vPosition);
				//비용 계산
				fCostFromStart = XMVectorGetX(XMVector3Length(XMVectorSetY(NodePos - StartPos, 0.f)));
				fTargetDis = XMVectorGetX(XMVector3Length(XMVectorSetY(DestPos - NodePos, 0.f)));

				if (true == OpenMap.insert({ Neighbor, fCostFromStart + fTargetDis }).second)
				{
					//이웃에 부모 인덱스 저장
					PriorityQueue.push({ fCostFromStart + fTargetDis, {iIndex, Neighbor} });
				}
				//중복이면
				else
				{
					//비용이 더 작은 걸 저장
					if (fCostFromStart + fTargetDis < OpenMap[Neighbor])
					{
						PriorityQueue.push({ fCostFromStart + fTargetDis, {iIndex, Neighbor} });
						OpenMap[Neighbor] = fCostFromStart + fTargetDis;
					}
				}
			}
		}
	}

	//경로없음
	//if (OpenMap.empty());


	m_Ways.clear();
	_uint iParent;
	m_Ways.push_back(iDestIndex);
	//닫힌 목록 저장
	for (_uint i = ClosedMap[iDestIndex]; i != iStartIndex; i = ClosedMap[iParent])
	{
		iParent = i;
		m_Ways.push_back(iParent);
	}

	//시작점까지 갔다가 다음 점으로 진행하는 거리와 다음 점으로 바로 가는 거리 비교해서 시작점으로 갈지 결정
	_vector vFromStart = XMLoadFloat3(&m_WayPoints[iStartIndex].vPosition) - m_pOwner->Get_FinalPosition() + XMLoadFloat3(&m_WayPoints[m_Ways[m_Ways.size() - 1]].vPosition) - XMLoadFloat3(&m_WayPoints[iStartIndex].vPosition);
	_vector vToNext = XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_Ways[m_Ways.size() - 1]].vPosition) - m_pOwner->Get_FinalPosition(), 0.f);
	_float3 vToNextDir; XMStoreFloat3(&vToNextDir, XMVector3Normalize(vToNext));

	//다음 점이 막혀있거나 거리가 더 길면 시작점으로 감
	if (true == m_pGameInstance->RayCast_Nearest_GroupFilter(vPos, vToNextDir, XMVectorGetX(XMVector3Length(vToNext)), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE)
		|| 2 == m_pGameInstance->Compare_VectorLength(XMVectorSetY(vFromStart, 0.f), XMVectorSetY(vToNext, 0.f)))
	{
		m_Ways.push_back(iStartIndex);
	}

	reverse(m_Ways.begin(), m_Ways.end());
	m_iCurWayIndex = 0;
	m_isOnWay = true;

	vResult = XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - XMLoadFloat3(&vPos), 0.f);

	XMStoreFloat3(_vDirection, XMVector3Normalize(vResult));
}

CBack_WayState* CBack_WayState::Create(void* _pArg)
{
	CBack_WayState* pInstance = new CBack_WayState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBack_WayState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBack_WayState::Free()
{
	__super::Free();
}
