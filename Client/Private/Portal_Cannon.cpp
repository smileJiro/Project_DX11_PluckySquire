#include "stdafx.h"
#include "Portal_Cannon.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

CPortal_Cannon::CPortal_Cannon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPortal(_pDevice, _pContext)
{
}

CPortal_Cannon::CPortal_Cannon(const CPortal_Cannon& _Prototype)
    :CPortal(_Prototype)
{
}

HRESULT CPortal_Cannon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPortal_Cannon::Initialize(void* _pArg)
{

    m_ePortalType = PORTAL_CANNON;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (nullptr == _pArg)
        return E_FAIL;

    PORTAL_DESC* pDesc = static_cast<PORTAL_DESC*>(_pArg);

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;


    m_fInteractChargeTime = 0.6f;
    m_eInteractType = INTERACT_TYPE::CHARGE;
    m_eInteractID = INTERACT_ID::PORTAL;
    m_bUIPlayerHeadUp = true;


    return S_OK;
}


HRESULT CPortal_Cannon::Setup_3D_Postion()
{
    HRESULT hr = __super::Setup_3D_Postion();


    if (FAILED(Init_Actor()))
        return E_FAIL;
    if (FAILED(Ready_Particle()))
        return E_FAIL;

    return hr;
}

HRESULT CPortal_Cannon::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    return S_OK;
}

void CPortal_Cannon::Interact(CPlayer* _pUser)
{
    //if (COORDINATE_2D == _pUser->Get_CurCoord()) {
    //    static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET))->Set_InitialData(m_strSectionName, m_iPortalIndex);
    //}
    //else if (COORDINATE_3D == _pUser->Get_CurCoord()) {
    //    static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D))->Set_Current_PortalID(m_iPortalIndex);
    //}

    //_pUser->JumpTo_Portal(this);
}

_bool CPortal_Cannon::Is_Interactable(CPlayer* _pUser)
{
    return true;
}

_float CPortal_Cannon::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

}

void CPortal_Cannon::Active_OnEnable()
{
    __super::Active_OnEnable();
    if (m_pEffectSystem)
    {
        //m_pEffectSystem->Set_Active(true);
        m_pEffectSystem->Active_Effect(false, 0);
    }
}

void CPortal_Cannon::Active_OnDisable()
{
    __super::Active_OnDisable();
    if (m_pEffectSystem && m_pEffectSystem->Is_Active())
    {
        m_pEffectSystem->Inactive_All();
    }
}

HRESULT CPortal_Cannon::Ready_PartObjects(PORTAL_DESC* _pDesc)
{

    CModelObject::MODELOBJECT_DESC ModelDesc{};
    ModelDesc.Build_2D_Model(LEVEL_STATIC,
        L"Portal_2D_JumpUp",
        L"Prototype_Component_Shader_VtxPosTex",
        (_uint)PASS_VTXPOSTEX::SPRITE2D
    );
    ModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    m_PartObjects[PORTAL_PART_2D] = static_cast<CModelObject*>
        (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &ModelDesc));
    if (nullptr == m_PartObjects[PORTAL_PART_2D])
    {
        MSG_BOX("Portal_2D_JumpUp Sprite Model Creation Failed");
        return E_FAIL;
    }
    return S_OK;
}

void CPortal_Cannon::On_InteractionStart(CPlayer* _pPlayer)
{
    _pPlayer->Start_Portal(this);
}

void CPortal_Cannon::On_InteractionEnd(CPlayer* _pPlayer)
{
}

CPortal_Cannon* CPortal_Cannon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPortal_Cannon* pInstance = new CPortal_Cannon(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created CPortal_Cannon Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPortal_Cannon::Clone(void* _pArg)
{
    CPortal_Cannon* pInstance = new CPortal_Cannon(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone CPortal_Cannon Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPortal_Cannon::Free()
{
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pEffectSystem);
    __super::Free();
}