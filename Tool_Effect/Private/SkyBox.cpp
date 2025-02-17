#include "pch.h"
#include "SkyBox.h"
#include "GameInstance.h"

CSkyBox::CSkyBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CSkyBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkyBox::Initialize(void* _pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(_pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_isRender = false;

    return S_OK;
}

void CSkyBox::Priority_Update(_float _fTimeDelta)
{
}

void CSkyBox::Update(_float _fTimeDelta)
{
}

void CSkyBox::Late_Update(_float _fTimeDelta)
{
    m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));
    
    m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_BLEND, this);
    //m_pGameInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CSkyBox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_BufferDesc();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkyBox::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube2"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkyBox::Bind_ShaderResources()
{
    if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSkyBox* pInstance = new CSkyBox(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSkyBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkyBox::Clone(void* _pArg)
{
    CSkyBox* pInstance = new CSkyBox(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CSkyBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkyBox::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    __super::Free();
}

HRESULT CSkyBox::Cleanup_DeadReferences()
{
    return S_OK;
}
