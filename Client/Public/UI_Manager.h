#pragma once
#include "Player.h"
#include "SettingPanelBG.h"
#include "ShopPanel_BG.h"
#include "ShopItemBG.h"
#include "Dialogue.h"
#include "Logo_Props.h"
#include "Narration.h"



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
	CDialog* m_pDiagloue = { nullptr };
	

	map<_uint, CSettingPanelBG*>		m_pSettingPanels;
	map<_uint, CShopPanel_BG*>			m_pShopPanels;


	vector<CShopItemBG*>				m_pShopItemBGs;
	vector<vector<CShopItemBG*>>		m_ShopItems;
	vector<CLogo_Props*>				m_LogoProps;

	CNarration*							m_pNarration = { nullptr };
	_tchar								m_strNarrationID[MAX_PATH];
	_bool								m_isPlayerNarration = { false };


	_bool								m_isMakeItem = { false };
	_bool								m_isMakeShop = { false };
	_bool								m_isESC = { false };
	_bool								m_isConfirmStore = { false };
	_bool								m_isUpdateShopPanel = { true };
	_bool								m_isLogoChooseStage = { true };
	_bool								m_isOnHitPlayer = { false };

	_int								m_iPreIndex = { 0 };
	_int								m_iSettingPanelIndex = { 0 };
	_int								m_iLogoIndex = { 0 };
	_bool								m_isStoreYesORNo = { true };

	_bool								m_isQIcon = { false };

	_int								m_iCurrentLevel = { -1 };

	vector<CDialog::DialogData>			m_DialogDatas;
	
	_tchar								m_tDialogId[MAX_PATH] = {};
	_tchar								m_strCurrentSection[MAX_PATH] = {};
	_int								m_iCurrnetLineIndex = { 0 };
	_bool								m_isPortraitRender = { false };
	_bool								m_isDisplayDialogue = { false };
	_bool								m_isDialogueFinishShopPanel = { false };
	_float4								m_vDialoguePos = { 0.f, 0.f, 0.f, 1.f };


	_float3								m_vCalDialoguePos = { 0.f, 0.f, 0.f };
	_float2								m_vShopPos = { 0.f, 0.f };

	/******** 테스트 ********/
	_uint								m_iTextIndex = { 0 };
	_bool								m_isVioletMeet = { false };

public:
	STAMP								Get_StampIndex() { return m_eStampIndex; }
	void								Set_StampIndex(STAMP _Stamp) { m_eStampIndex = _Stamp; }
	CPlayer*							Get_Player() { return m_pPlayer; }
	void								Set_Player(CPlayer* _Player) { m_pPlayer = _Player; Safe_AddRef(_Player); }
	void								Set_Dialogue(CDialog* _Dialogue) { m_pDiagloue = _Dialogue; Safe_AddRef(_Dialogue); }
	CDialog*							Get_pDialogue() { return m_pDiagloue; }
	_bool								Get_PlayerOnHit() { return m_isOnHitPlayer; }
	void								Set_PlayerOnHit(_bool _Hit) { m_isOnHitPlayer = _Hit; }
	void								Emplace_SettingPanels(_uint _ePanel, CSettingPanelBG* _pPanel);
	void								Emplace_ShopPanels(_uint _ePanel, CShopPanel_BG* _pPanel);
	void								Delete_ShopItems(_uint _index);
	map<_uint, CSettingPanelBG*>		Get_SettingPanels() { return m_pSettingPanels; }
	map<_uint, CShopPanel_BG*>			Get_ShopPanels() { return m_pShopPanels; }
	vector<CShopItemBG*>				Get_ShopItemBGs() { return m_pShopItemBGs; }
	vector<vector<CShopItemBG*>>		Get_ShopItems() { return m_ShopItems; }
	_bool								Get_isESC() { return m_isESC; }
	void								Set_isEsc(_bool _isEsc) { m_isESC = _isEsc; }
	_float2								Get_ShopPos() { return m_vShopPos; }
	void								Set_ShopPos(_float2 _vShopPos) { m_vShopPos = _vShopPos; }
	_bool								Get_ConfirmStore() { return m_isConfirmStore; }
	void								Set_ConfirmStore(_bool _Confirm) { m_isConfirmStore = _Confirm; }
	_bool								Get_StoreYesOrno() { return m_isStoreYesORNo; }
	void								Set_StoreYesOrno(_bool _yesOrno) { m_isStoreYesORNo = _yesOrno; }
	_int								Get_SettingPanelIndex() { return m_iSettingPanelIndex; }
	void								Set_SettingPanelIndex(_int _index) { m_iSettingPanelIndex = _index; }
	_int								Get_LogoIndex() { return m_iLogoIndex; }
	void								Set_LogoIndex(_int _index) { m_iLogoIndex = _index; }
	_bool								Get_ShopUpdate() { return m_isUpdateShopPanel; }
	void								Set_ShopUpdate(_bool _isUpdate) { m_isUpdateShopPanel = _isUpdate; }
	void								pushBack_ShopItem(vector<CShopItemBG*> _ItemBGs);
	void								Set_ChooseItem(_int _iIndex);
	_bool								Get_LogoChanseStage() { return m_isLogoChooseStage; }
	void								Set_LogoChangeState(_bool _LogoChooseStage) { m_isLogoChooseStage = _LogoChooseStage; }

	_bool								Get_isQIcon() { return m_isQIcon; }
	void								Set_isQIcon(_bool _is) { m_isQIcon = _is; }

	vector<CDialog::DialogData>			Get_Dialogue(const _wstring& _id);
	CDialog::DialogLine					Get_DialogueLine(const _wstring& _id, _int _LineIndex);
	_float4								Get_DialoguePos() { return m_vDialoguePos; }
	void								Set_DialoguePos(_float4 _vDialoguePos) { m_vDialoguePos = _vDialoguePos; }
	_float3								Get_CalDialoguePos() { return m_vCalDialoguePos; }
	void								Set_CalDialoguePos(_float3 _vPos) { m_vCalDialoguePos = _vPos; }
	void								Set_DisplayDialogue(_bool _Display) { m_isDisplayDialogue = _Display; }
	_bool								Get_DisplayDialogue() { return m_isDisplayDialogue; }
	_bool								Get_DialogueFinishShopPanel() { return m_isDialogueFinishShopPanel; }
	void								Set_DialogueFinishShopPanel(_bool _Finish) { m_isDialogueFinishShopPanel = _Finish; }

	vector<CLogo_Props*>				Get_LogoProps() { return m_LogoProps; }
	void								pushBack_LogoPorp(CLogo_Props* _props) { m_LogoProps.push_back(_props); Safe_AddRef(_props); }
	
	_tchar*								Get_DialogId() { return m_tDialogId; }
	_tchar*								Get_strSectionID() { return m_strCurrentSection; }
	void								Set_DialogId(const _tchar* _id, const _tchar* _strCurSection = nullptr, _bool _DisplayDialogue = true, _bool _DisPlayPortrait = true);

	_int								Get_DialogueLineIndex() { return 
		m_iCurrnetLineIndex; }
	void								Set_DialogueLineIndex(_int _index) { m_iCurrnetLineIndex = _index; }

	_bool								Get_PortraitRender() { return m_isPortraitRender; }
	void								Set_PortraitRender(_bool _Render) { m_isPortraitRender = _Render; }

	void								Pushback_Dialogue(CDialog::DialogData _DialogData);

	_bool								Get_isMakeItem() { return m_isMakeItem; }
	void								Set_isMakeItem(_bool _make) { m_isMakeItem = _make; }


	void								Set_Narration(CNarration* _Narration) { m_pNarration = _Narration; Safe_AddRef(_Narration); }
	CNarration*							Get_Narration() { return m_pNarration; }
	void								Set_VioletMeet(_bool _meet) { m_isVioletMeet = _meet; }
	_bool								Get_VioletMeet() { return m_isVioletMeet; }
	void								Set_PlayNarration(const _wstring& _strid) { m_pNarration->CBase::Set_Active(true); wsprintf(m_strNarrationID, _strid.c_str()); m_isPlayerNarration = true; }
	_tchar*								Get_strNarrationID() { return m_strNarrationID; }
	_bool								Get_PlayNarration() { return m_isPlayerNarration; }
	void								Set_TurnoffPlayNarration(_bool _Turnoff) { m_isPlayerNarration = _Turnoff; }

	_bool								isLeft_Right();




	/************ 테스트용도 **********************/
	void								Test_Update(_float _fTimedelta);
	_uint								Get_TestDiaglogIndex() { return m_iTextIndex; }
	void								Set_TestDialogIndex() { ++m_iTextIndex; }




	HRESULT								Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);


public:
	virtual void Free() override;
};

END