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

	m_fDelayTime = 0.2f;

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
	m_isOnWay = false;
	m_fAccTime = 0.f;
	cout << "Chase" << endl;
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
	//cout << "Chase" << endl;

	//플레이어 한테 직선으로 와야하는데 장애물이 있는 경우 장애물을 돌아서 혹은 길을 따라 이동해야함

	_vector vDir = XMVectorSetY(m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition(), 0.f);
	_float fDis = XMVectorGetX(XMVector3Length((vDir)));
	if (fDis <= Get_CurCoordRange(MONSTER_STATE::ATTACK))
	{
		m_pOwner->Stop_Rotate();
		m_pOwner->Stop_Move();
		Event_ChangeMonsterState(MONSTER_STATE::SNEAK_ATTACK, m_pFSM);
		return;
	}

	//추적 범위 벗어나면 가까운 정찰 웨이포인트로 복귀
	if (fDis > Get_CurCoordRange(MONSTER_STATE::CHASE))
	{
		m_pOwner->Stop_Rotate();
		m_pOwner->Stop_Move();

		if(false == m_pOwner->Is_FormationMode())
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_BACK, m_pFSM);
		else
			Event_ChangeMonsterState(MONSTER_STATE::FORMATION_BACK, m_pFSM);
	}
	else
	{
		if(true == m_isRenew)
		{
			//방향 y값이 0이 되는게 맞나?
			Determine_NextDirection(m_pTarget->Get_FinalPosition(), &m_vDir);
			m_isRenew = false;
			m_isTurn = true;
			m_isMove = false;
		}
		else if (false == m_isOnWay)
			Determine_NextDirection(m_pTarget->Get_FinalPosition(), &m_vDir);

		//예외처리
		if (XMVector3Equal(XMLoadFloat3(&m_vDir), XMVectorZero()))
		{
			m_pOwner->Stop_Rotate();
			m_pOwner->Stop_Move();
			Event_ChangeMonsterState(MONSTER_STATE::SNEAK_AWARE, m_pFSM);
			return;
		}

		//회전
		if (true == m_isTurn && false == m_isMove)
		{
			if (m_pOwner->Rotate_To_Radians(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_RotationPerSec()))
			{
				m_isMove = true;
				//m_pOwner->Change_Animation();
			}
			//else
			//{
			//	_bool isCW = true;
			//	_float fResult = XMVectorGetY(XMVector3Cross(m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vDir)));
			//	if (fResult < 0)
			//		isCW = false;

			//	m_pOwner->Turn_Animation(isCW);
			//}
		}

		//이동
		if (true == m_isMove)
		{
			//static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_LinearVelocity(XMLoadFloat3(&m_vDir), m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec());
			m_pOwner->Move(XMLoadFloat3(&m_vDir) * m_pOwner->Get_ControllerTransform()->Get_SpeedPerSec(), _fTimeDelta);

			//이미 경로가 있을때
			if (true == m_isOnWay && false == m_isPathFind)
			{
				//도착하면 다음 웨이포인트로 목표위치 바꿈
				if (m_pOwner->Check_Arrival(XMLoadFloat3(&m_WayPoints[m_Ways[m_iCurWayIndex]].vPosition), 0.3f))
				{
					m_pOwner->Stop_Rotate();
					m_pOwner->Stop_Move();
					++m_iCurWayIndex;
					m_isTurn = true;
					m_isMove = false;
					//목표 위치에 도착했으면 자유이동으로 전환
					if (m_Ways.size() <= m_iCurWayIndex)
					{
						m_isOnWay = false;
					}
				}
			}
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
