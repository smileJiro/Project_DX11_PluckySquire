#include "Effect.h"
#include "GameInstance.h"


CEffect::CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CEffect::CEffect(const CEffect& _Prototype)
	: CGameObject(_Prototype)
{
    m_pTextureCom = static_cast<CTexture*>(_Prototype.m_pTextureCom->Clone(nullptr));
    m_pParticleBufferCom = static_cast<CVIBuffer_Particle_Point*>(_Prototype.m_pParticleBufferCom->Clone(nullptr));
}


HRESULT CEffect::Initialize_Prototype(const _tchar* _szFilePath)
{
    json jsonEffectInfo;
    if (FAILED(m_pGameInstance->Load_Json(_szFilePath, &jsonEffectInfo)))
        return E_FAIL;

    _string strTexturePath = jsonEffectInfo["Texture"];
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, strTexturePath.c_str(), 1);
    if (nullptr == m_pTextureCom)
        return E_FAIL;

    m_pParticleBufferCom = CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, jsonEffectInfo["Buffer"]);
    if (nullptr == m_pParticleBufferCom)
        return E_FAIL;


    return S_OK;
}

HRESULT CEffect::Initialize(void* _pArg)
{
    // Desc에 대한 내용 ?
    EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(_pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CEffect::Priority_Update(_float _fTimeDelta)
{
}

void CEffect::Update(_float _fTimeDelta)
{
}

void CEffect::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // TODO : PASS 선택
    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pParticleBufferCom->Bind_BufferDesc()))
        return E_FAIL;
    if (FAILED(m_pParticleBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect::Bind_ShaderResources()
{
    // TODO : Local 기준 ? World 기준 ?
    if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect::Ready_Components(const EFFECT_DESC* _pDesc)
{
    if (FAILED(Add_Component(_pDesc->iProtoShaderLevel, _pDesc->szShaderTag,
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    m_Components.emplace(L"Com_Texture", m_pTextureCom);

    m_Components.emplace(L"Com_Buffer", m_pParticleBufferCom);

    return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath)
{
    CEffect* pInstance = new CEffect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_szFilePath)))
    {
        MSG_BOX("Failed to Cloned : CEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect::Clone(void* _pArg)
{
    CEffect* pInstance = new CEffect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect::Free()
{
    Safe_Release(m_pParticleBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CEffect::Cleanup_DeadReferences()
{
	return S_OK;
}
