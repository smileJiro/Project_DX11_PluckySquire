#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossWingSliceState.h"
#include "Monster.h"
#include "FSM.h"

CBossWingSliceState::CBossWingSliceState()
{
}

HRESULT CBossWingSliceState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 0.5f;
	m_iNumAttack = 1;
	
	return S_OK;
}


void CBossWingSliceState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
	m_fAccTime = 0.f;
	m_iAttackCount = 0;
}

void CBossWingSliceState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	m_fAccTime += _fTimeDelta;

	//m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	//АјАн
	//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_FinalPosition());
	//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);
	// 
	//if (m_iNumAttack > m_iAttackCount)
	//{
	//	m_pOwner->Attack();
	//	++m_iAttackCount;
	//}
	//else
	//{
	//	Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	//}

	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
	m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);

	if (true == m_pOwner->Get_AnimChangeable())
	{
		Event_ChangeBossState(BOSS_STATE::IDLE, m_pFSM);
	}
	
}

void CBossWingSliceState::State_Exit()
{
	m_fAccTime = 0.f;
	m_iAttackCount = 0;
}

CBossWingSliceState* CBossWingSliceState::Create(void* _pArg)
{
	CBossWingSliceState* pInstance = new CBossWingSliceState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossWingSliceState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossWingSliceState::Free()
{
	__super::Free();
}
