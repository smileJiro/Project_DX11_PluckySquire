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

	m_iPanelIndex = 0;

	return S_OK;
}

void CSettingPanel::Priority_Update(_float _fTimeDelta)
{
}

void CSettingPanel::Update(_float _fTimeDelta)
{

	
	
	if (true == m_isOpenPanel)
	{
		Move_Menu();

		if (KEY_DOWN(KEY::E))
		{
			Choose_Menu(_fTimeDelta);
		}
	}

	if (KEY_DOWN(KEY::ESC))
	{
		AllIcon_OpenClose(_fTimeDelta);
	}
}

void CSettingPanel::Late_Update(_float _fTimeDelta)
{
	if (true == m_isRender &&  SETTING_HEART != m_eSettingPanel && SETTING_ESCGOBLIN != m_eSettingPanel)
	{
		for (auto iter : CUI_Manager::GetInstance()->Get_SettingPanels())
		{
			iter.second->Child_LateUpdate(_fTimeDelta);
		}
	}
	else if (true == m_isRender && SETTING_HEART == m_eSettingPanel || SETTING_ESCGOBLIN == m_eSettingPanel  )
	{
		__super::Late_Update(_fTimeDelta);
	}
}



HRESULT CSettingPanel::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	if (true == m_isRender && SETTING_ESCGOBLIN != m_eSettingPanel)
	{
		wsprintf(m_tFont, TEXT("확인"));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f - 2.5f, g_iWinSizeY - g_iWinSizeY / 11.8f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f + 2.5f, g_iWinSizeY - g_iWinSizeY / 11.8f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f, g_iWinSizeY - g_iWinSizeY / 11.8f - 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f, g_iWinSizeY - g_iWinSizeY / 11.8f + 2.5f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX - g_iWinSizeX / 12.f, g_iWinSizeY - g_iWinSizeY / 11.8f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));

		__super::Render(_iTextureindex, _eShaderPass);
		Menu_Text();
	}
	else if (true == m_isRender && SETTING_ESCGOBLIN == m_eSettingPanel)
	{
		__super::Render();
	}

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
		CUI_Manager::GetInstance()->Set_SettingPanelIndex(0);
	}
}

void CSettingPanel::Move_Menu()
{
	_int iPanelIndex = CUI_Manager::GetInstance()->Get_SettingPanelIndex();

	if (KEY_DOWN(KEY::DOWN))
	{
		if (4 != iPanelIndex)
		{
			iPanelIndex += 1;
			CUI_Manager::GetInstance()->Set_SettingPanelIndex(iPanelIndex);
		}
		else
		{
			return;
		}
	}
	else if (KEY_DOWN(KEY::UP))
	{
		if (0 != iPanelIndex)
		{
			iPanelIndex -= 1;
			CUI_Manager::GetInstance()->Set_SettingPanelIndex(iPanelIndex);
		}
		else
		{
			return;
		}
	}
}

void CSettingPanel::Choose_Menu(_float _fTimeDelta)
{
	switch (CUI_Manager::GetInstance()->Get_SettingPanelIndex())
	{
	case 0:
	{
		for (auto iter : CUI_Manager::GetInstance()->Get_SettingPanels())
		{
			iter.second->Child_Update(_fTimeDelta);
		}


		
		isRender();
		isFontPrint();

	}
	break;

	case 1:
	{
		break;
	}
	break;

	case 2:
	{
		break;
	}
	break;

	case 3:
	{
		
	}
	break;

	case 4:
	{

	}
	break;

	}

}

void CSettingPanel::Menu_Text()
{
	if (CSettingPanel::m_isOpenPanel == true)
	{
		_int iPanelIndex = CUI_Manager::GetInstance()->Get_SettingPanelIndex();

		wsprintf(m_tFont, TEXT("제5장 - 아르티아!"));
		m_pGameInstance->Render_Font(TEXT("Font38"), m_tFont, _float2(g_iWinSizeX / 2.f - 150.f, g_iWinSizeY / 8 + 70.f), XMVectorSet(0.f, 0.f, 0.f, 1.0f));

		if (0 == iPanelIndex)
		{
			wsprintf(m_tFont, TEXT("계속하기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 80.f, g_iWinSizeY / 2.8f), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			wsprintf(m_tFont, TEXT("미니게임 건너뛰기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 160.f, g_iWinSizeY / 2.3f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("옵션"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.95f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("종료"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.7f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
		}
		else if (1 == iPanelIndex)
		{
			wsprintf(m_tFont, TEXT("계속하기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 80.f, g_iWinSizeY / 2.8f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("미니게임 건너뛰기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 160.f, g_iWinSizeY / 2.3f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			wsprintf(m_tFont, TEXT("옵션"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.95f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("종료"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.7f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
		}
		else if (2 == iPanelIndex)
		{
			wsprintf(m_tFont, TEXT("계속하기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 80.f, g_iWinSizeY / 2.8f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("미니게임 건너뛰기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 160.f, g_iWinSizeY / 2.3f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("옵션"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.95f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			wsprintf(m_tFont, TEXT("종료"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.7f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
		}
		else if (3 == iPanelIndex)
		{
			wsprintf(m_tFont, TEXT("계속하기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 80.f, g_iWinSizeY / 2.8f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("미니게임 건너뛰기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 160.f, g_iWinSizeY / 2.3f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("옵션"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.95f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("종료"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.7f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
		else if (4 == iPanelIndex)
		{
			wsprintf(m_tFont, TEXT("계속하기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 80.f, g_iWinSizeY / 2.8f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("미니게임 건너뛰기"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 160.f, g_iWinSizeY / 2.3f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("옵션"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.95f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
			wsprintf(m_tFont, TEXT("종료"));
			m_pGameInstance->Render_Font(TEXT("Font40"), m_tFont, _float2(g_iWinSizeX / 2.f - 50.f, g_iWinSizeY / 1.7f), XMVectorSet(0.f, 0.f, 0.f, 1.f));
		}
	}
}

void CSettingPanel::AllIcon_OpenClose(_float _fTimeDelta)
{
	if (SETTING_HEART != m_eSettingPanel && SETTING_ESCGOBLIN != m_eSettingPanel)
	{
		for (auto iter : CUI_Manager::GetInstance()->Get_SettingPanels())
		{
			iter.second->Child_Update(_fTimeDelta);
		}
	}
	else if (SETTING_HEART == m_eSettingPanel)
	{
		isRender();
		isFontPrint();
	}
	else if (SETTING_ESCGOBLIN == m_eSettingPanel)
	{
		isRender();
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






