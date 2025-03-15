#include "pch.h"
#include "TestShadingModel.h"
#include "GameInstance.h"
#include "EffectTool_Defines.h"


CTestShadingModel::CTestShadingModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CTestShadingModel::CTestShadingModel(const CTestShadingModel& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CTestShadingModel::Initialize(void* _pArg)
{
    CModelObject::MODELOBJECT_DESC* pDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->iCurLevelID = LEVEL_TOOL;
    pDesc->iModelPrototypeLevelID_3D = LEVEL_TOOL;
    pDesc->strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxMesh";
    //pDesc.iShaderPass_3D = ;
    pDesc->iPriorityID_3D = PR3D_EFFECT;
    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 5.f);
    //pDesc.tTransform3DDesc.fSpeedPerSec = 1.f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_tFresnel.vColor = _float4(0.107f, 0.909f, 1.f, 1.f);
    m_tFresnel.fExp = 1.5f;
    m_tFresnel.fBaseReflect = 0.0f;
    m_tFresnel.fStrength = 1.0f;

    m_pGameInstance->CreateConstBuffer(m_tFresnel, D3D11_USAGE_DYNAMIC, &m_pFresnelsBuffer);

    m_tColor.vDiffuseColor = _float4(1.f, 1.f, 1.f, 1.f);
    m_tColor.vBloomColor = _float4(1.f, 1.f, 1.f, 1.f);

    m_pGameInstance->CreateConstBuffer(m_tColor, D3D11_USAGE_DYNAMIC, &m_pColorBuffer);


    if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Diffuse"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMainTextureCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Distortion"),
        TEXT("Com_TextureDistortion"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
        return E_FAIL;


    return S_OK;
}

void CTestShadingModel::Update(_float _fTimeDelta)
{
    ImGui::Begin("Shading Model");

    if (ImGui::TreeNode("Fresnel"))
    {
        if (ImGui::DragFloat4("Color", (_float*)(&m_tFresnel.vColor), 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnel, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("Exp", &m_tFresnel.fExp, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnel, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("BaseReflect", &m_tFresnel.fBaseReflect, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnel, m_pFresnelsBuffer);
        }

        if (ImGui::DragFloat("Strength", &m_tFresnel.fStrength, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tFresnel, m_pFresnelsBuffer);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Color"))
    {
        if (ImGui::DragFloat4("Color", (_float*)&m_tColor.vDiffuseColor, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tColor, m_pColorBuffer);
        }

        if (ImGui::DragFloat4("Bloom Color", (_float*)&m_tColor.vBloomColor, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tColor, m_pColorBuffer);
        }

        if (ImGui::DragFloat4("Sub Color", (_float*)&m_tColor.vSubColor, 0.01f))
        {
            m_pGameInstance->UpdateConstBuffer(m_tColor, m_pColorBuffer);
        }


        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Texture Scaling"))
    {
        ImGui::DragFloat2("Diffuse", (_float*)&m_vDiffuseScaling, 0.01f);
        ImGui::DragFloat2("Noise", (_float*)&m_vNoiseScaling, 0.01f);
        ImGui::TreePop();

    }

    ImGui::End();

    __super::Update(_fTimeDelta);
}

void CTestShadingModel::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CTestShadingModel::Render()
{
    //__super::Render();
    Bind_ShaderResources_WVP();

    //_float4 vSubColor = { 0.1f, 0.7f, 0.9f, 1.f };
    //m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
    //m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vSubColor", &vSubColor, sizeof(_float4));
    if (nullptr != m_pMainTextureCom)
        m_pMainTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_3D], "g_DiffuseTexture");
    if (nullptr != m_pNoiseTextureCom)
        m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_3D], "g_NoiseTexture");


    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vNoiseScaling", &m_vNoiseScaling, sizeof(_float2));
    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vDiffuseScaling", &m_vDiffuseScaling, sizeof(_float2));

    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

    m_pShaderComs[COORDINATE_3D]->Bind_ConstBuffer("SingleFresnel", m_pFresnelsBuffer);
    m_pShaderComs[COORDINATE_3D]->Bind_ConstBuffer("ColorBuffer", m_pColorBuffer);
    m_pControllerModel->Render_Default(m_pShaderComs[COORDINATE_3D], m_iShaderPasses[COORDINATE_3D]);

    return S_OK;
}

CTestShadingModel* CTestShadingModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestShadingModel* pInstance = new CTestShadingModel(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTestShadingModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestShadingModel::Clone(void* _pArg)
{
    CTestShadingModel* pInstance = new CTestShadingModel(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestShadingModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestShadingModel::Free()
{
    Safe_Release(m_pFresnelsBuffer);
    Safe_Release(m_pColorBuffer);

    Safe_Release(m_pMainTextureCom);
    Safe_Release(m_pNoiseTextureCom);

    __super::Free();
}
