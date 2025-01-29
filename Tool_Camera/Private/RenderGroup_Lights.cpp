#include "stdafx.h"
#include "RenderGroup_Lights.h"
#include "GameInstance.h"

CRenderGroup_Lights::CRenderGroup_Lights(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_Lights::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_Lights::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    /* Diffuse, Normal 등의 정보를 기반으로 Shade RTV를 구성하는 단계 */

       /* 1. shade MRT 세팅 */
        // 0128 박예슬 : 오타같아서 고쳐놓겠어요(깃 이력 참조)
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear))) // 조명별 연산의 결과를 블렌드하며 누적시키는 RTV
        return E_FAIL;

    /* 2. 직교투영 Matrix 정보 전달. */
   
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    /* 3. 조명연산을 위해 픽셀의 좌표를 월드까지 내리는 과정에서 필요한 데이터 및 카메라의 위치 벡터 */
    _float4x4 viewMatrixInv = m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjMatrixInv = m_pGameInstance->Get_TransformInverseFloat4x4(CPipeLine::D3DTS_PROJ);
    _pRTShader->Bind_Matrix("g_ViewMatrixInv", &viewMatrixInv);
    _pRTShader->Bind_Matrix("g_ProjMatrixInv", &ProjMatrixInv);
    _pRTShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
    _pRTShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZ(), sizeof(_float));

    /* 4. Normal Texture Bind */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_NormalTexture", TEXT("Target_Normal"))))
        return E_FAIL;
    /* 5. Depth Texture Bind */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_DepthTexture", TEXT("Target_Depth"))))
        return E_FAIL;

    /* 4. 사각형 정점 정보 바인딩 */
    _pRTBuffer->Bind_BufferDesc();

    if (FAILED(m_pGameInstance->Render_Lights(_pRTShader, _pRTBuffer)))
        return E_FAIL;

    /* 5. 렌더 종료 시, 기존 Back Buffer RTV로 RTV 변경. */
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CRenderGroup_Lights* CRenderGroup_Lights::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_Lights* pInstance = new CRenderGroup_Lights(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_Lights");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_Lights::Free()
{

    __super::Free();
}
