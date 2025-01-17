#include "stdafx.h"
#include "UI_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	__super::Free();
}

