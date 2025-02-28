#include "pch.h"
#include "ETool_RenderGroup_Final.h"
#include "GameInstance.h"

CETool_RenderGroup_Final::CETool_RenderGroup_Final(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CETool_RenderGroup_Final::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CETool_RenderGroup_Final::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear))) 
        return E_FAIL;

    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    _pRTShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_VIEW));
    _pRTShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_PROJ));

    //_pRTShader->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_Shadow_Transform_Ptr(CShadow::D3DTS_VIEW));
    //_pRTShader->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_Shadow_Transform_Ptr(CShadow::D3DTS_PROJ));

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_Albedo", TEXT("Target_Diffuse"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_ShadeTexture", TEXT("Target_Shade"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_SpecularTexture", TEXT("Target_Specular"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;

    _pRTShader->Begin((_uint)PASS_DEFERRED::COMBINE);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CETool_RenderGroup_Final* CETool_RenderGroup_Final::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CETool_RenderGroup_Final* pInstance = new CETool_RenderGroup_Final(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CETool_RenderGroup_Final");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CETool_RenderGroup_Final::Free()
{
    __super::Free();
}
