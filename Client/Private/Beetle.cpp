#include "stdafx.h"
#include "Beetle.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "DetectionField.h"
#include "Sneak_DetectionField.h"
#include "Pooling_Manager.h"

CBeetle::CBeetle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CBeetle::CBeetle(const CBeetle& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CBeetle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBeetle::Initialize(void* _pArg)
{
    CBeetle::MONSTER_DESC* pDesc = static_cast<CBeetle::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(1080.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    pDesc->fAlertRange = 7.f;
    pDesc->fChaseRange = 8.f;
    pDesc->fAttackRange = 2.f;

    pDesc->fFOVX = 60.f;
    pDesc->fFOVY = 30.f;

    pDesc->fCoolTime = 2.f;

    pDesc->_tStat.iMaxHP = 5;
    pDesc->_tStat.iHP = 5;
    pDesc->_tStat.iDamg = 1;

    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    //m_pFSM->Add_State((_uint)MONSTER_STATE::IDLE);
    //m_pFSM->Add_State((_uint)MONSTER_STATE::PATROL);
    //m_pFSM->Add_State((_uint)MONSTER_STATE::ALERT);
    //m_pFSM->Add_State((_uint)MONSTER_STATE::STANDBY);
    //m_pFSM->Add_State((_uint)MONSTER_STATE::CHASE);
    //m_pFSM->Add_State((_uint)MONSTER_STATE::ATTACK);


    m_fStepHeightThreshold = 0.3f;

    m_fDashDistance = 10.f;
    m_fBackFlyTime = 0.5f;

    m_pFSM->Add_CombatState();
    if(true == m_isSneakMode)
    {
        m_pFSM->Add_SneakState();
        m_pFSM->Set_State((_uint)MONSTER_STATE::SNEAK_IDLE);
    }
    else
    {
        m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
        m_isCombatMode = true;
    }
        
    

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, RUN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, TURN_LEFT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, TURN_RIGHT, true);
    pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CBeetle::Animation_End, this, placeholders::_1, placeholders::_2));

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CBeetle::Priority_Update(_float _fTimeDelta)
{
    if (true == IsCool())
    {
        m_fAccTime += _fTimeDelta;
        if (m_fCoolTime <= m_fAccTime)
        {
            CoolTime_Off();
        }
    }
    //if (true == m_isBackFly)
    //{
    //    m_fAccTime += _fTimeDelta;
    //    if (m_fBackFlyTime <= m_fAccTime)
    //    {
    //        m_fAccTime = 0.f;
    //    }
    //}

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CBeetle::Update(_float _fTimeDelta)
{
    if (true == m_isDead)
    {
        m_isDash = false;
    }

    if (true == m_isBackFly)
    {
        _vector vBack = -1.f * XMVector3Normalize(Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
		vBack = XMVectorSetY(vBack, 0.5f);
        Get_ActorCom()->Set_LinearVelocity(XMVector3Normalize(vBack), Get_ControllerTransform()->Get_SpeedPerSec());

        m_isBackFly = false;
    }
    if ((_uint)FLY_TAKEOFF == static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_CurrentAnimIndex())
    {
		if (0.1f >= XMVectorGetY(static_cast<CActor_Dynamic*>(Get_ActorCom())->Get_LinearVelocity()))
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FLYLAND);
    }


    if (true == m_isDash)
    {
        _float fSpeed = Get_ControllerTransform()->Get_SpeedPerSec() * 2.f;
        m_vDir.y = 0;
        Move(XMVector3Normalize(XMLoadFloat3(&m_vDir)) * fSpeed, _fTimeDelta);
        //Get_ActorCom()->Set_LinearVelocity(XMVector3Normalize(XMLoadFloat3(&m_vDir)), fSpeed);
        m_fAccDistance += fSpeed * _fTimeDelta;

        if (m_fDashDistance <= m_fAccDistance || true == IsContactToTarget())
        {
            Stop_Move();
            m_isDash = false;
            m_fAccDistance = 0.f;
            CoolTime_On();
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACKRECOVERY);
        }
    }

    if(KEY_PRESSING(KEY::CTRL))
    {
        if (KEY_DOWN(KEY::NUMPAD4))
        {
            Switch_CombatMode();
        }
    }
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CBeetle::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CBeetle::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */
#ifdef _DEBUG
    if (COORDINATE_3D == Get_CurCoord())
    {
        m_pDetectionField->Render();
        if(false == m_isCombatMode)
            m_pSneak_DetectionField->Render();
    }
#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CBeetle::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        switch (MONSTER_STATE(m_iState))
        {
        case MONSTER_STATE::IDLE:
            m_pGameInstance->Start_SFX_Distance(_wstring(L"A_sfx_beetle_idle-") + to_wstring(rand() % 8), m_pControllerTransform->Get_State(CTransform::STATE_POSITION), 70.f, 0.f, 12.f);
    
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::PATROL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::BACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::ALERT:
            m_pGameInstance->Start_SFX(_wstring(L"A_sfx_beetle_alert-") + to_wstring(rand() % 5), 50.f);
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::STANDBY:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::ATTACK:
            m_pGameInstance->Start_SFX(_wstring(L"A_sfx_beetle_attack-") + to_wstring(rand() % 4), 50.f);
            //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACKSTRIKE);
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FLY_TAKEOFF);
            m_isBackFly = true;
            break;

        case MONSTER_STATE::HIT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DAMAGE);
            break;

        case MONSTER_STATE::DEAD:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DIE);
            break;

         //잠입의 경우 idle 애니메이션 랜덤 재생
        case MONSTER_STATE::SNEAK_IDLE:
        {
            _uint iNum = (_uint)ceil(m_pGameInstance->Compute_Random(0, 4));
            switch (iNum)
            {
            case 1:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(PAUSE1);
                break;
            case 2:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(PAUSE2);
                break;
            case 3:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WAIT1);
                break;
            case 4:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WAIT2);
                break;
            }
            m_pGameInstance->Start_SFX_Distance(_wstring(L"A_sfx_beetle_idle-") + to_wstring(rand() % 8), m_pControllerTransform->Get_State(CTransform::STATE_POSITION), 70.f, 0.f, 12.f);

            //m_pGameInstance->Start_SFX(_wstring(L"A_sfx_beetle_idle-") + to_wstring(rand() % 8), 50.f);
            break;
        }

        case MONSTER_STATE::SNEAK_PATROL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::SNEAK_AWARE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(PAUSE2);
            break;
                
        case MONSTER_STATE::SNEAK_INVESTIGATE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::SNEAK_ALERT:
            m_pGameInstance->Start_SFX(_wstring(L"A_sfx_beetle_alert-") + to_wstring(rand() % 5), 50.f);
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
            break;

        case MONSTER_STATE::SNEAK_CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::SNEAK_ATTACK:
            m_pGameInstance->Start_SFX(_wstring(L"A_sfx_beetle_attack-") + to_wstring(rand() % 4), 50.f);
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(CAUGHT);
            break;

        default:
            break;
        }
    }
}

void CBeetle::Turn_Animation(_bool _isCW)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    _uint AnimIdx;
    if (true == _isCW)
        AnimIdx = CBeetle::TURN_RIGHT;
    else
        AnimIdx = CBeetle::TURN_LEFT;

    if (AnimIdx != pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
        pModelObject->Switch_Animation(AnimIdx);
}

void CBeetle::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CBeetle::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case ALERT:
        Set_AnimChangeable(true);
        break;
        
    case FLY_TAKEOFF:
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FLYLAND);
        m_isBackFly = false;
        break;

    case FLYLAND:
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(CONCERED_ATTACKREADY);
        break;

    case CONCERED_ATTACKREADY:
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACKSTRIKE);
        Attack();
        break;

    //case ATTACKSTRIKE:
    //    pModelObject->Switch_Animation(ATTACKRECOVERY);
    //    break;

    case ATTACKRECOVERY:
        Set_AnimChangeable(true);
        break;

    case PAUSE2:
        Set_AnimChangeable(true);
        break;

    case CAUGHT:
        //Set_AnimChangeable(true);
        break;

    case DAMAGE:
        Set_AnimChangeable(true);
        break;

    case DIE:
    {
        Monster_Death();
        _float3 vPos;
        _float4 vRotation;
        m_pGameInstance->MatrixDecompose(nullptr, &vRotation, &vPos, Get_FinalWorldMatrix());
		
        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Beetle_Corpse"), COORDINATE_3D, &vPos, &vRotation);
    }
        break;

    default:
        break;
    }
}

void CBeetle::Attack()
{
    if (false == m_isDash)
    {
        Stop_Rotate();
        XMStoreFloat3(&m_vDir, Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
        m_isDash = true;
        Set_PreAttack(false);
    }
}

void CBeetle::Switch_CombatMode()
{
    if(true == m_isSneakMode)
    {
        if (false == m_isCombatMode)
        {
            m_isBackFly = false;
            m_isDash = false;
            m_isCombatMode = true;
        }
        m_pSneak_DetectionField->Set_Active(false);
        Set_AnimChangeable(true);
        m_pFSM->Change_State((_uint)MONSTER_STATE::IDLE);
    }
}

void CBeetle::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    if (false == m_isSneakMode)
        __super::OnContact_Enter(_My, _Other, _ContactPointDatas);

    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        m_isContactToTarget = true;
    }
}

void CBeetle::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CBeetle::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        m_isContactToTarget = false;
    }
}

void CBeetle::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    __super::On_Hit(_pHitter, _iDamg, _vForce);
}

HRESULT CBeetle::Ready_ActorDesc(void* _pArg)
{
    CBeetle::MONSTER_DESC* pDesc = static_cast<CBeetle::MONSTER_DESC*>(_pArg);

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
    SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
    ShapeDesc->fHalfHeight = 0.3f;
    ShapeDesc->fRadius = 0.4f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    XMStoreFloat4x4(&m_matQueryShapeOffset, XMMatrixRotationY(XMConvertToRadians(90.f)));

	m_fHalfBodySize = ShapeDesc->fHalfHeight + ShapeDesc->fRadius;

 //   //마찰용 박스
 //   SHAPE_BOX_DESC* BoxDesc = new SHAPE_BOX_DESC;
	//BoxDesc->vHalfExtents = { ShapeDesc->fRadius - 0.1f, ShapeDesc->fRadius - 0.1f, ShapeDesc->fRadius + ShapeDesc->fHalfHeight - 0.1f };
 //   
 //   /* 해당 Shape의 Flag에 대한 Data 정의 */
 //   //SHAPE_DATA* ShapeData = new SHAPE_DATA;
 //   ShapeData->pShapeDesc = BoxDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
 //   ShapeData->eShapeType = SHAPE_TYPE::BOX;     // Shape의 형태.
 //   ShapeData->eMaterial = ACTOR_MATERIAL::NORESTITUTION; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔
 //   Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
 //   ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	//XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, BoxDesc->vHalfExtents.y, 0.f)); // Shape의 LocalOffset을 행렬정보로 저장.

 //   /* 최종으로 결정 된 ShapeData를 PushBack */
 //   ActorDesc->ShapeDatas.push_back(*ShapeData);

    //마찰용 박스 앞에서 레이 쏨
    m_vRayOffset.y = ShapeDesc->fRadius - 0.1f;
    m_vRayOffset.z = ShapeDesc->fRadius + ShapeDesc->fHalfHeight - 0.1f;

    m_fRayHalfWidth = ShapeDesc->fRadius - 0.1f;

    //닿은 물체의 씬 쿼리를 켜는 트리거
    SHAPE_BOX_DESC* RayBoxDesc = new SHAPE_BOX_DESC;
    RayBoxDesc->vHalfExtents = { pDesc->fAlertRange * tanf(XMConvertToRadians(pDesc->fFOVX * 0.5f)), 2.f, pDesc->fAlertRange };
	//RayBoxDesc->vHalfExtents = { ShapeDesc->fRadius, 0.1f, pDesc->fAlertRange * 0.5f };

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    //SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = RayBoxDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::BOX;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::NORESTITUTION; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
	XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.f, 0.f, 0.f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);


    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER | OBJECT_GROUP::EXPLOSION;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CBeetle::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC FSMDesc;
    FSMDesc.fAlertRange = m_fAlertRange;
    FSMDesc.fChaseRange = m_fChaseRange;
    FSMDesc.fAttackRange = m_fAttackRange;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;
    FSMDesc.eWayIndex = m_eWayIndex;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &FSMDesc)))
        return E_FAIL;

#ifdef _DEBUG
    /* Com_DebugDraw_For_Client */
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_DebugDraw_For_Client"),
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

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_DetectionField"),
        TEXT("Com_DetectionField"), reinterpret_cast<CComponent**>(&m_pDetectionField), &DetectionDesc)))
        return E_FAIL;

    /* Com_Sneak_DetectionField */
    CSneak_DetectionField::SNEAKDETECTIONFIELDDESC Sneak_DetectionDesc;
    Sneak_DetectionDesc.fRadius = 8.f;
    Sneak_DetectionDesc.fOffset = 0.f;
    Sneak_DetectionDesc.pOwner = this;
    Sneak_DetectionDesc.pTarget = m_pTarget;
#ifdef _DEBUG
    Sneak_DetectionDesc.pDraw = m_pDraw;
#endif // _DEBUG

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sneak_DetectionField"),
        TEXT("Com_Sneak_DetectionField"), reinterpret_cast<CComponent**>(&m_pSneak_DetectionField), &Sneak_DetectionDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBeetle::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("beetle_01");
	BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

    BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBeetle* CBeetle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBeetle* pInstance = new CBeetle(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBeetle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBeetle::Clone(void* _pArg)
{
    CBeetle* pInstance = new CBeetle(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBeetle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBeetle::Free()
{

    __super::Free();
}
