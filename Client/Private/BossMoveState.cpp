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

	return S_OK;
}


void CBossMoveState::State_Enter()
{
	//테스트 위해 지정 포인트가 아닌 오프셋으로 적용
	//XMStoreFloat3(&m_vecMovePoints[0], m_pOwner->Get_FinalPosition() + (XMLoadFloat3(&m_vecMovePoints[0]) - XMLoadFloat3(&m_vecMovePoints[2])));
	//XMStoreFloat3(&m_vecMovePoints[1], m_pOwner->Get_FinalPosition() + (XMLoadFloat3(&m_vecMovePoints[1]) - XMLoadFloat3(&m_vecMovePoints[2])));
	//XMStoreFloat3(&m_vecMovePoints[2], m_pOwner->Get_FinalPosition());


	m_pOwner->Set_AnimChangeable(false);
	m_iMoveIndex = static_cast<CButterGrump*>(m_pOwner)->Get_CurMoveIndex();

	if (1 == m_iMoveIndex)
	{
		m_fOriginSpeed = m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec();
		m_pOwner->Get_ControllerTransform()->Set_SpeedPerSec(m_fOriginSpeed * 1.5f);
	}
}

void CBossMoveState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;


	_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());

	m_pOwner->Get_ControllerTransform()->Turn_To_DesireDir(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec() * _fTimeDelta);

	//m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

	CButterGrump* pBoss = static_cast<CButterGrump*>(m_pOwner);

	if(true == pBoss->Is_OnMove())
	{
		//지정 위치에 도착하면
		if (m_pOwner->Get_ControllerTransform()->MoveTo(XMLoadFloat3(&m_vecMovePoints[m_iMoveIndex]), _fTimeDelta))
		{
			pBoss->Set_OnMove(false);
		}
	}
	else
	{
		if (true == m_pOwner->Get_AnimChangeable())
		{
			Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
		}
	}
}

void CBossMoveState::State_Exit()
{
	if (1 == m_iMoveIndex)
	{
		m_pOwner->Get_ControllerTransform()->Set_SpeedPerSec(m_fOriginSpeed);
	}

	if (m_vecMovePoints.size() <= ++m_iMoveIndex)
		m_iMoveIndex = 0;
	static_cast<CButterGrump*>(m_pOwner)->Set_CurMoveIndex(m_iMoveIndex);
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
