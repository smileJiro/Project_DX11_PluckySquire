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


	if(true == m_pOwner->IsContactToTarget())
		Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);

	_vector vDir = m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition();
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));
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
			m_pOwner->Monster_MoveTo(m_pTarget->Get_FinalPosition(), _fTimeDelta);
			if (true == m_pOwner->Check_InAir_Next(_fTimeDelta))
				m_pOwner->Stop_Move();
			//m_pOwner->Move_To(m_pTarget->Get_FinalPosition());
			//m_pOwner->Move(static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Get_LinearVelocity(), _fTimeDelta);
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
