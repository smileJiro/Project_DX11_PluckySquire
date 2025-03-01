#include "stdafx.h"
#include "RenderGroup_Combine.h"
#include "GameInstance.h"

CRenderGroup_Combine::CRenderGroup_Combine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_Combine::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_Combine::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    /* 최종 결과물을 만들고 톤매핑 하여 백버퍼 혹은 최종 타겟에 그리는 단계 */
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear)))
        return E_FAIL;

    /* 1. 직교투영 Matrix 정보 전달. */
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    ///* 2. 현재는 라이팅 타겟을 그대로 바인딩하지만 추후에는 뭔가 다른 타겟을 바인딩하겠지 후처리끝난... */
    //if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_LightingTexture", TEXT("Target_Lighting"))))
    //{
    //    if (FAILED(m_pGameInstance->End_MRT()))
    //        return E_FAIL;
    //    return E_FAIL;
    //}
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_PostProcessingTexture", TEXT("Target_PostProcessing"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        return E_FAIL;
    }
    /* 3. Dof 관련 값은 이미 NewRenderer에서 채워줬음. */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DepthTexture", TEXT("Target_Depth"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        return E_FAIL;
    }
    /* 물체 뒤에 숨은 플레이어를 표현하기 위해 추가된 렌더타겟 */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_PlayerDepthTexture", TEXT("Target_PlayerDepth"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        return E_FAIL;
    }

    _pRTShader->Begin((_uint)PASS_DEFERRED::COMBINE);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG

void CRenderGroup_Combine::Update_Imgui()
{
    //ImGui::Begin("Render_Combine");

    //ImGui::SliderFloat("Exposure", &m_fExposure, 0.125f, 8.0f);
    //ImGui::SliderFloat("Gamma", &m_fGamma, 0.125f, 8.0f);

    //ImGui::End();
}
#endif // _DEBUG


CRenderGroup_Combine* CRenderGroup_Combine::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_Combine* pInstance = new CRenderGroup_Combine(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_Combine");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_Combine::Free()
{

    __super::Free();
}
