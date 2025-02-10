#include "pch.h"
#include "ETool_RenderGroup_AfterParticle.h"
#include "GameInstance.h"

CETool_RenderGroup_AfterParticle::CETool_RenderGroup_AfterParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CETool_RenderGroup_AfterParticle::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CETool_RenderGroup_AfterParticle::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
    if (FAILED(m_pGameInstance->Begin_MRT(m_strMRTTag, m_pDSV, m_isClear)))
        return E_FAIL;

    _pRTShader->Bind_Matrix("g_WorldMatrix", m_pGameInstance->Get_WorldMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ViewMatrix_Renderer());
    _pRTShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ProjMatrix_Renderer());


    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_RevealageTexture", TEXT("Target_ParticleRevelage"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(_pRTShader, "g_BloomTexture2", TEXT("Target_ParticleBloom"))))
        return E_FAIL;


    _pRTShader->Begin((_uint)PASS_DEFERRED::AFTER_PARTICLE);

    _pRTBuffer->Bind_BufferDesc();

    _pRTBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

CETool_RenderGroup_AfterParticle* CETool_RenderGroup_AfterParticle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CETool_RenderGroup_AfterParticle* pInstance = new CETool_RenderGroup_AfterParticle(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CETool_RenderGroup_AfterParticle");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CETool_RenderGroup_AfterParticle::Free()
{

    __super::Free();
}
