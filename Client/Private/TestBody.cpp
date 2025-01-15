#include "stdafx.h"
#include "TestBody.h"
#include "GameInstance.h"

CTestBody::CTestBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CTestBody::CTestBody(const CTestBody& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CTestBody::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CTestBody::Initialize(void* _pArg)
{
    TESTBODY_DESC* pDesc = static_cast<TESTBODY_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTestBody::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
}

void CTestBody::Update(_float _fTimeDelta)
{
    /* Update Parent Matrix */
    __super::Update(_fTimeDelta);
}

void CTestBody::Late_Update(_float _fTimeDelta)
{

    /* Add Render Group */
    if(COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if(COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

    /* Update Parent Matrix */
    __super::Late_Update(_fTimeDelta);
}



HRESULT CTestBody::Render_Shadow()
{
    return S_OK;
}

HRESULT CTestBody::Ready_Components()
{
    return S_OK;
}


CTestBody* CTestBody::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestBody* pInstance = new CTestBody(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTestBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestBody::Clone(void* _pArg)
{
    CTestBody* pInstance = new CTestBody(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestBody::Free()
{
    __super::Free();
}
