#include "stdafx.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ChaseWalkState.h"
#include "Monster.h"
#include "FSM.h"

CChaseWalkState::CChaseWalkState()
{
}

HRESULT CChaseWalkState::Initialize()
{
	//플레이어 위치 가져오기
	m_pTargetTransform = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0)->Get_ControllerTransform();
	Safe_AddRef(m_pTargetTransform);
		
	return S_OK;
}


void CChaseWalkState::State_Enter()
{
}

void CChaseWalkState::State_Update(_float _fTimeDelta)
{
	//추적 범위 벗어나면 IDLE 전환
	_float dis = XMVectorGetX(XMVector3Length((m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION))));
	if (dis >= 3.f)
		Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
	else
	{
		m_pOwner->Get_ControllerTransform()->LookAt_3D(m_pTargetTransform->Get_State(CTransform::STATE_POSITION));
		m_pOwner->Get_ControllerTransform()->Go_Straight(_fTimeDelta);
	}
}

void CChaseWalkState::State_Exit()
{
}

CChaseWalkState* CChaseWalkState::Create()
{
	CChaseWalkState* pInstance = new CChaseWalkState();

	if (FAILED(pInstance->Initialize()))
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
