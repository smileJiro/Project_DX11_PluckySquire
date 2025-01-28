#include "stdafx.h"
#include "RenderGroup_Final.h"
#include "GameInstance.h"

CRenderGroup_Final::CRenderGroup_Final(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_Final::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_Final::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    /* Diffuse, Normal 등의 정보를 기반으로 Shade RTV를 구성하는 단계 */

       /* 1. shade MRT 세팅 */
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag), m_pDSV, m_isClear)) // 조명별 연산의 결과를 블렌드하며 누적시키는 RTV
        return E_FAIL;

    /* 2. 직교투영 Matrix 정보 전달. */
   
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    /* 픽셀을 월드위치까지 변환하기 위한 역행렬 바인딩 */
    _pRTShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_VIEW));
    _pRTShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_PROJ));

    _pRTShader->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_Shadow_Transform_Ptr(CShadow::D3DTS_VIEW));
    _pRTShader->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_Shadow_Transform_Ptr(CShadow::D3DTS_PROJ));

    /* Diffuse 와 Shade RTV를 바인드하고 이제 이 두 데이터를 가지고 최종 화면을 그려낼 것 이다. */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_ShadeTexture", TEXT("Target_Shade"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_SpecularTexture", TEXT("Target_Specular"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_LightDepthTexture", TEXT("Target_LightDepth"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;

    _pRTShader->Begin((_uint)PASS_DEFERRED::FINAL);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CRenderGroup_Final* CRenderGroup_Final::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_Final* pInstance = new CRenderGroup_Final(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_Final");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_Final::Free()
{

    __super::Free();
}
