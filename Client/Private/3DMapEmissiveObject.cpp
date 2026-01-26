#include "stdafx.h"
#include "3DMapEmissiveObject.h"
#include "GameInstance.h"

C3DMapEmissiveObject::C3DMapEmissiveObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: C3DMapObject(_pDevice, _pContext)
{
}

C3DMapEmissiveObject::C3DMapEmissiveObject(const C3DMapEmissiveObject& _Prototype)
	: C3DMapObject(_Prototype)
{
}

HRESULT C3DMapEmissiveObject::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    // Color랑 Bloom Color 설정 가능.
    
    //EMISSIVEOBJECT_DESC* pDesc = static_cast<EMISSIVEOBJECT_DESC*>(_pArg);
    //
    //m_vColor = pDesc->m_vColor;
    //vBloomColor = pDesc->vBloomColor;

    //if (FAILED(Ready_TargetLight()))
    //    return E_FAIL;

    // 나중에 지우도록 하죠라..
    m_iShaderPasses[COORDINATE_3D] = (_uint)(PASS_VTXMESH::EMISSIVE_COLOR); 
    m_iPriorityID_3D = PR3D_EFFECT;
    return S_OK;
}

void C3DMapEmissiveObject::Late_Update(_float _fTimeDelta)
{


    /* Add Render Group */
    if (false == m_isCulling || false == m_isFrustumCulling)
    {
        Event_SetActive(m_pTargetLight, true);
        Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_EFFECT);
    }
    else
    {
        Event_SetActive(m_pTargetLight, false);
    }

    CGameObject::Late_Update_Component(_fTimeDelta);
}

HRESULT C3DMapEmissiveObject::Render()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;

    Bind_ShaderResources();

    m_pControllerModel->Render_Default(m_pShaderComs[COORDINATE_3D], m_iShaderPasses[COORDINATE_3D]);

    return S_OK;
}

HRESULT C3DMapEmissiveObject::Ready_TargetLight()
{
    ///* 점광원 */
    CONST_LIGHT LightDesc = {};
    LightDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    LightDesc.fFallOutStart = 0.0f;
    LightDesc.fFallOutEnd = 1.0f;
    LightDesc.vRadiance = _float3(4.0f, 4.0f, 4.0f);
    LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.2f, 1.f);
    LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
    LightDesc.vSpecular = _float4(0.0f,0.0f,0.0f,1.0f);

    _vector vLook = m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
    _float3 vOffset = {};
    XMStoreFloat3(&vOffset, vLook * 0.05f);
    if (FAILED(m_pGameInstance->Add_Light_Target(LightDesc, LIGHT_TYPE::POINT, this, vOffset, &m_pTargetLight, true)))
        return E_FAIL;

    Safe_AddRef(m_pTargetLight);
    m_pTargetLight->Set_Active(false);
    return S_OK;
}

HRESULT C3DMapEmissiveObject::Bind_ShaderResources()
{
    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vSubColor", &m_vBloomColor, sizeof(_float4));
    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4));

    return S_OK;
}

C3DMapEmissiveObject* C3DMapEmissiveObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C3DMapEmissiveObject* pInstance = new C3DMapEmissiveObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C3DMapEmissiveObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C3DMapEmissiveObject::Clone(void* _pArg)
{
    C3DMapEmissiveObject* pInstance = new C3DMapEmissiveObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C3DMapEmissiveObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C3DMapEmissiveObject::Free()
{

    Safe_Release(m_pTargetLight);
    __super::Free();
}
