#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "BossAttackState.h"
#include "Monster.h"
#include "FSM.h"

CBossAttackState::CBossAttackState()
{
}

HRESULT CBossAttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelay = 0.5f;
		
	return S_OK;
}


void CBossAttackState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CBossAttackState::State_Update(_float _fTimeDelta)
{
	m_fAccTime += _fTimeDelta;
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(m_pTarget->Get_Position() - m_pOwner->Get_Position());
	//АјАн
	//m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_Position());
	m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta);
	m_pOwner->Attack(_fTimeDelta);
	
}

void CBossAttackState::State_Exit()
{
	m_fAccTime = 0.f;
}

CBossAttackState* CBossAttackState::Create(void* _pArg)
{
	CBossAttackState* pInstance = new CBossAttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBossAttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossAttackState::Free()
{
	__super::Free();
}
