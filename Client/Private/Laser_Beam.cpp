#include "stdafx.h"
#include "Laser_Beam.h"
#include "Shader.h"

CLaser_Beam::CLaser_Beam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CLaser_Beam::CLaser_Beam(const CLaser_Beam& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CLaser_Beam::Initialize(void* _pArg)
{
	CModelObject::MODELOBJECT_DESC* pDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);


    pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D_UVCUT;

    HRESULT hr = __super::Initialize(_pArg);
    return hr;
}

void CLaser_Beam::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}


void CLaser_Beam::Late_Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        if (false == m_isFrustumCulling)
            Register_RenderGroup(m_iRenderGroupID_3D, m_iPriorityID_3D);
    }

    __super::Late_Update(_fTimeDelta);
}

HRESULT CLaser_Beam::Render()
{
    if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vCutStartUV", &m_fCutStartUV, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vCutEndUV", &m_fCutEndUV, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CModelObject* CLaser_Beam::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLaser_Beam* pInstance = new CLaser_Beam(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLaser_Beam::Clone(void* _pArg)
{
    CLaser_Beam* pInstance = new CLaser_Beam(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CLaser_Beam");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLaser_Beam::Free()
{
    __super::Free();
}
