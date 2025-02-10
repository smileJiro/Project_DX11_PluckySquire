#include "Particle_Sprite_Emitter.h"
#include "GameInstance.h"

#include "Effect_Module.h"
#include "Keyframe_Module.h"
#include "Translation_Module.h"

CParticle_Sprite_Emitter::CParticle_Sprite_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CEmitter(_pDevice, _pContext)
{
    m_eEffectType = SPRITE;
}

CParticle_Sprite_Emitter::CParticle_Sprite_Emitter(const CParticle_Sprite_Emitter& _Prototype)
	: CEmitter(_Prototype)
{
    if (nullptr != _Prototype.m_pTextureCom)
        m_pTextureCom = static_cast<CTexture*>(_Prototype.m_pTextureCom->Clone(nullptr));
    if (nullptr != _Prototype.m_pParticleBufferCom)
    {
        m_pParticleBufferCom = static_cast<CVIBuffer_Point_Particle*>(_Prototype.m_pParticleBufferCom->Clone(nullptr));
    }
}


HRESULT CParticle_Sprite_Emitter::Initialize_Prototype(const json& _jsonInfo)
{
    if (FAILED(__super::Initialize_Prototype(_jsonInfo)))
        return E_FAIL;

    if (false == _jsonInfo.contains("Texture"))
        return E_FAIL;

    _string strTexturePath = _jsonInfo["Texture"];
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, strTexturePath.c_str(), 1);
    if (nullptr == m_pTextureCom)
        return E_FAIL;

#ifdef  _DEBUG
    m_pTextureCom->Add_SRVName(STRINGTOWSTRING(_jsonInfo["Texture"]));
#endif //  _DEBUG


    if (_jsonInfo.contains("ShaderPass"))
    {
        EFFECT_SHADERPASS eShaderPass = _jsonInfo["ShaderPass"];
        m_iShaderPass = eShaderPass;
    }

    if (false == _jsonInfo.contains("Buffer"))
        return E_FAIL;
    m_pParticleBufferCom = CVIBuffer_Point_Particle::Create(m_pDevice, m_pContext, _jsonInfo["Buffer"], m_FloatDatas["SpawnRate"]);
    if (nullptr == m_pParticleBufferCom)
        return E_FAIL;

    for (auto pModule : m_Modules)
    {
        if (false == pModule->Is_Init())
            continue;

        m_pParticleBufferCom->Initialize_Module(pModule);
    }

    return S_OK;
}



HRESULT CParticle_Sprite_Emitter::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;
  

    return S_OK;
}

void CParticle_Sprite_Emitter::Priority_Update(_float _fTimeDelta)
{
}

void CParticle_Sprite_Emitter::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CParticle_Sprite_Emitter::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    if (m_isActive && m_iAccLoop)
        m_pGameInstance->Add_RenderObject_New(s_iRG_3D, s_iRGP_PARTICLE, this);
}

HRESULT CParticle_Sprite_Emitter::Render()
{
    if (nullptr == m_pTextureCom || nullptr == m_pParticleBufferCom || nullptr == m_pShaderCom)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Begin_NoInput(m_iShaderPass)))
        return E_FAIL;
    
    
    //if (FAILED(m_pParticleBufferCom->Bind_BufferDesc()))
    //    return E_FAIL;
    //if (FAILED(m_pParticleBufferCom->Render()))
    //    return E_FAIL;

    if (FAILED(m_pParticleBufferCom->Bind_BufferBySRV()))
        return E_FAIL;
    if (FAILED(m_pParticleBufferCom->Render_BySRV()))
        return E_FAIL;




    return S_OK;
}

void CParticle_Sprite_Emitter::Reset()
{
    if (m_pParticleBufferCom)
        m_pParticleBufferCom->Reset_Buffers(m_pComputeShader);
}


void CParticle_Sprite_Emitter::Update_Emitter(_float _fTimeDelta)
{
    if (nullptr == m_pParticleBufferCom)
        return;

    m_pComputeShader->Bind_RawValue("g_fTimeDelta", &_fTimeDelta, sizeof(_float));


    _float fKill;
    if (KILL == m_ePooling)
    {
        fKill = D3D11_FLOAT32_MAX * -1.f;
    }
    else
    {
        fKill = 0.f;
    }
    m_pComputeShader->Bind_RawValue("g_fKill", &fKill, sizeof(_float));
    m_pComputeShader->Begin(0);
    m_pParticleBufferCom->Begin_Compute(m_pComputeShader);
    m_pParticleBufferCom->Compute(m_pComputeShader);
    m_pComputeShader->End_Compute();

    if (STOP_SPAWN != m_eNowEvent)
    {
        if (SPAWN_RATE == m_eSpawnType)
        {
            _float fAbsolute;
            if (RELATIVE_POSITION == m_eSpawnPosition)
            {
                fAbsolute = 0.f;
                m_pComputeShader->Bind_RawValue("g_fAbsolute", &fAbsolute, sizeof(_float));
            }
            else if (ABSOLUTE_POSITION == m_eSpawnPosition)
            {
                fAbsolute = 1.f;
                m_pComputeShader->Bind_RawValue("g_fAbsolute", &fAbsolute, sizeof(_float));
                m_pComputeShader->Bind_Matrix("g_SpawnMatrix", &m_WorldMatrices[COORDINATE_3D]);
            }
            m_pComputeShader->Begin(1);

        }
        else if (BURST_SPAWN == m_eSpawnType)
        {
            _float fAbsolute;
            if (RELATIVE_POSITION == m_eSpawnPosition)
            {
                fAbsolute = 0.f;
                m_pComputeShader->Bind_RawValue("g_fAbsolute", &fAbsolute, sizeof(_float));
            }
            else if (ABSOLUTE_POSITION == m_eSpawnPosition)
            {
                fAbsolute = 1.f;
                m_pComputeShader->Bind_RawValue("g_fAbsolute", &fAbsolute, sizeof(_float));
                m_pComputeShader->Bind_Matrix("g_SpawnMatrix", &m_WorldMatrices[COORDINATE_3D]);
            }
            m_pComputeShader->Begin(2);

            m_eNowEvent = STOP_SPAWN;
            m_fInactiveDelayTime = m_fActiveTime;
        }
        m_pParticleBufferCom->Begin_Compute(m_pComputeShader);
        m_pParticleBufferCom->Compute(m_pComputeShader);
        m_pComputeShader->End_Compute();
    }

    for (auto pModule : m_Modules)
    {
        if (pModule->Is_Init())
            continue;
        
        m_pParticleBufferCom->Update_Module(pModule, m_pComputeShader);
    }






}

HRESULT CParticle_Sprite_Emitter::Bind_ShaderResources()
{
    if (RELATIVE_POSITION == m_eSpawnPosition)
    {
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_3D])))
            return E_FAIL;
    }
    else if (ABSOLUTE_POSITION == m_eSpawnPosition)
    {
        if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(Bind_ShaderValue_ByPass()))
        return E_FAIL;

    _float4 vLook;
    XMStoreFloat4(&vLook, m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW).r[2]);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLook", &vLook, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Sprite_Emitter::Bind_ShaderValue_ByPass()
{

    if (FAILED(Bind_Float("AlphaTest", "g_AlphaTest")))
        return E_FAIL;
    if (FAILED(Bind_Float("RGBTest", "g_RGBTest")))
        return E_FAIL;

    switch (m_iShaderPass)
    {
    case DEFAULT_BLOOM:
    {
        if (FAILED(Bind_Float("BloomThreshold", "g_fBloomThreshold")))
            return E_FAIL;
        break;
    }        
    case DEFAULT:
    {
        break;
    }
    case ROTATION_BILLBOARD_BLOOM:
    {
        if (FAILED(Bind_Float("BloomThreshold", "g_fBloomThreshold")))
            return E_FAIL;
        break;
    }
    case ROTATION_BILLBOARD:
    {
        break;
    }
    case VELOCITY_BILLBOARD_BLOOM:
    {
        if (FAILED(Bind_Float("BloomThreshold", "g_fBloomThreshold")))
            return E_FAIL;
        break;
    }
    case VELOCITY_BILLBOARD:
    {
        break;
    }
    default:
        break;
    }

    return S_OK;
}

HRESULT CParticle_Sprite_Emitter::Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc)
{
    if (FAILED(__super::Ready_Components(_pDesc)))
        return E_FAIL;

    if (nullptr != m_pTextureCom)
    {
        m_Components.emplace(L"Com_Texture", m_pTextureCom);
        Safe_AddRef(m_pTextureCom);
    }

    if (nullptr != m_pParticleBufferCom)
    {
        m_Components.emplace(L"Com_Buffer", m_pParticleBufferCom);
        Safe_AddRef(m_pParticleBufferCom);        
    }

    return S_OK;
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

    if (ImGui::Button("Reset"))
    {
        if (nullptr != m_pParticleBufferCom)
        {
            m_pParticleBufferCom->Tool_Reset_Buffers(m_FloatDatas["SpawnRate"], m_pComputeShader, m_Modules);
            m_isToolChanged = true;
        }
    }

    if (ImGui::TreeNode("Set Emitter State"))
    {
        __super::Tool_Update(_fTimeDelta);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Modules"))
    {
        if (ImGui::BeginListBox("List"))
        {
            if (ImGui::ArrowButton("Order Up", ImGuiDir::ImGuiDir_Up))
            {
                if (0 < m_iNowModuleIndex && 1 < m_Modules.size())
                {
                    swap(m_Modules[m_iNowModuleIndex], m_Modules[m_iNowModuleIndex - 1]);

                    for (_int i = 0; i < m_Modules.size(); ++i)
                    {
                        m_Modules[i]->Set_Order(i);
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::ArrowButton("Order Down", ImGuiDir::ImGuiDir_Down))
            {
                if (m_Modules.size() - 1 > m_iNowModuleIndex && 1 < m_Modules.size())
                {
                    swap(m_Modules[m_iNowModuleIndex], m_Modules[m_iNowModuleIndex + 1]);

                    for (_int i = 0; i < m_Modules.size(); ++i)
                    {
                        m_Modules[i]->Set_Order(i);
                    }

                }
            }

            int n = 0;
            for (auto& pModule : m_Modules)
            {
                const bool is_selected = (m_iNowModuleIndex == n);
                if (ImGui::Selectable(pModule->Get_TypeName().c_str(), is_selected))
                {
                    m_iNowModuleIndex = is_selected ? -1 : n;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
                ++n;
            }

            if (-1 != m_iNowModuleIndex)
            {
                if (ImGui::Button("Delete Module"))
                {
                    Safe_Release(m_Modules[m_iNowModuleIndex]);

                    auto iter = m_Modules.begin() + m_iNowModuleIndex;
                    m_Modules.erase(iter);

                    m_iNowModuleIndex = -1;
                    m_isToolChanged = true;
                }
            }

            ImGui::EndListBox();

            if (ImGui::TreeNode("Add Keyframe Module"))
            {
                static _int item_selected_idx = 0;
                const char* combo_preview_value = CKeyframe_Module::g_szModuleNames[item_selected_idx];

                if (ImGui::BeginCombo("Keyframe Module", combo_preview_value))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(CKeyframe_Module::g_szModuleNames); n++)
                    {
                        const bool is_selected = (item_selected_idx == n);

                        if (ImGui::Selectable(CKeyframe_Module::g_szModuleNames[n], is_selected))
                        {
                            item_selected_idx = n;

                            CKeyframe_Module* pModule = CKeyframe_Module::Create(m_pDevice, m_pContext, (CKeyframe_Module::MODULE_NAME)n, m_pParticleBufferCom->Get_NumInstance());
                            if (nullptr != pModule)
                            {
                                pModule->Set_Order((_int)m_Modules.size());
                                m_Modules.push_back(pModule);

                                sort(m_Modules.begin(), m_Modules.end(), [](const CEffect_Module* pSrc, const CEffect_Module* pDst)
                                    {
                                        return pSrc->Get_Order() < pDst->Get_Order();
                                    }
                                );
                            }

                            m_isToolChanged = true;
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Add Translation Module"))
            {                
                static _int item_selected_idx = 0;
                const char* combo_preview_value = CTranslation_Module::g_szModuleNames[item_selected_idx];

                if (ImGui::BeginCombo("Module Type", combo_preview_value))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(CTranslation_Module::g_szModuleNames); n++)
                    {
                        const bool is_selected = (item_selected_idx == n);

                        if (ImGui::Selectable(CTranslation_Module::g_szModuleNames[n], is_selected))
                        {
                            item_selected_idx = n;

                            CTranslation_Module* pModule = CTranslation_Module::Create((CTranslation_Module::MODULE_NAME)n, CTranslation_Module::g_szModuleNames[n]);
                            if (nullptr != pModule)
                            {
                                pModule->Set_Order((_int)m_Modules.size());
                                m_Modules.push_back(pModule);

                                sort(m_Modules.begin(), m_Modules.end(), [](const CEffect_Module* pSrc, const CEffect_Module* pDst)
                                    {
                                        return pSrc->Get_Order() < pDst->Get_Order();
                                    }
                                );
                                m_isToolChanged = true;

                            }
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }


                ImGui::TreePop();
            }


            if (-1 != m_iNowModuleIndex && m_iNowModuleIndex < m_Modules.size())
            {
                if (ImGui::TreeNode("Adjust Modules"))
                {
                    m_Modules[m_iNowModuleIndex]->Tool_Module_Update();

                    ImGui::TreePop();
                }
            }
        }



        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Pass"))
    {
        const _char* items[] = { "Default", "DEFAULT_BLOOM", "ROTATION_BILLBOARD", "ROTATION_BILLBOARD_BLOOM", "VELOCITY_BILLBOARD", "VELOCITY_BILLBOARD_BLOOM"};
        static _int item_selected_idx = 0;
        const char* combo_preview_value = items[item_selected_idx];

        if (ImGui::BeginCombo("Shader Type", combo_preview_value))
        {
            item_selected_idx = m_iShaderPass;
            ImGui::SetItemDefaultFocus();

            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_selected_idx == n);

                if (ImGui::Selectable(items[n], is_selected))
                {
                    item_selected_idx = n;
                    if (m_iShaderPass != n)
                    {
                        m_iShaderPass = (EFFECT_SHADERPASS)n;

                    }
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::TreePop();
    }

    if (m_pParticleBufferCom)
    {
        m_pParticleBufferCom->Tool_Update(_fTimeDelta);

        if (false == m_isToolChanged)
            m_isToolChanged = m_pParticleBufferCom->Is_ToolChanged();

        if (false == m_isToolChanged)
        {
            for (auto& pModule : m_Modules)
            {
                if (pModule->Is_Changed())
                {
                    m_isToolChanged = true;
                    break;
                }
            }

            if (m_isToolChanged)
            {
                m_pParticleBufferCom->Tool_Reset_Buffers(m_FloatDatas["SpawnRate"], m_pComputeShader, m_Modules);
            }
        }        
    }
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
            m_pParticleBufferCom = CVIBuffer_Point_Particle::Create(m_pDevice, m_pContext, iNumInstanceInput, m_FloatDatas["SpawnRate"]);

            if (nullptr != m_pParticleBufferCom)
                m_Components.emplace(L"Com_Buffer", m_pParticleBufferCom);
        }
    }

    ImGui::End();

}
HRESULT CParticle_Sprite_Emitter::Save(json& _jsonOut)
{
    if (nullptr == m_pParticleBufferCom || nullptr == m_pTextureCom)
    {
        MSG_BOX("Texture 혹은 Buffer 없음");
        return E_FAIL;
    }

    if (FAILED(__super::Save(_jsonOut)))
        return E_FAIL;

    _jsonOut["Texture"] = WSTRINGTOSTRING(*m_pTextureCom->Get_SRVName(0)).c_str();

    EFFECT_SHADERPASS eShaderPass = (EFFECT_SHADERPASS)m_iShaderPass;
    _jsonOut["ShaderPass"] = eShaderPass;

    json jsonBufferInfo;

    if (FAILED(m_pParticleBufferCom->Save_Buffer(jsonBufferInfo)))
    {
        return E_FAIL;
    }

    _jsonOut["Buffer"] = jsonBufferInfo;

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