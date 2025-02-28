#include "stdafx.h"
#include "Dialog_Manager.h"

IMPLEMENT_SINGLETON(CDialog_Manager)

CDialog_Manager::CDialog_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}



void CDialog_Manager::Set_DialogId(const _tchar* _id, const _tchar* _strCurSection, _bool _DisplayDialogue, _bool _DisPlayPortrait)
{

	if (nullptr == m_pDialogue)
		return;

	if (false == m_pDialogue->CBase::Is_Active())
	{
		m_pDialogue->CBase::Set_Active(true);
	}

	m_pDialogue->Set_DialogueId(_id);


	if (nullptr == _strCurSection)
	{
		m_pDialogue->Set_strCurSection(TEXT("NOTWORD"));
	}
	else
	{
		m_pDialogue->Set_strCurSection(_strCurSection);
	}

	m_pDialogue->Set_DisPlayDialogue(_DisplayDialogue);
	m_pDialogue->Set_DisplayPortraitRender(_DisPlayPortrait);
}

void CDialog_Manager::Set_DialogEnd()
{
	m_pDialogue->Set_strCurSection(L"");
	m_isDisPlayDialogue = false;
	m_pDialogue->Set_DisPlayDialogue(false);
	m_pDialogue->Set_DisplayPortraitRender(false);

	if (nullptr != m_pNPC)
	{
		Safe_Release(m_pNPC);
		m_pNPC = nullptr;
	}



}

HRESULT CDialog_Manager::Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID)
{
	if (nullptr != m_pDialogue)
	{
		Safe_Release(m_pDialogue);
		m_pDialogue = nullptr;
	}

	if (nullptr != m_pNPC)
	{
		Safe_Release(m_pNPC);
		m_pNPC = nullptr;
	}


	return S_OK;
}

HRESULT CDialog_Manager::Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{

	return S_OK;
}

HRESULT CDialog_Manager::Level_Enter(_int _iChangeLevelID)
{
	if (nullptr == m_pDialogue)
		return S_OK;

	m_pDialogue->NextLevelLoadJson(_iChangeLevelID);

	return S_OK;
}


void CDialog_Manager::Free()
{
	Safe_Release(m_pDialogue);
	Safe_Release(m_pNPC);

	__super::Free();
}

