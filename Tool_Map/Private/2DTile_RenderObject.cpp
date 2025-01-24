#include "stdafx.h"
#include "2DTile_RenderObject.h"

#include "GameInstance.h"


C2DTile_RenderObject::C2DTile_RenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

C2DTile_RenderObject::C2DTile_RenderObject(const C2DTile_RenderObject& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT C2DTile_RenderObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT C2DTile_RenderObject::Initialize(void* pArg)
{
	DEFAULT_RENDER_OBJECT_DESC* pDesc = nullptr;

	DEFAULT_RENDER_OBJECT_DESC Desc = {};
	pDesc = &Desc;

	if (pArg != nullptr)
		pDesc = static_cast<DEFAULT_RENDER_OBJECT_DESC*>(pArg);

	pDesc->fX = g_iWinSizeX >> 1;
	pDesc->fY = g_iWinSizeY >> 1;
	pDesc->fSizeX = g_iWinSizeX;
	pDesc->fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void C2DTile_RenderObject::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void C2DTile_RenderObject::Update(_float fTimeDelta)
{

}

void C2DTile_RenderObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT C2DTile_RenderObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);

	m_pVIBufferCom->Bind_BufferDesc();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT C2DTile_RenderObject::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	return S_OK;


}

HRESULT C2DTile_RenderObject::Bind_ShaderResources()
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Book_2D"))))
		return E_FAIL;


	return S_OK;
}

C2DTile_RenderObject* C2DTile_RenderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	C2DTile_RenderObject* pInstance = new C2DTile_RenderObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : C2DTile_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C2DTile_RenderObject::Clone(void* pArg)
{
	C2DTile_RenderObject* pInstance = new C2DTile_RenderObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : C2DTile_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DTile_RenderObject::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBufferCom);
}

HRESULT C2DTile_RenderObject::Cleanup_DeadReferences()
{
	return S_OK;
}
