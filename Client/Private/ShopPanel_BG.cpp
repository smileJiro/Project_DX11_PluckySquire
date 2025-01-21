#include "stdafx.h"
#include "ShopPanel_BG.h"




CShopPanel_BG::CShopPanel_BG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CShopPanel(_pDevice, _pContext)
{
}

CShopPanel_BG::CShopPanel_BG(const CShopPanel_BG& _Prototype)
	: CShopPanel(_Prototype)
{
}

HRESULT CShopPanel_BG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPanel_BG::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_iTextureCount = pDesc->iTextureCount;
	m_eShopPanel = pDesc->eShopPanelKind;
	m_isRender = false;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CShopPanel_BG::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel_BG::Child_Update(_float _fTimeDelta)
{
	isRender();
}

void CShopPanel_BG::Child_LateUpdate(_float _fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CShopPanel_BG::Render()
{
	if (true == m_isRender)
		__super::Render();

	return S_OK;
}

void CShopPanel_BG::isRender()
{
	if (m_isRender == false)
	{
		m_isRender = true;
	}
	else if (m_isRender == true)
		m_isRender = false;
}

HRESULT CShopPanel_BG::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	switch ((CUI::SHOPPANEL)m_iTextureCount)
	{
		
	case SHOP_BG:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopBG"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case SHOP_DIALOGUEBG:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopDialogue"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	}
	break;

	case SHOP_CHOOSEBG:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopChooseBG"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	}
	break;

	case SHOP_BULB:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ShopBulb"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;


	}
	break;

	case SHOP_BACKESC:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_BACK"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	}
	break;

	case SHOP_BACKARROW:
	{
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ESCBackArrow"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	}
	break;

	case SHOP_ENTER:
	{
		if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_Enter"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

	}
	break;
	}
	
	return S_OK;
}


CShopPanel_BG* CShopPanel_BG::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShopPanel_BG* pInstance = new CShopPanel_BG(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CShopPanel_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShopPanel_BG::Clone(void* _pArg)
{
	CShopPanel_BG* pInstance = new CShopPanel_BG(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CShopPanel_BG Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShopPanel_BG::Free()
{
	__super::Free();
}



HRESULT CShopPanel_BG::Cleanup_DeadReferences()
{
	return S_OK;
}

