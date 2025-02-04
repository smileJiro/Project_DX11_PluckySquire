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
		wsprintf(m_tFont, TEXT("999"));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 4.2f, g_iWinSizeY / 12.2f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX * 1.8f, g_iWinSizeY / 4.2f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));


		//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.f;
		//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fY = g_iWinSizeY - g_iWinSizeY / 5.5f;

		wsprintf(m_tFont, TEXT("선택"));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 1.525f) / 2.f - 2.5f,	g_iWinSizeY  * 1.92f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 1.525f) / 2.f + 2.5f,	g_iWinSizeY  * 1.92f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 1.525f) / 2.f,			g_iWinSizeY  * 1.92f - 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 1.525f) / 2.f,			g_iWinSizeY  * 1.92f + 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 1.525f) / 2.f,			g_iWinSizeY  * 1.92f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));


		wsprintf(m_tFont, TEXT("취소"));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 3.25f) / 2.f - 2.5f,	g_iWinSizeY  * 1.92f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 3.25f) / 2.f + 2.5f,	g_iWinSizeY  * 1.92f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 3.25f) / 2.f,			g_iWinSizeY  * 1.92f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 3.25f) / 2.f,			g_iWinSizeY  * 1.92f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2((g_iWinSizeX * 3.25f) / 2.f,			g_iWinSizeY  * 1.92f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));



		//wsprintf(m_tFont, TEXT("선택"));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f - 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f + 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f, g_iWinSizeY - g_iWinSizeY / 9.4f - 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f, g_iWinSizeY - g_iWinSizeY / 9.4f + 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		//
		//
		//wsprintf(m_tFont, TEXT("취소"));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f - 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f + 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		//m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

		
	}

	
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

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




	
	//if (KEY_DOWN(KEY::I) /* && SHOP_END != m_eShopPanel*/)
	//{
	//	if (true == Uimgr->Get_isESC())
	//		return;
	//
	//	isFontPrint();
	//
	//	for (auto iter : Uimgr->Get_ShopPanels())
	//	{
	//		if (SHOP_END != iter.second->Get_ShopPanel())
	//		{
	//			iter.second->Child_Update(_fTimeDelta);
	//		}
	//	}
	//
	//
	//	for (int i = 0; i < Uimgr->Get_ShopItems().size(); ++i)
	//	{
	//		for (int j = 0; j < 3; ++j)
	//		{
	//			Uimgr->Get_ShopItems()[i][j]->Child_Update(_fTimeDelta);
	//		}
	//
	//	}
	//}
	//
	//
	//if (KEY_DOWN(KEY::I) && true == Uimgr->Get_ConfirmStore())
	//{
	//	Uimgr->Set_ConfirmStore(false);
	//	m_iConfirmItemIndex = 0;
	//	Uimgr->Set_StoreYesOrno(true);
	//}
	//if (KEY_DOWN(KEY::ESC) && true == Uimgr->Get_ConfirmStore())
	//{
	//	Uimgr->Set_ConfirmStore(false);
	//	m_iConfirmItemIndex = 0;
	//	Uimgr->Set_StoreYesOrno(true);
	//}
	//
	//_bool isYesorNo = Uimgr->Get_StoreYesOrno();
	//if (true == m_isOpenPanel)
	//{
	//
	//	_float2 CursorPos = m_pGameInstance->Get_CursorPos();
	//	_int index = isInPanelItem(CursorPos);
	//
	//	if (KEY_DOWN(KEY::E) && false == Uimgr->Get_ConfirmStore())
	//	{
	//		if (-1 != index)
	//		{
	//			m_iConfirmItemIndex = index;
	//			Uimgr->Set_ConfirmStore(true);
	//			Uimgr->Set_StoreYesOrno(true);
	//		}
	//	}
	//	else if (KEY_DOWN(KEY::E) && true == Uimgr->Get_ConfirmStore())
	//	{
	//		if (true == isYesorNo)
	//		{
	//			Uimgr->Delete_ShopItems(m_iConfirmItemIndex);
	//			m_iConfirmItemIndex = 0;
	//			Uimgr->Set_ConfirmStore(false);
	//		}
	//		else if (false == isYesorNo)
	//		{
	//			Uimgr->Set_ConfirmStore(false);
	//			Uimgr->Set_StoreYesOrno(true);
	//		}
	//	
	//	}
	//}
	//
	//
	//
	//if (KEY_DOWN(KEY::UP) && true == Uimgr->Get_ConfirmStore())
	//{
	//
	//	CUI_Manager::GetInstance()->Set_StoreYesOrno(true);
	//
	//}
	//
	//if (KEY_DOWN(KEY::DOWN) && true == Uimgr->Get_ConfirmStore())
	//{
	//
	//	CUI_Manager::GetInstance()->Set_StoreYesOrno(false);
	//
	//}

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


				//pShopDescs[CUI::SHOPPANEL::SHOP_BG].fX = g_iWinSizeX / 2.f / 2.f;
				//pShopDescs[CUI::SHOPPANEL::SHOP_BG].fY = g_iWinSizeY / 2.5f;

				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fX = DEFAULT_SIZE_BOOK2D_X;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fY = DEFAULT_SIZE_BOOK2D_Y;

				//pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeX = 937.6f * 0.85f / 2.f;
				//pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeY = 617.6f * 0.85f;

				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeX = 937.6f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeY = 617.6f;

				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeX = 2344.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeY = 1544.f;



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
			
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fX = g_iWinSizeX / 2.f / 2.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fY = g_iWinSizeY - g_iWinSizeY / 3.5f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeX = 928.f * 0.7f / 2.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeY = 169.6f * 0.7f;

				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeX = 2320.f / 2.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeY = 424.f;


				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_DIALOGUEBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_DIALOGUEBG;
			
			
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[i].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG])))
					return E_FAIL;
			
				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;
			//
			//case CUI::SHOPPANEL::SHOP_CHOOSEBG:
			//{
			//	CGameObject* pShopYesOrNo = { nullptr };
			//	pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iCurLevelID = m_iCurLevelID;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fX = g_iWinSizeX / 2.8f;// / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fY = g_iWinSizeY - g_iWinSizeY / 3.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeX = 400.f * 0.7f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeY = 213.f * 0.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_CHOOSEBG;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_CHOOSEBG;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelYesNo"), pShopDescs[i].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG])))
			//		return E_FAIL;
			//
			//	CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
			//	Safe_Release(pShopPanel);
			//}
			//break;
			//
			//case CUI::SHOPPANEL::SHOP_BULB:
			//{
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fX = (g_iWinSizeX - g_iWinSizeX / 4.f) / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fY = g_iWinSizeY / 8.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeX = 320.f * 0.7f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeY = 136.f * 0.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BULB;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BULB].eShopPanelKind = CUI::SHOPPANEL::SHOP_BULB;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BULB])))
			//		return E_FAIL;
			//
			//	CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
			//	Safe_Release(pShopPanel);
			//}
			//break;
			//
			//case CUI::SHOPPANEL::SHOP_BACKESC:
			//{
			//
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fX = g_iWinSizeX / 2.75f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fY = g_iWinSizeY - g_iWinSizeY / 5.5f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeX = 40.f * 0.7f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeY = 40.f * 0.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ESCBG;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ESCBG;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG])))
			//		return E_FAIL;
			//
			//	CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
			//	Safe_Release(pShopPanel);
			//
			//
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fX = g_iWinSizeX / 2.75f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fY = g_iWinSizeY - g_iWinSizeY / 5.5f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeX = 72.f * 0.7f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeY = 72.f * 0.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKESC;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKESC;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC])))
			//		return E_FAIL;
			//
			//	CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
			//	Safe_Release(pShopPanel);
			//}
			//break;
			//
			//case CUI::SHOPPANEL::SHOP_BACKARROW:
			//{
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fX = g_iWinSizeX / 3.3f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fY = g_iWinSizeY - g_iWinSizeY / 5.5f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeX = 72.f * 0.7f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeY = 72.f * 0.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKARROW;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKARROW;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW])))
			//		return E_FAIL;
			//
			//	CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
			//	Safe_Release(pShopPanel);
			//}
			//break;
			//
			//
			//case CUI::SHOPPANEL::SHOP_ENTER:
			//{
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fY = g_iWinSizeY - g_iWinSizeY / 13.f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeX = 40.f * 0.85f / 2.f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeY = 40.f * 0.85f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTERBG;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTERBG;
			//
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fY = g_iWinSizeY - g_iWinSizeY / 5.5f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeX = 40.f * 0.7f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeY = 40.f * 0.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTERBG;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTERBG;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG])))
			//		return E_FAIL;
			//
			//	CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
			//	Safe_Release(pShopPanel);
			//
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fY = g_iWinSizeY - g_iWinSizeY / 13.f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeX = 72.f * 0.85f / 2.f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeY = 72.f * 0.85f;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTER;
			//	//pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTER;
			//
			//
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fY = g_iWinSizeY - g_iWinSizeY / 5.5f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeX = 72.f * 0.7f / 2.f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeY = 72.f * 0.7f;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTER;
			//	pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTER;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTER])))
			//		return E_FAIL;
			//
			//	CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
			//	Safe_Release(pShopPanel);
			//}
			//break;
			//
			//default:
			//	break;
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
				//vector<CShopItemBG*> _vItemBG;
				//eShopDesc.iCurLevelID = iCurLevel;
				//eShopDesc.fX = g_iWinSizeX / 2.f;
				//eShopDesc.fY = g_iWinSizeY / 4.f;
				//eShopDesc.fSizeX = 860.f;
				//eShopDesc.fSizeY = 110.f;
				//eShopDesc.iShopItemCount = i;
				//eShopDesc.iSkillLevel = 0;
				//eShopDesc.isChooseItem = true;
				//eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				vector<CShopItemBG*> _vItemBG;
				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 2.f / 2.f;
				eShopDesc.fY = g_iWinSizeY / 4.f;
				eShopDesc.fSizeX = 860.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 110.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);

				//eShopDesc.iCurLevelID = iCurLevel;
				//eShopDesc.fX = g_iWinSizeX / 3.5f;
				//eShopDesc.fY = g_iWinSizeY / 4.f;
				//eShopDesc.fSizeX = 128.f;
				//eShopDesc.fSizeY = 128.f;
				//eShopDesc.iShopItemCount = i;
				//eShopDesc.iSkillLevel = 0;
				//eShopDesc.isChooseItem = true;
				//eShopDesc.eShopSkillKind = CUI::SKILLSHOP_SCROLLITEM;

				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 3.5f / 1.6f;
				eShopDesc.fY = g_iWinSizeY / 4.f;
				eShopDesc.fSizeX = 128.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 128.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_SCROLLITEM;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);

				//eShopDesc.iCurLevelID = iCurLevel;
				//eShopDesc.fX = g_iWinSizeX - g_iWinSizeX / 3.f;
				//eShopDesc.fY = g_iWinSizeY / 4.f;
				//eShopDesc.fSizeX = 60.f;
				//eShopDesc.fSizeY = 72.f;
				//eShopDesc.iShopItemCount = i;
				//eShopDesc.iSkillLevel = 0;
				//eShopDesc.isChooseItem = true;
				//eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;



				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.2f;
				eShopDesc.fY = g_iWinSizeY / 4.f;
				eShopDesc.fSizeX = 60.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 72.f * 0.8f * 0.85f ;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);
				CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

			}
			break;

			case 1:
			{
				vector<CShopItemBG*> _vItemBG;
				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 2.f / 2.f;
				eShopDesc.fY = g_iWinSizeY / 2.75f;
				eShopDesc.fSizeX = 860.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 110.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);

				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 3.5f / 1.6f;
				eShopDesc.fY = g_iWinSizeY / 2.75f;
				eShopDesc.fSizeX = 128.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 128.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 2;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_ATTACKPLUSBADGE;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);


				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.2f;
				eShopDesc.fY = g_iWinSizeY / 2.75f;
				eShopDesc.fSizeX = 60.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 72.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);


				CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

			}
			break;

			case 2:
			{
				vector<CShopItemBG*> _vItemBG;
				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 2.f / 2.f;
				eShopDesc.fY = g_iWinSizeY - g_iWinSizeY / 1.9f;
				eShopDesc.fSizeX = 860.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 110.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);



				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 3.5f / 1.6f;
				eShopDesc.fY = g_iWinSizeY - g_iWinSizeY / 1.9f;
				eShopDesc.fSizeX = 128.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 128.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_JUMPATTACKBADGE;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);




				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = (g_iWinSizeX - g_iWinSizeX / 3.f) / 2.2f;
				eShopDesc.fY = g_iWinSizeY - g_iWinSizeY / 1.9f;
				eShopDesc.fSizeX = 60.f * 0.8f * 0.85f / 2.f;
				eShopDesc.fSizeY = 72.f * 0.8f * 0.85f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);


				CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

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






