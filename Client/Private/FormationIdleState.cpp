#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "FormationIdleState.h"
#include "Monster.h"
#include "FSM.h"

CFormationIdleState::CFormationIdleState()
{
}

HRESULT CFormationIdleState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CFormationIdleState::State_Enter()
{
}

void CFormationIdleState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (nullptr != m_pTarget)
	{
		if(m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
		{
			//몬스터 인식 범위 안에 들어오면 대열에서 빠지고 인식상태로 전환

			if (COORDINATE_3D == m_pOwner->Get_CurCoord())
			{
				if (true == Check_Target3D(true))
				{
					m_pOwner->Stop_Rotate();
					m_pOwner->Stop_Move();

					m_pOwner->Remove_From_Formation();

					_vector vDir = XMVectorSetY(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition(), 0.f);
					_float fDis = XMVectorGetX(XMVector3Length((vDir)));
					//공격 범위 안일 경우 바로 공격으로 전환
					if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
						Event_ChangeMonsterState(MONSTER_STATE::SNEAK_ATTACK, m_pFSM);
					return;
				}

				//플레이어가 인식되지 않았을 경우 소리가 나면 대열에서 빠지고 경계로 전환 
				if (m_pOwner->IsTarget_In_Sneak_Detection())
				{
					m_pOwner->Stop_Rotate();
					m_pOwner->Stop_Move();
					m_pOwner->Remove_From_Formation();
					m_pFSM->Set_Sneak_AwarePos(m_pOwner->Get_FinalPosition());
					Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
					return;
				}
			}
		}
	}

	//대열이 다시 움직이기 시작할 경우 move 전환
	if (false == m_pOwner->Is_Formation_Stop())
	{
		Event_ChangeMonsterState(MONSTER_STATE::FORMATION_MOVE, m_pFSM);
	}

}

void CFormationIdleState::State_Exit()
{
}

CFormationIdleState* CFormationIdleState::Create(void* _pArg)
{
	CFormationIdleState* pInstance = new CFormationIdleState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CFormationIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFormationIdleState::Free()
{
	__super::Free();
}
