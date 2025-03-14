#include "stdafx.h"
#include "Portal_Cannon.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"
#include "PlayerData_Manager.h"

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
    m_pPlayerDataMgr = CPlayerData_Manager::GetInstance();
    m_ePortalType = PORTAL_CANNON;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (nullptr == _pArg)
        return E_FAIL;

    PORTAL_DESC* pDesc = static_cast<PORTAL_DESC*>(_pArg);

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;
    
    /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = m_fTargetDetectRange;
    CircleDesc.vScale = { 1.f, 1.f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER_TRIGGER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_ColliderDetectRange"), reinterpret_cast<CComponent**>(&m_pDetectCollider), &CircleDesc)))
        return E_FAIL;
    m_p2DColliderComs.push_back(m_pDetectCollider);
	Safe_AddRef(m_pDetectCollider);

    m_fInteractChargeTime =0.6f;
    m_eInteractType = INTERACT_TYPE::CHARGE;
    m_eInteractID = INTERACT_ID::PORTAL;
    m_pBody->Register_OnAnimEndCallBack(bind(&CPortal_Cannon::On_AnimEnd, this, placeholders::_1, placeholders::_2));

    Set_PortalState(IDLE);
    return S_OK;
}

HRESULT CPortal_Cannon::Init_Actor()
{
    return S_OK;
}

void CPortal_Cannon::Update(_float _fTimeDelta)
{
	m_ePlayerCoordiante = m_pPlayerDataMgr->Get_CurrentPlayer_Ptr()->Get_CurCoord();
    if(IDLE == m_ePortalState
        && COORDINATE_3D == m_ePlayerCoordiante)
	{
		Set_PortalState(INACTIVE);
	}
    else if(INACTIVE == m_ePortalState
        && COORDINATE_2D == m_ePlayerCoordiante)
    {
		Set_PortalState(IDLE);
    }
    __super::Update(_fTimeDelta);
}


HRESULT CPortal_Cannon::Setup_3D_Postion()
{
    HRESULT hr = __super::Setup_3D_Postion();


    if (FAILED(Init_Actor()))
        return E_FAIL;
    if (FAILED(Ready_DefaultParticle()))
        return E_FAIL;

    return hr;
}

void CPortal_Cannon::Set_PortalState(PORTAL_STATE _eState)
{
	if (m_ePortalState == _eState)
		return;
    switch (_eState)
    {
    case Client::CPortal_Cannon::IDLE:
		m_pBody->Switch_Animation(ANIM_IDLE_LOOP);
        break;
    case Client::CPortal_Cannon::CHARGE:
		m_pBody->Switch_Animation(ANIM_INTO_ACTIVE);
        break;
    case Client::CPortal_Cannon::INACTIVE:
		m_pBody->Switch_Animation(ANIM_INACTIVE_LOOP);
        break;
    case Client::CPortal_Cannon::TARGETEED:
		m_pBody->Switch_Animation(ANIM_LANDING_LOOP);
        break;
    case Client::CPortal_Cannon::GOAL_IN:
		m_pBody->Switch_Animation(ANIM_LANDING_JIGGLE);
        break;
    default:
        break;
    }
	m_ePortalState = _eState;
}

void CPortal_Cannon::Set_ShootDirection(_fvector _vDirection)
{
    m_vShootDirection = _vDirection; 

}

CPortal_Cannon* CPortal_Cannon::Find_TargetPortal(_fvector _vDirection)
{
    E_DIRECTION eELookDir = To_EDirection(_vDirection);
	CPortal_Cannon* pTargetPortal = nullptr;
    for (auto& pAdj : m_AdjCannonPortals)
    {
        _vector vDir = XMVector2Normalize( pAdj->Get_FinalPosition() - Get_FinalPosition());
        vDir = XMVectorSetZ(vDir, 0.f);
        vDir = XMVectorSetW(vDir, 0.f);
        E_DIRECTION eEAdjDir = To_EDirection(vDir);
        if (eELookDir == eEAdjDir)
        {
            pTargetPortal = pAdj;
            break;
        }
    }
	return pTargetPortal;
}

HRESULT CPortal_Cannon::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    return S_OK;
}

void CPortal_Cannon::Shoot_Ground(CPlayer* _pUser, E_DIRECTION _e2DLookDir )
{

    if (XMVector4EqualInt(XMVectorZero(), m_vShootDirection) )
    {
        _pUser->Add_Impuls(_vector{ 0.f,1.f,0.f } *12.f);
        _vector vLook = EDir_To_Vector(_e2DLookDir);
        swap(vLook.m128_f32[1], vLook.m128_f32[2]);
        _pUser->LookDirectionXZ_Dynamic(vLook);

    }
    else
    {
        _vector v3DPos = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_State(CTransform::STATE_POSITION);
        _pUser->LookDirectionXZ_Dynamic(m_vShootDirection);
        _float fYRadian = XMConvertToRadians(70.f);
        _vector vTargetPos = v3DPos + m_vShootDirection * m_fShootDistance;
        while (false == _pUser->Get_ActorDynamic()->Start_ParabolicTo(vTargetPos, fYRadian))
            fYRadian += XMConvertToRadians(1.f);
    }
    m_pBody->Switch_Animation(ANIM_LAUNCH);
}

void CPortal_Cannon::Shoot_Target(CPlayer* _pUser, CPortal_Cannon* _vTarget)
{
    _vector vTargetPos = _vTarget->Get_FinalPosition();
	_vector vDir = XMVector3Normalize(vTargetPos - Get_FinalPosition()) ;
	vDir = XMVectorSetY(vDir, 0.f);
	vDir = XMVectorSetW(vDir, 0.f);
	_pUser->LookDirectionXZ_Dynamic(vDir);
    _float fYRadian = XMConvertToRadians(70.f);
    while (false == _pUser->Get_ActorDynamic()->Start_ParabolicTo(vTargetPos, fYRadian))
        fYRadian += XMConvertToRadians(1.f);

    m_pBody->Switch_Animation(ANIM_LAUNCH);
}

void CPortal_Cannon::Interact(CPlayer* _pUser)
{
    /*
    if (COORDINATE_2D == _pUser->Get_CurCoord()) {
        static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET))->Set_InitialData(m_strSectionName, m_iPortalIndex);
    }
    else if (COORDINATE_3D == _pUser->Get_CurCoord()) {
        static_cast<CCamera_2D*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D))->Set_Current_PortalID(m_iPortalIndex);
    }

    _pUser->JumpTo_Portal(this);*/
}

_bool CPortal_Cannon::Is_Interactable(CPlayer* _pUser)
{
    return  INACTIVE != m_ePortalState;
}

_float CPortal_Cannon::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

}

void CPortal_Cannon::Active_OnEnable()
{
    __super::Active_OnEnable();
    if (m_pDefaultEffect)
    {
        //m_pDefaultEffect->Set_Active(true);
        m_pDefaultEffect->Active_Effect(false, 0);
    }
}

void CPortal_Cannon::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (COORDINATE_2D == m_ePlayerCoordiante)
    {
        if (ANIM_INTO_ACTIVE == iAnimIdx)
            m_pBody->Switch_Animation(ANIM_ACTIVE_LOOP);
        else if (ALNIM_INTO_IDLE == iAnimIdx)
            m_pBody->Switch_Animation(ANIM_IDLE_LOOP);
        else if (ANIM_LAUNCH == iAnimIdx)
			Set_PortalState(IDLE);
        else if (ANIM_LANDING_JIGGLE == iAnimIdx)
			Set_PortalState(IDLE);
    }
    else
    {
        if (ANIM_INTO_ACTIVE == iAnimIdx)
            m_pBody->Switch_Animation(ANIM_INACTIVE_LOOP);
        else if (ALNIM_INTO_IDLE == iAnimIdx)
            m_pBody->Switch_Animation(ANIM_INACTIVE_LOOP);
        else if (ANIM_LAUNCH == iAnimIdx)
            Set_PortalState(IDLE);
        else if (ANIM_LANDING_JIGGLE == iAnimIdx)
            m_pBody->Switch_Animation(ANIM_INACTIVE_LOOP);
    }


}

void CPortal_Cannon::Active_OnDisable()
{
    __super::Active_OnDisable();
    if (m_pDefaultEffect && m_pDefaultEffect->Is_Active())
    {
        m_pDefaultEffect->Inactive_All();
    }
}

HRESULT CPortal_Cannon::Ready_PartObjects(PORTAL_DESC* _pDesc)
{

    CModelObject::MODELOBJECT_DESC ModelDesc{};
    ModelDesc.Build_2D_Model(LEVEL_STATIC,
        L"Portal_2D_Cannon",
        L"Prototype_Component_Shader_VtxPosTex",
        (_uint)PASS_VTXPOSTEX::SPRITE2D
    );
    ModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    m_PartObjects[PORTAL_PART_2D] = m_pBody = static_cast<CModelObject*>
        (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &ModelDesc));
    if (nullptr == m_PartObjects[PORTAL_PART_2D])
    {
        MSG_BOX("Portal_2D_Cannon Sprite Model Creation Failed");
        return E_FAIL;
    }
    Safe_AddRef(m_pBody);
    return S_OK;
}

void CPortal_Cannon::On_InteractionStart(CPlayer* _pPlayer)
{
    _pPlayer->Start_Portal(this);
}

void CPortal_Cannon::On_Pressing(CPlayer* _pPlayer, _float _fTimeDelta)
{

}

void CPortal_Cannon::On_InteractionEnd(CPlayer* _pPlayer)
{

}

void CPortal_Cannon::On_InteractionCancel(CPlayer* _pPlayer)
{

}

void CPortal_Cannon::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	CPortal_Cannon* pCannon = dynamic_cast<CPortal_Cannon*>(_pOtherObject);
    if (nullptr != pCannon && pCannon!= this)
        m_AdjCannonPortals.insert(pCannon);
}

void CPortal_Cannon::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CPortal_Cannon::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	CPortal_Cannon* pCannon = dynamic_cast<CPortal_Cannon*>(_pOtherObject);
	if (nullptr != pCannon && pCannon != this)
		m_AdjCannonPortals.erase(pCannon);
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
	Safe_Release(m_pBody);
	Safe_Release(m_pDetectCollider);
    __super::Free();
}