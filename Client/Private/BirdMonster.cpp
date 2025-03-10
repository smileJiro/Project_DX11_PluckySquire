#include "stdafx.h"
#include "BirdMonster.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Effect_Manager.h"
#include "DetectionField.h"

CBirdMonster::CBirdMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CBirdMonster::CBirdMonster(const CBirdMonster& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CBirdMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBirdMonster::Initialize(void* _pArg)
{
    CBirdMonster::MONSTER_DESC* pDesc = static_cast<CBirdMonster::MONSTER_DESC*>(_pArg);
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 15.f;
    pDesc->fAttackRange = 10.f;

    pDesc->fDelayTime = 1.f;
    pDesc->fCoolTime = 3.f;

    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 60.f;

    m_tStat.iHP = 5;
    m_tStat.iMaxHP = 5;

    /* Create Test Actor (Desc를 채우는 함수니까. __super::Initialize() 전에 위치해야함. )*/
    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_FlyUnit_State();
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CBirdMonster::Animation_End, this, placeholders::_1, placeholders::_2));

    Bind_AnimEventFunc("Attack", bind(&CBirdMonster::Attack, this));

    /* Com_AnimEventGenerator */
    CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
    m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_BirdMonsterAttackAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CBirdMonster::Priority_Update(_float _fTimeDelta)
{
    if (true == m_isDelay)
    {
        m_fAccTime += _fTimeDelta;

        if (m_fDelayTime <= m_fAccTime)
        {
            Delay_Off();
            if (3 <= m_iAttackCount)
            {
                CoolTime_On();
            }
        }
    }

    if (true == m_isCool)
    {
        m_fAccTime += _fTimeDelta;

        if (m_fCoolTime <= m_fAccTime)
        {
            CoolTime_Off();
            m_iAttackCount = 0;
        }
    }


    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CBirdMonster::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CBirdMonster::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CBirdMonster::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

    if (COORDINATE_3D == Get_CurCoord())
        m_pDetectionField->Render();

#endif // _DEBUG

    __super::Render();

    /* Font Render */

    return S_OK;
}

void CBirdMonster::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        switch (MONSTER_STATE(m_iState))
        {
        case MONSTER_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::ALERT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT_EDIT);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::PATROL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::STANDBY:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACK);
            break;

        case MONSTER_STATE::HIT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(HIT);
            break;

        case MONSTER_STATE::DEAD:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DIE);
            break;

        default:
            break;
        }
    }
}

void CBirdMonster::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CBirdMonster::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case ALERT_EDIT:
        Set_AnimChangeable(true);
        break;
        
    case ATTACK:
        //딜레이 동안은 애니 전환 안됨. 따라서 상태 전환도 불가
        if (false == m_isDelay)
        {
            Set_AnimChangeable(true);
            Delay_On();
        }
        break;

    case HIT:
        Set_AnimChangeable(true);
        break;

    case DIE:
        Monster_Death();
        //Set_AnimChangeable(true);

        //CEffect_Manager::GetInstance()->Active_Effect(TEXT("MonsterDead"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());

        ////확률로 전구 생성
        //if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
        //{
        //    _float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
        //    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bulb"), COORDINATE_3D, &vPos);
        //}

        //Event_DeleteObject(this);
        break;

    default:
        break;
    }
}

void CBirdMonster::Attack()
{   
    if (false == m_isDelay && false == m_isCool)
    {
        _float3 vScale, vPosition;
        _float4 vRotation;
        COORDINATE eCoord = Get_CurCoord();

        if (false == m_pGameInstance->MatrixDecompose(&vScale, &vRotation, &vPosition, m_pControllerTransform->Get_WorldMatrix()))
            return;

        if (COORDINATE_3D == eCoord)
        {
            vPosition.y += vScale.y * 0.8f;
            XMStoreFloat4(&vRotation, m_pGameInstance->Direction_To_Quaternion(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pTarget->Get_FinalPosition() - Get_FinalPosition()));
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Projectile_BirdMonster"), eCoord, &vPosition, &vRotation);
        }
        else if (COORDINATE_2D == eCoord)
        {
            //공격 위치 맞추기
            switch (Get_2DDirection())
            {
            case Client::F_DIRECTION::LEFT:
                vPosition.x -= 50.f;
                vPosition.y += 20.f;
                break;
            case Client::F_DIRECTION::RIGHT:
                vPosition.x += 50.f;
                vPosition.y += 40.f;
                break;
            case Client::F_DIRECTION::UP:
                vPosition.y += 70.f;
                break;
            case Client::F_DIRECTION::DOWN:
                vPosition.y -= 30.f;
                break;
            default:
                break;
            }

            _float fAngle = m_pGameInstance->Get_Angle_Between_Vectors(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pTarget->Get_FinalPosition() - XMLoadFloat3(&vPosition));
            fAngle=Restrict_2DRangeAttack_Angle(fAngle);
            XMStoreFloat4(&vRotation, XMQuaternionRotationAxis(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(fAngle)));

            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Projectile_BirdMonster"), eCoord, &vPosition, &vRotation);

        }
        ++m_iAttackCount;

    }
}

HRESULT CBirdMonster::Ready_ActorDesc(void* _pArg)
{
    CBirdMonster::MONSTER_DESC* pDesc = static_cast<CBirdMonster::MONSTER_DESC*>(_pArg);

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
    ShapeDesc->fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

	m_fHalfBodySize = ShapeDesc->fRadius;


    //쿼리를 켜기 위한 트리거
    SHAPE_CAPSULE_DESC* TriggerDesc = new SHAPE_CAPSULE_DESC;
    TriggerDesc->fHalfHeight = 0.3f;
    TriggerDesc->fRadius = 0.6f; //pDesc->fAlertRange;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    ShapeData->pShapeDesc = TriggerDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, TriggerDesc->fRadius * 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
    ShapeData->FilterData.MyGroup = OBJECT_GROUP::RAY_TRIGGER;
    ShapeData->FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::INTERACTION_OBEJCT;

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

HRESULT CBirdMonster::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC FSMDesc;
    FSMDesc.fAlertRange = m_fAlertRange;
    FSMDesc.fChaseRange = m_fChaseRange;
    FSMDesc.fAttackRange = m_fAttackRange;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;

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

    return S_OK;
}

HRESULT CBirdMonster::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("Namastarling_Rig_01");
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

CBirdMonster* CBirdMonster::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBirdMonster* pInstance = new CBirdMonster(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBirdMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBirdMonster::Clone(void* _pArg)
{
    CBirdMonster* pInstance = new CBirdMonster(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBirdMonster");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBirdMonster::Free()
{

    __super::Free();
}
