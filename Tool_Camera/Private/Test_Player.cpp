#include "stdafx.h"
#include "Test_Player.h"
#include "Test_Body_Player.h"

#include "GameInstance.h"

CTest_Player::CTest_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject(_pDevice, _pContext)
{
}

CTest_Player::CTest_Player(const CTest_Player& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CTest_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTest_Player::Initialize(void* _pArg)
{
    CTest_Player::CONTAINEROBJ_DESC* pDesc = static_cast<CTest_Player::CONTAINEROBJ_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->iNumPartObjects = PART_LAST;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 200.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    return S_OK;
}

void CTest_Player::Priority_Update(_float _fTimeDelta)
{
    CContainerObject::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CTest_Player::Update(_float _fTimeDelta)
{
    Key_Input(_fTimeDelta);

    CContainerObject::Update(_fTimeDelta); /* Part Object Update */
}

void CTest_Player::Late_Update(_float _fTimeDelta)
{
    CContainerObject::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CTest_Player::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CTest_Player::Key_Input(_float _fTimeDelta)
{
    //if (KEY_PRESSING(KEY::TAB))
    //{
    //    Change_Coordinate(COORDINATE_2D, _float3(0.0f, 0.0f, 0.0f));
    //    m_PartObjects[PART_BODY]->Change_Coordinate(COORDINATE_2D, _float3(0.0f, 0.0f, 0.0f));

    //}
    //if (KEY_PRESSING(KEY::O))
    //{
    //    Change_Coordinate(COORDINATE_3D, _float3(0.0f, 0.0f, 0.0f));
    //    m_PartObjects[PART_BODY]->Change_Coordinate(COORDINATE_3D, _float3(0.0f, 0.0f, 0.0f));
    //}

    /* Test Move Code */
    if (KEY_PRESSING(KEY::A))
    {
        if (KEY_PRESSING(KEY::W))
        {
            m_pControllerTransform->Go_Straight(_fTimeDelta);
        }
        else if (KEY_PRESSING(KEY::S))
        {
            m_pControllerTransform->Go_Backward(_fTimeDelta);
        }

        m_pControllerTransform->Go_Left(_fTimeDelta);
    }
    else if (KEY_PRESSING(KEY::D))
    {
        if (KEY_PRESSING(KEY::W))
        {
            m_pControllerTransform->Go_Straight(_fTimeDelta);
        }
        else if (KEY_PRESSING(KEY::S))
        {
            m_pControllerTransform->Go_Backward(_fTimeDelta);
        }

        m_pControllerTransform->Go_Right(_fTimeDelta);
    }
    else if (KEY_PRESSING(KEY::W))
    {
        m_pControllerTransform->Go_Straight(_fTimeDelta);
    }
    else if (KEY_PRESSING(KEY::S))
    {
        m_pControllerTransform->Go_Backward(_fTimeDelta);
    }
}

HRESULT CTest_Player::Ready_Components()
{
    return S_OK;
}

HRESULT CTest_Player::Ready_PartObjects()
{
    /* Part Body */
    CTest_Body_Player::TESTBODY_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag = TEXT("Prototype_Component_Model_Test");
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::COLOR_ALPHA;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform2DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 20.f;

    BodyDesc.tTransform3DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_Test_Body_Player"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CTest_Player* CTest_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTest_Player* pInstance = new CTest_Player(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTest_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTest_Player::Clone(void* _pArg)
{
    CTest_Player* pInstance = new CTest_Player(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTest_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTest_Player::Free()
{
    __super::Free();
}
