#include "stdafx.h"
#include "ShopPanel.h"
#include "UI_Manager.h"

CShopPanel::CShopPanel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CShopPanel::CShopPanel(const CShopPanel& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CShopPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_eShopPanel = SHOP_END;

	if (false == Uimgr->Get_isMakeItem())
	{
		Uimgr->Set_isMakeItem(true);
		Ready_ShopPannel(LEVEL_ID(pDesc->iCurLevelID), pDesc->strLayerTag);
		Ready_Item(LEVEL_ID(pDesc->iCurLevelID), pDesc->strLayerTag);
	}

	return S_OK;
}

void CShopPanel::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel::Update(_float _fTimeDelta)
{
	if (!m_isOpenPanel || CUI_Manager::GetInstance()->Get_ConfirmStore())
	{
		Update_KeyInput(_fTimeDelta, -1);
		return;
	}

	_float2 cursorPos = m_pGameInstance->Get_CursorPos();


	if (!isInPanel(cursorPos))
	{
		Update_KeyInput(_fTimeDelta, -1);
		return;
	}

	_int iIndex = isInPanelItem(cursorPos);

	if (iIndex != -1 && iIndex != m_iPreindex)
	{
		CUI_Manager::GetInstance()->Set_ChooseItem(iIndex);
		m_iPreindex = iIndex;
	}

	Update_KeyInput(_fTimeDelta, iIndex);


	
}

void CShopPanel::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender && SHOP_CHOOSEBG != m_eShopPanel)
	{
		for (int i = 0; i < CUI_Manager::GetInstance()->Get_ShopPanels().size(); ++i)
		{
 			CUI_Manager::GetInstance()->Get_ShopPanels()[i]->Child_LateUpdate(_fTimeDelta);
		}


		for (int i = 0; i < CUI_Manager::GetInstance()->Get_ShopItems().size(); ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				CUI_Manager::GetInstance()->Get_ShopItems()[i][j]->Child_LateUpdate(_fTimeDelta);
			}
			
		}

	}
	else
	{
		__super::Late_Update(_fTimeDelta);
	}

}

HRESULT CShopPanel::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	if (true == m_isRender)
	{
		//__super::Render(_iTextureindex, _eShaderPass);

		_float2 BGPos = { 0.f, 0.f };
		BGPos.x = Uimgr->Get_ShopPanels()[0]->Get_FX();
		BGPos.y = Uimgr->Get_ShopPanels()[0]->Get_FY();

		/* 나중에 수정 필요 */
		_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);

		wsprintf(m_tFont, TEXT("999"));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX * 1.8f, g_iWinSizeY / 4.2f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

		wsprintf(m_tFont, TEXT("선택"));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 1.15f - 2.5f,	BGPos.y * 1.84f),			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 1.15f + 2.5f,	BGPos.y * 1.84f),			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 1.15f,			BGPos.y * 1.84f - 2.5f),	XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 1.15f,			BGPos.y * 1.84f + 2.5f),	XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 1.15f,			BGPos.y * 1.84f),			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

		wsprintf(m_tFont, TEXT("취소"));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 0.805f - 2.5f,	BGPos.y * 1.84f),			XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 0.805f + 2.5f,	BGPos.y * 1.84f),			XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 0.805f,			BGPos.y * 1.84f - 2.5f),	XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 0.805f,			BGPos.y * 1.84f + 2.5f),	XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font30"), m_tFont, _float2(BGPos.x * 0.805f,			BGPos.y * 1.84f),			XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

	}


	if (true == m_isRender)
		__super::Render(_iTextureindex, _eShaderPass);


	if (true == CUI_Manager::GetInstance()->Get_ConfirmStore())
	{
		_bool YesorNo = CUI_Manager::GetInstance()->Get_StoreYesOrno();


		if (true == YesorNo)
		{
			wsprintf(m_tFont, TEXT("예"));
			m_pGameInstance->Render_Scaling_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX * 1.65f, g_iWinSizeY * 1.45f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), 0.f, _float2(0.f, 0.f), 1.5f);
			wsprintf(m_tFont, TEXT("아니요"));
			m_pGameInstance->Render_Scaling_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX * 1.65f, g_iWinSizeY * 1.6f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), 0.f, _float2(0.f, 0.f), 1.5f);
		}
		else if (false == YesorNo)
		{
			wsprintf(m_tFont, TEXT("예"));
			m_pGameInstance->Render_Scaling_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX * 1.65f, g_iWinSizeY * 1.45f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), 0.f, _float2(0.f, 0.f), 1.5f);
			wsprintf(m_tFont, TEXT("아니요"));
			m_pGameInstance->Render_Scaling_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX * 1.65f, g_iWinSizeY * 1.6f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), 0.f, _float2(0.f, 0.f), 1.5f);
		}
	}


	return S_OK;
}



_bool CShopPanel::isInPanel(_float2 _vMousePos)
{
	

	if (349 <= _vMousePos.x && 1252 >= _vMousePos.x && 56 <= _vMousePos.y && 797 >= _vMousePos.y)
	{
		return true;
	}
	else
		return false;
}

_int CShopPanel::isInPanelItem(_float2 _vMousePos)
{
 	_int iItemCounts = (_uint)CUI_Manager::GetInstance()->Get_ShopItems().size();

	if (373 <= _vMousePos.x && 1228 >= _vMousePos.x)
	{

		// 0개
		if (171 <= _vMousePos.y && 278 >= _vMousePos.y)
		{
			if (0 >= iItemCounts)
			{
				return -1;
			}

			cout << 0 << endl;
			return 0;
		}
		else if (279 <= _vMousePos.y && 305 >= _vMousePos.y)
		{
			if (0 >= iItemCounts)
			{
				return -1;
			}

			cout << -1 << endl;
			return -1;
		}
		// 1개
		else if (306 <= _vMousePos.y && 413 >= _vMousePos.y)
		{
			if (1 >= iItemCounts)
			{
				return -1;
			}

			cout << 1 << endl;
			return 1;
		}
		else if (414 <= _vMousePos.y && 440 >= _vMousePos.y)
		{
			if (1 >= iItemCounts)
			{
				return -1;
			}

			cout << -1 << endl;
			return -1;
		}
		// 2개
		else if (441 <= _vMousePos.y && 548 >= _vMousePos.y)
		{
			if (2 >= iItemCounts)
			{
				return -1;
			}

			cout << 2 << endl;
			return 2;
		}
		else if (549 <= _vMousePos.y)
		{
			if (2 >= iItemCounts)
			{
				return -1;
			}

			cout << -1 << endl;
			return -1;
		}
	}
	else
		return -1;


	return -1;

}

void CShopPanel::Update_KeyInput(_float _fTimeDelta, _int _index)
{
	

	// SHOP  패널 오픈 기능
	
	bool iskeyI = KEY_DOWN(KEY::I);
	bool iskeyESC = KEY_DOWN(KEY::ESC);
	bool iskeyE = KEY_DOWN(KEY::E);

	if (true == iskeyI) /* && SHOP_END != m_eShopPanel*/
	{
		if (true == Uimgr->Get_isESC())
			return;

		isFontPrint();

		for (auto iter : Uimgr->Get_ShopPanels())
		{
			if (SHOP_END != iter.second->Get_ShopPanel())
			{
				iter.second->Child_Update(_fTimeDelta);
			}
		}


		for (int i = 0; i < Uimgr->Get_ShopItems().size(); ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				Uimgr->Get_ShopItems()[i][j]->Child_Update(_fTimeDelta);
			}
		}
	}

	if (false == m_isOpenPanel)
		return;


	if (true == iskeyI && true == Uimgr->Get_ConfirmStore())
	{
		Uimgr->Set_ConfirmStore(false);
		m_iConfirmItemIndex = 0;
		Uimgr->Set_StoreYesOrno(true);
	}
	if (true == iskeyESC && true == Uimgr->Get_ConfirmStore())
	{
		Uimgr->Set_ConfirmStore(false);
		m_iConfirmItemIndex = 0;
		Uimgr->Set_StoreYesOrno(true);
	}

	_bool isYesorNo = Uimgr->Get_StoreYesOrno();

	if (true == m_isOpenPanel)
	{

		_float2 CursorPos = m_pGameInstance->Get_CursorPos();
		_int index = isInPanelItem(CursorPos);

		if (true == iskeyE && false == Uimgr->Get_ConfirmStore())
		{
			if (-1 != index)
			{
				m_iConfirmItemIndex = index;
				Uimgr->Set_ConfirmStore(true);
				Uimgr->Set_StoreYesOrno(true);
			}
		}
		else if (true == iskeyE && true == Uimgr->Get_ConfirmStore())
		{
			if (true == isYesorNo)
			{
				Uimgr->Delete_ShopItems(m_iConfirmItemIndex);
				m_iConfirmItemIndex = 0;
				Uimgr->Set_ConfirmStore(false);
			}
			else if (false == isYesorNo)
			{
				Uimgr->Set_ConfirmStore(false);
				Uimgr->Set_StoreYesOrno(true);
			}

		}
	}

	if (KEY_DOWN(KEY::UP) && true == Uimgr->Get_ConfirmStore())
	{

		CUI_Manager::GetInstance()->Set_StoreYesOrno(true);

	}

	if (KEY_DOWN(KEY::DOWN) && true == Uimgr->Get_ConfirmStore())
	{

		CUI_Manager::GetInstance()->Set_StoreYesOrno(false);

	}

}

HRESULT CShopPanel::Ready_ShopPannel(LEVEL_ID _eCurLevel, const _wstring& _strLayerTag)
{
	CUI::UIOBJDESC pDesc = {};
	CUI::UIOBJDESC pShopDescs[CUI::SHOPPANEL::SHOP_END] = {};
	CUI::UIOBJDESC pShopYesNo = {};
	_uint ShopPanelUICount = { CUI::SHOPPANEL::SHOP_END };


	pDesc.iCurLevelID = _eCurLevel;

	for (_uint i = 0; i < CUI::SHOPPANEL::SHOP_END; ++i)
	{
		pShopDescs[i].iCurLevelID = LEVEL_GAMEPLAY;
	}

	if (ShopPanelUICount != CUI_Manager::GetInstance()->Get_ShopPanels().size())
	{
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

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;

			case CUI::SHOPPANEL::SHOP_DIALOGUEBG:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeX = 2320.f * 0.8f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeY = 424.f * 0.8f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_DIALOGUEBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_DIALOGUEBG;
			
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[i].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;
			
			case CUI::SHOPPANEL::SHOP_CHOOSEBG:
			{
				CGameObject* pShopYesOrNo = { nullptr };
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iCurLevelID = m_iCurLevelID;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeX = 800.f * 0.8f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeY = 416.f * 0.8f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_CHOOSEBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_CHOOSEBG;
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelYesNo"), pShopDescs[i].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;
			
			case CUI::SHOPPANEL::SHOP_BULB:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeX = 640.f * 0.8f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeY = 272.f * 0.8f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BULB;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].eShopPanelKind = CUI::SHOPPANEL::SHOP_BULB;
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BULB])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;
			
			case CUI::SHOPPANEL::SHOP_BACKESC:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeX = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeY = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ESCBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ESCBG;
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeX = 144.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeY = 144.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKESC;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKESC;
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;
			
			case CUI::SHOPPANEL::SHOP_BACKARROW:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeX = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeY = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKARROW;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKARROW;
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;
			
			
			case CUI::SHOPPANEL::SHOP_ENTER:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeX = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeY = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTERBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTERBG;
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);

				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fX = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fY = 0.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeX = 144.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeY = 144.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTER;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTER;
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTER])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;
			
			default:
				break;
			}

			

		}
		
	}

	return S_OK;
}

HRESULT CShopPanel::Ready_Item(LEVEL_ID _eCurLevel, const _wstring& _strLayerTag)
{
	if (5 != CUI_Manager::GetInstance()->Get_ShopItems().size())
	{
		_uint iCurLevel = _eCurLevel;


		for (_uint i = 0; i < 5; ++i)
		{
			CGameObject* pShopItem = { nullptr };
			CUI::UIOBJDESC eShopDesc;


			switch (i)
			{
			case 0:
			{
				vector<CShopItemBG*> _vItemBG;
				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = 0.f;
				eShopDesc.fY = 0.f;
				eShopDesc.fSizeX = 1720.f;
				eShopDesc.fSizeY = 220.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);

				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = 0.f;
				eShopDesc.fY = 0.f;
				eShopDesc.fSizeX = 256.f;
				eShopDesc.fSizeY = 256.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_SCROLLITEM;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);

				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = 0.f;
				eShopDesc.fY = 0.f;
				eShopDesc.fSizeX = 120.f;
				eShopDesc.fSizeY = 144.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);
				CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);
			//
			}
			break;

			case 1:
			{
			//	vector<CShopItemBG*> _vItemBG;
			//	eShopDesc.iCurLevelID = iCurLevel;
			//	eShopDesc.fX = 0.f;
			//	eShopDesc.fY = 0.f;
			//	eShopDesc.fSizeX = 1720.f;
			//	eShopDesc.fSizeY = 220.f;
			//	eShopDesc.iShopItemCount = i;
			//	eShopDesc.iSkillLevel = 0;
			//	eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
			//		return E_FAIL;
			//
			//	CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);
			//	_vItemBG.push_back(pItemBG);
			//
			//	eShopDesc.iCurLevelID = iCurLevel;
			//	eShopDesc.fX = 0.f;
			//	eShopDesc.fY = 0.f;
			//	eShopDesc.fSizeX = 256.f;
			//	eShopDesc.fSizeY = 256.f;
			//	eShopDesc.iShopItemCount = i;
			//	eShopDesc.iSkillLevel = 2;
			//	eShopDesc.eShopSkillKind = CUI::SKILLSHOP_ATTACKPLUSBADGE;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
			//		return E_FAIL;
			//
			//	pItemBG = static_cast<CShopItemBG*>(pShopItem);
			//	_vItemBG.push_back(pItemBG);
			//
			//
			//	eShopDesc.iCurLevelID = iCurLevel;
			//	eShopDesc.fX = 0.f;
			//	eShopDesc.fY = 0.f;
			//	eShopDesc.fSizeX = 120.f;
			//	eShopDesc.fSizeY = 144.f;
			//	eShopDesc.iShopItemCount = i;
			//	eShopDesc.iSkillLevel = 0;
			//	eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
			//		return E_FAIL;
			//
			//	pItemBG = static_cast<CShopItemBG*>(pShopItem);
			//	_vItemBG.push_back(pItemBG);
			//
			//
			//	CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

			}
			break;

			case 2:
			{
			//	vector<CShopItemBG*> _vItemBG;
			//	eShopDesc.iCurLevelID = iCurLevel;
			//	eShopDesc.fX = 0.f;
			//	eShopDesc.fY = 0.f;
			//	eShopDesc.fSizeX = 1720.f;
			//	eShopDesc.fSizeY = 220.f;
			//	eShopDesc.iShopItemCount = i;
			//	eShopDesc.iSkillLevel = 0;
			//	eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
			//		return E_FAIL;
			//
			//	CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);
			//	_vItemBG.push_back(pItemBG);
			//
			//
			//
			//	eShopDesc.iCurLevelID = iCurLevel;
			//	eShopDesc.fX = 0.f;
			//	eShopDesc.fY = 0.f;
			//	eShopDesc.fSizeX = 256.f;
			//	eShopDesc.fSizeY = 256.f;
			//	eShopDesc.iShopItemCount = i;
			//	eShopDesc.iSkillLevel = 0;
			//	eShopDesc.eShopSkillKind = CUI::SKILLSHOP_JUMPATTACKBADGE;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
			//		return E_FAIL;
			//
			//	pItemBG = static_cast<CShopItemBG*>(pShopItem);
			//	_vItemBG.push_back(pItemBG);
			//
			//
			//
			//
			//	eShopDesc.iCurLevelID = iCurLevel;
			//	eShopDesc.fX = 0.f;
			//	eShopDesc.fY = 0.f;
			//	eShopDesc.fSizeX = 1720.f;
			//	eShopDesc.fSizeY = 220.f;
			//	eShopDesc.iShopItemCount = i;
			//	eShopDesc.iSkillLevel = 0;
			//	eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
			//		return E_FAIL;
			//
			//	pItemBG = static_cast<CShopItemBG*>(pShopItem);
			//	_vItemBG.push_back(pItemBG);
			//
			//
			//	CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

			}
			break;

			case 3:
			{
				vector<CShopItemBG*> _vItemBG;

			}
			break;

			case 4:
			{
				vector<CShopItemBG*> _vItemBG;

			}
			break;

			}
		}
	}

	return S_OK;
}

HRESULT CShopPanel::Ready_Components()
{
	return S_OK;
}


void CShopPanel::Change_BookScale_ForShopItem(_float2 _vRTSize)
{
	_float3 vScaleSize;
	_float2 vCalSize;
	vScaleSize = m_pControllerTransform->Get_Scale();
	_float2 BGPos = { 0.f, 0.f };
	BGPos.x = Uimgr->Get_ShopPanels()[0]->Get_FX();
	BGPos.y = Uimgr->Get_ShopPanels()[0]->Get_FY();

	switch (m_eSkillShopIcon)
	{
	case SKILLSHOP_BG: 
	{
		if (this == Uimgr->Get_ShopItems()[0][0])
		{
			m_fX;
			m_fY = BGPos.y - _vRTSize.y * vScaleSize.y * 0.8f;
			int a = 0;
		}
		else if (this == Uimgr->Get_ShopItems()[1][0])
		{

		}
		else if (this == Uimgr->Get_ShopItems()[2][0])
		{

		}

		
	}
	break;

	case SKILLSHOP_JUMPATTACKBADGE: 
	{

	}
	break;

	case SKILLSHOP_SPINATTACKBADGE: 
	{

	}
	break;

	case SKILLSHOP_ATTACKPLUSBADGE: 
	{

	}
	break;

	case SKILLSHOP_THROWATTBADGE: 
	{

	}
	break;

	case SKILLSHOP_SCROLLITEM: 
	{

	}
	break;

	case SKILLSHOP_BULB: 
	{

	}
	break;

	}

	vCalSize = _float2(m_vOriginSize.x * vScaleSize.x, m_vOriginSize.y * vScaleSize.y);

	m_pControllerTransform->Set_Scale(COORDINATE_2D, vCalSize.x, vCalSize.y, 1.f);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - m_fSizeX * 0.5f, -m_fY + m_fSizeY * 0.5f, 0.f, 1.f));

	//XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	//XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y, 0.0f, 1.0f));


}

CShopPanel* CShopPanel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel* pInstance = new CShopPanel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel::Clone(void* _pArg)
{
	CShopPanel* pInstance = new CShopPanel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel::Free()
{

	__super::Free();
}






