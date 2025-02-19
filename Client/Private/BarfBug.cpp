#include "stdafx.h"
#include "BarfBug.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Projectile_BarfBug.h"
#include "DetectionField.h"
#include "Section_Manager.h"
#include "Effect2D_Manager.h"


CBarfBug::CBarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CBarfBug::CBarfBug(const CBarfBug& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CBarfBug::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBarfBug::Initialize(void* _pArg)
{
    CBarfBug::MONSTER_DESC* pDesc = static_cast<CBarfBug::MONSTER_DESC*>(_pArg);
    pDesc->isCoordChangeEnable = true;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 100.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 8.f;
    pDesc->fAlert2DRange = 300.f;   //*50 하면 될듯?
    pDesc->fChase2DRange = 800.f;
    pDesc->fAttack2DRange = 400.f;
    pDesc->fDelayTime = 1.f;
    pDesc->fCoolTime = 3.f;

    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 30.f;

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

    m_pFSM->Add_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::PATROL);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ALERT);
    m_pFSM->Add_State((_uint)MONSTER_STATE::STANDBY);
    m_pFSM->Add_State((_uint)MONSTER_STATE::CHASE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ATTACK);
    m_pFSM->Add_State((_uint)MONSTER_STATE::HIT);
    m_pFSM->Add_State((_uint)MONSTER_STATE::DEAD);
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Set_PatrolBound();

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_DOWN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_UP, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, WALK_DOWN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, WALK_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, WALK_UP, true);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, WALK, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, TURN_LEFT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, TURN_RIGHT, true);

    pModelObject->Set_Animation(Animation::IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CBarfBug::Animation_End, this, placeholders::_1, placeholders::_2));

    Bind_AnimEventFunc("Attack", bind(&CBarfBug::Attack, this));

    /* Com_AnimEventGenerator */
    CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
    m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_BarfBugAttackAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
    m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_BarfBug2DAttackAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("2DAnimEventGenerator"), m_pAnimEventGenerator);


    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }
    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CBarfBug::Priority_Update(_float _fTimeDelta)
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

    __super::Priority_Update(_fTimeDelta); 
}

void CBarfBug::Update(_float _fTimeDelta)
{
#ifdef _DEBUG
    //if (KEY_DOWN(KEY::F5))
    //{
    //    _int iCurCoord = (_int)Get_CurCoord();
    //    (_int)iCurCoord ^= 1;
    //    _float3 vNewPos;

    //    if (iCurCoord == COORDINATE_2D)
    //        vNewPos = _float3(500.f, 6.f, 0.f);
    //    else
    //        vNewPos = _float3(-10.0f, 0.35f, -23.0f);

    //    Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);
    //}
#endif // _DEBUG

    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CBarfBug::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CBarfBug::Render()
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
#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CBarfBug::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    __super::OnContact_Enter(_My, _Other, _ContactPointDatas);
}

void CBarfBug::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{

    __super::OnContact_Stay(_My, _Other, _ContactPointDatas);
}

void CBarfBug::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    __super::OnContact_Exit(_My, _Other, _ContactPointDatas);
}

void CBarfBug::On_Hit(CGameObject* _pHitter, _int _iDamg)
{
    cout << "Barfbug hit" << endl;
    __super::On_Hit(_pHitter, _iDamg);
}

HRESULT CBarfBug::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    m_pFSM->Set_PatrolBound();

    return S_OK;
}

void CBarfBug::Change_Animation()
{
    if (m_iState != m_iPreState)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            switch (MONSTER_STATE(m_iState))
            {
            case MONSTER_STATE::IDLE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
                break;

            case MONSTER_STATE::PATROL:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
                break;

            case MONSTER_STATE::ALERT:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
                break;

            case MONSTER_STATE::STANDBY:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
                break;

            case MONSTER_STATE::CHASE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
                break;

            case MONSTER_STATE::ATTACK:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(BARF);
                break;

            case MONSTER_STATE::HIT:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DAMAGE);
                break;

            case MONSTER_STATE::DEAD:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DIE);
                break;

            default:
                break;
            }
        }

        else if (COORDINATE_2D == Get_CurCoord())
        {
            CBarfBug::Animation2D eAnim = ANIM2D_LAST;
            switch (MONSTER_STATE(m_iState))
            {
            case MONSTER_STATE::IDLE:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = IDLE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = IDLE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = IDLE_RIGHT;
                break;

            case MONSTER_STATE::PATROL:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = WALK_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = WALK_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = WALK_RIGHT;
                break;

            case MONSTER_STATE::ALERT:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = ALERT_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = ALERT_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = ALERT_RIGHT;
                break;

            case MONSTER_STATE::STANDBY:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = IDLE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = IDLE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = IDLE_RIGHT;
                break;

            case MONSTER_STATE::CHASE:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = WALK_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = WALK_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = WALK_RIGHT;
                break;

            case MONSTER_STATE::ATTACK:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = ATTACK_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = ATTACK_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = ATTACK_RIGHT;
                break;

            case MONSTER_STATE::HIT:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = HIT_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = HIT_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = HIT_RIGHT;
                break;

            case MONSTER_STATE::DEAD:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = DEATH_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = DEATH_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = DEATH_RIGHT;
                break;

            default:
                break;
            }

            if (ANIM2D_LAST == eAnim)
                return;

            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(eAnim);
        }
    }
}

void CBarfBug::Attack()
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
            vPosition.y += vScale.y * 0.5f;
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Projectile_BarfBug"), eCoord, &vPosition, &vRotation);
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

            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Projectile_BarfBug"), eCoord, &vPosition, &vRotation);
        }
        ++m_iAttackCount;

    }
}

void CBarfBug::Turn_Animation(_bool _isCW)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    _uint AnimIdx;
    if (true == _isCW)
        AnimIdx = TURN_RIGHT;
    else
        AnimIdx = TURN_LEFT;

    if (AnimIdx != pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(AnimIdx);
}

void CBarfBug::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    if (COORDINATE_3D == _eCoord)
    {
        switch ((CBarfBug::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
        {
        case ALERT:
            Set_AnimChangeable(true);
            break;

        case BARF:
            //딜레이 동안은 애니 전환 안됨. 따라서 상태 전환도 불가
            if (false == m_isDelay)
            {
                Set_AnimChangeable(true);
                Delay_On();
            }
            break;

        case DAMAGE:
            Set_AnimChangeable(true);
            break;

        case DIE:
            Set_AnimChangeable(true);
            //풀링에 넣을 시 변경
            //Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);

            Event_DeleteObject(this);
            break;

        default:
            break;
        }
    }

    else if (COORDINATE_2D == _eCoord)
    {
        switch ((CBarfBug::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
        {
        case ALERT_DOWN:
        case ALERT_RIGHT:
        case ALERT_UP:
            Set_AnimChangeable(true);
            break;

        case ATTACK_DOWN:
        case ATTACK_RIGHT:
        case ATTACK_UP:
            if (false == m_isDelay)
            {
                Set_AnimChangeable(true);
                Delay_On();
            }
            break;

        case HIT_DOWN:
        case HIT_RIGHT:
        case HIT_UP:
            Set_AnimChangeable(true);
            break;

        case DEATH_DOWN:
        case DEATH_RIGHT:
        case DEATH_UP:
            Set_AnimChangeable(true);
            //풀링에 넣을 시 변경
            //Event_ChangeMonsterState(MONSTER_STATE::IDLE, m_pFSM);

            CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Death_Burst"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), Get_ControllerTransform()->Get_WorldMatrix());

            Event_DeleteObject(this);
            break;

        default:
            break;
        }
    }
}

HRESULT CBarfBug::Ready_ActorDesc(void* _pArg)
{
    CBarfBug::MONSTER_DESC* pDesc = static_cast<CBarfBug::MONSTER_DESC*>(_pArg);

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
    ShapeDesc->fHalfHeight = 0.15f;
    ShapeDesc->fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_CAPSULE; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 1.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    //마찰용 박스
    SHAPE_BOX_DESC* BoxDesc = new SHAPE_BOX_DESC;
    BoxDesc->vHalfExtents = { 0.3f, 0.1f, 0.3f };

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    //SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = BoxDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::BOX;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_FOOT;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, BoxDesc->vHalfExtents.y, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    //   //맵 오브젝트가 앞에 있으면 탐지를 막기 위한 트리거
    //   SHAPE_CAPSULE_DESC* TriggerDesc = new SHAPE_CAPSULE_DESC;
    //   TriggerDesc->fHalfHeight = 0.3f;
    //   TriggerDesc->fRadius = 0.3f;

    //   /* 해당 Shape의 Flag에 대한 Data 정의 */
    //   ShapeData->pShapeDesc = TriggerDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    //   ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    //   ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    //   ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
       //XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, TriggerDesc->fRadius * 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    //   /* 최종으로 결정 된 ShapeData를 PushBack */
    //   ActorDesc->ShapeDatas.push_back(*ShapeData);

       /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CBarfBug::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC FSMDesc;
    FSMDesc.fAlertRange = m_fAlertRange;
    FSMDesc.fChaseRange = m_fChaseRange;
    FSMDesc.fAttackRange = m_fAttackRange;
    FSMDesc.fAlert2DRange = m_fAlert2DRange;
    FSMDesc.fChase2DRange = m_fChase2DRange;
    FSMDesc.fAttack2DRange = m_fAttack2DRange;
    FSMDesc.isMelee = false;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;

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

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 50.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius-10.f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID= OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBarfBug::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();
    BodyDesc.strModelPrototypeTag_2D = TEXT("BarferBug");
    BodyDesc.strModelPrototypeTag_3D = TEXT("barfBug_Rig");
	BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
    BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBarfBug* CBarfBug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBarfBug* pInstance = new CBarfBug(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBarfBug::Clone(void* _pArg)
{
    CBarfBug* pInstance = new CBarfBug(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBarfBug::Free()
{
    __super::Free();
}
