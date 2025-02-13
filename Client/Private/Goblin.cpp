#include "stdafx.h"
#include "Goblin.h"
#include "GameInstance.h"
#include "FSM.h"
#include "DetectionField.h"
#include "ModelObject.h"

CGoblin::CGoblin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CGoblin::CGoblin(const CGoblin& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CGoblin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGoblin::Initialize(void* _pArg)
{
    CGoblin::MONSTER_DESC* pDesc = static_cast<CGoblin::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 300.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 10.f;
    pDesc->fAttackRange = 1.f;
    pDesc->fAlert2DRange = 5.f;
    pDesc->fChase2DRange = 12.f;
    pDesc->fAttack2DRange = 2.f;
    pDesc->fDelayTime = 3.f;
    
    pDesc->fHP = 5.f;

    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 30.f;

    /* Create Test Actor (Desc를 채우는 함수니까. __super::Initialize() 전에 위치해야함. )*/
    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_Chase_NoneAttackState();
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, CHASE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, PATROL, true);
    pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CGoblin::Animation_End, this, placeholders::_1, placeholders::_2));

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CGoblin::Priority_Update(_float _fTimeDelta)
{
    if (true == m_isDelay)
    {
        m_fAccTime += _fTimeDelta;
        if (m_fDelayTime <= m_fAccTime)
        {
            m_fAccTime = 0.f;
            m_isDelay = false;
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CGoblin::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CGoblin::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CGoblin::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    if (COORDINATE_3D == Get_CurCoord())
        m_pDetectionField->Render();

    if (COORDINATE_2D == Get_CurCoord())
    {
        for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
        {
            m_p2DColliderComs[i]->Render();
        }
    }
#endif

    /* Font Render */

    return S_OK;
}

void CGoblin::Attack()
{
    Set_AnimChangeable(true);
    Delay_On();
}

void CGoblin::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        switch (MONSTER_STATE(m_iState))
        {
        case MONSTER_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::PATROL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(PATROL);
            break;

        case MONSTER_STATE::ALERT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(CHASE);
            break;

        case MONSTER_STATE::STANDBY:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::HIT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(HIT);
            break;

        case MONSTER_STATE::DEAD:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DEATH);
            break;

        default:
            break;
        }
    }
}

void CGoblin::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CGoblin::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case ALERT:
        Set_AnimChangeable(true);
        break;

    case HIT:
        pModelObject->Switch_Animation(WAKE);
        break;

    case WAKE:
        Set_AnimChangeable(true);
        break;

    default:
        break;
    }
}

void CGoblin::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    __super::OnContact_Enter(_My, _Other, _ContactPointDatas);

    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse)
    {
        if ((_uint)MONSTER_STATE::CHASE == m_iState)
        {
            Attack();
            m_isContactToTarget = true;
        }
    }
}

void CGoblin::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CGoblin::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse)
    {
        if (true == m_isContactToTarget)
        {
            m_isContactToTarget = false;
        }
    }
}

void CGoblin::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup && (_uint)SHAPE_USE::SHAPE_TRIGER == _My.pShapeUserData->iShapeUse)
    {
        if ((_uint)MONSTER_STATE::ATTACK == m_iState)
        {
            Event_Hit(this, _Other.pActorUserData->pOwner, Get_Stat().fDamg);
            _float3 vRepulse; XMStoreFloat3(&vRepulse, 10.f * XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
            _Other.pActorUserData->pOwner->Get_ActorCom()->Add_Impulse(vRepulse);
            Attack();
            Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);
        }
    }
}

void CGoblin::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CGoblin::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CGoblin::On_Hit(CGameObject* _pHitter, _float _fDamg)
{
    __super::On_Hit(_pHitter, _fDamg);
}

HRESULT CGoblin::Ready_ActorDesc(void* _pArg)
{
    CGoblin::MONSTER_DESC* pDesc = static_cast<CGoblin::MONSTER_DESC*>(_pArg);

    pDesc->eActorType = ACTOR_TYPE::DYNAMIC;
    CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc->pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc->FreezeRotation_XYZ[0] = true;
    ActorDesc->FreezeRotation_XYZ[1] = false;
    ActorDesc->FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc->FreezePosition_XYZ[0] = false;
    ActorDesc->FreezePosition_XYZ[1] = false;
    ActorDesc->FreezePosition_XYZ[2] = false;

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_SPHERE_DESC* ShapeDesc = new SHAPE_SPHERE_DESC;
    ShapeDesc->fRadius = 0.4f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_CAPSULE; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fRadius + 0.1f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    //마찰용 박스
    SHAPE_BOX_DESC* BoxDesc = new SHAPE_BOX_DESC;
    BoxDesc->vHalfExtents = { ShapeDesc->fRadius - 0.1f, ShapeDesc->fRadius - 0.1f, ShapeDesc->fRadius - 0.1f };

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    //SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = BoxDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::BOX;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::NORESTITUTION; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, BoxDesc->vHalfExtents.y, 0.f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    //공격용 구
    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_SPHERE_DESC* AttackShapeDesc = new SHAPE_SPHERE_DESC;
    AttackShapeDesc->fRadius = 0.6f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    ShapeData->pShapeDesc = AttackShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_CAPSULE; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, AttackShapeDesc->fRadius + 0.1f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CGoblin::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC FSMDesc;
    FSMDesc.fAlertRange = m_fAlertRange;
    FSMDesc.fChaseRange = m_fChaseRange;
    FSMDesc.fAttackRange = m_fAttackRange;
    FSMDesc.fAlert2DRange = m_fAlert2DRange;
    FSMDesc.fChase2DRange = m_fChase2DRange;
    FSMDesc.fAttack2DRange = m_fAttack2DRange;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;
    FSMDesc.isMelee = true;

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &FSMDesc)))
        return E_FAIL;

#ifdef _DEBUG
    /* Com_DebugDraw_For_Client */
    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_DebugDraw_For_Client"),
        TEXT("Com_DebugDraw_For_Client"), reinterpret_cast<CComponent**>(&m_pDraw))))
        return E_FAIL;
#endif // _DEBUG

    /* Com_DetectionField */
    CDetectionField::DETECTIONFIELDDESC DetectionDesc;
    DetectionDesc.fRange = m_fAlertRange;
    DetectionDesc.fFOVX = m_fFOVX;
    DetectionDesc.fFOVY = m_fFOVY;
    DetectionDesc.fOffset = 0.f;
    DetectionDesc.pOwner = this;
    DetectionDesc.pTarget = m_pTarget;
#ifdef _DEBUG
    DetectionDesc.pDraw = m_pDraw;
#endif // _DEBUG

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_DetectionField"),
        TEXT("Com_DetectionField"), reinterpret_cast<CComponent**>(&m_pDetectionField), &DetectionDesc)))
        return E_FAIL;

    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
    AABBDesc.pOwner = this;
    AABBDesc.vExtents = { 50.f, 50.f };
    AABBDesc.vScale = { 1.0f, 1.0f };
    AABBDesc.vOffsetPosition = { 0.f, AABBDesc.vExtents.y };
    AABBDesc.isBlock = true;
    AABBDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGoblin::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("goblin_Rig");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

    BodyDesc.strModelPrototypeTag_2D = TEXT("Goblin");
    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

    BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
    BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CGoblin* CGoblin::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CGoblin* pInstance = new CGoblin(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CGoblin");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGoblin::Clone(void* _pArg)
{
    CGoblin* pInstance = new CGoblin(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CGoblin");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGoblin::Free()
{

    __super::Free();
}
