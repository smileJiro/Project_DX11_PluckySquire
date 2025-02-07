#include "pch.h"
#include "RenderGroup_Blur.h"
#include "GameInstance.h"

CRenderGroup_Blur::CRenderGroup_Blur(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_Blur::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	RGMRT_BLUR_DESC* pDesc = static_cast<RGMRT_BLUR_DESC*>(_pArg);

	m_pDSV1 = pDesc->pDSV1;
	Safe_AddRef(m_pDSV1);


	return S_OK;
}

HRESULT CRenderGroup_Blur::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur1"), m_pDSV, true)))
        return E_FAIL;

    Setup_Viewport(_float2(g_iWinSizeX / 6.f, g_iWinSizeY / 6.f));

    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    _float2 vTexelSize;
    vTexelSize = { 6.f / g_iWinSizeX, 6.f / g_iWinSizeY };
    _pRTShader->Bind_RawValue("g_TexelSize", &vTexelSize, sizeof(_float2));

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_BloomTexture", TEXT("Target_DownSample1"))))
        return E_FAIL;

    _pRTShader->Begin((_uint)PASS_DEFERRED::BLUR);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur2"), m_pDSV1, true)))
        return E_FAIL;

    Setup_Viewport(_float2(g_iWinSizeX / 24.f, g_iWinSizeY / 24.f));

    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    vTexelSize = { 24.f / g_iWinSizeX, 24.f / g_iWinSizeY };
    _pRTShader->Bind_RawValue("g_TexelSize", &vTexelSize, sizeof(_float2));

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_BloomTexture", TEXT("Target_DownSample2"))))
        return E_FAIL;

    _pRTShader->Begin((_uint)PASS_DEFERRED::BLUR);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;


    Setup_Viewport(_float2((_float)m_pGameInstance->Get_ViewportWidth(), (_float)m_pGameInstance->Get_ViewportHeight()));

    return S_OK;
}

CRenderGroup_Blur* CRenderGroup_Blur::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_Blur* pInstance = new CRenderGroup_Blur(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_Blur");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_Blur::Free()
{
	Safe_Release(m_pDSV1);

	__super::Free();
}
