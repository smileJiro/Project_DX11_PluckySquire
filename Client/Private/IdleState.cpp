#include "stdafx.h"
#include "IdleState.h"
#include "GameObject.h"
#include "Monster.h"
#include "FSM.h"

CIdleState::CIdleState()
{
}

HRESULT CIdleState::Initialize_Prototype()
{
	//플레이어 위치 가져오기
	/*m_pTargetTransform = m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->Get_Transform();
	Safe_AddRef(m_pTargetTransform);*/

	return S_OK;
}


void CIdleState::State_Enter()
{
}

void CIdleState::State_Update(_float _fTimeDelta)
{
	//몬스터 범위 안에 들어오면 추적으로 전환
	//_float dis = XMVectorGetX(XMVector3Length((m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION))));
	//if (dis < 3.f)
	//{
	//	Event_ChangeMonsterState(MONSTER_STATE::CHASE, m_pFSM);
	//}
}

void CIdleState::State_Exit()
{
}

CIdleState* CIdleState::Create()
{
	CIdleState* pInstance = new CIdleState();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

//CIdleState* CIdleState::Clone(void* _pArg)
//{
//	CIdleState* pInstance = new CIdleState(*this);
//
//	if (FAILED(pInstance->Initialize(_pArg)))
//	{
//		MSG_BOX("Failed to Cloned : CIdleState");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CIdleState::Free()
{
	Safe_Release(m_pTargetTransform);

	__super::Free();
}
