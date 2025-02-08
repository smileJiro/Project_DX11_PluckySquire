#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Sneak_IdleState.h"
#include "Monster.h"
#include "FSM.h"

CSneak_IdleState::CSneak_IdleState()
{
}

HRESULT CSneak_IdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 3.f;

	return S_OK;
}


void CSneak_IdleState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CSneak_IdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	m_fAccTime += _fTimeDelta;

	if (nullptr != m_pTarget)
	{
		//몬스터 인식 범위 안에 들어오면 인식상태로 전환
		if (m_pOwner->IsTarget_In_Detection())
		{
			//------테스트
			_vector vTargetDir = m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition();
			_float3 vPos; XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
			_float3 vDir; XMStoreFloat3(&vDir, XMVector3Normalize(vTargetDir));
			_float3 vOutPos;
			CActorObject* pActor = nullptr;
			if (m_pGameInstance->RayCast_Nearest(vPos, vDir, Get_CurCoordRange(MONSTER_STATE::ALERT), &vOutPos, &pActor))
			{
				//
				if (OBJECT_GROUP::RAY_OBJECT ^ static_cast<ACTOR_USERDATA*>(pActor->Get_ActorCom()->Get_RigidActor()->userData)->iObjectGroup)
				{
					//플레이어가 레이 오브젝트보다 가까우면 인식
					if (2 == m_pGameInstance->Compare_VectorLength(vTargetDir, XMLoadFloat3(&vOutPos) - m_pOwner->Get_FinalPosition()))
					{
						Event_ChangeMonsterState(MONSTER_STATE::SNEAK_ALERT, m_pFSM);
						return;
					}
				}
			}
			//레이 충돌 안했을 때(장애물이 없었을 때)
			else
			{
				Event_ChangeMonsterState(MONSTER_STATE::SNEAK_ALERT, m_pFSM);
				return;
			}
			//---------
		}

		//플레이어가 인식되지 않았을 경우 소리가 나면 경계로 전환 
		if (m_pOwner->IsTarget_In_Sneak_Detection())
		{
			Set_Sneak_InvestigatePos(m_pTarget->Get_FinalPosition());
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
			return;
		}
	}
	
	if (m_fDelayTime <= m_fAccTime)
	{
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_PATROL, m_pFSM);
	}

}

void CSneak_IdleState::State_Exit()
{
	m_fAccTime = 0.f;
}

CSneak_IdleState* CSneak_IdleState::Create(void* _pArg)
{
	CSneak_IdleState* pInstance = new CSneak_IdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CSneak_IdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_IdleState::Free()
{
	__super::Free();
}
