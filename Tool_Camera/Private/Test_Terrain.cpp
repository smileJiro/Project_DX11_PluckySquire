#include "stdafx.h"
#include "Test_Terrain.h"

#include "GameInstance.h"

CTest_Terrain::CTest_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CTest_Terrain::CTest_Terrain(const CTest_Terrain& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CTest_Terrain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTest_Terrain::Initialize(void* _pArg)
{
    MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    CRay::RAY_DESC RayDesc = {};
    RayDesc.fViewportWidth = (_float)g_iWinSizeX;
    RayDesc.fViewportHeight = (_float)g_iWinSizeY;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Ray",
        TEXT("Com_Ray"), reinterpret_cast<CComponent**>(&m_pRayCom), &RayDesc)))
        return E_FAIL;

    return S_OK;
}

void CTest_Terrain::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}

void CTest_Terrain::Update(_float _fTimeDelta)
{
    /* Update Parent Matrix */
    CPartObject::Update(_fTimeDelta);
}

void CTest_Terrain::Late_Update(_float _fTimeDelta)
{
    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord()) {
        if (true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_Position(), 5.0f))
            m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_NONBLEND, this);
    }
    //else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
    //    m_pGameInstance->Add_RenderObject(CRenderer::RG_EFFECT, this);

    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
}


HRESULT CTest_Terrain::Render_Shadow()
{
    return S_OK;
}

HRESULT CTest_Terrain::Ready_Components()
{
    return S_OK;
}

CTest_Terrain* CTest_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTest_Terrain* pInstance = new CTest_Terrain(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTest_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTest_Terrain::Clone(void* _pArg)
{
    CTest_Terrain* pInstance = new CTest_Terrain(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTest_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTest_Terrain::Free()
{
    __super::Free();
}
