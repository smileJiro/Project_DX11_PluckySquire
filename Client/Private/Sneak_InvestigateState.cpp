#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_InvestigateState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_InvestigateState::CSneak_InvestigateState()
{
}

HRESULT CSneak_InvestigateState::Initialize(void* _pArg)
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


void CSneak_InvestigateState::State_Enter()
{
}

void CSneak_InvestigateState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;
	if (nullptr == m_pOwner)
		return;

	_vector vDir = m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition();
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));	//3D상에서 y값도 더해서 거리 계산하는거 주의
	XMVectorSetY(vDir, XMVectorGetY(m_pOwner->Get_FinalPosition()));
	if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	{
		//공격 준비로 전환
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_ATTACK, m_pFSM);
		return;
	}

	//이동하다 소리가 나면 범위 내에서 가장 먼 위치로 다음 위치를 갱신
	if (m_pOwner->IsTarget_In_Sneak_Detection())
	{
		Set_Sneak_InvestigatePos(m_pTarget->Get_FinalPosition());
	}

	//위치에 도착했는데 안 보이면 경계상태로 이동
	if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
	}
	else
	{
		//소리 난 위치로 장애물 피해서 추적
		m_pOwner->Move_To(m_pTarget->Get_FinalPosition());
		m_pOwner->Rotate_To_Radians(vDir, m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());
	}
}

void CSneak_InvestigateState::State_Exit()
{
}

CSneak_InvestigateState* CSneak_InvestigateState::Create(void* _pArg)
{
	CSneak_InvestigateState* pInstance = new CSneak_InvestigateState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_InvestigateState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_InvestigateState::Free()
{
	__super::Free();
}
