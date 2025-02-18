#include "stdafx.h"
#include "Portal.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"

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

    PORTAL_DESC* pDesc = static_cast<PORTAL_DESC*>(_pArg);
    pDesc->iNumPartObjects = PORTAL_PART_LAST;
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->isCoordChangeEnable = true;
    pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    m_fTriggerRadius =  pDesc->fTriggerRadius;
    m_iPortalIndex =  pDesc->iPortalIndex;
    m_fInteractChargeTime = 0.0f;
    // Actor Object는 차후에, ReadyObject 를 따로 불러 생성.
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;
    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

	return S_OK;
}

void CPortal::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CPortal::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CPortal::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CPortal::Render()
{
    if(m_pActorCom)
        m_pActorCom->Render();
	return __super::Render();
}

HRESULT CPortal::Init_Actor()
{
    Safe_Release(m_pActorCom);



    _vector f2DPosition = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);

    // 월드맵이 설정됐으니, 포지션 설정.
    _vector f3DPosition = SECTION_MGR->Get_WorldPosition_FromWorldPosMap(m_strSectionName, _float2(XMVectorGetX(f2DPosition), XMVectorGetY(f2DPosition)));


    _float4 fStorePostion = {};

    XMStoreFloat4(&fStorePostion, f3DPosition);

    Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Set_State(CTransform::STATE_POSITION, fStorePostion);
    Change_Coordinate(COORDINATE_3D, nullptr);

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
    
    if (SUCCEEDED(hr))
        Active_OnEnable();

    Change_Coordinate(COORDINATE_2D, nullptr);


    return hr;
}

void CPortal::Use_Portal(CPlayer* _pUser)
{
    _vector vPos = Get_ControllerTransform()[COORDINATE_2D].Get_State(CTransform::STATE_POSITION);

    _vector v3DPos = SECTION_MGR->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { XMVectorGetX(vPos),XMVectorGetY(vPos) });

    _int iCurCoord = (_int)_pUser->Get_CurCoord();
    (_int)iCurCoord ^= 1;
    _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);

    //if (FAILED(Change_Coordinate((COORDINATE)iCurCoord, nullptr)))
    //{
    //    MSG_BOX("Portal Logic Check - CPotal::Interact");
    //    return;
    //}

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
}

HRESULT CPortal::Ready_Components(PORTAL_DESC* _pDesc)
{ 
    CCollider* pCollider = nullptr;
    /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = _pDesc->fTriggerRadius;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&pCollider), &CircleDesc)))
        return E_FAIL;

    m_p2DColliderComs.push_back(pCollider);

    return S_OK;
}

HRESULT CPortal::Ready_PartObjects(PORTAL_DESC* _pDesc)
{
    CModelObject::MODELOBJECT_DESC ModelDesc{};
    ModelDesc.Build_2D_Model(_pDesc->iCurLevelID,
        L"Portal_2D",
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
    return S_OK;
}

void CPortal::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    int a = 1;
}

void CPortal::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    int a = 1;

}

void CPortal::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

HRESULT CPortal::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(m_pControllerTransform->Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    return S_OK;
}

void CPortal::Interact(CPlayer* _pUser)
{
    if (COORDINATE_2D == _pUser->Get_CurCoord()) {
        static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET))->Set_InitialData(m_strSectionName, m_iPortalIndex);
    }

	_pUser->JumpTo_Portal(this);
}

_bool CPortal::Is_Interactable(CPlayer* _pUser)
{
    return true;
}

_float CPortal::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

}

void CPortal::On_Touched(CPlayer* _pPlayer)
{
	_pPlayer->Start_Portal(this);
}

CPortal* CPortal::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPortal* pInstance = new CPortal(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created CPortal Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPortal::Clone(void* _pArg)
{
    CPortal* pInstance = new CPortal(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone CPortal Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPortal::Free()
{
    Safe_Release(m_pColliderCom);
    __super::Free();
}