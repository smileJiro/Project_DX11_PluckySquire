#include "stdafx.h"
#include "ChaseWalkState.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CChaseWalkState::CChaseWalkState()
{
}

HRESULT CChaseWalkState::Initialize_Prototype()
{
	//플레이어 위치 가져오기
	/*m_pTargetTransform = m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->Get_Transform();
	Safe_AddRef(m_pTargetTransform);*/

	return S_OK;
}


void CChaseWalkState::State_Enter()
{
}

void CChaseWalkState::State_Update(_float _fTimeDelta)
{
	//추적 범위 벗어나면 IDLE 전환
	//_float dis = XMVectorGetX(XMVector3Length((m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION))));
	//if (dis >= 3.f)
	//	Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	//else
	//	추적 행동
}

void CChaseWalkState::State_Exit()
{
}

CChaseWalkState* CChaseWalkState::Create()
{
	CChaseWalkState* pInstance = new CChaseWalkState();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CChaseWalkState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

//CChaseWalkState* CChaseWalkState::Clone(void* _pArg)
//{
//	CChaseWalkState* pInstance = new CChaseWalkState(*this);
//
//	if (FAILED(pInstance->Initialize(_pArg)))
//	{
//		MSG_BOX("Failed to Cloned : CChaseWalkState");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CChaseWalkState::Free()
{
	Safe_Release(m_pTargetTransform);

	__super::Free();
}
