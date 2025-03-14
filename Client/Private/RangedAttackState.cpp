#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "RangedAttackState.h"
#include "Monster.h"
#include "FSM.h"

CRangedAttackState::CRangedAttackState()
{
}

HRESULT CRangedAttackState::Initialize(void* _pArg)
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


void CRangedAttackState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
	m_pOwner->Set_PreAttack(true);
}

void CRangedAttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (m_pTarget->Get_CurCoord() != m_pOwner->Get_CurCoord())
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		return;
	}
	else if (COORDINATE_2D == m_pOwner->Get_CurCoord() && m_pOwner->Get_Include_Section_Name() != m_pTarget->Get_Include_Section_Name())
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		return;
	}


	if (true == m_pOwner->Get_PreAttack())
	{
		if (COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
			if (true == m_pOwner->Rotate_To_Radians(XMVectorSetY(vDir, 0.f), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
			{
				m_pOwner->Stop_Rotate();
			}
		}
		//else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		//{
		//	m_pOwner->Change_Dir();
		//}
	}

	_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
	//if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	//{
	//	//m_pOwner->Attack();
	//}
	//else
	//{
	//	Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);
	//}
	if(true == m_pOwner->Get_AnimChangeable())
		Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);

	////공격 범위 벗어나고 추적 범위 내면 Chase 전환
	//if (fDis > Get_CurCoordRange(MONSTER_STATE::ATTACK) && fDis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
	//{
	//	Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	//	return;
	//}
	////범위 전부를 벗어나면 Idle 전환
	//if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	//{
	//	Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	//}
	//else
	//{
	//	//타겟 방향으로 회전 후 공격
	//	/*if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
	//	{
	//		m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTarget->Get_FinalPosition());
	//	}
	//	else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
	//	{
	//		m_pOwner->Change_Dir();
	//	}*/
	//	m_pOwner->Attack();
	//}
}

void CRangedAttackState::State_Exit()
{
	m_pOwner->Set_PreAttack(false);
}

CRangedAttackState* CRangedAttackState::Create(void* _pArg)
{
	CRangedAttackState* pInstance = new CRangedAttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CRangedAttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRangedAttackState::Free()
{
	__super::Free();
}
