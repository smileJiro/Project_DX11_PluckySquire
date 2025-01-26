#include "Particle_Sprite_Emitter.h"
#include "GameInstance.h"


CParticle_Sprite_Emitter::CParticle_Sprite_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CParticle_Emitter(_pDevice, _pContext)
{
    m_eType = SPRITE;
}

CParticle_Sprite_Emitter::CParticle_Sprite_Emitter(const CParticle_Sprite_Emitter& _Prototype)
	: CParticle_Emitter(_Prototype)
{
    if (nullptr != _Prototype.m_pTextureCom)
        m_pTextureCom = static_cast<CTexture*>(_Prototype.m_pTextureCom->Clone(nullptr));
    if (nullptr != _Prototype.m_pParticleBufferCom)
        m_pParticleBufferCom = static_cast<CVIBuffer_Point_Particle*>(_Prototype.m_pParticleBufferCom->Clone(nullptr));
}


/* 
    Json을 통해서 데이터를 불러옵니다
    Prototype Initialize 상황에서 Texture, Buffer을 같이 로드합니다.
*/
HRESULT CParticle_Sprite_Emitter::Initialize_Prototype(const _tchar* _szFilePath)
{
    json jsonEffectInfo;
    if (false == jsonEffectInfo.contains("Texture"))
        return E_FAIL;

    _string strTexturePath = jsonEffectInfo["Texture"];
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, strTexturePath.c_str(), 1);
    if (nullptr == m_pTextureCom)
        return E_FAIL;


    m_pTextureCom->Add_SRVName(STRINGTOWSTRING(jsonEffectInfo["Texture"]));


    if (false == jsonEffectInfo.contains("Buffer"))
        return E_FAIL;

    m_pParticleBufferCom = CVIBuffer_Point_Particle::Create(m_pDevice, m_pContext, jsonEffectInfo["Buffer"]);
    if (nullptr == m_pParticleBufferCom)
        return E_FAIL;


    return S_OK;
}

HRESULT CParticle_Sprite_Emitter::Initialize_Prototype(const json& _jsonInfo)
{
    if (false == _jsonInfo.contains("Texture"))
        return E_FAIL;

    _string strTexturePath = _jsonInfo["Texture"];
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, strTexturePath.c_str(), 1);
    if (nullptr == m_pTextureCom)
        return E_FAIL;

    m_pTextureCom->Add_SRVName(STRINGTOWSTRING(_jsonInfo["Texture"]));


    if (false == _jsonInfo.contains("Buffer"))
        return E_FAIL;

    m_pParticleBufferCom = CVIBuffer_Point_Particle::Create(m_pDevice, m_pContext, _jsonInfo["Buffer"]);
    if (nullptr == m_pParticleBufferCom)
        return E_FAIL;



    return S_OK;
}



HRESULT CParticle_Sprite_Emitter::Initialize(void* _pArg)
{
    PARTICLE_EMITTER_DESC* pDesc = static_cast<PARTICLE_EMITTER_DESC*>(_pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CParticle_Sprite_Emitter::Priority_Update(_float _fTimeDelta)
{
}

void CParticle_Sprite_Emitter::Update(_float _fTimeDelta)
{
    Update_Component(_fTimeDelta);
}

void CParticle_Sprite_Emitter::Late_Update(_float _fTimeDelta)
{

    m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);
}

HRESULT CParticle_Sprite_Emitter::Render()
{
    if (nullptr == m_pTextureCom || nullptr == m_pParticleBufferCom || nullptr == m_pShaderCom)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // TODO : PASS 선택
    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pParticleBufferCom->Bind_BufferDesc()))
        return E_FAIL;
    if (FAILED(m_pParticleBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Sprite_Emitter::Bind_ShaderResources()
{
    // TODO : Local 기준 ? World 기준 ?
    if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Near", m_pGameInstance->Get_NearZ(), sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Far", m_pGameInstance->Get_FarZ(), sizeof(_float))))
        return E_FAIL;
        


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    _float4 vLook;
    XMStoreFloat4(&vLook, -1.f * m_pGameInstance->Get_CameraVector(CTransform::STATE_LOOK));
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLook", &vLook, sizeof(_float4))))
        return E_FAIL;



    return S_OK;
}

HRESULT CParticle_Sprite_Emitter::Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc)
{
    if (FAILED(__super::Ready_Components(_pDesc)))
        return E_FAIL;

    if (nullptr != m_pTextureCom)
        m_Components.emplace(L"Com_Texture", m_pTextureCom);

    if (nullptr != m_pParticleBufferCom)
        m_Components.emplace(L"Com_Buffer", m_pParticleBufferCom);

    return S_OK;
}

CParticle_Sprite_Emitter* CParticle_Sprite_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath)
{
    CParticle_Sprite_Emitter* pInstance = new CParticle_Sprite_Emitter(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_szFilePath)))
    {
        MSG_BOX("Failed to Cloned : CEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CParticle_Sprite_Emitter* CParticle_Sprite_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonInfo)
{
    CParticle_Sprite_Emitter* pInstance = new CParticle_Sprite_Emitter(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_jsonInfo)))
    {
        MSG_BOX("Failed to Cloned : CParticle_Sprite_Emitter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Sprite_Emitter::Clone(void* _pArg)
{
    CParticle_Sprite_Emitter* pInstance = new CParticle_Sprite_Emitter(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle_Sprite_Emitter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Sprite_Emitter::Free()
{
    Safe_Release(m_pParticleBufferCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CParticle_Sprite_Emitter::Cleanup_DeadReferences()
{
	return S_OK;
}

#ifdef _DEBUG
void CParticle_Sprite_Emitter::Tool_Setting()
{
    if (m_pParticleBufferCom)
        m_pParticleBufferCom->Tool_Setting();
}
void CParticle_Sprite_Emitter::Tool_Update(_float _fTimeDelta)
{
    ImGui::Begin("Adjust_Sprite_Emitter");
    


    if (m_pParticleBufferCom)
        m_pParticleBufferCom->Tool_Update(_fTimeDelta);
    else
    {
        static _int iNumInstanceInput = 1;
        
        if (ImGui::InputInt("Instance Count", &iNumInstanceInput))
        {
            if (0 >= iNumInstanceInput)
            {
                iNumInstanceInput = 1;
            }
        }

        if (ImGui::Button("Create_Buffer"))
        {
            m_pParticleBufferCom = CVIBuffer_Point_Particle::Create(m_pDevice, m_pContext, iNumInstanceInput);

            if (nullptr != m_pParticleBufferCom)
                m_Components.emplace(L"Com_Buffer", m_pParticleBufferCom);
        }

    }


    ImGui::End();

}
HRESULT CParticle_Sprite_Emitter::Save(json& _jsonOut)
{
    _jsonOut["Type"] = SPRITE;
    _jsonOut["Texture"] = WSTRINGTOSTRING(*m_pTextureCom->Get_SRVName(0)).c_str();
    
    // TODO :


    return S_OK;
}
void CParticle_Sprite_Emitter::Set_Texture(CTexture* _pTextureCom)
{
    if (m_pTextureCom)
    {
        Safe_Release(m_pTextureCom);
    }
       
    m_pTextureCom = _pTextureCom;
    m_Components[L"Com_Texture"] = m_pTextureCom;
    Safe_AddRef(m_pTextureCom);

}
CParticle_Sprite_Emitter* CParticle_Sprite_Emitter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CParticle_Sprite_Emitter* pInstance = new CParticle_Sprite_Emitter(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle_Sprite_Emitter");
        Safe_Release(pInstance);
    }

    return pInstance;
}

#endif