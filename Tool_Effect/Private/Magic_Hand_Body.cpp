#include "pch.h"
#include "Magic_Hand_Body.h"
#include "Controller_Model.h"
#include "GameInstance.h"

CMagic_Hand_Body::CMagic_Hand_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CMagic_Hand_Body::CMagic_Hand_Body(const CMagic_Hand_Body& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CMagic_Hand_Body::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMagic_Hand_Body::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_tFresnels.tInner.vColor = _float4(0.01f, 0.058f, 0.028f, 1.0f);
    m_tFresnels.tInner.fExp = 0.f;
    m_tFresnels.tInner.fBaseReflect = 1.2f;
    m_tFresnels.tInner.fStrength = 0.43f;

    m_tFresnels.tOuter.vColor = _float4(0.107f, 0.28f, 0.1f, 1.f);
    m_tFresnels.tOuter.fExp = 1.5f;
    m_tFresnels.tOuter.fBaseReflect = 0.0f;
    m_tFresnels.tOuter.fStrength = 1.0f;

    m_pGameInstance->CreateConstBuffer(m_tFresnels, D3D11_USAGE_DYNAMIC, &m_pFresnelsBuffer);

    return S_OK;
}

void CMagic_Hand_Body::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CMagic_Hand_Body::Update(_float _fTimeDelta)
{
    ImGui::Begin("Magic Hand Fresenel");

    if (ImGui::TreeNode("Inner"))
    {
        if (ImGui::DragFloat4("Color", (_float*)(&m_tFresnels.tInner.vColor), 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("Exp", &m_tFresnels.tInner.fExp, 1.f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("BaseReflect", &m_tFresnels.tInner.fBaseReflect, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("Strength", &m_tFresnels.tInner.fStrength, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Outer"))
    {
        if (ImGui::DragFloat4("Color", (_float*)(&m_tFresnels.tOuter.vColor), 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("Exp", &m_tFresnels.tOuter.fExp, 1.f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("BaseReflect", &m_tFresnels.tOuter.fBaseReflect, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("Strength", &m_tFresnels.tOuter.fStrength, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnels, m_pFresnelsBuffer);
        }

        ImGui::TreePop();
    }

    ImGui::DragFloat("Specular", &m_fSpecular, 0.01f);
    ImGui::DragFloat("BloomThreshold", &m_fBloomThreshold, 0.01f);


    ImGui::End();
    __super::Update(_fTimeDelta);
}

void CMagic_Hand_Body::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CMagic_Hand_Body::Render()
{
    //__super::Render();
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    
    Bind_ShaderResources();

    m_pShaderComs[COORDINATE_3D]->Bind_ConstBuffer("MultiFresnels", m_pFresnelsBuffer);

    m_pControllerModel->Render_Default(m_pShaderComs[COORDINATE_3D], m_iShaderPasses[COORDINATE_3D]);

    return S_OK;
}


HRESULT CMagic_Hand_Body::Bind_ShaderResources()
{
    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fSpecular", &m_fSpecular, sizeof(_float));
    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fBloomThreshold", &m_fBloomThreshold, sizeof(_float));

    return S_OK;
}

CMagic_Hand_Body* CMagic_Hand_Body::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMagic_Hand_Body* pInstance = new CMagic_Hand_Body(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMagic_Hand_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMagic_Hand_Body::Clone(void* _pArg)
{
    CMagic_Hand_Body* pInstance = new CMagic_Hand_Body(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CMagic_Hand_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMagic_Hand_Body::Free()
{
    Safe_Release(m_pFresnelsBuffer);

    __super::Free();
}


HRESULT CMagic_Hand_Body::Cleanup_DeadReferences()
{
    return S_OK;
}

void CMagic_Hand_Body::Active_OnEnable()
{
}

void CMagic_Hand_Body::Active_OnDisable()
{
}