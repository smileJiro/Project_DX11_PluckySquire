#include "pch.h"
#include "ETool_RenderGroup_Lights.h"
#include "GameInstance.h"

CETool_RenderGroup_Lights::CETool_RenderGroup_Lights(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CETool_RenderGroup_Lights::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CETool_RenderGroup_Lights::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{

    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear))) // 조명별 연산의 결과를 블렌드하며 누적시키는 RTV
        return E_FAIL;


    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    _pRTShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_VIEW));
    _pRTShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_PROJ));
    _pRTShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
    _pRTShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZ(), sizeof(_float));

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_NormalTexture", TEXT("Target_Normal"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;

    _pRTBuffer->Bind_BufferDesc();

    if (FAILED(m_pGameInstance->Render_Lights(_pRTShader, _pRTBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CETool_RenderGroup_Lights* CETool_RenderGroup_Lights::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CETool_RenderGroup_Lights* pInstance = new CETool_RenderGroup_Lights(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CETool_RenderGroup_Lights");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CETool_RenderGroup_Lights::Free()
{
    __super::Free();
}
