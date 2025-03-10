#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ChaseWalkState.h"
#include "Monster.h"
#include "FSM.h"

CChaseWalkState::CChaseWalkState()
{
}

HRESULT CChaseWalkState::Initialize(void* _pArg)
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


void CChaseWalkState::State_Enter()
{
}

void CChaseWalkState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	if (m_pTarget->Get_CurCoord() != m_pOwner->Get_CurCoord())
	{
		if (COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			m_pOwner->Stop_Move();
			m_pOwner->Stop_Rotate();
		}
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		return;
	}
	else if (COORDINATE_2D == m_pOwner->Get_CurCoord() && m_pOwner->Get_Include_Section_Name() != m_pTarget->Get_Include_Section_Name())
	{
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
		return;
	}

	_vector vDir = m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition();
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));	//3D상에서 y값도 더해서 거리 계산하는거 주의
	XMVectorSetY(vDir, XMVectorGetY(m_pOwner->Get_FinalPosition()));
	if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	{
		if(COORDINATE_3D == m_pOwner->Get_CurCoord())
			m_pOwner->Stop_Rotate();
		//공격 준비로 전환
		Event_ChangeMonsterState(MONSTER_STATE::STANDBY, m_pFSM);
		return;
	}

	//추적 범위 벗어나면 IDLE 전환
	if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		if (COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			m_pOwner->Stop_Move();
			m_pOwner->Stop_Rotate();
		}
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	}
	else
	{
		if(COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			//추적
			//m_pOwner->Get_ControllerTransform()->MoveToTarget(XMVectorSetY(m_pTarget->Get_FinalPosition(), XMVectorGetY(m_pOwner->Get_FinalPosition())), _fTimeDelta);
			//m_pOwner->Get_ControllerTransform()->Set_AutoRotationYDirection(vDir);
			//m_pOwner->Get_ControllerTransform()->Update_AutoRotation(_fTimeDelta * 2.f);
			//m_pOwner->Move_To(m_pTarget->Get_FinalPosition());
			m_pOwner->Monster_MoveTo(m_pTarget->Get_FinalPosition(), _fTimeDelta);
			if (true == m_pOwner->Check_InAir_Next(_fTimeDelta))
				m_pOwner->Stop_Move();

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

void CChaseWalkState::State_Exit()
{
}

CChaseWalkState* CChaseWalkState::Create(void* _pArg)
{
	CChaseWalkState* pInstance = new CChaseWalkState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CChaseWalkState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChaseWalkState::Free()
{
	__super::Free();
}
