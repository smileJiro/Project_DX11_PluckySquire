#include "stdafx.h"
#include "RenderGroup_AfterEffect.h"
#include "GameInstance.h"

CRenderGroup_AfterEffect::CRenderGroup_AfterEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_AfterEffect::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderGroup_AfterEffect::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());

    // 파이널(가제)타겟 + 지금까지 그려진 이펙트를 합쳐서 그려낸다.
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_FinalTexture", TEXT("Target_Combine"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_EffectColorTexture", TEXT("Target_EffectColor"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_AccumulateTexture", TEXT("Target_ParticleAccumulate"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_RevealageTexture", TEXT("Target_ParticleRevelage"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_BloomTexture", TEXT("Target_DownBlur1"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_BloomTexture2", TEXT("Target_DownBlur2"))))
        return E_FAIL;

    _pRTShader->Begin((_uint)PASS_DEFERRED::AFTER_EFFECT);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    return S_OK;
}

CRenderGroup_AfterEffect* CRenderGroup_AfterEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_AfterEffect* pInstance = new CRenderGroup_AfterEffect(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_AfterEffect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_AfterEffect::Free()
{

    __super::Free();
}
