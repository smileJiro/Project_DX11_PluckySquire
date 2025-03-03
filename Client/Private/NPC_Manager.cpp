#include "stdafx.h"
#include "NPC_Manager.h"

IMPLEMENT_SINGLETON(CNPC_Manager)

CNPC_Manager::CNPC_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CNPC_Manager::Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID)
{

	if (nullptr != m_pNPC)
	{
		Safe_Release(m_pNPC);
		m_pNPC = nullptr;
	}

	if (nullptr != m_pOnlyNpc)
	{
		Safe_Release(m_pOnlyNpc);
		m_pOnlyNpc = nullptr;
	}


	return S_OK;
}

HRESULT CNPC_Manager::Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{

	return S_OK;
}

HRESULT CNPC_Manager::Level_Enter(_int _iChangeLevelID)
{
	if (nullptr == m_pOnlyNpc)
		return S_OK;

	m_pOnlyNpc->NextLevelLoadJson(_iChangeLevelID);

	return S_OK;
}


void CNPC_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pOnlyNpc);
	Safe_Release(m_pNPC);
	Safe_Release(m_pGameInstance);
	__super::Free();
}

