#include "stdafx.h"
#include "Test_Body_Player.h"

#include "GameInstance.h"

CTest_Body_Player::CTest_Body_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CTest_Body_Player::CTest_Body_Player(const CTest_Body_Player& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CTest_Body_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTest_Body_Player::Initialize(void* _pArg)
{
    TESTBODY_DESC* pDesc = static_cast<TESTBODY_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTest_Body_Player::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}

void CTest_Body_Player::Update(_float _fTimeDelta)
{
    /* Update Parent Matrix */
    CPartObject::Update(_fTimeDelta);
}

void CTest_Body_Player::Late_Update(_float _fTimeDelta)
{
    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CTest_Body_Player::Render()
{
    if (FAILED(CModelObject::Bind_ShaderResources_WVP()))
        return E_FAIL;

    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];

    return S_OK;
}

HRESULT CTest_Body_Player::Ready_Components()
{
    return S_OK;
}

CTest_Body_Player* CTest_Body_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTest_Body_Player* pInstance = new CTest_Body_Player(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTest_Body_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTest_Body_Player::Clone(void* _pArg)
{
    CTest_Body_Player* pInstance = new CTest_Body_Player(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTest_Body_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTest_Body_Player::Free()
{
    __super::Free();
}
