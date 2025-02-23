#include "stdafx.h"
#include "Cube.h"

#include "GameInstance.h"

CCube::CCube(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CCube::CCube(const CCube& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CCube::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCube::Initialize(void* _pArg)
{
    MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    CRay::RAY_DESC RayDesc = {};
    RayDesc.fViewportWidth = (_float)g_iWinSizeX;
    RayDesc.fViewportHeight = (_float)g_iWinSizeY;
    if (FAILED(Add_Component(LEVEL_CAMERA_TOOL, L"Prototype_Component_Ray",
        TEXT("Com_Ray"), reinterpret_cast<CComponent**>(&m_pRayCom), &RayDesc)))
        return E_FAIL;

    return S_OK;
}

void CCube::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}

void CCube::Update(_float _fTimeDelta)
{
    /* Update Parent Matrix */
    CPartObject::Update(_fTimeDelta);
}

void CCube::Late_Update(_float _fTimeDelta)
{
    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord()) {
        if (true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 5.0f))
            m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_GEOMETRY, this);
    }
    //else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
    //    m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);

    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CCube::Render_Shadow()
{
    return S_OK;
}

HRESULT CCube::Ready_Components()
{
    return S_OK;
}

CCube* CCube::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCube* pInstance = new CCube(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCube");
        Safe_Release(pInstance);
    }

    return pInstance;
}
CGameObject* CCube::Clone(void* _pArg)
{
    CCube* pInstance = new CCube(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCube");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCube::Free()
{
    __super::Free();
}

