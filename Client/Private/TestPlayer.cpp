#include "stdafx.h"
#include "TestPlayer.h"
#include "GameInstance.h"
#include "TestBody.h"
#include "Cam_Manager.h"

CTestPlayer::CTestPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject(_pDevice, _pContext)
{
}

CTestPlayer::CTestPlayer(const CTestPlayer& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CTestPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTestPlayer::Initialize(void* _pArg)
{
    CTestPlayer::CONTAINEROBJ_DESC* pDesc = static_cast<CTestPlayer::CONTAINEROBJ_DESC*>(_pArg);
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

void CTestPlayer::Priority_Update(_float _fTimeDelta)
{



    CContainerObject::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CTestPlayer::Update(_float _fTimeDelta)
{
    Key_Input(_fTimeDelta);

    CContainerObject::Update(_fTimeDelta); /* Part Object Update */
}

void CTestPlayer::Late_Update(_float _fTimeDelta)
{

    CContainerObject::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CTestPlayer::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CTestPlayer::Key_Input(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::NUM1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
        m_PartObjects[PART_BODY]->Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
        
    }
    if (KEY_DOWN(KEY::M))
    {
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(++itmpIdx);
    }
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

HRESULT CTestPlayer::Ready_Components()
{
    return S_OK;
}

HRESULT CTestPlayer::Ready_PartObjects()
{
    /* Part Body */
    CTestBody::TESTBODY_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.iModelPrototypeLevelID_2D = LEVEL_GAMEPLAY;
    BodyDesc.i3DModelPrototypeLevelID = LEVEL_GAMEPLAY;
    BodyDesc.strModelPrototypeTag_2D = TEXT("Prototype_Component_Texture_PickBulb");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Latch_SkelMesh_NewRig");
    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::DEFAULT;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform2DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;


    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;
    

    return S_OK;
}

CTestPlayer* CTestPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestPlayer* pInstance = new CTestPlayer(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTestPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestPlayer::Clone(void* _pArg)
{
    CTestPlayer* pInstance = new CTestPlayer(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestPlayer::Free()
{

    __super::Free();
}
