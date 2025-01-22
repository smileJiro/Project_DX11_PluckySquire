#pragma once
#include "Player.h"
#include "SettingPanelBG.h"
#include "ShopPanel_BG.h"

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

	enum STAMP { STAMP_STOP, STAMP_BOMB, STAMP_END};

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

private:
	CGameInstance*					m_pGameInstance;
	STAMP							m_eStampIndex = { STAMP_STOP };
	CPlayer*						m_pPlayer = { nullptr };

	map<_uint, CSettingPanelBG*>	m_pSettingPanels;
	map<_uint, CShopPanel_BG*>		m_pShopPanels;

public:
	STAMP							Get_StampIndex() { return m_eStampIndex; }
	void							Set_StampIndex(STAMP _Stamp) { m_eStampIndex = _Stamp; }

	CPlayer*						Get_Player() { return m_pPlayer; }
	void							Set_Player(CPlayer* _Player) { m_pPlayer = _Player; Safe_AddRef(_Player); }

	void							Emplace_SettingPanels(_uint _ePanel, CSettingPanelBG* _pPanel);
	void							Emplace_ShopPanels(_uint _ePanel, CShopPanel_BG* _pPanel);

	map<_uint, CSettingPanelBG*>	Get_SettingPanels() { return m_pSettingPanels; }
	map<_uint, CShopPanel_BG*>		Get_ShopPanels()	{ return m_pShopPanels; }

public:
	virtual void Free() override;
};

END