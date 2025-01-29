#include "stdafx.h"
#include "UI.h"
//#include "UI_Manager.h"




CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
	
}

CUI::CUI(const CUI& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	return S_OK;
}

void CUI::Priority_Update(_float fTimeDelta)
{
}

void CUI::Update(_float fTimeDelta)
{

}

void CUI::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
	m_pGameInstance->Add_RenderObject_New(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI,this);
}

HRESULT CUI::Render(_int _iTextureindex, PASS_VTXPOSTEX _eShaderPass)
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture", _iTextureindex)))
		return E_FAIL;

	m_pShaderComs[COORDINATE_2D]->Begin((_uint)_eShaderPass);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();


	return S_OK;
}

HRESULT CUI::Ready_Components()
{
	return S_OK;
}

HRESULT CUI::Bind_ShaderResources()
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}


void CUI::Free()
{
	Safe_Release(m_pShaderComs[COORDINATE_2D]);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
	


}
