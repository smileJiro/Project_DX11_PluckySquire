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

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_fDelayTime = 3.f;

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

	m_fAccTime += _fTimeDelta;

	if (nullptr != m_pTarget)
	{
		//몬스터 인식 범위 안에 들어오면 인식상태로 전환
		_float fDis = m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition());
		if (COORDINATE_2D == m_pOwner->Get_CurCoord())
		{
			if (fDis <= m_fAlert2DRange)
			{
				Event_ChangeMonsterState(MONSTER_STATE::ALERT, m_pFSM);
				return;
			}
		}

		else if (COORDINATE_3D == m_pOwner->Get_CurCoord())
		{
			if (m_pOwner->IsTarget_In_Detection())
			{
				//------테스트
				_vector vTargetDir = m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition();
				_float3 vPos; XMStoreFloat3(&vPos, m_pOwner->Get_FinalPosition());
				_float3 vDir; XMStoreFloat3(&vDir, XMVector3Normalize(vTargetDir));
				_float3 vOutPos;
				CActorObject* pActor = nullptr;
				if (m_pGameInstance->RayCast_Nearest(vPos, vDir, Get_CurCoordRange(MONSTER_STATE::ALERT), &vOutPos, &pActor))
				{//ACTOR_TYPE::STATIC != pActor->Get_ActorType() && 
					if (!(OBJECT_GROUP::RAY_OBJECT & static_cast<ACTOR_USERDATA*>(pActor->Get_ActorCom()->Get_RigidActor()->userData)->iObjectGroup))
					{
						//플레이어가 레이 오브젝트보다 가까우면 인식
						if (2 == m_pGameInstance->Compare_VectorLength(vTargetDir, XMLoadFloat3(&vOutPos) - m_pOwner->Get_FinalPosition()))
						{
							Event_ChangeMonsterState(MONSTER_STATE::ALERT, m_pFSM);
							return;
						}
					}
				}
				//---------
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
