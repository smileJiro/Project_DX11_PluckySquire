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

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto iter : m_pSettingPanels)
	{
		Safe_Release(iter.second);
	}

	m_pSettingPanels.clear();

	__super::Free();
}

