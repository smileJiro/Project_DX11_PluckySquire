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

	return S_OK;
}

void CShopPanel::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel::Update(_float _fTimeDelta)
{
	// 오픈 후 마우스가 안에 있는가?
	if (true == m_isOpenPanel &&  false == CUI_Manager::GetInstance()->Get_ConfirmStore())
	{
		_float2 CursorPos = m_pGameInstance->Get_CursorPos();
		_int index = isInPanelItem(CursorPos);
		if (true == isInPanel(CursorPos))
		{
			if (m_iPreindex != index && -1 != index)
			{
				CUI_Manager::GetInstance()->Set_ChooseItem(isInPanelItem(CursorPos));
				m_iPreindex = isInPanelItem(CursorPos);
			}

			if (1 == CUI_Manager::GetInstance()->Get_ShopItems().size())
			{
				CUI_Manager::GetInstance()->Set_ChooseItem(isInPanelItem(CursorPos));
				m_iPreindex = isInPanelItem(CursorPos);
			}
		}
		else
			return;
	}


	Update_KeyInput(_fTimeDelta);

	
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
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 4.2f, g_iWinSizeY / 12.2f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

		wsprintf(m_tFont, TEXT("선택"));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f - 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f + 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f, g_iWinSizeY - g_iWinSizeY / 9.4f - 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f, g_iWinSizeY - g_iWinSizeY / 9.4f + 2.5f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 3.2f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));


		wsprintf(m_tFont, TEXT("취소"));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f - 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f + 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));


		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
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
			m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 4.25f, g_iWinSizeY - g_iWinSizeY / 2.8f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			wsprintf(m_tFont, TEXT("아니요"));
			m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 4.25f, g_iWinSizeY - g_iWinSizeY / 3.65f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		}
		else if (false == YesorNo)
		{
			wsprintf(m_tFont, TEXT("예"));
			m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 4.25f, g_iWinSizeY - g_iWinSizeY / 2.8f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
			wsprintf(m_tFont, TEXT("아니요"));
			m_pGameInstance->Render_Font(TEXT("Font54"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 4.25f, g_iWinSizeY - g_iWinSizeY / 3.65f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
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

void CShopPanel::Update_KeyInput(_float _fTimeDelta)
{


	// SHOP  패널 오픈 기능
	if (KEY_DOWN(KEY::I) /* && SHOP_END != m_eShopPanel*/)
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


	if (KEY_DOWN(KEY::I) && true == Uimgr->Get_ConfirmStore())
	{
		Uimgr->Set_ConfirmStore(false);
		m_iConfirmItemIndex = 0;
		Uimgr->Set_StoreYesOrno(true);
	}
	if (KEY_DOWN(KEY::ESC) && true == Uimgr->Get_ConfirmStore())
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

		if (KEY_DOWN(KEY::E) && false == Uimgr->Get_ConfirmStore())
		{
			if (-1 != index)
			{
				m_iConfirmItemIndex = index;
				Uimgr->Set_ConfirmStore(true);
				Uimgr->Set_StoreYesOrno(true);
			}
		}
		else if (KEY_DOWN(KEY::E) && true == Uimgr->Get_ConfirmStore())
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






