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
	
	m_eWayIndex = pDesc->eWayIndex;

	__super::Initialize(_pArg);

	return S_OK;
}

HRESULT CState_Sneak::Initialize_WayPoints(SNEAKWAYPOINTINDEX _eWayIndex)
{
	if (SNEAKWAYPOINTINDEX::LAST == _eWayIndex)
		return S_OK;

	//웨이포인트 세팅
	switch (_eWayIndex)
	{
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_1:
		m_WayPoints.push_back({ _float3(-31.f, 6.56f, 24.f) });
		m_WayPoints.push_back({_float3(-30.5f, 6.55f, 20.5f)});
		m_WayPoints.push_back({ _float3(-29.7f, 6.55f, 18.45f) });
		m_WayPoints.push_back({ _float3(-27.8f, 6.55f, 18.22f) });
		m_WayPoints.push_back({ _float3(-24.f, 6.54f, 18.f) });
		m_WayPoints.push_back({ _float3(-22.4f, 6.55f, 18.f) });
		m_WayPoints.push_back({_float3(-18.4f, 6.53f, 18.f)});
		m_WayPoints.push_back({_float3(-16.1f, 6.53f, 18.5f)});
		m_WayPoints.push_back({_float3(-12.6f, 6.51f, 19.5f)});
		m_WayPoints.push_back({_float3(-11.6f, 6.52f, 22.f)});
		m_WayPoints.push_back({ _float3(-13.7f, 6.52f, 23.3f) });
		m_WayPoints.push_back({ _float3(-16.5f, 6.56f, 22.6f) });
		m_WayPoints.push_back({ _float3(-20.f, 6.5f, 22.5f) });
		m_WayPoints.push_back({ _float3(-23.6f, 6.55f, 21.22f) });
		m_WayPoints.push_back({ _float3(-16.3f, 6.54f, 20.5f) });

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
		m_WayPoints[9].Neighbors.push_back(10);
		m_WayPoints[10].Neighbors.push_back(9);
		m_WayPoints[10].Neighbors.push_back(11);
		m_WayPoints[11].Neighbors.push_back(10);
		m_WayPoints[11].Neighbors.push_back(12);
		m_WayPoints[12].Neighbors.push_back(11);
		m_WayPoints[12].Neighbors.push_back(13);
		m_WayPoints[13].Neighbors.push_back(12);
		m_WayPoints[13].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(13);
		m_WayPoints[13].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(13);
		m_WayPoints[14].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(14);
		m_WayPoints[14].Neighbors.push_back(11);
		m_WayPoints[11].Neighbors.push_back(14);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2:
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2_2:
		m_WayPoints.push_back({ _float3(36.5f, 0.35f, -7.3f) });
		m_WayPoints.push_back({ _float3(33.5f, 0.35f, -7.f) });
		m_WayPoints.push_back({ _float3(32.5f, 0.35f, -4.5f) });
		m_WayPoints.push_back({ _float3(30.f, 0.35f, -3.7f) });
		m_WayPoints.push_back({ _float3(30.5f, 0.35f, -0.15f) });
		//m_WayPoints.push_back({ _float3(32.5f, 0.35f, -3.5f) });
		m_WayPoints.push_back({ _float3(32.15f, 0.35f, 1.66f) });
		m_WayPoints.push_back({ _float3(32.6f, 0.35f, 5.8f) });
		m_WayPoints.push_back({ _float3(35.f, 0.35f, 6.37f) });
		m_WayPoints.push_back({ _float3(37.f, 0.35f, 5.3f) });
		m_WayPoints.push_back({ _float3(38.5f, 0.35f, 3.87f) });
		m_WayPoints.push_back({ _float3(39.3f, 0.35f, 2.48f) });
		m_WayPoints.push_back({ _float3(39.5f, 0.35f, -0.3f) });
		m_WayPoints.push_back({ _float3(40.4f, 0.35f, 3.3f) });
		m_WayPoints.push_back({ _float3(42.3f, 0.35f, 4.86f) });
		m_WayPoints.push_back({ _float3(41.f, 0.35f, 8.f) });
		m_WayPoints.push_back({ _float3(39.5f, 0.35f, 10.5f) });

		m_WayPoints[0].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(0);
		m_WayPoints[1].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(1);
		m_WayPoints[2].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(2);
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
		m_WayPoints[9].Neighbors.push_back(10);
		m_WayPoints[10].Neighbors.push_back(9);
		m_WayPoints[10].Neighbors.push_back(11);
		m_WayPoints[11].Neighbors.push_back(10);
		m_WayPoints[10].Neighbors.push_back(12);
		m_WayPoints[12].Neighbors.push_back(10);
		m_WayPoints[12].Neighbors.push_back(13);
		m_WayPoints[13].Neighbors.push_back(12);
		m_WayPoints[13].Neighbors.push_back(14);
		m_WayPoints[14].Neighbors.push_back(13);
		m_WayPoints[14].Neighbors.push_back(15);
		m_WayPoints[15].Neighbors.push_back(14);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_3:
		m_WayPoints.push_back({ _float3(46.f, 0.35f, -0.5f) });
		m_WayPoints.push_back({ _float3(47.f, 0.35f, 2.f) });
		m_WayPoints.push_back({ _float3(49.f, 0.35f, 3.4f) });
		m_WayPoints.push_back({ _float3(50.f, 0.35f, 5.5f) });
		m_WayPoints.push_back({ _float3(51.5f, 0.35f, 8.f) });
		m_WayPoints.push_back({ _float3(52.5f, 0.35f, 5.5f) });
		m_WayPoints.push_back({ _float3(53.f, 0.35f, 2.5f) });
		m_WayPoints.push_back({ _float3(51.5f, 0.35f, 1.f) });
		m_WayPoints.push_back({ _float3(49.5f, 0.35f, -1.f) });

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
		m_WayPoints[3].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(3);
		m_WayPoints[5].Neighbors.push_back(6);
		m_WayPoints[6].Neighbors.push_back(5);
		m_WayPoints[6].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(6);
		m_WayPoints[2].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(2);
		m_WayPoints[7].Neighbors.push_back(8);
		m_WayPoints[8].Neighbors.push_back(7);
		m_WayPoints[8].Neighbors.push_back(0);
		m_WayPoints[0].Neighbors.push_back(8);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_1:
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_2:
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_3:
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_4:
		m_WayPoints.push_back({ _float3(26.5f, 8.58f, 25.f) });
		m_WayPoints.push_back({ _float3(29.5f, 8.58f, 28.5f) });

		m_WayPoints.push_back({ _float3(29.5f, 8.6f, 31.5f) });
		m_WayPoints.push_back({ _float3(32.5f, 8.6f, 30.5f) });
		m_WayPoints.push_back({ _float3(31.3f, 8.6f, 27.9f) });

		m_WayPoints.push_back({ _float3(35.5f, 8.58f, 31.5f) });
		m_WayPoints.push_back({ _float3(34.f, 8.5f, 27.5f) });

		m_WayPoints.push_back({ _float3(31.8f, 8.58f, 24.4f) });
		m_WayPoints.push_back({ _float3(36.f, 8.6f, 22.9f) });
		m_WayPoints.push_back({ _float3(33.2f, 8.6f, 21.2f) });

		m_WayPoints[0].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(0);
		m_WayPoints[1].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(1);
		m_WayPoints[2].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(2);
		m_WayPoints[3].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(6);
		m_WayPoints[6].Neighbors.push_back(3);
		m_WayPoints[5].Neighbors.push_back(6);
		m_WayPoints[6].Neighbors.push_back(5);
		m_WayPoints[4].Neighbors.push_back(6);
		m_WayPoints[6].Neighbors.push_back(4);
		m_WayPoints[6].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(6);
		m_WayPoints[7].Neighbors.push_back(8);
		m_WayPoints[8].Neighbors.push_back(7);
		m_WayPoints[8].Neighbors.push_back(9);
		m_WayPoints[9].Neighbors.push_back(8);
		m_WayPoints[9].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(9);
		break;





	case SNEAKWAYPOINTINDEX::CHAPTER8_1:
		m_WayPoints.push_back({ _float3(13.f, 21.58f, 5.5f) });
		m_WayPoints.push_back({ _float3(9.3f, 21.58f, 5.5f) });
		m_WayPoints.push_back({ _float3(9.3f, 21.58f, 2.14f) });
		m_WayPoints.push_back({ _float3(13.f, 21.58f, 2.14f) });
		m_WayPoints.push_back({ _float3(7.47f, 21.58f, 1.f) });

		m_WayPoints[0].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(0);
		m_WayPoints[1].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(1);
		m_WayPoints[2].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(2);
		m_WayPoints[3].Neighbors.push_back(0);
		m_WayPoints[0].Neighbors.push_back(3);
		break;



	case SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE1:
		m_WayPoints.push_back({ _float3(17.5f, 11.11f, 3.4f) });
		m_WayPoints.push_back({ _float3(22.f, 11.11f, 3.4f) });
		m_WayPoints.push_back({ _float3(22.f, 11.11f, -1.6f) });
		m_WayPoints.push_back({ _float3(17.5f, 11.11f, -1.6f) });

		m_WayPoints[0].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(0);
		m_WayPoints[1].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(1);
		m_WayPoints[2].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(2);
		m_WayPoints[3].Neighbors.push_back(0);
		m_WayPoints[0].Neighbors.push_back(3);
		break;


	case SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE_FINAL_1:
	case SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE_FINAL_2:
		m_WayPoints.push_back({ _float3(-6.5f, 18.41f, 44.5f) });
		m_WayPoints.push_back({ _float3(-6.5f, 18.25f, 40.5f) });
		m_WayPoints.push_back({ _float3(-10.5f, 18.25f, 40.5f) });
		m_WayPoints.push_back({ _float3(-10.5f, 18.25f, 44.5f) });
		m_WayPoints.push_back({ _float3(-13.5f, 18.25f, 46.5f) });
		m_WayPoints.push_back({ _float3(-4.7f, 18.41f, 46.5f) });
		m_WayPoints.push_back({ _float3(-4.7f, 18.25f, 40.5f) });
		m_WayPoints.push_back({ _float3(-8.6f, 18.25f, 37.5f) });
		m_WayPoints.push_back({ _float3(-14.f, 18.25f, 41.f) });

		m_WayPoints[0].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(0);
		m_WayPoints[1].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(1);
		m_WayPoints[2].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(2);
		m_WayPoints[3].Neighbors.push_back(0);
		m_WayPoints[0].Neighbors.push_back(3);

		m_WayPoints[3].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(3);
		m_WayPoints[4].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(0);
		m_WayPoints[0].Neighbors.push_back(4);
		m_WayPoints[5].Neighbors.push_back(0);
		m_WayPoints[0].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(3);
		m_WayPoints[3].Neighbors.push_back(5);
		m_WayPoints[5].Neighbors.push_back(6);
		m_WayPoints[6].Neighbors.push_back(5);
		m_WayPoints[6].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(6);
		m_WayPoints[6].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(6);
		m_WayPoints[7].Neighbors.push_back(1);
		m_WayPoints[1].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(7);
		m_WayPoints[7].Neighbors.push_back(8);
		m_WayPoints[8].Neighbors.push_back(7);
		m_WayPoints[8].Neighbors.push_back(2);
		m_WayPoints[2].Neighbors.push_back(8);
		m_WayPoints[3].Neighbors.push_back(8);
		m_WayPoints[8].Neighbors.push_back(3);
		m_WayPoints[8].Neighbors.push_back(4);
		m_WayPoints[4].Neighbors.push_back(8);
		break;

	default:
		break;
	}


	//인접 노드 간 거리 저장
	//m_Weights.emplace(make_pair(0, 1), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[1].vPosition) - XMLoadFloat3(&m_WayPoints[0].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(1, 0), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[1].vPosition) - XMLoadFloat3(&m_WayPoints[0].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(1, 2), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[2].vPosition) - XMLoadFloat3(&m_WayPoints[1].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(2, 1), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[2].vPosition) - XMLoadFloat3(&m_WayPoints[1].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(2, 3), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[3].vPosition) - XMLoadFloat3(&m_WayPoints[2].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(3, 2), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[3].vPosition) - XMLoadFloat3(&m_WayPoints[2].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(3, 4), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[4].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(4, 3), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[4].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(4, 5), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[5].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(5, 4), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[5].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(5, 6), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[6].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(6, 5), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[6].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(6, 7), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[7].vPosition) - XMLoadFloat3(&m_WayPoints[6].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(7, 6), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[7].vPosition) - XMLoadFloat3(&m_WayPoints[6].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(7, 8), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[8].vPosition) - XMLoadFloat3(&m_WayPoints[7].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(8, 7), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[8].vPosition) - XMLoadFloat3(&m_WayPoints[7].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(8, 9), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[8].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(9, 8), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[8].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(9, 5), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(5, 9), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[9].vPosition) - XMLoadFloat3(&m_WayPoints[5].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(9, 10), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[10].vPosition) - XMLoadFloat3(&m_WayPoints[9].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(10, 9), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[10].vPosition) - XMLoadFloat3(&m_WayPoints[9].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(10, 11), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[10].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(11, 10), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[10].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(11, 3), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(3, 11), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[3].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(11, 4), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));
	//m_Weights.emplace(make_pair(4, 11), XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_WayPoints[11].vPosition) - XMLoadFloat3(&m_WayPoints[4].vPosition), 0.f))));

	return S_OK;
}

void CState_Sneak::Initialize_PatrolPoints(SNEAKWAYPOINTINDEX _iWayIndex)
{
	switch (_iWayIndex)
	{
	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_1:
		m_PatrolWays.push_back(11);
		m_PatrolWays.push_back(12);
		m_PatrolWays.push_back(13);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2:
		m_PatrolWays.push_back(2);
		m_PatrolWays.push_back(5);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_2_2:
		m_PatrolWays.push_back(13);
		m_PatrolWays.push_back(14);
		m_PatrolWays.push_back(15);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_3:
		m_PatrolWays.push_back(1);
		m_PatrolWays.push_back(2);
		m_PatrolWays.push_back(3);
		m_PatrolWays.push_back(4);
		m_PatrolWays.push_back(5);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_1:
		m_PatrolWays.push_back(0);
		m_PatrolWays.push_back(1);
		m_isPatrolCycle = true;
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_2:
		m_PatrolWays.push_back(2);
		m_PatrolWays.push_back(3);
		m_PatrolWays.push_back(4);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_3:
		m_PatrolWays.push_back(5);
		m_PatrolWays.push_back(6);
		m_isPatrolCycle = true;
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER2_BRIDGE_4:
		m_PatrolWays.push_back(7);
		m_PatrolWays.push_back(8);
		m_PatrolWays.push_back(9);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER8_1:
		m_PatrolWays.push_back(0);
		m_PatrolWays.push_back(1);
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE1:
		m_PatrolWays.push_back(0);
		m_PatrolWays.push_back(1);
		m_PatrolWays.push_back(2);
		m_PatrolWays.push_back(3);
		m_isPatrolCycle = true;
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE_FINAL_1:
		m_PatrolWays.push_back(0);
		m_PatrolWays.push_back(1);
		m_PatrolWays.push_back(2);
		m_PatrolWays.push_back(3);
		m_isPatrolCycle = true;
		break;

	case Client::SNEAKWAYPOINTINDEX::CHAPTER8_BEETLE_FINAL_2:
		m_PatrolWays.push_back(4);
		m_PatrolWays.push_back(5);
		m_PatrolWays.push_back(6);
		m_PatrolWays.push_back(7);
		m_PatrolWays.push_back(8);
		m_isPatrolCycle = true;
		break;

	default:
		break;
	}
}

void CState_Sneak::Initialize_PatrolDirections(SNEAKWAYPOINTINDEX _iWayIndex)
{
	switch (_iWayIndex)
	{
	case Client::SNEAKWAYPOINTINDEX::CHAPTER8_1:
		m_PatrolDirections.push_back({ 0.f, 0.f, -1.f });
		m_PatrolDirections.push_back({ 0.f, 0.f, -1.f });
		break;
	}
}

void CState_Sneak::Determine_NextDirection(_fvector& _vDestination, _float3* _vDirection)
{
	if (m_WayPoints.empty())
	{
		_vector vResult = _vDestination - m_pOwner->Get_FinalPosition();
		if (0.01f >= XMVectorGetY(vResult))
			XMVectorSetY(vResult, 0.f);
		XMStoreFloat3(_vDirection, XMVector3Normalize(vResult));
		return;
	}

	_float3 vOffset = m_pOwner->Get_RayOffset();
	_float3 vRayPos; XMStoreFloat3(&vRayPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	_vector vRayDir = XMVector3Normalize(XMVectorSetY(_vDestination - XMLoadFloat3(&vRayPos), 0.f));
	_float3 vRayDirection; XMStoreFloat3(&vRayDirection, vRayDir);
	_float3 vPos; XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
	vOffset.x -= m_pOwner->Get_RayHalfWidth();
	_float3 vLeftPos; XMStoreFloat3(&vLeftPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	vOffset.x += m_pOwner->Get_RayHalfWidth() * 2.f;
	_float3 vRightPos; XMStoreFloat3(&vRightPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	_vector vLeftRayDir = XMVector3Normalize(XMVectorSetY(_vDestination - XMLoadFloat3(&vLeftPos), 0.f));
	_vector vRightRayDir = XMVector3Normalize(XMVectorSetY(_vDestination - XMLoadFloat3(&vRightPos), 0.f));
	_float3 vLeftRayDirection; XMStoreFloat3(&vLeftRayDirection, vLeftRayDir);
	_float3 vRightRayDirection; XMStoreFloat3(&vRightRayDirection, vRightRayDir);
	_vector vResult = XMVectorZero();
	_float fTargetDis = XMVectorGetX(XMVector3Length(XMVectorSetY(_vDestination - XMLoadFloat3(&vRayPos), 0.f)));

	//자유이동시 웨이포인트 타지 않음
	if (false == m_isOnWay)
	{
		//막혀있지 않다면 타겟 방향대로 이동
		//if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vRayPos, vRayDirection, XMVectorGetX(XMVector3Length(XMVectorSetY(_vDestination - XMLoadFloat3(&vRayPos), 0.f))), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE)
		//	&& false == m_pGameInstance->RayCast_Nearest_GroupFilter(vLeftPos, vRayDirection, XMVectorGetX(XMVector3Length(XMVectorSetY(_vDestination - XMLoadFloat3(&vRayPos), 0.f))), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE)
		//	&& false == m_pGameInstance->RayCast_Nearest_GroupFilter(vRightPos, vRayDirection, XMVectorGetX(XMVector3Length(XMVectorSetY(_vDestination - XMLoadFloat3(&vRayPos), 0.f))), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
		//{
		//	vResult = _vDestination - m_pOwner->Get_FinalPosition();
		//	XMStoreFloat3(_vDirection, XMVector3Normalize(XMVectorSetY(vResult, 0.f)));
		//	return;
		//}
		 
		if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(vRayPos, vRayDirection, XMVectorGetX(XMVector3Length(XMVectorSetY(_vDestination - XMLoadFloat3(&vRayPos), 0.f))), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE)
			&& false == m_pGameInstance->RayCast_Nearest_GroupFilter(vLeftPos, vLeftRayDirection, XMVectorGetX(XMVector3Length(XMVectorSetY(_vDestination - XMLoadFloat3(&vLeftPos), 0.f))), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE)
			&& false == m_pGameInstance->RayCast_Nearest_GroupFilter(vRightPos, vRightRayDirection, XMVectorGetX(XMVector3Length(XMVectorSetY(_vDestination - XMLoadFloat3(&vRightPos), 0.f))), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
		{
			vResult = _vDestination - m_pOwner->Get_FinalPosition();
			if (0.01f >= XMVectorGetY(vResult))
				XMVectorSetY(vResult, 0.f);
			XMStoreFloat3(_vDirection, XMVector3Normalize(vResult));
			return;
		}
		//타겟 방향이 막혀있으면 웨이포인트 이동
		else
		{
			m_isPathFind = true;
		}
	}

	if(m_isPathFind)
	{
		_float3 vTargetPos; XMStoreFloat3(&vTargetPos, _vDestination);
		_float3 vDest = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
		_float3 vPoint = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
		_uint iDestIndex = -1;
		_uint iStartIndex = -1;

		//타겟 위치에 가까운 웨이포인트 찾아 목표 위치로 지정 + 현재 위치와 가까운 타겟 포인트 찾기
		for (_uint Index = 0; Index < m_WayPoints.size(); ++Index)
		{
			_vector vTargetToPointDis = XMVectorSetY(_vDestination - XMLoadFloat3(&m_WayPoints[Index].vPosition), 0.f);
			//_float3 vTargetTo; XMStoreFloat3(&vTargetTo, XMVector3Normalize(vTargetToPointDis));
			_vector vPositionToPointDis = XMVectorSetY(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vPos), 0.f);
			
			if (1 == m_pGameInstance->Compare_VectorLength(XMLoadFloat3(&vDest), vTargetToPointDis))
			{
				_float3 WayPos = m_WayPoints[Index].vPosition;
				_float3 DestPos = vTargetPos;
				//바닥에 레이 닿을 수 있어 살짝 올림
				WayPos.y += 0.1f;
				DestPos.y = WayPos.y;
				_float3 DestDir;
				XMStoreFloat3(&DestDir, XMVector3Normalize(XMLoadFloat3(&DestPos) - XMLoadFloat3(&WayPos)));
				//도착점에서 목표 위치까지 막혀있지 않은지 체크
				if (false == m_pGameInstance->RayCast_Nearest_GroupFilter(WayPos, DestDir, XMVectorGetX(XMVector3Length(XMLoadFloat3(&DestPos) - XMLoadFloat3(&WayPos))),
					OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
				{
					XMStoreFloat3(&vDest, vTargetToPointDis);
					iDestIndex = Index;
				}
			}

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
					XMStoreFloat3(&vPoint, vPositionToPointDis);
					iStartIndex = Index;
				}
			}
		}

		//예외처리
		if (-1 == iDestIndex || -1 == iStartIndex || iDestIndex == iStartIndex)
		{
			XMStoreFloat3(_vDirection, XMVectorSet(0.f, 0.f, 0.f, 0.f));
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
		if (OpenMap.empty())
			return;


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

		//if(false == m_pGameInstance->RayCast_Nearest_GroupFilter(vPos, vToNextDir, XMVectorGetX(XMVector3Length(vToNext)), OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
		//{
		//	if (2 == m_pGameInstance->Compare_VectorLength(XMVectorSetY(vFromStart, 0.f), XMVectorSetY(vToNext, 0.f)))
		//	{
		//		m_Ways.push_back(iStartIndex);
		//	}
		//}
		
		reverse(m_Ways.begin(), m_Ways.end());
		m_iCurWayIndex = 0;
		m_isOnWay = true;
		m_isPathFind = false;

		vResult = XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - XMLoadFloat3(&vPos),0.f);
	}
	else
	{
		vResult = XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - XMLoadFloat3(&vPos),0.f);
	}
	if(0.01f < XMVectorGetX(XMVector3LengthSq(vResult)))
	{
		XMStoreFloat3(_vDirection, XMVector3Normalize(vResult));
	}
	else
	{
		XMStoreFloat3(_vDirection, vResult);
	}
}

void CState_Sneak::Determine_BackDirection(_fvector& _vDestination, _float3* _vDirection)
{
	_float3 vOffset = m_pOwner->Get_RayOffset();
	_float3 vRayPos; XMStoreFloat3(&vRayPos, XMVector3Transform(XMLoadFloat3(&vOffset), m_pOwner->Get_FinalWorldMatrix()));
	_vector vRayDir = XMVector3Normalize(XMVectorSetY(_vDestination - XMLoadFloat3(&vRayPos), 0.f));
	_float3 vRayDirection; XMStoreFloat3(&vRayDirection, vRayDir);
	_float3 vPos; XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
	_vector vResult = XMVectorZero();

	if (m_isPathFind)
	{
		_float3 vTargetPos; XMStoreFloat3(&vTargetPos, _vDestination);
		_float3 vDest = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
		_float3 vPoint = _float3(100.f, 0.f, 100.f); //큰 임의값 적용
		_uint iDestIndex = 0;
		_uint iStartIndex = 0;

		//타겟 위치에 가까운 웨이포인트 찾아 목표 위치로 지정 
		

		//현재 위치와 가까운 타겟 포인트 찾기
		for (_uint Index = 0; Index < m_WayPoints.size(); ++Index)
		{
			_vector vTargetToPointDis = XMVectorSetY(_vDestination - XMLoadFloat3(&m_WayPoints[Index].vPosition), 0.f);
			//_float3 vTargetTo; XMStoreFloat3(&vTargetTo, XMVector3Normalize(vTargetToPointDis));
			_vector vPositionToPointDis = XMVectorSetY(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vPos), 0.f);

			if (1 == m_pGameInstance->Compare_VectorLength(XMLoadFloat3(&vDest), vTargetToPointDis))
			{
				XMStoreFloat3(&vDest, vTargetToPointDis);
				iDestIndex = Index;
			}

			//시작점 찾는데 시작 점을 여러개로 쓸지 보고 판단
			if (1 == m_pGameInstance->Compare_VectorLength(XMLoadFloat3(&vPoint), vPositionToPointDis))
			{
				_float3 vPosTo; XMStoreFloat3(&vPosTo, XMVector3Normalize(vPositionToPointDis));
				//가는길에 장애물 없으면
				if (true == m_pGameInstance->RayCast_Nearest_GroupFilter(vPos, vPosTo, XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_WayPoints[Index].vPosition) - XMLoadFloat3(&vPos))),
					OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE))
				{
					XMStoreFloat3(&vPoint, vPositionToPointDis);
					iStartIndex = Index;
				}
			}
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
		_vector vToNext = XMLoadFloat3(&m_WayPoints[m_Ways[m_Ways.size() - 1]].vPosition) - m_pOwner->Get_FinalPosition();
		if (2 == m_pGameInstance->Compare_VectorLength(XMVectorSetY(vFromStart, 0.f), XMVectorSetY(vToNext, 0.f)))
		{
			m_Ways.push_back(iStartIndex);
		}

		reverse(m_Ways.begin(), m_Ways.end());
		m_iCurWayIndex = 0;
		m_isOnWay = true;
		m_isPathFind = false;

		vResult = XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - XMLoadFloat3(&vPos), 0.f);
	}
	else
	{
		vResult = XMVectorSetY(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition) - XMLoadFloat3(&vPos), 0.f);
	}

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

void CState_Sneak::Free()
{
	__super::Free();
}
