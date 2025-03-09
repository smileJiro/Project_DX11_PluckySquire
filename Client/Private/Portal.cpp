#include "stdafx.h"
#include "Portal.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "Camera_2D.h"

CPortal::CPortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject(_pDevice, _pContext)
{
}

CPortal::CPortal(const CPortal& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CPortal::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPortal::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    m_fInteractChargeTime = 1.f;
    m_eInteractType = INTERACT_TYPE::CHARGE;
    m_eInteractID = INTERACT_ID::PORTAL;

    PORTAL_DESC* pDesc = static_cast<PORTAL_DESC*>(_pArg);
    pDesc->iNumPartObjects = PORTAL_PART_LAST;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    m_isFirstActive = pDesc->isFirstActive;
    m_fTriggerRadius = pDesc->fTriggerRadius;

    m_iPortalIndex = pDesc->iPortalIndex;
    // Actor Object는 차후에, ReadyObject 를 따로 불러 생성.
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (!m_isFirstActive)
        Set_Active(false);


    return S_OK;
}

void CPortal::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CPortal::Update(_float fTimeDelta)
{
    if (nullptr != m_pEffectSystem && false == m_pEffectSystem->Is_Active())
        m_pEffectSystem->Active_Effect(true);

    __super::Update(fTimeDelta);
}

void CPortal::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CPortal::Render()
{
#ifdef _DEBUG
    if (m_pActorCom)
        m_pActorCom->Render();
#endif // _DEBUG
#ifdef _DEBUG
    if (!m_p2DColliderComs.empty())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

    return __super::Render();
}

HRESULT CPortal::Setup_3D_Postion()
{
    _vector f2DPosition = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);

    // 월드맵이 설정됐으니, 포지션 설정.
    _vector f3DPosition = SECTION_MGR->Get_WorldPosition_FromWorldPosMap(m_strSectionName, _float2(XMVectorGetX(f2DPosition), XMVectorGetY(f2DPosition)));

    if (m_pActorCom != nullptr || true == XMVector3Equal(f3DPosition, XMVectorZero()))
    {
        return E_FAIL;
    }

    _float4 fStorePostion = {};

    XMStoreFloat4(&fStorePostion, f3DPosition);

    Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Set_State(CTransform::STATE_POSITION, fStorePostion);
    Change_Coordinate(COORDINATE_3D, nullptr);


    m_isReady_3D = true;

    return S_OK;
}

HRESULT CPortal::Init_Actor()
{
    if (false == m_isReady_3D)
        return E_FAIL;

    CActorObject::ACTOROBJECT_DESC ActorObjectDesc = {};
    CActor::ACTOR_DESC ActorDesc{};
    ActorObjectDesc.eActorType = ACTOR_TYPE::STATIC;
    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc.pOwner = this;

    SHAPE_SPHERE_DESC ShapeSpereDesc{};

    SHAPE_DATA ShapeData;

    ShapeSpereDesc.fRadius = m_fTriggerRadius;
    ShapeData.pShapeDesc = &ShapeSpereDesc;

    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.isTrigger = false;
    ShapeData.isSceneQuery = false;
    ShapeData.iShapeUse = true;


    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/

    ActorDesc.tFilterData.MyGroup = m_iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_TRIGGER;

    ActorObjectDesc.pActorDesc = &ActorDesc;
    HRESULT hr = CActorObject::Ready_Components(&ActorObjectDesc);

    //if (SUCCEEDED(hr))
    //    Active_OnEnable();

    //Change_Coordinate(COORDINATE_2D, nullptr);
    return hr;
}

void CPortal::Use_Portal(CPlayer* _pUser)
{
    _vector vPos = Get_FinalPosition(COORDINATE_2D);

    _vector v3DPos = Get_FinalPosition(COORDINATE_3D);
    
    _int iCurCoord = (_int)_pUser->Get_CurCoord();
    (_int)iCurCoord ^= 1;
    _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);

    switch (iCurCoord)
    {
    case COORDINATE_2D:
        XMStoreFloat3(&vNewPos, vPos);
        break;
    case COORDINATE_3D:
        XMStoreFloat3(&vNewPos, v3DPos);
        break;
    default:
        break;
    }

    if (iCurCoord == COORDINATE_2D)
        SECTION_MGR->Add_GameObject_ToSectionLayer(m_strSectionName, _pUser, SECTION_2D_PLAYMAP_OBJECT);
    else
        SECTION_MGR->Remove_GameObject_FromSectionLayer(m_strSectionName, _pUser);

    Event_Change_Coordinate(_pUser, (COORDINATE)iCurCoord, &vNewPos);

    if (m_pEffectSystem)
        m_pEffectSystem->Active_Effect(true, 1);
}

HRESULT CPortal::Ready_Components(PORTAL_DESC* _pDesc)
{
    CCollider* pCollider = nullptr;
    /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 100.f;
    CircleDesc.vScale = { 1.f, 1.f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&pCollider), &CircleDesc)))
        return E_FAIL;

    m_p2DColliderComs.push_back(pCollider);

    return S_OK;
}

HRESULT CPortal::Ready_Particle()
{
    if (false == m_isReady_3D)
        return E_FAIL;

    _vector f3DPosition = Get_FinalPosition(COORDINATE_3D);


    // 포탈 이펙트 생성.
    m_pControllerTransform->Get_Transform(COORDINATE_3D)->Get_WorldMatrix_Ptr();

    CEffect_System::EFFECT_SYSTEM_DESC EffectDesc = {};
    EffectDesc.eStartCoord = COORDINATE_3D;
    EffectDesc.isCoordChangeEnable = false;
    EffectDesc.iSpriteShaderLevel = LEVEL_STATIC;
    EffectDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";

    EffectDesc.iModelShaderLevel = LEVEL_STATIC;
    EffectDesc.szModelShaderTags = L"Prototype_Component_Shader_VtxMeshInstance";

    EffectDesc.iEffectShaderLevel = LEVEL_STATIC;
    EffectDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";

    EffectDesc.iSingleSpriteShaderLevel = LEVEL_STATIC;
    EffectDesc.szSingleSpriteShaderTags = L"Prototype_Component_Shader_VtxPoint";
    EffectDesc.iSingleSpriteBufferLevel = LEVEL_STATIC;
    EffectDesc.szSingleSpriteBufferTags = L"Prototype_Component_VIBuffer_Point";

    EffectDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";
    EffectDesc.szMeshComputeShaderTag = L"Prototype_Component_Compute_Shader_MeshInstance";

    m_pEffectSystem = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Portal.json"), &EffectDesc));
    if (nullptr == m_pEffectSystem)
        return E_FAIL;

    _matrix WorldMatrix = XMMatrixIdentity();
    switch ((NORMAL_DIRECTION)((_int)roundf(XMVectorGetW(f3DPosition))))
    {
    case NORMAL_DIRECTION::POSITIVE_X:
    {
        WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f));
        break;
    }
    case NORMAL_DIRECTION::NEGATIVE_X:
    {
        WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
        break;
    }
    case NORMAL_DIRECTION::POSITIVE_Y:
    {
        WorldMatrix = XMMatrixIdentity();
        break;
    }
    case NORMAL_DIRECTION::NEGATIVE_Y:
    {
        WorldMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f));
        break;
    }
    case NORMAL_DIRECTION::POSITIVE_Z:
    {
        WorldMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
        break;
    }
    case NORMAL_DIRECTION::NEGATIVE_Z:
    {
        WorldMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
        break;
    }
    }
    WorldMatrix.r[3] = XMVectorSetW(f3DPosition, 1.f);

    m_pEffectSystem->Set_EffectMatrix(WorldMatrix);
    m_PartObjects[PORTAL_PART_3D] = m_pEffectSystem;

    Safe_AddRef(m_pEffectSystem);
    return E_NOTIMPL;
}


HRESULT CPortal::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    return S_OK;
}


void CPortal::Active_OnEnable()
{
    if (m_isFirstActive)
    {
        __super::Active_OnEnable();
        if (m_pEffectSystem)
        {
            //m_pEffectSystem->Set_Active(true);
            m_pEffectSystem->Active_Effect(false, 0);
        }
    }
    else
    {
		Set_Active(false);
    }
}



NORMAL_DIRECTION CPortal::Get_PortalNormal()
{
    _vector v3DPos = Get_FinalPosition(COORDINATE_3D);

    return (NORMAL_DIRECTION)((_int)roundf(XMVectorGetW(v3DPos))); /* 추후 노말을 기준으로 힘의 방향을 결정해도 돼*/

}

void CPortal::Active_OnDisable()
{
    __super::Active_OnDisable();
    if (m_pEffectSystem && m_pEffectSystem->Is_Active())
    {
        m_pEffectSystem->Inactive_All();
    }
}


void CPortal::Set_FirstActive(_bool _bFirstActive)
{
    m_isFirstActive = _bFirstActive;
    Set_Active(m_isFirstActive);
    static_cast<CModelObject*>(m_PartObjects[PORTAL_PART_2D])->Start_FadeAlphaIn();

}


void CPortal::Free()
{
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pEffectSystem);
    __super::Free();
}