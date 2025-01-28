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
	
	pDesc->fX = RTSIZE_BOOK2D_X * 0.5f;
	pDesc->fY = RTSIZE_BOOK2D_Y * 0.5f;
	pDesc->fSizeX = (_float)RTSIZE_BOOK2D_X;
	pDesc->fSizeY = (_float)RTSIZE_BOOK2D_Y;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	// 왜 ui클래스 내려서 테스트했지 ㅄ 다시 업데이트해줌 
	m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - m_fSizeX * 0.5f, -m_fY + m_fSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)m_fSizeX, (_float)m_fSizeY, 0.0f, 1.0f));

	return S_OK;
}

void C2DDefault_RenderObject::Priority_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);
	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_BOOK2D, this);
}

void C2DDefault_RenderObject::Update(_float fTimeDelta)
{

}

void C2DDefault_RenderObject::Late_Update(_float fTimeDelta)
{
}

HRESULT C2DDefault_RenderObject::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);

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
	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_SampleMap"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;


}

HRESULT C2DDefault_RenderObject::Bind_ShaderResources()
{

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
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
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

HRESULT C2DDefault_RenderObject::Cleanup_DeadReferences()
{
	return S_OK;
}
