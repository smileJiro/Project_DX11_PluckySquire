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
	SNEAKSTATEDESC* pDesc = static_cast<SNEAKSTATEDESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	m_fDelayTime = 1.f;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	Initialize_WayPoints(pDesc->eWayIndex);
		
	return S_OK;
}


void CSneak_ChaseState::State_Enter()
{
	m_isTurn = false;
	m_isMove = false;
	m_isRenew = true;
	m_fAccTime = 0.f;
}

void CSneak_ChaseState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if(false == m_isRenew)
	{
		m_fAccTime += _fTimeDelta;
		if (m_fDelayTime <= m_fAccTime)
		{
			m_fAccTime = 0.f;
			m_isRenew = true;
		}
	}


	//플레이어 한테 직선으로 와야하는데 장애물이 있는 경우 장애물을 돌아서 혹은 길을 따라 이동해야함

	_vector vDir = XMVectorSetY(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition(), 0.f);
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));
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
		if(true == m_isRenew)
		{
			Determine_NextDirection(m_pTarget->Get_FinalPosition(), &m_vDir);
			m_isRenew = false;
			m_isTurn = true;
			m_isMove = false;
		}

		//추적
		if (true == m_isTurn && false == m_isMove)
		{
			if (m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
			{
				m_isMove = true;
			}
		}
		//매프레임 회전하는거 수정해야함
		if(true == m_isMove)
		{
			static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_LinearVelocity(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
			m_isTurn = false;
		}
	}
}

void CSneak_ChaseState::State_Exit()
{
	m_isTurn = false;
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
