#include "stdafx.h"
#include "FresnelModelObject.h"
#include "GameInstance.h"

CFresnelModelObject::CFresnelModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CFresnelModelObject::CFresnelModelObject(const CFresnelModelObject& _Prototype)
    : CModelObject(_Prototype)
{
}


HRESULT CFresnelModelObject::Initialize(void* _pArg)
{
    FRESNEL_MODEL_DESC* pDesc = static_cast<FRESNEL_MODEL_DESC*>(_pArg);

    m_pFresnelBuffer = pDesc->pFresnelBuffer;
    m_pColorBuffer = pDesc->pColorBuffer;

    Safe_AddRef(m_pFresnelBuffer);
    Safe_AddRef(m_pColorBuffer);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFresnelModelObject::Render()
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        Bind_ShaderResources_WVP();

        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

        if (nullptr != m_pFresnelBuffer)
            m_pShaderComs[COORDINATE_3D]->Bind_ConstBuffer("SingleFresnel", m_pFresnelBuffer);
        if (nullptr != m_pColorBuffer)
            m_pShaderComs[COORDINATE_3D]->Bind_ConstBuffer("ColorBuffer", m_pColorBuffer);
        m_pControllerModel->Render_Default(m_pShaderComs[COORDINATE_3D], m_iShaderPasses[COORDINATE_3D]);
    }
    else
        __super::Render();

    return S_OK;
}

CFresnelModelObject* CFresnelModelObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFresnelModelObject* pInstance = new CFresnelModelObject(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create CFresnelModelObject");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CFresnelModelObject* CFresnelModelObject::Clone(void* _pArg)
{
    CFresnelModelObject* pInstance = new CFresnelModelObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CFresnelModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFresnelModelObject::Free()
{
    Safe_Release(m_pFresnelBuffer);
    Safe_Release(m_pColorBuffer);

    __super::Free();
}
