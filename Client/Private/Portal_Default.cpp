#include "stdafx.h"
#include "Portal_Default.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

CPortal_Default::CPortal_Default(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPortal(_pDevice, _pContext)
{
}

CPortal_Default::CPortal_Default(const CPortal_Default& _Prototype)
    :CPortal(_Prototype)
{
}

HRESULT CPortal_Default::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPortal_Default::Initialize(void* _pArg)
{

	m_ePortalType = PORTAL_DEFAULT;

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




    return S_OK;
}


HRESULT CPortal_Default::Setup_3D_Postion()
{
    HRESULT hr = __super::Setup_3D_Postion();

    if (FAILED(hr))
        return S_OK;
    
    if (FAILED(Init_Actor()))
        return E_FAIL;
    if (FAILED(Ready_DefaultParticle()))
        return E_FAIL;
    if (FAILED(Ready_InOutParticle()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPortal_Default::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    return S_OK;
}

void CPortal_Default::Interact(CPlayer* _pUser)
{
    if (COORDINATE_2D == _pUser->Get_CurCoord()) {
        static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET))->Set_InitialData(m_strSectionName, m_iPortalIndex);
    }
    else if (COORDINATE_3D == _pUser->Get_CurCoord()) {
        static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D))->Set_Current_PortalID(m_iPortalIndex);
    }

    _pUser->JumpTo_Portal(this);
}

_bool CPortal_Default::Is_Interactable(CPlayer* _pUser)
{
    return true;
}

_float CPortal_Default::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

}

void CPortal_Default::Active_OnEnable()
{
    __super::Active_OnEnable();
    if (m_pDefaultEffect)
    {
        //m_pDefaultEffect->Set_Active(true);
        m_pDefaultEffect->Active_Effect(false, 0);
    }
}

void CPortal_Default::Active_OnDisable()
{
    __super::Active_OnDisable();
    if (m_pDefaultEffect && m_pDefaultEffect->Is_Active())
    {
        m_pDefaultEffect->Inactive_All();
    }
}

HRESULT CPortal_Default::Ready_PartObjects(PORTAL_DESC* _pDesc)
{
    CModelObject::MODELOBJECT_DESC ModelDesc{};
    ModelDesc.Build_2D_Model(LEVEL_STATIC,
        L"Portal_2D_Active",
        L"Prototype_Component_Shader_VtxPosTex",
        (_uint)PASS_VTXPOSTEX::SPRITE2D
    );
    ModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    m_PartObjects[PORTAL_PART_2D] = static_cast<CModelObject*>
        (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &ModelDesc));
    if (nullptr == m_PartObjects[PORTAL_PART_2D])
    {
        MSG_BOX("Portal 2D Sprite Model Creation Failed");
        return E_FAIL;
    }

    ((CModelObject*)m_PartObjects[PORTAL_PART_2D])->Set_AnimationLoop(COORDINATE_2D, 0, true);
    ((CModelObject*)m_PartObjects[PORTAL_PART_2D])->Switch_Animation(0);
    return S_OK;
}

void CPortal_Default::On_InteractionStart(CPlayer* _pPlayer)
{
    _pPlayer->Start_Portal(this);
}

void CPortal_Default::On_InteractionCancel(CPlayer* _pPlayer)
{
}

void CPortal_Default::On_InteractionEnd(CPlayer* _pPlayer)
{
}

CPortal_Default* CPortal_Default::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPortal_Default* pInstance = new CPortal_Default(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created CPortal_Default Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPortal_Default::Clone(void* _pArg)
{
    CPortal_Default* pInstance = new CPortal_Default(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone CPortal_Default Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPortal_Default::Free()
{
    __super::Free();
}