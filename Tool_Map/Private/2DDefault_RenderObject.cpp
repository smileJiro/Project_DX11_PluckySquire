#include "stdafx.h"
#include "2DDefault_RenderObject.h"

#include "GameInstance.h"


C2DDefault_RenderObject::C2DDefault_RenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

C2DDefault_RenderObject::C2DDefault_RenderObject(const C2DDefault_RenderObject& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT C2DDefault_RenderObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT C2DDefault_RenderObject::Initialize(void* pArg)
{
	DEFAULT_RENDER_OBJECT_DESC* pDesc = nullptr;
	
	DEFAULT_RENDER_OBJECT_DESC Desc = {};
	pDesc = &Desc;

	if (pArg != nullptr)
		pDesc = static_cast<DEFAULT_RENDER_OBJECT_DESC*>(pArg);
	
	_float fBookY = RTSIZE_BOOK2D_Y * ((_float)g_iWinSizeX / (_float)RTSIZE_BOOK2D_X);
	pDesc->fX = g_iWinSizeX >> 1;
	pDesc->fY = (_uint)fBookY >> 1;
	pDesc->fSizeX = g_iWinSizeX;
	pDesc->fSizeY = fBookY;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fTargetSize = { (_float)RTSIZE_BOOK2D_X ,(_float)RTSIZE_BOOK2D_Y };

	return S_OK;
}

void C2DDefault_RenderObject::Priority_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);
	m_isBackColorRender = false;
}

void C2DDefault_RenderObject::Update(_float fTimeDelta)
{

}

void C2DDefault_RenderObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT C2DDefault_RenderObject::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

		//ColorAlpha 3
		//Default 0
	if (!m_isBackColorRender)
	{
		m_isBackColorRender = !m_isBackColorRender;
		m_pShader->Begin(3);
	}
	else
	{
		if (!m_is2DMode)
			return S_OK;
		m_pShader->Begin(0);
	}


	m_pVIBufferCom->Bind_BufferDesc();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT C2DDefault_RenderObject::Ready_Components()
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

HRESULT C2DDefault_RenderObject::Bind_ShaderResources()
{

	Update_RenderWorld();
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (!m_isBackColorRender)
	{ 
		XMStoreFloat4x4(&m_TargetProjMatrix, XMMatrixOrthographicLH((_float)m_fTargetSize.x, (_float)m_fTargetSize.y, 0.0f, 1.0f));
		if (FAILED(m_pShader->Bind_RawValue("g_vColors", &m_fBackColor, sizeof(_float4))))
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_TargetProjMatrix)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
	}
	


	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Book_2D"))))
		return E_FAIL;


	return S_OK;
}

void C2DDefault_RenderObject::Update_RenderWorld()
{
	if (!m_isBackColorRender)
	{
		m_pControllerTransform->Set_Scale(m_fTargetSize.x, m_fTargetSize.y, 1.f);
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fTargetSize.x * 0.5f - m_fTargetSize.x * 0.5f, -(m_fTargetSize.y * 0.5f) + m_fTargetSize.y * 0.5f, 0.f, 1.f));
	}
	else 
	{
		m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - m_fSizeX * 0.5f, -m_fY + m_fSizeY * 0.5f, 0.f, 1.f));

	}


}

C2DDefault_RenderObject* C2DDefault_RenderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	C2DDefault_RenderObject* pInstance = new C2DDefault_RenderObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : C2DDefault_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C2DDefault_RenderObject::Clone(void* pArg)
{
	C2DDefault_RenderObject* pInstance = new C2DDefault_RenderObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : C2DDefault_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DDefault_RenderObject::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBufferCom);
}

HRESULT C2DDefault_RenderObject::Cleanup_DeadReferences()
{
	return S_OK;
}
