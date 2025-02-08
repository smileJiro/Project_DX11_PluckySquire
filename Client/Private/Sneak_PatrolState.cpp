#include "stdafx.h"
#include "Sneak_PatrolState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CSneak_PatrolState::CSneak_PatrolState()
{
}

HRESULT CSneak_PatrolState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fSneak_PatrolOffset = 7.f;
	m_fSneak_Patrol2DOffset = m_fSneak_PatrolOffset * 100.f;
	m_iPrevDir = -1;
	m_iDir = -1;
	//m_fDelayTime = 1.f;

	m_Waypoints.push_back(_float3(-17.f, 6.55f, 23.f));
	m_Waypoints.push_back(_float3(-20.f, 6.55f, 23.f));
	m_Waypoints.push_back(_float3(-23.f, 6.55f, 20.5f));
		
	return S_OK;
}

void CSneak_PatrolState::Set_Bound(_float3& _vPosition)
{
	//일단 현재 위치 기준으로 잡음
	_vector vResult=XMLoadFloat3(&_vPosition);
	_vector vOffset = XMVectorZero();
	if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
		vOffset = XMVectorReplicate(m_fSneak_PatrolOffset);
	else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
		vOffset = XMVectorReplicate(m_fSneak_Patrol2DOffset);

	XMStoreFloat3(&m_tSneak_PatrolBound.vMin, vResult - vOffset);
	XMStoreFloat3(&m_tSneak_PatrolBound.vMax, vResult + vOffset);
}


void CSneak_PatrolState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CSneak_PatrolState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	//일단 적용해봄
	//if(COORDINATE_3D == m_pOwner->Get_CurCoord())
	//{
	//	if (true == m_isTurn)
	//	{
	//		m_pOwner->Rotate_To(XMLoadFloat3(&m_vRotate), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());
	//		//각속도 0이면
	//		if (XMVectorGetY(XMVectorEqual(static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Get_AngularVelocity(), XMVectorZero())))
	//		{
	//			m_isTurn = false;
	//		}
	//		return;
	//	}
	//}

	if (true == m_isMove)
		m_fAccTime += _fTimeDelta;

	if (nullptr != m_pTarget)
	{
		if (m_pOwner->Get_CurCoord() == COORDINATE_LAST)
			return;
		//적 발견 시 ALERT 전환
		if (m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
		{	
			Check_Target3D(true);

			//플레이어가 인식되지 않았을 경우 소리가 나면 경계로 전환 
			if (m_pOwner->IsTarget_In_Sneak_Detection())
			{
				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
				return;
			}
		}
	}

	//정해진 웨이포인트에서 순찰해야함
	if (false == m_isTurn)
	{
		Determine_Direction();
	}
	
	//정해진 웨이포인트가 아니면 복귀 해야함
	//Check_Bound(_fTimeDelta);

	//이동
	Sneak_PatrolMove(_fTimeDelta, m_iDir);
}

void CSneak_PatrolState::State_Exit()
{
	m_fAccTime = 0.f;
	m_fAccDistance = 0.f;
	m_isTurn = false;
}

void CSneak_PatrolState::Sneak_PatrolMove(_float _fTimeDelta, _int _iDir)
{
	if (m_Waypoints.size() <= m_iCurWayIndex)
		return;

	//기본적으로 추적중에 y값 상태 변화는 없다고 가정
	_vector vDir = XMVector3Normalize(Set_Sneak_PatrolDirection(_iDir));

	if (true == m_isTurn && false == m_isMove)
	{
		if (m_pOwner->Rotate_To_Radians(vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
		{
			m_isMove = true;

			m_pOwner->Change_Animation();
		}
		else
		{
			_bool isCW = true;
			_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), vDir));
			if (fResult < 0)
				isCW = false;

			m_pOwner->Turn_Animation(isCW);
		}
	}

	if (true == m_isMove)
	{
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(vDir + m_pOwner->Get_FinalPosition());
		//m_pOwner->Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
		//m_pOwner->Add_Force(vDir * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec()); //임시 속도
		m_pOwner->Get_ActorCom()->Set_LinearVelocity(vDir, m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec()); //임시 속도
	}

	if (true == m_isMove)
	{
		m_fAccDistance += m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta;
		if (m_fMoveDistance <= m_fAccDistance)
		{
			m_fAccDistance = 0.f;
			m_isTurn = false;
			m_isMove = false;

			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}
	}
}

void CSneak_PatrolState::Determine_Direction()
{
	if (COORDINATE::COORDINATE_LAST == m_pOwner->Get_CurCoord())
		return;


	//다음 웨이 포인트로 넘어감.
	if (false == m_isBack)
	{
		++m_iCurWayIndex;

		if (m_Waypoints.size() - 1 == m_iCurWayIndex)
			m_isBack = true;

		//예외처리
		if (m_Waypoints.size()-1 < m_iCurWayIndex)
		{
			m_iCurWayIndex = m_Waypoints.size() - 1;
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
	if (0 == m_iCurWayIndex || m_Waypoints.size() - 1 == m_iCurWayIndex)
		m_fDelayTime = m_pGameInstance->Compute_Random(1.f, 3.f);
	else
	{
		m_fDelayTime = m_pGameInstance->Compute_Random(0.f, 2.f);
	}
}

_vector CSneak_PatrolState::Set_Sneak_PatrolDirection(_int _iDir)
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

void CSneak_PatrolState::Check_Bound(_float _fTimeDelta)
{
	_float3 vPos;
	_bool isOut = false;
	//델타타임으로 다음 위치 예상해서 막기
	XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition() + Set_Sneak_PatrolDirection(m_iDir) * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta);
	//나갔을 때 반대방향으로
	//XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		if (m_tSneak_PatrolBound.vMin.x > vPos.x || m_tSneak_PatrolBound.vMax.x < vPos.x || m_tSneak_PatrolBound.vMin.z > vPos.z || m_tSneak_PatrolBound.vMax.z < vPos.z)
		{
			isOut = true;
		}
	}
	else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
	{
		//일단 2D는 처리안해놓음
		/*if (m_tSneak_PatrolBound.vMin.x > vPos.x || m_tSneak_PatrolBound.vMax.x < vPos.x || m_tSneak_PatrolBound.vMin.y > vPos.y || m_tSneak_PatrolBound.vMax.y < vPos.y)
		{
			isOut = true;
		}*/
	}

	if (true == isOut)
	{
		m_isBack = true;
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
	}
	//반대방향으로 진행해도 경계를 벗어나는 경우가 있나?
	if (true == m_isBack)
	{
		if (COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			if (4 > m_iDir)
				m_iDir += 4;
			else
				m_iDir -= 4;
		}

		else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			if (2 > m_iDir)
				m_iDir += 2;
			else
				m_iDir -= 2;
		}

		m_isBack = false;
	}
}

CSneak_PatrolState* CSneak_PatrolState::Create(void* _pArg)
{
	CSneak_PatrolState* pInstance = new CSneak_PatrolState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_PatrolState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_PatrolState::Free()
{
	__super::Free();
}
