#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ChaseAttackState.h"
#include "Monster.h"
#include "FSM.h"

CChaseAttackState::CChaseAttackState()
{
}

HRESULT CChaseAttackState::Initialize(void* _pArg)
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


void CChaseAttackState::State_Enter()
{
}

void CChaseAttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if(true == m_pOwner->IsContactToTarget())
		Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);

	_vector vDir = m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition();
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));	//3D상에서 y값도 더해서 거리 계산하는거 주의
	XMVectorSetY(vDir, XMVectorGetY(m_pOwner->Get_FinalPosition()));

	//추적 범위 벗어나면 IDLE 전환
	if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
	else
	{
		if(COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			//추적
			m_pOwner->Move_To(m_pTarget->Get_FinalPosition());
			m_pOwner->Rotate_To_Radians(vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());
		}
		else if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			//방향전환
			m_pOwner->Change_Dir();
			m_pOwner->Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
		}
	}
}

void CChaseAttackState::State_Exit()
{
}

CChaseAttackState* CChaseAttackState::Create(void* _pArg)
{
	CChaseAttackState* pInstance = new CChaseAttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CChaseAttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaseAttackState::Free()
{
	__super::Free();
}
