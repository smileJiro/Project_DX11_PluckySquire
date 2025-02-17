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


void CSideScroll_PatrolState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CSideScroll_PatrolState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	//m_pOwner->Get_ControllerTransform()->Go_Right(_fTimeDelta);
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
