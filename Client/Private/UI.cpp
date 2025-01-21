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
	__super::Initialize(pArg);

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
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI::Render(_int _index)
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_DiffuseTexture", _index)))
		return E_FAIL;


	m_pShaderComs[COORDINATE_2D]->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();


	return S_OK;
}

HRESULT CUI::Ready_Components()
{
	//if (FAILED(Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Shader_VtxPosTex"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
	//	return E_FAIL;
	//
	///* Com_VIBuffer */
	//if (FAILED(Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	//	return E_FAIL;
	//
	///* Com_Texture */
	//if (FAILED(Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_StartLogo"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

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
