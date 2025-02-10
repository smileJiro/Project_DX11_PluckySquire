#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_ChaseState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_ChaseState::CSneak_ChaseState()
{
}

HRESULT CSneak_ChaseState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CSneak_ChaseState::State_Enter()
{
}

void CSneak_ChaseState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	//플레이어 한테 직선으로 와야하는데 장애물이 있는 경우 장애물을 돌아서 혹은 길을 따라 이동해야함

	_vector vDir = m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition();
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));	//3D상에서 y값도 더해서 거리 계산하는거 주의
	XMVectorSetY(vDir, XMVectorGetY(m_pOwner->Get_FinalPosition()));
	if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	{
		//공격 준비로 전환
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_ATTACK, m_pFSM);
		return;
	}

	//추적 범위 벗어나면 가까운 웨이포인트로 복귀하고 IDLE 전환 (복귀하도록 수정해야함)
	if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
	}
	else
	{
		//추적 (장애물 피해 가도록 수정해야함)
		m_pOwner->Move_To(m_pTarget->Get_FinalPosition());
		//m_pOwner->Rotate_To_Radians(vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());
	}
}

void CSneak_ChaseState::State_Exit()
{
}

CSneak_ChaseState* CSneak_ChaseState::Create(void* _pArg)
{
	CSneak_ChaseState* pInstance = new CSneak_ChaseState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_ChaseState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_ChaseState::Free()
{
	__super::Free();
}
