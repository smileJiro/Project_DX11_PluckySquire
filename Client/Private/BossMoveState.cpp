#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossMoveState.h"
#include "ButterGrump.h"
#include "FSM.h"

CBossMoveState::CBossMoveState()
{
}

HRESULT CBossMoveState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_vecMovePoints.push_back({ -29.5f, 50.5f, -7.5f });
	m_vecMovePoints.push_back({ 29.5f, 60.5f, -1.5f });
	m_vecMovePoints.push_back({ 0.53f, 60.35f, -8.0f});

	//테스트 위해 지정 포인트가 아닌 오프셋으로 적용
	XMStoreFloat3(&m_vecMovePoints[0], m_pOwner->Get_FinalPosition() + (XMLoadFloat3(&m_vecMovePoints[0]) - XMLoadFloat3(&m_vecMovePoints[2])));
	XMStoreFloat3(&m_vecMovePoints[1], m_pOwner->Get_FinalPosition() + (XMLoadFloat3(&m_vecMovePoints[1]) - XMLoadFloat3(&m_vecMovePoints[2])));
	XMStoreFloat3(&m_vecMovePoints[2], m_pOwner->Get_FinalPosition());

	return S_OK;
}


void CBossMoveState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CBossMoveState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

	//지정 위치에 도착하면
	if (m_pOwner->Get_ControllerTransform()->MoveToTarget(XMLoadFloat3(&m_vecMovePoints[m_iMoveIndex]), _fTimeDelta))
	{
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
		++m_iMoveIndex;
		if (m_vecMovePoints.size() <= m_iMoveIndex)
			m_iMoveIndex = 0;
	}
}

void CBossMoveState::State_Exit()
{
	m_fAccTime = 0.f;
}

CBossMoveState* CBossMoveState::Create(void* _pArg)
{
	CBossMoveState* pInstance = new CBossMoveState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossMoveState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossMoveState::Free()
{
	__super::Free();
}
