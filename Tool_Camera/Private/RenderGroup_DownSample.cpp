#include "stdafx.h"
#include "RenderGroup_DownSample.h"
#include "GameInstance.h"

CRenderGroup_DownSample::CRenderGroup_DownSample(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_DownSample::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;


	m_pLowerDSV = m_pGameInstance->Find_DSV(TEXT("DSV_Downsample2"));
	Safe_AddRef(m_pLowerDSV);

	return S_OK;
}


HRESULT CRenderGroup_DownSample::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DownSample1"), m_pDSV, true)))
        return E_FAIL;

    Setup_Viewport(_float2(g_iWinSizeX / 6.f, g_iWinSizeY / 6.f));

    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    _float2 vTexelSize;
    vTexelSize = { 1.f / g_iWinSizeX, 1.f / g_iWinSizeY };
    _pRTShader->Bind_RawValue("g_TexelSize", &vTexelSize, sizeof(_float2));

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_BloomTexture", TEXT("Target_Bloom"))))
        return E_FAIL;

    _pRTShader->Begin((_uint)PASS_DEFERRED::BLURDOWN2);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DownSample2"), m_pLowerDSV, true)))
        return E_FAIL;

    Setup_Viewport(_float2(g_iWinSizeX / 24.f, g_iWinSizeY / 24.f));

    vTexelSize = { 6.f / g_iWinSizeX, 6.f / g_iWinSizeY };
    _pRTShader->Bind_RawValue("g_TexelSize", &vTexelSize, sizeof(_float2));

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_BloomTexture", TEXT("Target_DownSample1"))))
        return E_FAIL;


    _pRTShader->Begin((_uint)PASS_DEFERRED::BLURDOWN);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;


    Setup_Viewport(_float2((_float)m_pGameInstance->Get_ViewportWidth(), (_float)m_pGameInstance->Get_ViewportHeight()));

    return S_OK;
}

CRenderGroup_DownSample* CRenderGroup_DownSample::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_DownSample* pInstance = new CRenderGroup_DownSample(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_DownSample");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_DownSample::Free()
{
    Safe_Release(m_pLowerDSV);

    __super::Free();
}
