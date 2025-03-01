#include "stdafx.h"
#include "RenderGroup_DirectLights.h"
#include "GameInstance.h"

CRenderGroup_DirectLights::CRenderGroup_DirectLights(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_DirectLights::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_DirectLights::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    /* Diffuse, Normal 등의 정보를 기반으로 Shade RTV를 구성하는 단계 */

    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear))) // 조명별 연산의 결과를 블렌드하며 누적시키는 RTV
        return E_FAIL;

    /* 2. 직교투영 Matrix 정보 전달. */
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    /* 3. 조명연산을 위해 픽셀의 좌표를 월드까지 내리는 과정에서 필요한 데이터 및 카메라의 위치 벡터 */
    _pRTShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_VIEW));
    _pRTShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_PROJ));

    _pRTShader->Bind_RawValue("g_vCamWorld", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
    _float f = *m_pGameInstance->Get_FarZ();
    _pRTShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZ(), sizeof(_float));

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_AlbedoTexture", TEXT("Target_Albedo"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        return E_FAIL;
    }
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_NormalTexture", TEXT("Target_Normal"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        return E_FAIL;
    }
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_ORMHTexture", TEXT("Target_ORMH"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;
        return E_FAIL;
    }
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DepthTexture", TEXT("Target_Depth"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        return E_FAIL;
    }
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_EtcTexture", TEXT("Target_Etc"))))
    {
        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        return E_FAIL;
    }

    /* 4. 사각형 정점 정보 바인딩 */
    _pRTBuffer->Bind_BufferDesc();
    
    if (FAILED(m_pGameInstance->Render_Lights(_pRTShader, _pRTBuffer)))
        return E_FAIL;

    /* 5. 렌더 종료 시, 기존 Back Buffer RTV로 RTV 변경. */
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CRenderGroup_DirectLights* CRenderGroup_DirectLights::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_DirectLights* pInstance = new CRenderGroup_DirectLights(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_DirectLights");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_DirectLights::Free()
{

    __super::Free();
}
