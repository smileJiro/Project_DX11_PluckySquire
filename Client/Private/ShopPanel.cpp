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
	//if (KEY_DOWN(KEY::I) && SHOP_END == m_eShopPanel)
	//{
	//	isRender();
	//	
	//}

	if (KEY_DOWN(KEY::I) /* && SHOP_END != m_eShopPanel*/)
	{
		if (true == CUI_Manager::GetInstance()->Get_isESC())
			return;

		isFontPrint();

		for (auto iter : CUI_Manager::GetInstance()->Get_ShopPanels())
		{
			if (SHOP_END != iter.second->Get_ShopPanel())
			{
				iter.second->Child_Update(_fTimeDelta);
			}
		}


		for (auto iter : CUI_Manager::GetInstance()->Get_ShopItems())
		{
			for (size_t i = 0; i < 3; ++i)
			{
				iter.second[i]->Child_Update(_fTimeDelta);
			}


		}


	}
}

void CShopPanel::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender && SHOP_CHOOSEBG != m_eShopPanel)
	{
		for (auto iter : CUI_Manager::GetInstance()->Get_ShopPanels())
		{
			iter.second->Child_LateUpdate(_fTimeDelta);
		}


		for (auto iter : CUI_Manager::GetInstance()->Get_ShopItems())
		{
			for (size_t i = 0; i < 3; ++i)
			{
				iter.second[i]->Child_LateUpdate(_fTimeDelta);
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


		//g_iWinSizeX / 3.125f;
		//g_iWinSizeY - g_iWinSizeY / 13.f

	}





	if (true == m_isRender)
		__super::Render(_iTextureindex, _eShaderPass);


	return S_OK;
}



void CShopPanel::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

void CShopPanel::isFontPrint()
{
	if (false == m_isOpenPanel)
	{
		m_isOpenPanel = true;
	}
	else if (true == m_isOpenPanel)
	{
		m_isOpenPanel = false;
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






