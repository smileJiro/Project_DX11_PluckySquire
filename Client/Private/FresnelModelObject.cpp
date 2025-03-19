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

    if (nullptr != pDesc->pFresnelBuffer)
    {
        m_pFresnelBuffer = pDesc->pFresnelBuffer;
        Safe_AddRef(m_pFresnelBuffer);
    }
    if (nullptr != pDesc->pColorBuffer)
    {
        m_pColorBuffer = pDesc->pColorBuffer;
        Safe_AddRef(m_pColorBuffer);
    }

    m_vDiffuseScaling = pDesc->vDiffuseScaling;
    m_vNoiseScaling = pDesc->vNoiseScaling;
    m_pScaling = pDesc->pScaling;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFresnelModelObject::Render()
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        Bind_ShaderResources_WVP();

        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
        if (nullptr != m_pMainTextureCom)
            m_pMainTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_3D], "g_DiffuseTexture");
        if (nullptr != m_pNoiseTextureCom)
            m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderComs[COORDINATE_3D], "g_NoiseTexture");

        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vNoiseScaling", &m_vNoiseScaling, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vDiffuseScaling", &m_vDiffuseScaling, sizeof(_float2));

        if (nullptr != m_pFresnelBuffer)
            m_pShaderComs[COORDINATE_3D]->Bind_ConstBuffer("SingleFresnel", m_pFresnelBuffer);
        if (nullptr != m_pColorBuffer)
            m_pShaderComs[COORDINATE_3D]->Bind_ConstBuffer("ColorBuffer", m_pColorBuffer);
        
        // 시간에 따른 Scaling이 필요할 경우.
        if (nullptr != m_pScaling)
            m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fScalingTime", m_pScaling, sizeof(_float));


        m_pControllerModel->Render_Default(m_pShaderComs[COORDINATE_3D], m_iShaderPasses[COORDINATE_3D]);
    }
    else
        __super::Render();

    return S_OK;
}

HRESULT CFresnelModelObject::Ready_Components(const FRESNEL_MODEL_DESC* _pDesc)
{
    if (0 != lstrcmp(_pDesc->szDiffusePrototypeTag, L""))
    {       
        if (FAILED(Add_Component(m_iCurLevelID, _pDesc->szDiffusePrototypeTag, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pMainTextureCom))))
            return E_FAIL;
    }

    if (0 != lstrcmp(_pDesc->szNoisePrototypeTag, L""))
    {
        if (FAILED(Add_Component(m_iCurLevelID, _pDesc->szNoisePrototypeTag, TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
            return E_FAIL;
    }

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

CGameObject* CFresnelModelObject::Clone(void* _pArg)
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
    Safe_Release(m_pNoiseTextureCom);
    Safe_Release(m_pMainTextureCom);

    Safe_Release(m_pFresnelBuffer);
    Safe_Release(m_pColorBuffer);

    __super::Free();
}
