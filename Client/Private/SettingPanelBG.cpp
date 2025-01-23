#include "stdafx.h"
#include "SettingPanelBG.h"





CSettingPanelBG::CSettingPanelBG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSettingPanel(_pDevice, _pContext)
{
}

CSettingPanelBG::CSettingPanelBG(const CSettingPanelBG& _Prototype)
	: CSettingPanel(_Prototype)
{
}

HRESULT CSettingPanelBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSettingPanelBG::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_iTextureCount = pDesc->iTextureCount;
	m_eSettingPanel = pDesc->eSettingPanelKind;
	m_isRender = false;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CSettingPanelBG::Priority_Update(_float _fTimeDelta)
{
}

void CSettingPanelBG::Child_Update(_float _fTimeDelta)
{
	isRender();
}

void CSettingPanelBG::Child_LateUpdate(_float _fTimeDelta)
{
	if (true == m_isRender)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CSettingPanelBG::Render()
{
	if (true == m_isRender)
		__super::Render(0, PASS_VTXPOSTEX::DEFAULT);

	return S_OK;
}

void CSettingPanelBG::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

HRESULT CSettingPanelBG::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */

	switch ((CUI::SETTINGPANEL)m_iTextureCount)
	{
	case SETTING_BG :
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OptionBG"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case SETTING_BULB:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBulb"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case SETTING_BACKESC:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBack"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case SETTING_BACKARROW:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBackArrow"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case SETTING_ESCENTER:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCEnter"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SETTING_ENTERBG:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IconBG"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;
	case SETTING_ESCBG:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IconBG"),
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	
	break;
	}


	return S_OK;
}


CSettingPanelBG* CSettingPanelBG::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSettingPanelBG* pInstance = new CSettingPanelBG(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CSettingPanelBG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSettingPanelBG::Clone(void* _pArg)
{
	CSettingPanelBG* pInstance = new CSettingPanelBG(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CSettingPanelBG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSettingPanelBG::Free()
{


	__super::Free();
}

void CSettingPanelBG::Update(_float _fTimeDelta)
{
}

void CSettingPanelBG::Late_Update(_float _fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CSettingPanelBG::Cleanup_DeadReferences()
{
	return S_OK;
}

