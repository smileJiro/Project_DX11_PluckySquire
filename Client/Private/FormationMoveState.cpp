#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "FormationMoveState.h"
#include "Monster.h"
#include "FSM.h"

CFormationMoveState::CFormationMoveState()
{
}

HRESULT CFormationMoveState::Initialize(void* _pArg)
{
	STATEDESC* pDesc = static_cast<STATEDESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fAlert2DRange = pDesc->fAlert2DRange;

	if (0 <= pDesc->fDelayTime)
	{
		m_fDelayTime = pDesc->fDelayTime;
	}
	else
	{
		m_fDelayTime = 3.f;
	}

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}


void CFormationMoveState::State_Enter()
{
	m_fAccTime = 0.f;
}

void CFormationMoveState::State_Update(_float _fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (nullptr != m_pTarget)
	{
		if(m_pTarget->Get_CurCoord() == m_pOwner->Get_CurCoord())
		{
			//몬스터 인식 범위 안에 들어오면 인식상태로 전환

			if (COORDINATE_3D == m_pOwner->Get_CurCoord())
			{
				if (true == Check_Target3D(true))
				{
					m_pOwner->Stop_Rotate();
					m_pOwner->Stop_Move();

					_vector vDir = XMVectorSetY(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition(), 0.f);
					_float fDis = XMVectorGetX(XMVector3Length((vDir)));
					//공격 범위 안일 경우 바로 공격으로 전환
					if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
						Event_ChangeMonsterState(MONSTER_STATE::ATTACK, m_pFSM);
					return;
				}

				//플레이어가 인식되지 않았을 경우 소리가 나면 경계로 전환 
				if (m_pOwner->IsTarget_In_Sneak_Detection())
				{
					m_pOwner->Stop_Rotate();
					m_pOwner->Stop_Move();
					m_pFSM->Set_Sneak_AwarePos(m_pOwner->Get_FinalPosition());
					Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
					return;
				}
			}
		}
	}

	//포메이션으로부터 목표 위치 받아서 이동
	if (false == m_isTurn)
	{
		//m_pOwner->
		m_isTurn = true;
	}

}

void CFormationMoveState::State_Exit()
{
	m_isTurn = false;
	m_isMove = false;
}

CFormationMoveState* CFormationMoveState::Create(void* _pArg)
{
	CFormationMoveState* pInstance = new CFormationMoveState();

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CFormationMoveState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFormationMoveState::Free()
{
	__super::Free();
}
