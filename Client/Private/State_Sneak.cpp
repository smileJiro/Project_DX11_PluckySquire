#include "stdafx.h"
#include "GameInstance.h"
#include "State_Sneak.h"
#include "Monster.h"
#include "FSM.h"

CState_Sneak::CState_Sneak()
{
}

HRESULT CState_Sneak::Initialize(void* _pArg)
{
	SNEAKSTATEDESC* pDesc = static_cast<SNEAKSTATEDESC*>(_pArg);
	
	__super::Initialize(_pArg);

	return S_OK;
}

HRESULT CState_Sneak::Initialize_WayPoints(WAYPOINTINDEX _eWayIndex)
{
	if (WAYPOINTINDEX::LAST == _eWayIndex)
		return S_OK;

	//웨이포인트 세팅
	switch (_eWayIndex)
	{
	case Client::WAYPOINTINDEX::CHAPTER2_1:
		m_WayPoints.push_back({ _float3(-31.f, 6.56f, 24.f) });
		m_WayPoints.push_back({_float3(-30.7f, 6.55f, 20.f)});
		m_WayPoints.push_back({ _float3(-29.f, 6.55f, 18.f) });
		m_WayPoints.push_back({ _float3(-24.f, 6.54f, 18.f) });
		m_WayPoints.push_back({ _float3(-22.f, 6.55f, 18.f) });
		m_WayPoints.push_back({_float3(-16.5f, 6.53f, 19.f)});
		m_WayPoints.push_back({_float3(-13.3f, 6.51f, 19.f)});
		m_WayPoints.push_back({_float3(-11.1f, 6.52f, 21.f)});
		m_WayPoints.push_back({ _float3(-13.3f, 6.52f, 23.25f) });
		m_WayPoints.push_back({ _float3(-17.f, 6.55f, 23.f) });
		m_WayPoints.push_back({ _float3(-20.f, 6.55f, 23.f) });
		m_WayPoints.push_back({ _float3(-23.f, 6.55f, 20.5f) });

		m_WayPoints[0].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(0);
		m_WayPoints[1].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(1);
		m_WayPoints[2].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(2);
		m_WayPoints[3].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(3);
		m_WayPoints[4].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(4);
		m_WayPoints[5].Neighbors.push_back(6);
		m_WayPoints[6].Neighbors.push_back(5);
		m_WayPoints[6].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(6);
		m_WayPoints[7].Neighbors.push_back(8);
		m_WayPoints[8].Neighbors.push_back(7);
		m_WayPoints[8].Neighbors.push_back(9);
		m_WayPoints[9].Neighbors.push_back(8);
		m_WayPoints[9].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(9);
		m_WayPoints[9].Neighbors.push_back(10);
		m_WayPoints[10].Neighbors.push_back(9);
		m_WayPoints[10].Neighbors.push_back(11);
		m_WayPoints[11].Neighbors.push_back(10);
		m_WayPoints[11].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(11);
		m_WayPoints[11].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(11);
		break;

	default:
		break;
	}


	//인접 노드 간 거리 저장
	m_Weights.emplace(make_pair(0, 1), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[1].vPosition) - XMLoadFloat3(&m_WayPoints[0].vPosition), 0.f))));
	m_Weights.emplace(make_pair(1, 0), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[1].vPosition) - XMLoadFloat3(&m_WayPoints[0].vPosition), 0.f))));
	m_Weights.emplace(make_pair(1, 2), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[2].vPosition) - XMLoadFloat3(&m_WayPoints[1].vPosition), 0.f))));
	m_Weights.emplace(make_pair(2, 1), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[2].vPosition) - XMLoadFloat3(&m_WayPoints[1].vPosition), 0.f))));
	m_Weights.emplace(make_pair(2, 3), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[3].vPosition) - XMLoadFloat3(&m_WayPoints[2].vPosition), 0.f))));
	m_Weights.emplace(make_pair(3, 2), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[3].vPosition) - XMLoadFloat3(&m_WayPoints[2].vPosition), 0.f))));
	m_Weights.emplace(make_pair(3, 4), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[4].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	m_Weights.emplace(make_pair(4, 3), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[4].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	m_Weights.emplace(make_pair(4, 5), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[5].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));
	m_Weights.emplace(make_pair(5, 4), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[5].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));
	m_Weights.emplace(make_pair(5, 6), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[6].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	m_Weights.emplace(make_pair(6, 5), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[6].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	m_Weights.emplace(make_pair(6, 7), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[7].vPosition) - XMLoadFloat3(&m_WayPoints[6].vPosition), 0.f))));
	m_Weights.emplace(make_pair(7, 6), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[7].vPosition) - XMLoadFloat3(&m_WayPoints[6].vPosition), 0.f))));
	m_Weights.emplace(make_pair(7, 8), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[8].vPosition) - XMLoadFloat3(&m_WayPoints[7].vPosition), 0.f))));
	m_Weights.emplace(make_pair(8, 7), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[8].vPosition) - XMLoadFloat3(&m_WayPoints[7].vPosition), 0.f))));
	m_Weights.emplace(make_pair(8, 9), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[8].vPosition), 0.f))));
	m_Weights.emplace(make_pair(9, 8), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[8].vPosition), 0.f))));
	m_Weights.emplace(make_pair(9, 5), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	m_Weights.emplace(make_pair(5, 9), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	m_Weights.emplace(make_pair(9, 10), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[10].vPosition) - XMLoadFloat3(&m_WayPoints[9].vPosition), 0.f))));
	m_Weights.emplace(make_pair(10, 9), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[10].vPosition) - XMLoadFloat3(&m_WayPoints[9].vPosition), 0.f))));
	m_Weights.emplace(make_pair(10, 11), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[10].vPosition), 0.f))));
	m_Weights.emplace(make_pair(11, 10), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[10].vPosition), 0.f))));
	m_Weights.emplace(make_pair(11, 3), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	m_Weights.emplace(make_pair(3, 11), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	m_Weights.emplace(make_pair(11, 4), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));
	m_Weights.emplace(make_pair(4, 11), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));

	return S_OK;
}

void CState_Sneak::Determine_NextDirection(_fvector& _vDestination, _float3* _vDirection)
{
	_float3 vOffset = m_pOwner->Get_RayOffset();
	_float3 vPos; XMStoreFloat3(&vPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	_vector vRayDir = XMVector3Normalize(XMVectorSetY(_vDestination - XMLoadFloat3(&vPos), 0.f));
	_float3 vRayDirection; XMStoreFloat3(&vRayDirection, vRayDir);
	_vector vResult = XMVectorZero();

	if(m_isPathFind)
	{
		_float3 vTargetPos; XMStoreFloat3(&vTargetPos, _vDestination);
		_float3 vDest = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
		_float3 vPoint = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
		_uint iDestIndex = 0;
		_uint iStartIndex = 0;

		//타겟 위치에 가까운 웨이포인트 찾아 목표 위치로 지정 + 현재 위치와 가까운 타겟 포인트 찾기
		for (_uint Index = 0; Index < m_WayPoints.size(); ++Index)
		{
			_vector vTargetToPointDis = XMVectorSetY(_vDestination - XMLoadFloat3(&m_WayPoints[Index].vPosition), 0.f);
			_vector vPositionToPointDis = XMVectorSetY(XMLoadFloat3(&vPos) - XMLoadFloat3(&m_WayPoints[Index].vPosition), 0.f);
			if (1 == m_pGameInstance->Compare_VectorLength(XMLoadFloat3(&vDest), vTargetToPointDis))
			{
				XMStoreFloat3(&vDest, vTargetToPointDis);
				iDestIndex = Index;
			}

			//시작점 찾는데 시작 점을 여러개로 쓸지 보고 판단
			if (1 == m_pGameInstance->Compare_VectorLength(XMLoadFloat3(&vPoint), vPositionToPointDis))
			{
				XMStoreFloat3(&vPoint, vPositionToPointDis);
				iStartIndex = Index;
			}
		}

		//목표 위치로 가는 웨이포인트 경로 찾기
		priority_queue <pair<_float, pair<_uint, _uint>>> PriorityQueue;	//비용, 부모 인덱스, 자기 인덱스
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
					_vector NodePos = XMLoadFloat3(&m_WayPoints[iIndex].vPosition);
					//비용 계산
					fCostFromStart = XMVectorGetX(XMVector3Length(XMVectorSetY(NodePos - StartPos, 0.f)));
					fTargetDis = XMVectorGetX(XMVector3Length(XMVectorSetY(DestPos - StartPos, 0.f)));

					if (true == OpenMap.insert({ Neighbor, fCostFromStart + fTargetDis }).second)
					{
						//이웃과 부모 인덱스 저장
						PriorityQueue.push({ fCostFromStart + fTargetDis, {iParentIndex, Neighbor} });
					}
					//중복이면
					else
					{
						//비용이 더 작은 걸 저장
						if (fCostFromStart + fTargetDis < OpenMap[Neighbor])
						{
							PriorityQueue.push({ fCostFromStart + fTargetDis, {iParentIndex, Neighbor} });
							OpenMap[Neighbor] = fCostFromStart + fTargetDis;
						}
					}
				}
			}
		}

		//경로없음
		if (OpenMap.empty());


		m_Ways.clear();
		_uint iParent;
		m_Ways.push_back(iDestIndex);
		//닫힌 목록 저장
		for (_uint i = ClosedMap[iDestIndex]; i != iStartIndex; i = ClosedMap[iParent])
		{
			iParent = i;
			m_Ways.push_back(iParent);
		}

		m_Ways.push_back(iStartIndex);
		reverse(m_Ways.begin(), m_Ways.end());
		m_iCurWayIndex = 0;
		m_isOnWay = true;
		m_isPathFind = false;

		vResult = XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - XMLoadFloat3(&vPos);
	}
	else
	{
		vResult = XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - XMLoadFloat3(&vPos);
	}

	//타겟 방향이 막혀있으면 웨이포인트 이동
	//if (true == m_pGameInstance->RayCast_Nearest(vPos, vRayDirection, 2.f))
	//{
	//	
	//}
	//막혀있지 않다면 타겟 방향대로 이동
	/*else
	{
		vResult = vRayDir;
	}*/

	XMStoreFloat3(_vDirection, XMVector3Normalize(vResult));
}

HRESULT CState_Sneak::CleanUp()
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

void CState_Sneak::Set_Sneak_InvestigatePos(_fvector _vPosition)
{
	XMStoreFloat3(&m_vSneakPos, _vPosition);
}

void CState_Sneak::Free()
{
	__super::Free();
}
