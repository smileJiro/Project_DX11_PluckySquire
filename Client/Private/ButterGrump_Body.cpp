#include "stdafx.h"
#include "ButterGrump_Body.h"
#include "GameInstance.h"

CButterGrump_Body::CButterGrump_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CButterGrump_Body::CButterGrump_Body(const CButterGrump_Body& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CButterGrump_Body::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CButterGrump_Body::Initialize(void* _pArg)
{

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_vecHitRender.resize((_uint)MESH_LAST);

    return S_OK;
}

HRESULT CButterGrump_Body::Render()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];
    if (COORDINATE_3D == eCoord)
    {
        pShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZ(), sizeof(_float));
        /* Hit */
        pShader->Bind_RawValue("g_isHit", &m_isHitRender, sizeof(_int));
        _float fHitRatio = m_vHitRenderTime.y / m_vHitRenderTime.x;
        pShader->Bind_RawValue("g_fHitRatio", &fHitRatio, sizeof(_float));
    }
    else if (COORDINATE_2D == eCoord)
    {
        _float fFadeAlphaRatio = m_vFadeAlpha.y / m_vFadeAlpha.x;
        switch (m_eFadeAlphaState)
        {
        case Engine::CModelObject::FADEALPHA_DEFAULT:
            fFadeAlphaRatio = 1.0f;
            break;
        case Engine::CModelObject::FADEALPHA_IN:
            fFadeAlphaRatio = m_vFadeAlpha.y / m_vFadeAlpha.x;
            break;
        case Engine::CModelObject::FADEALPHA_OUT:
            fFadeAlphaRatio = 1.0f - (m_vFadeAlpha.y / m_vFadeAlpha.x);
            break;
        default:
            break;
        }

        pShader->Bind_RawValue("g_fSprite2DFadeAlphaRatio", &fFadeAlphaRatio, sizeof(_float));


        /* Stoppable */
        pShader->Bind_RawValue("g_isStoppable", &m_isStoppable, sizeof(_int));
        pShader->Bind_RawValue("g_vStoppableColor", &m_vStoppableColor, sizeof(_float4));
        _float fStoppableRatio = m_vStoppableTime.y / m_vStoppableTime.x;
        pShader->Bind_RawValue("g_fStoppableRatio", &fStoppableRatio, sizeof(_float));

        /* Hit */
        pShader->Bind_RawValue("g_isHit", &m_isHitRender, sizeof(_int));
        _float fHitRatio = m_vHitRenderTime.y / m_vHitRenderTime.x;
        pShader->Bind_RawValue("g_fHitRatio", &fHitRatio, sizeof(_float));

        if (PASS_VTXPOSTEX::COLOR_ALPHA == (PASS_VTXPOSTEX)iShaderPass)
            pShader->Bind_RawValue("g_vColors", &m_vPosTexColor, sizeof(_float4));
    }

    _int iNumMeshes = static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D))->Get_NumMeshes();
    vector<pair<_bool, _float2>> vecHitRender;

    for (_int i = 0; i < iNumMeshes; ++i)
    {
        vecHitRender.push_back(make_pair(false, _float2(1.0f, 1.0f)));
        if (i == MESHINDEX::MESH_TONGUE)
        {
            vecHitRender[i] = m_vecHitRender[MESHINDEX::MESH_TONGUE];
        }
        else if (i == MESHINDEX::MESH_LEFTEYE)
        {
            vecHitRender[i] = m_vecHitRender[MESHINDEX::MESH_LEFTEYE];
        }
        else if (i == MESHINDEX::MESH_RIGHTEYE)
        {
            vecHitRender[i] = m_vecHitRender[MESHINDEX::MESH_RIGHTEYE];
        }
    }
    static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D))->Render(pShader, iShaderPass, vecHitRender);

    return S_OK;
}

HRESULT CButterGrump_Body::Ready_Components(MODELOBJECT_DESC* _pDesc)
{
    return S_OK;
}

CButterGrump_Body* CButterGrump_Body::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CButterGrump_Body* pInstance = new CButterGrump_Body(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CButterGrump_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButterGrump_Body::Clone(void* _pArg)
{
    CButterGrump_Body* pInstance = new CButterGrump_Body(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CButterGrump_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButterGrump_Body::Free()
{

    __super::Free();
}
