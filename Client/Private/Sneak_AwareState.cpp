#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_AwareState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_AwareState::CSneak_AwareState()
{
}

HRESULT CSneak_AwareState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CSneak_AwareState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CSneak_AwareState::State_Update(_float _fTimeDelta)
{	if (nullptr == m_pOwner)
		return;
	
	if (nullptr != m_pTarget)
	{
		//소리난 위치로 회전
		_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vSneakPos) - m_pOwner->Get_FinalPosition());
		m_pOwner->Rotate_To_Radians(XMVectorSetY(vDir, 0.f), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec());

		//몬스터 인식 범위 안에 들어오면 인식상태로 전환
		if (true == Check_Target3D(true))
			return;

		//플레이어가 인식되지 않았는데 소리가 나면 위치 저장 후 경계추적으로 전환 
		if (m_pOwner->IsTarget_In_Sneak_Detection())
		{
			Set_Sneak_InvestigatePos(m_pTarget->Get_FinalPosition());
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_INVESTIGATE, m_pFSM);
			return;
		}

		//인식 되지 않고 소리도 안나면 idle 전환 (지금 애니메이션 재생동안은 전환 안되니까)
		else
		{
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_IDLE, m_pFSM);
		}
	}
}

void CSneak_AwareState::State_Exit()
{
}

CSneak_AwareState* CSneak_AwareState::Create(void* _pArg)
{
	CSneak_AwareState* pInstance = new CSneak_AwareState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_AwareState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_AwareState::Free()
{
	__super::Free();
}
