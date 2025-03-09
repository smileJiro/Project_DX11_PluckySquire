#include "stdafx.h"
#include "RenderGroup_Final.h"
#include "GameInstance.h"

CRenderGroup_Final::CRenderGroup_Final(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup(_pDevice, _pContext)
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
    /* 2. 직교투영 Matrix 정보 전달. */
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    /* Diffuse 와 Shade RTV를 바인드하고 이제 이 두 데이터를 가지고 최종 화면을 그려낼 것 이다. */
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_FinalTexture", TEXT("Target_Final"))))
        return E_FAIL;

    _pRTShader->Begin((_uint)PASS_DEFERRED::FINAL);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();


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
