#include "stdafx.h"
#include "PatrolState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CPatrolState::CPatrolState()
{
}

HRESULT CPatrolState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fPatrolOffset = 7.f;
	m_iDir = -1;
	m_iPrevDir = -1;
	m_fDelayTime = 1.f;
		
	return S_OK;
}

void CPatrolState::Set_Bound(_float3& _vPosition)
{
	m_tPatrolBound.fMinX = _vPosition.x - m_fPatrolOffset;
	m_tPatrolBound.fMaxX = _vPosition.x + m_fPatrolOffset;
	m_tPatrolBound.fMinZ = _vPosition.z - m_fPatrolOffset;
	m_tPatrolBound.fMaxZ = _vPosition.z + m_fPatrolOffset;
}


void CPatrolState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CPatrolState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (true == m_isMove)
		m_fAccTime += _fTimeDelta;

	//적 발견 시 ALERT 전환
	_float dis = XMVectorGetX(XMVector3Length((m_pTarget->Get_Position() - m_pOwner->Get_Position())));
	if (dis <= m_fAlertRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::ALERT, m_pFSM);
		return;
	}

	/*순찰 이동 로직*/
	//랜덤으로 방향 설정
	
	if (false == m_isMove)
	{
		while (true)
		{
			//8 방향 중 랜덤 방향 지정
			m_iDir = static_cast<_int>(floor(m_pGameInstance->Compute_Random(0.f, 8.f)));
			if (m_iDir != m_iPrevDir || 0 > m_iPrevDir)	//직전에 갔던 방향은 가지 않음
			{
				m_iPrevDir = m_iDir;
				m_fMoveDistance = m_pGameInstance->Compute_Random(0.5f * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec(), 1.5f * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
				m_isMove = true;
				break;
			}
		}
	}
	
	//다음 위치가 구역을 벗어나는지 체크 후 벗어나면 정지 후 반대방향으로 진행
	if(false == m_isBack)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pOwner->Get_Position() + Determine_NextDirection(m_iDir));
		if (m_tPatrolBound.fMinX > vPos.x || m_tPatrolBound.fMaxX < vPos.x || m_tPatrolBound.fMinZ > vPos.z || m_tPatrolBound.fMaxZ < vPos.z)
		{
			m_isBack = true;
			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
			return;
		}
	}
	else
	{
		if (4 > m_iDir)
			m_iDir += 4;
		else
			m_iDir -= 4;

		m_isBack = false;
	}

	//이동
	PatrolMove(_fTimeDelta, m_iDir);
}

void CPatrolState::State_Exit()
{
	m_fAccTime = 0.f;
	m_fAccDistance = 0.f;
}

void CPatrolState::PatrolMove(_float _fTimeDelta, _int _iDir)
{
	if (0 > _iDir || 8 < _iDir)
		return;

	//회전중인 거도 해야함 일단 이동만 해봄



	if (true == m_isMove)
	{
		//기본적으로 추적중에 y값 상태 변화는 없다고 가정
		
		_vector vDir = Determine_NextDirection(_iDir);
		m_pOwner->Get_ControllerTransform()->LookAt_3D(vDir + m_pOwner->Get_Position());
		//m_pOwner->Get_ControllerTransform()->Go_Straight(_fTimeDelta);
		m_pOwner->Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
		m_fAccDistance += m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta;

		if (m_fMoveDistance <= m_fAccDistance)
		{
			m_fAccDistance = 0.f;
			m_isMove = false;

			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		}
	}
}

_vector CPatrolState::Determine_NextDirection(_int _iDir)
{
	_vector vDir = {};
	switch (_iDir)
	{
	case 0:
		vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "상" << endl;
		break;
	case 1:
		vDir = XMVectorSet(1.f, 0.f, 1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "우상" << endl;
		break;
	case 2:
		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "우" << endl;
		break;
	case 3:
		vDir = XMVectorSet(1.f, 0.f, -1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "우하" << endl;
		break;
	case 4:
		vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "하" << endl;
		break;
	case 5:
		vDir = XMVectorSet(-1.f, 0.f, -1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "좌하" << endl;
		break;
	case 6:
		vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "좌" << endl;
		break;
	case 7:
		vDir = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
		//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position() + vDir);
		//cout << "좌상" << endl;
		break;
	default:
		break;
	}

	return vDir;
}

CPatrolState* CPatrolState::Create(void* _pArg)
{
	CPatrolState* pInstance = new CPatrolState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CPatrolState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPatrolState::Free()
{
	__super::Free();
}
