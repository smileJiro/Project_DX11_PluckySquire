#include "stdafx.h"
#include "SideScroll_PatrolState.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CSideScroll_PatrolState::CSideScroll_PatrolState()
{
}

HRESULT CSideScroll_PatrolState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_iPrevDir = -1;
	m_iDir = -1;
	m_eDir = F_DIRECTION::F_DIR_LAST;
	m_fDelayTime = 1.f;
		
	return S_OK;
}

void CSideScroll_PatrolState::Initialize_SideScroll_PatrolBound(SIDESCROLL_PATROLBOUND _eSideScroll_Bound)
{
	switch (_eSideScroll_Bound)
	{
	case Client::SIDESCROLL_PATROLBOUND::CHAPTER2_1_1:
		m_tPatrolBound.fMinX = 150.f;
		m_tPatrolBound.fMaxX = 480.f;
		break;
	case Client::SIDESCROLL_PATROLBOUND::CHAPTER2_1_2:
		m_tPatrolBound.fMinX = -500.f;
		m_tPatrolBound.fMaxX = -320.f;
		break;
	case Client::SIDESCROLL_PATROLBOUND::CHAPTER2_1_3:
		m_tPatrolBound.fMinX = -400.f;
		m_tPatrolBound.fMaxX = -130.f;
		break;
	case Client::SIDESCROLL_PATROLBOUND::CHAPTER2_1_4:
		m_tPatrolBound.fMinX = -650.f;
		m_tPatrolBound.fMaxX = -520.f;
		break;
	case Client::SIDESCROLL_PATROLBOUND::CHAPTER2_1_5:
		m_tPatrolBound.fMinX = -650.f;
		m_tPatrolBound.fMaxX = -440.f;
		break;
	}
}


void CSideScroll_PatrolState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CSideScroll_PatrolState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Get_ControllerTransform()->Go_Right(_fTimeDelta);
	_float4 vPos; XMStoreFloat4(&vPos, m_pOwner->Get_FinalPosition());
	//오른쪽을 넘어서면
	if (m_tPatrolBound.fMaxX <= vPos.x)
	{
		vPos.x = m_tPatrolBound.fMaxX;
		m_pOwner->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vPos);
		m_pOwner->Set_2D_Direction(F_DIRECTION::LEFT);
	}

	//왼쪽을 넘어서면
	if (m_tPatrolBound.fMinX >= vPos.x)
	{
		vPos.x = m_tPatrolBound.fMinX;
		m_pOwner->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vPos);
		m_pOwner->Set_2D_Direction(F_DIRECTION::RIGHT);
	}
}

void CSideScroll_PatrolState::State_Exit()
{
	m_fAccTime = 0.f;
	m_fAccDistance = 0.f;
	m_fMoveDistance = 0.f;
	m_isTurn = false;
	m_isMove = false;
}

CSideScroll_PatrolState* CSideScroll_PatrolState::Create(void* _pArg)
{
	CSideScroll_PatrolState* pInstance = new CSideScroll_PatrolState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSideScroll_PatrolState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSideScroll_PatrolState::Free()
{
	__super::Free();
}
