#include "stdafx.h"
#include "NPC_Manager.h"

IMPLEMENT_SINGLETON(CNPC_Manager)

CNPC_Manager::CNPC_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


CNPC_Social* CNPC_Manager::Find_SocialNPC(wstring _NPCName)
{
	auto iter = find_if(m_pNpcSocials.begin(), m_pNpcSocials.end(),
		[&_NPCName](CNPC_Social* SocialNpc)
		{
			return SocialNpc->Get_NPCName() == _NPCName;
		});

	if (iter != m_pNpcSocials.end())
		return *iter;
	else
		return nullptr;
}

HRESULT CNPC_Manager::ChangeDialogue(_wstring _NPCName, _wstring _DialogueID)
{
	CNPC_Social* pSocialNpc = nullptr;

	pSocialNpc = Find_SocialNPC(_NPCName);

	if (nullptr == pSocialNpc)
		return E_FAIL;

	pSocialNpc->ChangeDialogueID(_DialogueID);
	return S_OK;
}

HRESULT CNPC_Manager::Remove_SocialNPC(_wstring _NPCName)
{

	CNPC_Social* pSocialNpc = nullptr;
	pSocialNpc = Find_SocialNPC(_NPCName);

	if (nullptr == pSocialNpc)
		return E_FAIL;

	Event_DeleteObject(pSocialNpc);


	return S_OK;
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

	if (nullptr != m_pWorldNpc)
	{
		Safe_Release(m_pWorldNpc);
		m_pWorldNpc = nullptr;
	}

	for (int i = 0; i < m_pNpcSocials.size(); ++i)
	{
		Safe_Release(m_pNpcSocials[i]);
	}
	m_pNpcSocials.clear();

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
	Safe_Release(m_pWorldNpc);




	for (int i = 0; i < m_pNpcSocials.size(); ++i)
	{
		Safe_Release(m_pNpcSocials[i]);
	}
	m_pNpcSocials.clear();

	__super::Free();
}

