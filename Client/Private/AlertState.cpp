#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "AlertState.h"
#include "Monster.h"
#include "FSM.h"

CAlertState::CAlertState()
{
}

HRESULT CAlertState::Initialize(void* _pArg)
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


void CAlertState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CAlertState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	
	_float dis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
	if (dis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	{
		if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			//굳이 멤버변수로 써야하나
			m_isTurn = true;
			_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());

			//다 돌았으면 회전 애니메이션 재생 안하도록
			if (true == m_pOwner->Rotate_To_Radians(XMVectorSetY(vDir, 0.f), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
				m_isTurn = false;

			if (false == m_isTurn)
				Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);
		}
		else
			Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);
		
		return;
	}
	if (dis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	}
	else
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
}

void CAlertState::State_Exit()
{
}

CAlertState* CAlertState::Create(void* _pArg)
{
	CAlertState* pInstance = new CAlertState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CAlertState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlertState::Free()
{
	__super::Free();
}
