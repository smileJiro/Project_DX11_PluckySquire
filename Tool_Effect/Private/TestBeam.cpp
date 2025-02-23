#include "pch.h"
#include "TestBeam.h"
#include "GameInstance.h"
#include "EffectTool_Defines.h"

CTestBeam::CTestBeam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CTestBeam::CTestBeam(const CTestBeam& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CTestBeam::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CTestBeam::Initialize(void* _pArg)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.eStartCoord = COORDINATE_3D;
    Desc.iCurLevelID = LEVEL_TOOL;
    Desc.isCoordChangeEnable = false;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, 0.f, 1.f));

    //m_pBufferCom->Begin_Update();
    //m_pBufferCom->Update_StartPosition(XMVectorSet(0.f, 0.f, 0.f, 1.f));
    //m_pBufferCom->Update_EndPosition(XMVectorSet(5.f, 5.f, 5.f, 1.f));
    //m_pBufferCom->End_Update();

    m_pBufferCom->Set_RandomMin(_float3(-2.f, 0.f, -2.f));
    m_pBufferCom->Set_RandomMax(_float3(-2.f, 0.f, 2.f));
    m_pBufferCom->Initialize_Positions(_float3(0.f, 0.f, 0.f), _float3(5.f, 5.f, 5.f));

    m_vColor = _float4(0.96f, 0.5f, 0.31f, 0.85f);
    return S_OK;
}

void CTestBeam::Update(_float _fTimeDelta)
{
    ImGui::Begin("Test Beam");

    static _float3 vStartPos = _float3(0.f, 0.f, 0.f);
    static _float3 vEndPos = _float3(0.f, 0.f, 0.f);

    if (ImGui::DragFloat3("StartPos", (_float*)&vStartPos, 0.01f))
    {
        m_pBufferCom->Begin_Update();
        m_pBufferCom->Update_StartPosition(XMLoadFloat3(&vStartPos));
        m_pBufferCom->End_Update();
    }

    if (ImGui::DragFloat3("EndPos", (_float*)&vEndPos, 0.01f))
    {
        m_pBufferCom->Begin_Update();
        m_pBufferCom->Update_EndPosition(XMLoadFloat3(&vEndPos));
        m_pBufferCom->End_Update();
    }

    if (ImGui::DragFloat4("Color", (_float*)&m_vColor, 0.001f))
    {
      
    }

    if (ImGui::Button("Converge"))
    {
        m_bUpdate = !m_bUpdate;
    }

    if (ImGui::Button("Reset"))
    {
        m_pBufferCom->Reset_Positions();
    }


    if (m_bUpdate)
    {
        m_pBufferCom->Begin_Update();
        m_pBufferCom->Converge_Points(_fTimeDelta * 2.f);
        m_pBufferCom->End_Update();
    }


    ImGui::End();
}

void CTestBeam::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_PARTICLE, this);

}

HRESULT CTestBeam::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pBufferCom->Bind_BufferDesc()))
        return E_FAIL;
    if (FAILED(m_pBufferCom->Render()))
        return E_FAIL;


    return S_OK;
}

HRESULT CTestBeam::Bind_ShaderResources()
{
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    // TEMP
    _float fWidth = 1.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fWidth", &fWidth, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return E_FAIL;

    _float4 vLook;
    XMStoreFloat4(&vLook, m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW).r[2]);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLook", &vLook, sizeof(_float4))))
        return E_FAIL;


    if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CTestBeam::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Beam"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_DefaultRibbon"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_TestBeam"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    return S_OK;
}

CTestBeam* CTestBeam::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestBeam* pInstance = new CTestBeam(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTestBeam");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestBeam::Clone(void* _pArg)
{
    CTestBeam* pInstance = new CTestBeam(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestBeam");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestBeam::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CTestBeam::Cleanup_DeadReferences()
{
    return S_OK;
}
