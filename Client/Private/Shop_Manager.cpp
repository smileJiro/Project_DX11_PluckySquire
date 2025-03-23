#include "stdafx.h"
#include "Shop_Manager.h"
#include "PlayerData_Manager.h"


IMPLEMENT_SINGLETON(CShop_Manager)

CShop_Manager::CShop_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CShop_Manager::Create_ShopPanel(_int _iChangeLevel, wstring _strLayerTag)
{

	//if (LEVEL_CHAPTER_2 != _iChangeLevel &&  LEVEL_CHAPTER_4 != _iChangeLevel && LEVEL_CHAPTER_6 != _iChangeLevel &&  LEVEL_CHAPTER_8 != _iChangeLevel)
	//{
	//	return E_FAIL;
	//}
	if (LEVEL_CHAPTER_6 != _iChangeLevel &&
		LEVEL_CHAPTER_2 != _iChangeLevel &&
		LEVEL_CHAPTER_4 != _iChangeLevel)
		return S_OK;



	CUI::UIOBJDESC pDesc = {};
	CUI::UIOBJDESC pShopDescs[CUI::SHOPPANEL::SHOP_END] = {};
	CUI::UIOBJDESC pShopYesNo = {};
	_uint ShopPanelUICount = { CUI::SHOPPANEL::SHOP_END };


	pDesc.iCurLevelID = _iChangeLevel;

	for (_uint i = 0; i < CUI::SHOPPANEL::SHOP_END; ++i)
	{
		pShopDescs[i].iCurLevelID = _iChangeLevel;
	}

	//if (ShopPanelUICount != CUI_Manager::GetInstance()->Get_ShopPanels().size())
	//{
	for (_uint i = 0; i < CUI::SHOPPANEL::SHOP_END; ++i)
	{
		CGameObject* pShopPanel = { nullptr };
		CUI::UIOBJDESC pShopPanelDesc{};


		switch (i)
		{
		case CUI::SHOPPANEL::SHOP_BG:
		{
			pShopDescs[CUI::SHOPPANEL::SHOP_BG].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_BG].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeX = 2344.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeY = 1544.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BG;
			pShopDescs[CUI::SHOPPANEL::SHOP_BG].eShopPanelKind = CUI::SHOPPANEL::SHOP_BG;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BG])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BG].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));
			Set_ShopBG(static_cast<CShopPanel_BG_New*>(pShopPanel));
		}
		break;

		case CUI::SHOPPANEL::SHOP_DIALOGUEBG:
		{
			pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeX = 2320.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeY = 424.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_DIALOGUEBG;
			pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_DIALOGUEBG;


			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[i].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));
		}
		break;

		case CUI::SHOPPANEL::SHOP_CHOOSEBG:
		{
			CGameObject* pShopYesOrNo = { nullptr };
			pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iCurLevelID = _iChangeLevel;
			pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeX = 800.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeY = 416.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_CHOOSEBG;
			pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_CHOOSEBG;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelYesNo"), pShopDescs[i].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));
		}
		break;

		case CUI::SHOPPANEL::SHOP_BULB:
		{
			pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeX = 640.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeY = 272.f * 0.8f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BULB;
			pShopDescs[CUI::SHOPPANEL::SHOP_BULB].eShopPanelKind = CUI::SHOPPANEL::SHOP_BULB;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BULB])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));
		}
		break;

		case CUI::SHOPPANEL::SHOP_BACKESC:
		{
			pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeX = 72.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeY = 72.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ESCBG;
			pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ESCBG;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));

			pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeX = 144.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeY = 144.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKESC;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKESC;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));
		}
		break;

		case CUI::SHOPPANEL::SHOP_BACKARROW:
		{
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeX = 72.f * 1.2f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeY = 72.f * 1.2f;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKARROW;
			pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKARROW;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));
		}
		break;


		case CUI::SHOPPANEL::SHOP_ENTER:
		{
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeX = 72.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeY = 72.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTERBG;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTERBG;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));

			pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fX = DEFAULT_SIZE_BOOK2D_X;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fY = DEFAULT_SIZE_BOOK2D_Y;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeX = 144.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeY = 144.f;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTER;
			pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTER;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTER])))
				return E_FAIL;

			Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount, static_cast<CShopPanel_BG_New*>(pShopPanel));
		}
		break;
		//
		//default:
		//	break;
		}
	}

	return S_OK;
}

HRESULT CShop_Manager::Create_Item(_int _iChangeLevel, wstring _strLayerTag)
{

	//if (LEVEL_CHAPTER_2 != _iChangeLevel && LEVEL_CHAPTER_4 != _iChangeLevel &&
	//	LEVEL_CHAPTER_6 != _iChangeLevel && LEVEL_CHAPTER_8 != _iChangeLevel)
	//{
	//	return S_OK;
	//}


	if (LEVEL_CHAPTER_6 != _iChangeLevel &&
		LEVEL_CHAPTER_2 != _iChangeLevel &&
		LEVEL_CHAPTER_4 != _iChangeLevel)
		return S_OK;


	_uint iCurLevel = _iChangeLevel;

	for (_uint i = 0; i < 4; ++i)
	{
		CGameObject* pShopItem = { nullptr };
		CShopPanel_New::ShopUI eShopDesc;

		switch (i)
		{
		case 0:
		{
			vector<CShopItemBG_New*> _vItemBG;
			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 1720.f * 0.8f;
			eShopDesc.fSizeY = 220.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = 0;
			eShopDesc.isChooseItem = true;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			CShopItemBG_New* pItemBG = static_cast<CShopItemBG_New*>(pShopItem);

			_vItemBG.push_back(pItemBG);

			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 256.f * 0.8f;
			eShopDesc.fSizeY = 256.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = m_iAttackPlusLevel;
			eShopDesc.isChooseItem = true;

			if (0 == m_iJumpAttackLevel)
			{
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_JUMPATTACKBADGE;
				eShopDesc.iPrice = 10;
				eShopDesc.strName = TEXT("점프 공격 배지");
				eShopDesc.iSkillLevel = m_iJumpAttackLevel;

			}
			else
			{
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_ATTACKPLUSBADGE;
				eShopDesc.iPrice = 10;
				eShopDesc.strName = TEXT("검 공격력 강화");
				eShopDesc.iSkillLevel = m_iAttackPlusLevel;
			}


			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			pItemBG = static_cast<CShopItemBG_New*>(pShopItem);

			_vItemBG.push_back(pItemBG);

			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 120.f * 0.8f;
			eShopDesc.fSizeY = 144.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = 0;
			eShopDesc.isChooseItem = true;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			pItemBG = static_cast<CShopItemBG_New*>(pShopItem);

			_vItemBG.push_back(pItemBG);
			pushBack_ShopItem(_vItemBG);

		}
		break;

		case 1:
		{
			vector<CShopItemBG_New*> _vItemBG;
			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 1720.f * 0.8f;
			eShopDesc.fSizeY = 220.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = 0;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			CShopItemBG_New* pItemBG = static_cast<CShopItemBG_New*>(pShopItem);
			_vItemBG.push_back(pItemBG);

			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 256.f * 0.8f;
			eShopDesc.fSizeY = 256.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = m_iThrowAttackLevel;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_THROWATTBADGE;
			eShopDesc.iPrice = 20;
			eShopDesc.strName = TEXT("검 던지기 배지");

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			pItemBG = static_cast<CShopItemBG_New*>(pShopItem);
			_vItemBG.push_back(pItemBG);


			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 120.f * 0.8f;
			eShopDesc.fSizeY = 144.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = 0;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			pItemBG = static_cast<CShopItemBG_New*>(pShopItem);
			_vItemBG.push_back(pItemBG);


			pushBack_ShopItem(_vItemBG);

		}
		break;

		case 2:
		{
			vector<CShopItemBG_New*> _vItemBG;
			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 1720.f * 0.8f;
			eShopDesc.fSizeY = 220.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = 0;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			CShopItemBG_New* pItemBG = static_cast<CShopItemBG_New*>(pShopItem);
			_vItemBG.push_back(pItemBG);

			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 256.f * 0.8f;
			eShopDesc.fSizeY = 256.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = m_iSpinAttackLevel;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_SPINATTACKBADGE;
			eShopDesc.iPrice = 30;
			eShopDesc.strName = TEXT("회전 공격 배지");

			

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			pItemBG = static_cast<CShopItemBG_New*>(pShopItem);
			_vItemBG.push_back(pItemBG);

			eShopDesc.iCurLevelID = iCurLevel;
			eShopDesc.fX = 0.f;
			eShopDesc.fY = 0.f;
			eShopDesc.fSizeX = 120.f * 0.8f;
			eShopDesc.fSizeY = 144.f * 0.8f;
			eShopDesc.iShopItemCount = i;
			eShopDesc.iSkillLevel = 0;
			eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
				return E_FAIL;

			pItemBG = static_cast<CShopItemBG_New*>(pShopItem);
			_vItemBG.push_back(pItemBG);

			pushBack_ShopItem(_vItemBG);

		}
		break;
		}
	}
	
	return S_OK;

}

void CShop_Manager::Delete_ShopItems(_uint _index)
{
	Skill_LevelUp(_index);

	for (int i = 0; i < m_ShopItems[_index].size(); ++i)
	{

 		Event_DeleteObject(m_ShopItems[_index][i]);
		CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(m_ShopItems[_index][i]);
		Safe_Release(m_ShopItems[_index][i]);
		//CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(TEXT("Chapter5_P0102"), m_ShopItems[_index][i]);
	}




	m_ShopItems.erase(m_ShopItems.begin() + _index);

	// 뱃지 포지션 파악
	m_BadgePositions.erase(m_BadgePositions.begin() + _index);
	m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_shop_purchase_confirm"), 0.0f, 30.f, false);

	if (0 < m_ShopItems.size())
	{
		if (1 == m_ShopItems.size())
		{
			for (int i = 0; i < m_ShopItems[0].size(); ++i)
			{
				m_ShopItems[0][i]->Set_isChooseItem(true);
			}
			return;
		}

		if (0 < _index)
		{
			for (int i = 0; i < m_ShopItems[_index - 1].size(); ++i)
			{
				m_ShopItems[_index - 1][i]->Set_isChooseItem(true);
			}
		}
		else if (0 == _index)
		{
			for (int i = 0; i < m_ShopItems[_index].size(); ++i)
			{
				m_ShopItems[_index][i]->Set_isChooseItem(true);
			}
		}
		
	}


		
	//m_isUpdateShopPanel = true;
	int a = 0;
	
	

}

void CShop_Manager::Skill_LevelUp(_uint _index)
{
	_int iLevel = m_ShopItems[_index][1]->Get_SkillLevel();
	CUI::SKILLSHOP eSkillIcon = m_ShopItems[_index][1]->Get_SkillShopIcon();

	CPlayerData_Manager* pPlayerData = CPlayerData_Manager::GetInstance();

	switch (eSkillIcon)
	{
	case CUI::SKILLSHOP::SKILLSHOP_JUMPATTACKBADGE:
	{
		++m_iJumpAttackLevel;
		pPlayerData->Set_JumpSkillLevel(m_iJumpAttackLevel);
	}

		
		break;

	case CUI::SKILLSHOP::SKILLSHOP_SPINATTACKBADGE:
	{
		++m_iSpinAttackLevel;
		pPlayerData->Set_WhirlSkillLevel(m_iSpinAttackLevel);
	}
		
		break;

	case CUI::SKILLSHOP::SKILLSHOP_ATTACKPLUSBADGE:
	{
		++m_iAttackPlusLevel;
		pPlayerData->Set_AttackDamageLevel(m_iAttackPlusLevel);
	}
		
		break;

	case CUI::SKILLSHOP::SKILLSHOP_THROWATTBADGE:
	{
		++m_iThrowAttackLevel;
		pPlayerData->Set_ThrowSkillLevel(m_iThrowAttackLevel);
	}
		
		break;

	}
}

void CShop_Manager::OpenClose_Shop()
{
	for (auto iter : m_pShopPanels)
	{
		if (CUI::SHOP_END != iter.second->Get_ShopPanel())
		{
			iter.second->isRender();

			if (false == CSection_Manager::GetInstance()->Is_CurSection(iter.second))
				CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(iter.second, 8);
		}
	}

	for (int i = 0; i < m_ShopItems.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			m_ShopItems[i][j]->isRender();
			if (false == CSection_Manager::GetInstance()->Is_CurSection(m_ShopItems[i][j]))
				CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(m_ShopItems[i][j], 8);
		}
	}

	false == m_isOpenShop ? m_isOpenShop = true : m_isOpenShop = false;

}

void CShop_Manager::Set_ChooseItem(_int _iIndex)
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
			for (_int j = 0; j < m_ShopItems[i].size(); ++j)
			{
				m_ShopItems[i][j]->Set_isChooseItem(false);
			} 
		}

		for (_int i = 0; i <= m_ShopItems[_iIndex].size() - 1; ++i)
		{
			m_ShopItems[_iIndex][i]->Set_isChooseItem(true);
		}
		m_iPreIndex = _iIndex;
	}
}

void CShop_Manager::Emplace_ShopPanels(_uint _ePanel, CShopPanel_BG_New* _pPanel)
{
	m_pShopPanels.emplace(_ePanel, _pPanel);
	Safe_AddRef(_pPanel);
}

void CShop_Manager::pushBack_ShopItem(vector<CShopItemBG_New*> _ItemBGs)
{
	m_ShopItems.push_back(_ItemBGs);

	for (auto& pShopItemBG : _ItemBGs)
	{
		Safe_AddRef(pShopItemBG);
	}
}

HRESULT CShop_Manager::Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID)
{
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

	if (nullptr != m_pShop)
	{
		Safe_Release(m_pShop);
		m_pShop = nullptr;
	}

	if (nullptr != m_ShopBG)
	{
		Safe_Release(m_ShopBG);
		m_ShopBG = nullptr;
	}


	m_isOpenShop = false;
	m_iPreIndex = 0;
	m_isConfirm = false;
	m_isOpenConfirmUI = false;
	m_isPurchase = true;


	return S_OK;
}

HRESULT CShop_Manager::Level_Enter(_int _iChangeLevelID)
{

	if(FAILED(Create_ShopPanel(_iChangeLevelID, TEXT("Layer_Shopanel"))))
		return E_FAIL;

	if(FAILED(Create_Item(_iChangeLevelID, TEXT("Layer_ShopItem"))))
		return E_FAIL;

	return S_OK;
}


void CShop_Manager::Free()
{
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


	Safe_Release(m_pGameInstance);
	__super::Free();

	Safe_Release(m_pShop);
	Safe_Release(m_ShopBG);
}

