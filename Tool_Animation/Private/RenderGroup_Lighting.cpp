#include "stdafx.h"
#include "RenderGroup_Lighting.h"
#include "GameInstance.h"

CRenderGroup_Lighting::CRenderGroup_Lighting(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_Lighting::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    return S_OK;
}

HRESULT CRenderGroup_Lighting::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    /* Geometry에서 그린 정보들을 기반으로 조명연산을 수행하는 단계  */
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear)))
        return E_FAIL;

    /* 1. 직교투영 Matrix 정보 전달. */
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    _float4x4 matView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
    _pRTShader->Bind_Matrix("g_CamViewMatrix", &matView);
    _float4x4 matProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
    _pRTShader->Bind_Matrix("g_CamProjMatrix", &matProj);

    //if (FAILED(_pRTShader->Bind_RawValue("g_RandomTexcoords", (_float*)&m_tRandomTexcoord.vRandomTexcoord[0].x, sizeof(_float) * 64 * 2)))
    //    return E_FAIL;

    /* 2. Albedo, Normal, ORMH, Depth 등에 대한 RTV를 바인딩은 이전 과정 >>> RENDER_DIRECTLIGHTS 에서 바인딩했음.*/
     /* 3. 조명연산을 위해 픽셀의 좌표를 월드까지 내리는 과정에서 필요한 데이터 및 카메라의 위치 벡터 */

    /* 3. 환경맵 텍스쳐 바인딩 */

    if (FAILED(m_pGameInstance->Bind_IBLTexture(_pRTShader, "g_BRDFTexture", "g_IBLSpecularTexture", "g_IBLIrradianceTexture")))
    {
        m_pGameInstance->Resolve_RT_MSAA(TEXT("Target_Lighting"));
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        return E_FAIL;
    }

    /* 직접광원 렌더타겟 텍스쳐 바인딩 */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DirectLightsTexture", TEXT("Target_DirectLightAcc"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        return E_FAIL;
    }

    _pRTShader->Begin((_uint)PASS_DEFERRED::LIGHTING);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    m_pGameInstance->Resolve_RT_MSAA(TEXT("Target_Lighting"));
    return S_OK;
}

CRenderGroup_Lighting* CRenderGroup_Lighting::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_Lighting* pInstance = new CRenderGroup_Lighting(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_Lighting");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_Lighting::Free()
{

    __super::Free();
}
