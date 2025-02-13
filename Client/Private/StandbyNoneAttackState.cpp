#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "StandbyNoneAttackState.h"
#include "Monster.h"
#include "FSM.h"

CStandbyNoneAttackState::CStandbyNoneAttackState()
{
}

HRESULT CStandbyNoneAttackState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;
	m_fDelayTime = pDesc->fDelayTime;
	m_fCoolTime = pDesc->fCoolTime;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CStandbyNoneAttackState::State_Enter()
{
	m_fAccTime = 0.f;
	m_isDelay = false;
	m_isCool = false;
}

void CStandbyNoneAttackState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;
	
	_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
	if (fDis <= Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		if (false == m_pOwner->IsDelay() && false == m_pOwner->IsCool())
		{
			Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
			return;
		}
		else if (true == m_pOwner->IsDelay() || true == m_pOwner->IsCool())
		{
			//가만히 있으면서 타겟 따라 회전 + 애니메이션
			if (COORDINATE::COORDINATE_3D == m_pOwner->Get_CurCoord())
			{
				_vector vDir = XMVector3Normalize(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
			
				m_pOwner->Rotate_To_Radians(XMVectorSetY(vDir, 0.f), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());
			}
			else if (COORDINATE::COORDINATE_2D == m_pOwner->Get_CurCoord())
			{
				m_pOwner->Change_Dir();
			}
		}
	}
	//범위 나가면 idle 전환
	else if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
}

void CStandbyNoneAttackState::State_Exit()
{
}

CStandbyNoneAttackState* CStandbyNoneAttackState::Create(void* _pArg)
{
	CStandbyNoneAttackState* pInstance = new CStandbyNoneAttackState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CStandbyNoneAttackState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStandbyNoneAttackState::Free()
{
	__super::Free();
}
