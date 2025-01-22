#include "stdafx.h"
#include "SettingPanel.h"
#include "UI_Manager.h"
#include "SettingPanelBG.h"

CSettingPanel::CSettingPanel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CSettingPanel::CSettingPanel(const CSettingPanel& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CSettingPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSettingPanel::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	return S_OK;
}

void CSettingPanel::Priority_Update(_float _fTimeDelta)
{
}

void CSettingPanel::Update(_float _fTimeDelta)
{
	//if (KEY_DOWN(KEY::ESC) && SETTING_HEART == m_eSettingPanel)
	//{
	//	isRender();
	//}
	//else if (KEY_DOWN(KEY::ESC) && SETTING_HEART != m_eSettingPanel)
	//{
	//	for (auto iter : CUI_Manager::GetInstance()->Get_SettingPanels())
	//	{
	//		if (SETTING_HEART != iter.second->Get_SettingPanel())
	//		{
	//			iter.second->Child_Update(_fTimeDelta);
	//		}
	//	}
	//
	//
	//}


	if (KEY_DOWN(KEY::ESC))
	{

		if (SETTING_HEART != m_eSettingPanel)
		{
			for (auto iter : CUI_Manager::GetInstance()->Get_SettingPanels())
			{
				//if (iter.second->Get_SettingPanel())
				//{
					iter.second->Child_Update(_fTimeDelta);
				//}
			}
		}
		else if (SETTING_HEART == m_eSettingPanel)
		{
			isRender();
			isFontPrint();
		}
		
		
		

		

	}


}

void CSettingPanel::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender &&  SETTING_HEART != m_eSettingPanel)
	{
		for (auto iter : CUI_Manager::GetInstance()->Get_SettingPanels())
		{
			iter.second->Child_LateUpdate(_fTimeDelta);
		}
	}
	else if (true == m_isRender && SETTING_HEART == m_eSettingPanel)
	{
		__super::Late_Update(_fTimeDelta);
	}

}



HRESULT CSettingPanel::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	if (true == m_isRender)
	{
		wsprintf(m_tFont, TEXT("È®ÀÎ"));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f - 2.5f, g_iWinSizeY - g_iWinSizeY / 11.8f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f + 2.5f, g_iWinSizeY - g_iWinSizeY / 11.8f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f, g_iWinSizeY - g_iWinSizeY / 11.8f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f, g_iWinSizeY - g_iWinSizeY / 11.8f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));


		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f, g_iWinSizeY - g_iWinSizeY / 11.8f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		__super::Render(_iTextureindex, _eShaderPass);
	}
		


	/*

		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f - 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f + 2.5f, g_iWinSizeY - g_iWinSizeY / 9.4f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 3.345f, g_iWinSizeY - g_iWinSizeY / 9.4f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
	
	*/



	return S_OK;
}



void CSettingPanel::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

void CSettingPanel::isFontPrint()
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


HRESULT CSettingPanel::Ready_Components()
{
	return S_OK;
}


CSettingPanel* CSettingPanel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSettingPanel* pInstance = new CSettingPanel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CSettingPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSettingPanel::Clone(void* _pArg)
{
	CSettingPanel* pInstance = new CSettingPanel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CSettingPanel Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSettingPanel::Free()
{
	__super::Free();
}






