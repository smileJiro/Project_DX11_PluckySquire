#include "stdafx.h"
#include "ShopPanel.h"

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

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CShopPanel::Priority_Update(_float _fTimeDelta)
{
}

void CShopPanel::Update(_float _fTimeDelta)
{

	if (KEY_DOWN(KEY::ESC))
	{
		isRender();
	}

}

void CShopPanel::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CShopPanel::Render(_int _index)
{
	if (true == m_isRender)
		__super::Render(_index);

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

HRESULT CShopPanel::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_OptionBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


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

HRESULT CShopPanel::Cleanup_DeadReferences()
{
	return S_OK;
}

