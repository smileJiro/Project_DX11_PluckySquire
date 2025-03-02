#include "pch.h"
#include "Object_Background.h"
#include "GameInstance.h"

CObject_Background::CObject_Background(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CObject_Background::CObject_Background(const CObject_Background& _Prototype)
	: CGameObject(_Prototype)
{
}

HRESULT CObject_Background::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Background::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pControllerTransform->Set_Scale(_float3(100.f, 100.f, 100.f));
    //m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, 5.f, 1.f));


    m_vColor = _float4(0.5f, 0.f, 0.7f, 1.f);

    return S_OK;
}

void CObject_Background::Priority_Update(_float _fTimeDelta)
{
}

void CObject_Background::Update(_float _fTimeDelta)
{
    ImGui::Begin("Background Color");

    if (ImGui::DragFloat4("Background Color", (_float*)&m_vColor, 0.01f))
    {
        
    }

    ImGui::End();

    m_pControllerTransform->Set_State(CTransform::STATE_POSITION, *m_pGameInstance->Get_CamPosition());
}

void CObject_Background::Late_Update(_float _fTimeDelta)
{
    //m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_BLEND, this);
    //m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CObject_Background::Render()
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

HRESULT CObject_Background::Bind_ShaderResources()
{
    if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CObject_Background::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube2"), 
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
        return E_FAIL;

    
    return S_OK;
}

CObject_Background* CObject_Background::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CObject_Background* pInstance = new CObject_Background(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CObject_Background");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Background::Clone(void* _pArg)
{
    CObject_Background* pInstance = new CObject_Background(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CObject_Background");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Background::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBufferCom);

    __super::Free();
}

HRESULT CObject_Background::Cleanup_DeadReferences()
{
	return S_OK;
}

