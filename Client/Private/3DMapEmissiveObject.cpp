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
    //m_vBloomColor = pDesc->m_vBloomColor;

    // 나중에 지우도록 하죠라..
    m_iShaderPasses[COORDINATE_3D] = (_uint)(PASS_VTXMESH::EMISSIVE_COLOR); 
    m_iPriorityID_3D = PR3D_EFFECT;
    return S_OK;
}

HRESULT C3DMapEmissiveObject::Render()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;

    Bind_ShaderResources();

    m_pControllerModel->Render_Default(m_pShaderComs[COORDINATE_3D], m_iShaderPasses[COORDINATE_3D]);

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
    __super::Free();

}
