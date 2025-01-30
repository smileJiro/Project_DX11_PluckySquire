#pragma once
#include "Player.h"
#include "SettingPanelBG.h"
#include "ShopPanel_BG.h"
#include "ShopItemBG.h"
#include "Dialogue.h"

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

	enum STAMP { STAMP_STOP, STAMP_BOMB, STAMP_END };

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

private:
	CGameInstance* m_pGameInstance;
	STAMP								m_eStampIndex = { STAMP_STOP };
	CPlayer* m_pPlayer = { nullptr };

	map<_uint, CSettingPanelBG*>		m_pSettingPanels;
	map<_uint, CShopPanel_BG*>			m_pShopPanels;


	vector<CShopItemBG*>				m_pShopItemBGs;
	vector<vector<CShopItemBG*>>		m_ShopItems;



	_bool								m_isMakeItem = { false };
	_bool								m_isMakeShop = { false };
	_bool								m_isESC = { false };
	_bool								m_isConfirmStore = { false };
	_bool								m_isLogoChooseStage = { false };

	_int								m_iPreIndex = { 0 };
	_int								m_iSettingPanelIndex = { 0 };
	_int								m_iLogoIndex = { 1 };
	_bool								m_isStoreYesORNo = { true };

	_int								m_iCurrentLevel = { -1 };

	vector<CDialog::DialogData>			m_DialogDatas;
	_tchar								m_tDialogId[MAX_PATH] = {};
	_int								m_iCurrnetLineIndex = { 0 };
	_bool								m_isPortraitRender = { true };





public:
	STAMP								Get_StampIndex() { return m_eStampIndex; }
	void								Set_StampIndex(STAMP _Stamp) { m_eStampIndex = _Stamp; }
	CPlayer* Get_Player() { return m_pPlayer; }
	void								Set_Player(CPlayer* _Player) { m_pPlayer = _Player; Safe_AddRef(_Player); }
	void								Emplace_SettingPanels(_uint _ePanel, CSettingPanelBG* _pPanel);
	void								Emplace_ShopPanels(_uint _ePanel, CShopPanel_BG* _pPanel);
	void								Delete_ShopItems(_uint _index);
	map<_uint, CSettingPanelBG*>		Get_SettingPanels() { return m_pSettingPanels; }
	map<_uint, CShopPanel_BG*>			Get_ShopPanels() { return m_pShopPanels; }
	vector<CShopItemBG*>				Get_ShopItemBGs() { return m_pShopItemBGs; }
	vector<vector<CShopItemBG*>>		Get_ShopItems() { return m_ShopItems; }
	_bool								Get_isESC() { return m_isESC; }
	void								Set_isEsc(_bool _isEsc) { m_isESC = _isEsc; }
	_bool								Get_ConfirmStore() { return m_isConfirmStore; }
	void								Set_ConfirmStore(_bool _Confirm) { m_isConfirmStore = _Confirm; }
	_bool								Get_StoreYesOrno() { return m_isStoreYesORNo; }
	void								Set_StoreYesOrno(_bool _yesOrno) { m_isStoreYesORNo = _yesOrno; }
	_int								Get_SettingPanelIndex() { return m_iSettingPanelIndex; }
	void								Set_SettingPanelIndex(_int _index) { m_iSettingPanelIndex = _index; }
	_int								Get_LogoIndex() { return m_iLogoIndex; }
	void								Set_LogoIndex(_int _index) { m_iLogoIndex = _index; }
	void								pushBack_ShopItem(vector<CShopItemBG*> _ItemBGs) { m_ShopItems.push_back(_ItemBGs); }
	void								pushBack_ShopItemBGs(CShopItemBG* _pBGs) { m_pShopItemBGs.push_back(_pBGs); }
	void								Set_ChooseItem(_int _iIndex);
	_bool								Get_LogoChanseStage() { return m_isLogoChooseStage; }
	void								Set_LogoChangeState(_bool _LogoChooseStage) { m_isLogoChooseStage = _LogoChooseStage; }

	vector<CDialog::DialogData>			Get_Dialogue(const _wstring& _id);
	CDialog::DialogLine					Get_DialogueLine(const _wstring& _id, _int _LineIndex);

	_tchar* Get_DialogId() { return m_tDialogId; }
	void								Set_DialogId(const _tchar* _id) { wsprintf(m_tDialogId, _id); }

	_int								Get_DialogueLineIndex() { return m_iCurrnetLineIndex; }
	void								Set_DialogueLineIndex(_int _index) { m_iCurrnetLineIndex = _index; }

	_bool								Get_PortraitRender() { return m_isPortraitRender; }
	void								Set_PortraitRender(_bool _Render) { m_isPortraitRender = _Render; }

	void								Pushback_Dialogue(CDialog::DialogData _DialogData);

	_bool								Get_isMakeItem() { return m_isMakeItem; }
	void								Set_isMakeItem(_bool _make) { m_isMakeItem = _make; }

	HRESULT								Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);


public:
	virtual void Free() override;
};

END