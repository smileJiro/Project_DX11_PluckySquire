#include "stdafx.h"
#include "GameInstance.h"
#include "State.h"
#include "Monster.h"
#include "FSM.h"

CState::CState()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CState::Initialize(void* _pArg)
{
	//플레이어 위치 가져오기
	CGameObject* pObject = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (nullptr == pObject)
	{
		cout << "STATE : NO PLAYER" << endl;
		return S_OK;
	}
	else
	{
		m_pTargetTransform = pObject->Get_ControllerTransform();
		Safe_AddRef(m_pTargetTransform);
	}

	return S_OK;
}


void CState::Set_Owner(CMonster* _pOwner)
{
	m_pOwner = _pOwner;
}

void CState::Free()
{
	Safe_Release(m_pGameInstance);
	m_pOwner = nullptr;
	__super::Free();
}
