#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MeleeAttackState.h"
#include "Monster.h"
#include "FSM.h"

CMeleeAttackState::CMeleeAttackState()
{
}

HRESULT CMeleeAttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CMeleeAttackState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CMeleeAttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	//АјАн
	//if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
	//{
	//	m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_FinalPosition());
	//}
	//else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
	//{
	//	m_pOwner->Change_Dir();
	//}
	Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);
}

void CMeleeAttackState::State_Exit()
{
}

CMeleeAttackState* CMeleeAttackState::Create(void* _pArg)
{
	CMeleeAttackState* pInstance = new CMeleeAttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CMeleeAttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeleeAttackState::Free()
{
	__super::Free();
}
