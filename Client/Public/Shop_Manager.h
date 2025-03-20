#pragma once
#include "ShopPanel_New.h"
#include "ShopItemBG_New.h"
#include "ShopPanel_BG_New.h"

/*
스킬 레벨을 저장한다.
스킬 레벨을 아이템 생성 시 던져준다.
구매 시 가격 체크를 해주고 플레이어에게 통보한다.



*/

BEGIN(Client)
class CShop_Manager final : public CBase
{
	DECLARE_SINGLETON(CShop_Manager)

private:
	CShop_Manager();
	virtual ~CShop_Manager() = default;



		/*
		SKILLSHOP_BG,
		SKILLSHOP_JUMPATTACKBADGE,
		SKILLSHOP_SPINATTACKBADGE,
		SKILLSHOP_ATTACKPLUSBADGE,
		SKILLSHOP_THROWATTBADGE,
		SKILLSHOP_SCROLLITEM,*/

public:
	void									Set_ShopBG(CShopPanel_BG_New* _ShopBG) { m_ShopBG = _ShopBG; Safe_AddRef(_ShopBG); }
	CShopPanel_BG_New*						Get_ShopBG() 
	{ 
		if (nullptr == m_ShopBG)
			assert(m_ShopBG);

		return m_ShopBG; 
	}
	void									pushBack_ShopItem(vector<CShopItemBG_New*> _ItemBGs);
	void									Emplace_ShopPanels(_uint _ePanel, CShopPanel_BG_New* _pPanel);
	map<_uint, CShopPanel_BG_New*>			Get_ShopPanels() { return m_pShopPanels; }
	vector<vector<CShopItemBG_New*>>		Get_ShopItems() { 
		return m_ShopItems; 
	}
	_bool									Get_isOpenShop() { return m_isOpenShop; }
	void									Delete_ShopItems(_uint _index);
	void									Skill_LevelUp(_uint _index);

	void									OpenClose_Shop();
	void									Set_ChooseItem(_int _iIndex);

	// GET SET
	CShopPanel_New*							Get_Shop() { return m_pShop; }
	void									Set_Shop(CShopPanel_New* _pShop) { m_pShop = _pShop; Safe_AddRef(_pShop); }
	_float2									Get_ShopBGPos() { 
		return m_vShopPos; 
	}
	void									Set_ShopBGPos(_float2 _vShopBgPos) { m_vShopPos = _vShopBgPos; }
	_bool									Get_Confirm() { return m_isConfirm; }
	void									Set_Confirm(_bool _isConfirm) { m_isConfirm = _isConfirm; }
	_bool									Get_OpenConfirmUI() { return m_isOpenConfirmUI; }
	void									Set_OpenConfirmUI(_bool _isOpenConfirmUI) { m_isOpenConfirmUI = _isOpenConfirmUI; }
	_bool									Get_isPurchase() { return m_isPurchase; }
	void									Set_isPurchase(_bool _isPurchase) { m_isPurchase = _isPurchase; }
	_float2									Get_BadgePos() { return m_vBadgePos; }
	void									Set_BadgePos(_float2 _BadgePos) { m_vBadgePos = _BadgePos; }
	void									pushBack_BadgePos(_float2 _vBadgePos) { m_BadgePositions.push_back(_vBadgePos); }
	vector<_float2>							Get_BadgePositions() { 
		return m_BadgePositions; 
	}
	void									Set_BadgePositions(_int _index, _float2 _vPos) { m_BadgePositions[_index] = _vPos; }
	_int									Get_ChooseIndex() { return m_iChooseIndex; }
	void									Set_ChooseIndex(_int _ChooseIdx) { m_iChooseIndex = _ChooseIdx; }
	
	
	
	
	HRESULT									Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT									Level_Enter(_int _iChangeLevelID);
	

private:
	HRESULT									Create_ShopPanel(_int _iChangeLevel, wstring _strLayerTag);
	HRESULT									Create_Item(_int _iChangeLevel, wstring _strLayerTag);

	
public:
	virtual void Free() override;

private:
	CGameInstance*							m_pGameInstance;
	CShopPanel_New*							m_pShop = { nullptr };
	vector<vector<CShopItemBG_New*>>		m_ShopItems;
	map<_uint, CShopPanel_BG_New*>			m_pShopPanels;
	_float2									m_vShopPos = { _float2(0.f, 0.f) };
	_bool									m_isOpenShop = { false };
	_int									m_iPreIndex = { 0 };
	_bool									m_isConfirm = { false };
	_bool									m_isOpenConfirmUI = { false };
	_bool									m_isPurchase = { true };

	vector<_float2>							m_BadgePositions;
	_float2									m_vBadgePos = { 0.f, 0.f };
	_int									m_iChooseIndex = { 0 };

	

	_uint									m_iJumpAttackLevel = { 0 };
	_uint									m_iSpinAttackLevel = { 0 };
	_uint									m_iThrowAttackLevel = { 0 };
	_uint									m_iAttackPlusLevel = { 0 };
	CShopPanel_BG_New*						m_ShopBG = { nullptr };




public:
	/* TEST 용도 */
	_int									Get_ItemCount() { return m_iItemCount; }
	void									Set_ItemCount(_int _itestCount) { m_iItemCount = _itestCount; }
	_int									m_iItemCount = { 0 };

};

END