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

	m_fPatrolOffset = 10.f;
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
}

void CPatrolState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (false == m_isMove)
		m_fAccTime += _fTimeDelta;
	//if (m_fDelayTime <= m_fAccTime)
	//{
	//	m_fAccTime = 0.f;
	//	m_isDelay = false;
	//}

	//적 발견 시 ALERT 전환
	_float dis = XMVectorGetX(XMVector3Length((m_pTarget->Get_Position() - m_pOwner->Get_Position())));
	if (dis <= m_fAlertRange)
	{
		Event_ChangeMonsterState(MONSTER_STATE::ALERT, m_pFSM);
		return;
	}

	//딜레이 동안 움직이지 않도록
	/*if (true == m_isDelay)
		return;*/

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
	
	//구역 벗어났는지 체크 후 벗어나면 정지 후 반대방향으로 진행
	if(false == m_isBack)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pOwner->Get_Position());
		if (m_tPatrolBound.fMinX > vPos.x || m_tPatrolBound.fMaxX < vPos.x || m_tPatrolBound.fMinZ > vPos.z || m_tPatrolBound.fMaxZ < vPos.z)
		{
			if (4 > m_iDir)
				m_iDir += 4;
			else
				m_iDir -= 4;

			m_isBack = true;
		}
	}

	//이동
	PatrolMove(_fTimeDelta, m_iDir);
}

void CPatrolState::State_Exit()
{
	//m_isDelay = false;
	m_isMove = false;
	m_isBack = false;
	m_fAccTime = 0.f;
	m_fAccDistance = 0.f;
}

void CPatrolState::PatrolMove(_float _fTimeDelta, _int _iDir)
{
	if (0 > _iDir || 8 < _iDir)
		return;

	//회전중인 거도 해야함 체크 일단 이동만 해봄



	if (true == m_isMove)
	{
		//기본적으로 추적중에 y값 상태 변화는 없다고 가정
		_vector vDir = {};
		switch (_iDir)
		{
		case 0:
			vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		case 1:
			vDir = XMVectorSet(1.f, 0.f, 1.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		case 2:
			vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		case 3:
			vDir = XMVectorSet(1.f, 0.f, -1.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		case 4:
			vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		case 5:
			vDir = XMVectorSet(-1.f, 0.f, -1.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		case 6:
			vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		case 7:
			vDir = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
			m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pOwner->Get_Position()+vDir);
			break;
		default:
			break;
		}
		std::cout << _iDir << endl;
		//m_pOwner->Get_ControllerTransform()->Go_Straight(_fTimeDelta);
		m_pOwner->Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
		m_fAccDistance += m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta;

		if (m_fMoveDistance <= m_fAccDistance)
		{
			m_fAccDistance = 0.f;
			m_isMove = false;
			m_isBack = false;
			//m_isDelay = true;

			Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		}
	}
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
