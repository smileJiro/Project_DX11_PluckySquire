#include "pch.h"
#include "Effect_Reference.h"
#include "GameInstance.h"

CEffect_Reference::CEffect_Reference(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CEffect_Reference::CEffect_Reference(const CEffect_Reference& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CEffect_Reference::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Reference::Initialize(void* _pArg)
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
    Set_Active(false);
    return S_OK;
}

void CEffect_Reference::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);

}

HRESULT CEffect_Reference::Render()
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

HRESULT CEffect_Reference::Bind_ShaderResources()
{
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CEffect_Reference::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Reference"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    return S_OK;
}

CEffect_Reference* CEffect_Reference::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CEffect_Reference* pInstance = new CEffect_Reference(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CEffect_Reference");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Reference::Clone(void* _pArg)
{
    CEffect_Reference* pInstance = new CEffect_Reference(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffect_Reference");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Reference::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CEffect_Reference::Cleanup_DeadReferences()
{
    return S_OK;
}
