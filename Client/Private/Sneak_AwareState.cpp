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
	m_fChaseRange = pDesc->fChaseRange;
	m_fChase2DRange = pDesc->fChase2DRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fAttack2DRange = pDesc->fAttack2DRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
		
	return S_OK;
}


void CSneak_AwareState::State_Enter()
{
	m_pOwner->Set_AnimChangeable(false);
}

void CSneak_AwareState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;
	
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
				if (!(OBJECT_GROUP::RAY_OBJECT & static_cast<ACTOR_USERDATA*>(pActor->Get_ActorCom()->Get_RigidActor()->userData)->iObjectGroup))
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

		//플레이어가 인식되지 않았을 경우 소리가 나면 경계추적으로 전환 
		if (m_pOwner->IsTarget_In_Sneak_Detection())
		{
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_INVESTIGATE, m_pFSM);
			return;
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
