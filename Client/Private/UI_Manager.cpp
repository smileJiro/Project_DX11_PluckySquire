#include "stdafx.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUI_Manager::Emplace_SettingPanels(_uint _ePanel, CSettingPanelBG* _pPanel)
{
	m_pSettingPanels.emplace(_ePanel, _pPanel);
	Safe_AddRef(_pPanel);
}

void CUI_Manager::Emplace_ShopPanels(_uint _ePanel, CShopPanel_BG* _pPanel)
{
	m_pShopPanels.emplace(_ePanel, _pPanel);
	Safe_AddRef(_pPanel);
}

void CUI_Manager::Delete_ShopItems(_uint _index)
{

	//_float fY = m_ShopItems[_index][0]->Get_FY();
	//_float NewfY = { 0.f };
	//
	////_float BGfX = g_iWinSizeX / 2.f; // 배경
	////_float BDfX = g_iWinSizeX / 3.5f; // 스킬아이콘
	////_float BUfX = g_iWinSizeX - g_iWinSizeX / 3.f; // 전구
	//
	//_float BGfX = g_iWinSizeX / 2.f / 2.f;
	//_float BDfX = g_iWinSizeX / 3.5f / 1.6f;
	//_float BUfX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.2f;
	//
	//
	//BGfX = BGfX - g_iWinSizeX * 0.5f;
	//BDfX = BDfX - g_iWinSizeX * 0.5f;
	//BUfX = BUfX - g_iWinSizeX * 0.5f;
	//
	//_float fIdx0 = g_iWinSizeY / 4.f;
	//_float fIdx1 = g_iWinSizeY / 2.75f;
	//_float fIdx2 = g_iWinSizeY - g_iWinSizeY / 1.9f;
	//
	//fIdx0 = -fIdx0 + g_iWinSizeY * 0.5f;
	//fIdx1 = -fIdx1 + g_iWinSizeY * 0.5f;
	//fIdx2 = -fIdx2 + g_iWinSizeY * 0.5f;


	for (int i = 0; i < m_ShopItems[_index].size(); ++i)
	{
		Event_DeleteObject(m_ShopItems[_index][i]);
		CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(m_ShopItems[_index][i]);
	}

	m_ShopItems.erase(m_ShopItems.begin() + _index);
	m_isUpdateShopPanel = true;

	///_float fHeight = 135.f; // 한 줄의 높이 (고정)
	//float fHeight = 103.f; // 한 줄의 높이 (고정)
	//float baseY = g_iWinSizeY / 4.f;    // 첫 번째 줄의 기준 Y 좌표 (예: 화면 중앙 기준)


	//for (int i = 0; i < m_ShopItems.size(); ++i)
	//{
	//	for (int j = 0; j < m_ShopItems[i].size(); ++j)
	//	{
	//		_float newY = baseY - (i * fHeight); 
	//
	//		if (0 == j)
	//		{
	//			m_ShopItems[i][j]->Get_Transform()->Set_State(CTransform::STATE_POSITION,
	//				XMVectorSet(BGfX, newY, 0.f, 1.f));
	//		}
	//		else if (1 == j)
	//		{
	//			m_ShopItems[i][j]->Get_Transform()->Set_State(CTransform::STATE_POSITION,
	//				XMVectorSet(BDfX, newY, 0.f, 1.f));
	//		}
	//		else if (2 == j)
	//		{
	//			m_ShopItems[i][j]->Get_Transform()->Set_State(CTransform::STATE_POSITION,
	//				XMVectorSet(BUfX, newY, 0.f, 1.f));
	//		}
	//	}
	//}
}

void CUI_Manager::pushBack_ShopItem(vector<CShopItemBG*> _ItemBGs)
{
	m_ShopItems.push_back(_ItemBGs);

	for (auto& pShopItemBG : _ItemBGs)
	{
		Safe_AddRef(pShopItemBG);
	}
}
void CUI_Manager::Set_ChooseItem(_int _iIndex)
{
	if (-1 == _iIndex)
	{
		return;
	}
	
	if (m_iPreIndex == _iIndex && 1 != m_ShopItems.size())
	{
		return;
	}
	else if (m_iPreIndex != _iIndex || 1 == m_ShopItems.size())
	{
		for (_int i = 0; i < m_ShopItems.size(); ++i)
		{
			for (_int j = 0; j < m_ShopItems.size(); ++j)
			{
				m_ShopItems[i][j]->Set_isChooseItem(false);
			}
		}
	
		for (_int i = 0; i <= _iIndex; ++i)
		{
			m_ShopItems[_iIndex][i]->Set_isChooseItem(true);
		}
		m_iPreIndex = _iIndex;
	}
}

vector<CDialog::DialogData> CUI_Manager::Get_Dialogue(const _wstring& _id)
{
	auto iter = find_if(m_DialogDatas.begin(), m_DialogDatas.end(), 
		[&_id](const CDialog::DialogData& dialog)
		{ 
			return dialog.id == _id; 
		});


	if (iter != m_DialogDatas.end())
	{
		return { *iter };
	}
	else
	{
		return {};
	}
}

CDialog::DialogLine CUI_Manager::Get_DialogueLine(const _wstring& _id, _int _LineIndex)
{
	auto iter = find_if(m_DialogDatas.begin(), m_DialogDatas.end(), [&](const CDialog::DialogData& Data)
		{
			return Data.id == _id;
		});

	if (iter != m_DialogDatas.end())
	{
		if (_LineIndex < iter->lines.size())
		{
			return iter->lines[_LineIndex];
		}
	}

	return CDialog::DialogLine{};
}


void CUI_Manager::Set_DialogId(const _tchar* _id, const _tchar* strCurSection,  _bool _DisplayDialogue, _bool _DisPlayPortrait)
{
	if (false == m_pDiagloue->CBase::Is_Active())
	{
		m_pDiagloue->CBase::Set_Active(true);
	}

	wsprintf(m_tDialogId, _id); 
	if (nullptr == strCurSection)
	{
		wsprintf(m_strCurrentSection, TEXT("NOTWORD"));
	}
	else
	{
		wsprintf(m_strCurrentSection, strCurSection);
	}

	m_isDisplayDialogue = _DisplayDialogue; 
	m_isPortraitRender = _DisPlayPortrait;
}

void CUI_Manager::Pushback_Dialogue(CDialog::DialogData _DialogData)
{
	m_DialogDatas.push_back(_DialogData);
}

void CUI_Manager::Test_Update(_float _fTimedelta)
{
	if (KEY_DOWN(KEY::J) && 0 <= m_iTextIndex)
	{
		switch (m_iTextIndex)
		{
		case 0:
		{
			Uimgr->Set_PlayNarration(TEXT("Chapter1_P0102_Narration_01"));
			++m_iTextIndex;
		}
		break;

		case 1:
		{
			Uimgr->Set_PlayNarration(TEXT("Chapter1_P0506_Narration_01"));
		}
		break;

		}

		
	}


			




}

HRESULT CUI_Manager::Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID)
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pDiagloue);

	if(iCurLevelID == LEVEL_LOGO)
		Level_Logo_Exit(_iChangeLevelID, _iNextChangeLevelID);

	if (_iChangeLevelID == LEVEL_LOADING)
	{
		m_iCurrentLevel = _iNextChangeLevelID;
	}

	for (_int i = 0; i < m_ShopItems.size(); ++i)
	{
		for (_int j = 0; j < m_ShopItems[i].size(); ++j)
		{
			Safe_Release(m_ShopItems[i][j]);
		}
	}
	m_ShopItems.clear();

	for (auto iter : m_pSettingPanels)
	{
		Safe_Release(iter.second);
	}
	m_pSettingPanels.clear();

	for (auto iter : m_pShopPanels)
	{
		Safe_Release(iter.second);
	}
	m_pShopPanels.clear();

	Uimgr->Set_isMakeItem(false);

	Safe_Release(m_pNarration);


	return S_OK;

}

HRESULT CUI_Manager::Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	if (_iChangeLevelID == LEVEL_LOADING)
	{
		m_iCurrentLevel = _iNextChangeLevelID;
	}

	for (_int i = 0; i < m_LogoProps.size(); ++i)
	{
		Safe_Release(m_LogoProps[i]);
	}
	m_LogoProps.clear();

	return S_OK;
}

HRESULT CUI_Manager::Level_Enter(_int _iChangeLevelID)
{
	return S_OK;
}


void CUI_Manager::Free()
{
	
	Safe_Release(m_pPlayer);
	Safe_Release(m_pDiagloue);

	for (auto iter : m_pSettingPanels)
	{
		Safe_Release(iter.second);
	}
	m_pSettingPanels.clear();

	for (auto& i : m_ShopItems)
	{
		for (auto& j : i)
		{
			Safe_Release(j);
		}
	}
	m_ShopItems.clear();

	for (auto iter : m_pShopPanels)
	{
		Safe_Release(iter.second);
	}
	m_pShopPanels.clear();
	
	for (_int i = 0; i < m_LogoProps.size(); ++i)
	{
		Safe_Release(m_LogoProps[i]);
	}
	m_LogoProps.clear();

	vector<CDialog::DialogData>().swap(m_DialogDatas);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pNarration);

	__super::Free();
}

