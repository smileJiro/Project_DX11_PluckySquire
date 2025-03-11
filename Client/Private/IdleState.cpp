#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "IdleState.h"
#include "Monster.h"
#include "FSM.h"

CIdleState::CIdleState()
{
}

HRESULT CIdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;

	if (0 <= pDesc->fDelayTime)
	{
		m_fDelayTime = pDesc->fDelayTime;
	}
	else
	{
		m_fDelayTime = 3.f;
	}

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CIdleState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CIdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (false == m_pOwner->Is_Stay())
		m_fAccTime += _fTimeDelta;

	if (nullptr != m_pTarget)
	{
		if(m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
		{
			//몬스터 인식 범위 안에 들어오면 인식상태로 전환
			_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
			if (COORDINATE_2D == m_pOwner->Get_CurCoord() && m_pOwner->Get_Include_Section_Name() == m_pTarget->Get_Include_Section_Name())
			{
				if (fDis <= m_fAlert2DRange)
				{
					Event_ChangeMonsterState(MONSTER_STATE::ALERT, m_pFSM);
					return;
				}
			}

			else if (COORDINATE_3D == m_pOwner->Get_CurCoord())
			{
				Check_Target3D();
			}
		}
	}
	
	if (m_fDelayTime <= m_fAccTime)
	{
		Event_ChangeMonsterState(MONSTER_STATE::PATROL, m_pFSM);
	}

}

void CIdleState::State_Exit()
{
	m_fAccTime = 0.f;
}

CIdleState* CIdleState::Create(void* _pArg)
{
	CIdleState* pInstance = new CIdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIdleState::Free()
{
	__super::Free();
}
