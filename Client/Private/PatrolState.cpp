#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "PatrolState.h"
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

	m_fPatrolOffset = 5.f;
	m_iPrevDir = -1;
		
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
	_int iDir = -1;
	
	if (false == m_isMove)
	{
		while (true)
		{
			iDir = static_cast<_int>(floor(m_pGameInstance->Compute_Random(0.f, 8.f)));
			if (iDir != m_iPrevDir || 0 > m_iPrevDir)	//이거 고쳐야댐
				break;
			m_iPrevDir = iDir;
			m_fMoveTime = m_pGameInstance->Compute_Random(0.5f, 1.5f);
		}
	}
	
	//구역 벗어났는지 체크 후 벗어나면 정지 후 반대방향으로 진행

	//이동
	PatrolMove(_fTimeDelta, iDir);
}

void CPatrolState::State_Exit()
{
	m_isMove = false;
	m_fAccTime = 0.f;
}

void CPatrolState::PatrolMove(_float _fTimeDelta, _int _iDir)
{
	//회전중인 거도 해야함 체크 일단 이동만 해봄

	if (m_fMoveTime <= m_fAccTime)
	{
		m_fAccTime = 0.f;
		m_isMove = false;
		return;
	}

	if (false == m_isMove || -1 == _iDir)
	{
		switch (_iDir)
		{
		case 0:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(0.f, 0.f, 1.f, 0.f));
			break;
		case 1:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(1.f, 0.f, 1.f, 0.f));
			break;
		case 2:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(0.f, 0.f, 1.f, 0.f));
			break;
		case 3:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(1.f, 0.f, -1.f, 0.f));
			break;
		case 4:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(0.f, 0.f, -1.f, 0.f));
			break;
		case 5:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(-1.f, 0.f, -1.f, 0.f));
			break;
		case 6:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
			break;
		case 7:
			m_pOwner->Get_ControllerTransform()->LookAt_3D(XMVectorSet(-1.f, 0.f, 1.f, 0.f));
			break;
		default:
			break;
		}
	}
	m_pOwner->Get_ControllerTransform()->Go_Straight(_fTimeDelta);
	m_isMove = true;
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
