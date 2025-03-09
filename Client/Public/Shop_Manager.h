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
	void								pushBack_ShopItem(vector<CShopItemBG_New*> _ItemBGs);
	void								Emplace_ShopPanels(_uint _ePanel, CShopPanel_BG_New* _pPanel);

	void								OpenClose_Shop();

	HRESULT								Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);
	
public:
	virtual void Free() override;

private:
	HRESULT								Create_ShopPanel(_int _iChangeLevel, wstring _strLayerTag);
	HRESULT								Create_Item(_int _iChangeLevel, wstring _strLayerTag);

	


private:
	CGameInstance*							m_pGameInstance;
	vector<vector<CShopItemBG_New*>>		m_ShopItems;
	map<_uint, CShopPanel_BG_New*>			m_pShopPanels;


	_uint									m_iJumpAttackLevel = { 0 };
	_uint									m_iSpinAttackLevel = { 0 };
	_uint									m_iThrowAttackLevel = { 0 };
	_uint									m_iAttackPlusLevel = { 0 };


};

END