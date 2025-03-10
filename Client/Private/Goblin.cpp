#include "stdafx.h"
#include "Goblin.h"
#include "GameInstance.h"
#include "FSM.h"
#include "DetectionField.h"
#include "ModelObject.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"
#include "Section_Manager.h"
#include "Pooling_Manager.h"

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
    //pDesc->eStartCoord = COORDINATE_3D;
    //pDesc->isCoordChangeEnable = true;

    if (false == pDesc->isCoordChangeEnable)
    {
        if (COORDINATE_3D == pDesc->eStartCoord)
        {
            pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720.f);
            pDesc->tTransform3DDesc.fSpeedPerSec = 4.f;
        }
        else
        {
            pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
            pDesc->tTransform2DDesc.fSpeedPerSec = 80.f;
        }
    }
    else
    {
        pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720.f);
        pDesc->tTransform3DDesc.fSpeedPerSec = 4.f;

        pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
        pDesc->tTransform2DDesc.fSpeedPerSec = 80.f;
    }

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 10.f;
    pDesc->fAttackRange = 1.f;
    pDesc->fAlert2DRange = 300.f;
    pDesc->fChase2DRange = 600.f;
    pDesc->fAttack2DRange = 100.f;
    pDesc->fDelayTime = 2.f;

    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 30.f;

    pDesc->_tStat.iHP = 5;
    pDesc->_tStat.iMaxHP = 5;
    pDesc->_tStat.iDamg = 1;

	m_fHalfBodySize = 0.5f;

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

    if(false == Get_ControllerTransform()->Is_CoordChangeEnable())
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, CHASE, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, PATROL, true);
        }

        else if (COORDINATE_2D == Get_CurCoord())
        {
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_DOWN, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_UP, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL_DOWN, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL_RIGHT, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL_UP, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_DOWN, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_RIGHT, true);
            pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_UP, true);
        }
    }

    else
    {
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, CHASE, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, PATROL, true);

        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_DOWN, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_UP, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL_DOWN, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL_RIGHT, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, PATROL_UP, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_DOWN, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_RIGHT, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_UP, true);
    }

    if (COORDINATE_3D == Get_CurCoord())
        pModelObject->Set_Animation(Animation::IDLE);
    else if (COORDINATE_2D == Get_CurCoord())
    {
        Set_2D_Direction(F_DIRECTION::DOWN);
        pModelObject->Set_Animation(Animation2D::IDLE_DOWN);
    }

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
            Delay_Off();
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CGoblin::Update(_float _fTimeDelta)
{
#ifdef _DEBUG
    //if (KEY_DOWN(KEY::F5))
    //{
    //    _int iCurCoord = (_int)Get_CurCoord();
    //    (_int)iCurCoord ^= 1;
    //    _float3 vNewPos;

    //    if (iCurCoord == COORDINATE_2D)
    //        vNewPos = _float3(300.f, 6.f, 0.f);
    //    else
    //        vNewPos = _float3(-10.0f, 0.35f, -23.0f);

    //    Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);
    //}
#endif // _DEBUG

    Check_InAir_Next(_fTimeDelta);

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


    __super::Render();
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
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(PATROL);
                break;

            case MONSTER_STATE::ALERT:
                m_pGameInstance->Start_SFX(_wstring(L"A_sfx_goblin_alert_") + to_wstring(rand() % 8), 50.f);
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
                m_pGameInstance->Start_SFX_Delay(_wstring(L"A_sfx_goblin_die_") + to_wstring(rand() % 5), 0.2f, 50.f);
                break;

            default:
                break;
            }
        }

        else if (COORDINATE_2D == Get_CurCoord())
        {
            CGoblin::Animation2D eAnim = ANIM2D_LAST;
            switch (MONSTER_STATE(m_iState))
            {
            case Client::MONSTER_STATE::IDLE:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = IDLE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = IDLE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = IDLE_RIGHT;
                break;
            case Client::MONSTER_STATE::PATROL:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = PATROL_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = PATROL_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = PATROL_RIGHT;
                break;
            case Client::MONSTER_STATE::ALERT:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = ALERT_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = ALERT_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = ALERT_RIGHT;

                m_pGameInstance->Start_SFX(_wstring(L"A_sfx_goblin_alert_") + to_wstring(rand() % 8), 30.f);
                break;
            case Client::MONSTER_STATE::STANDBY:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = IDLE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = IDLE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = IDLE_RIGHT;
                break;
            case Client::MONSTER_STATE::CHASE:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = CHASE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = CHASE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = CHASE_RIGHT;
                break;
            case Client::MONSTER_STATE::HIT:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = HIT_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = HIT_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = HIT_RIGHT;
                break;
            case Client::MONSTER_STATE::DEAD:
                if (F_DIRECTION::UP == m_e2DDirection)
                    eAnim = DEATH_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = DEATH_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = DEATH_RIGHT;
                m_pGameInstance->Start_SFX(_wstring(L"A_sfx_goblin_die_") + to_wstring(rand() % 5), 50.f);                
                break;
            default:
                break;
            }

            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(eAnim);
        }
    }
}

void CGoblin::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    if(COORDINATE_3D == Get_CurCoord())
    {
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

        case DEATH:
            Monster_Death();
            //Set_AnimChangeable(true);

            ////CEffect_Manager::GetInstance()->Active_Effect(TEXT("MonsterDead"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());
            //CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("MonsterDead"), true, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

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

    else if (COORDINATE_2D == Get_CurCoord())
    {
        switch ((CGoblin::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
        {
        case ALERT_DOWN:
        case ALERT_RIGHT:
        case ALERT_UP:
            Set_AnimChangeable(true);
            break;

        case HIT_DOWN:
        case HIT_RIGHT:
        case HIT_UP:
            Set_AnimChangeable(true);
            break;

        case DEATH_DOWN:
        case DEATH_RIGHT:
        case DEATH_UP:
            Monster_Death();
            //Set_AnimChangeable(true);

            //CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Death_Burst"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), Get_ControllerTransform()->Get_WorldMatrix());

            ////확률로 전구 생성
            //if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
            //{
            //    _float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
            //    _wstring strCurSection = CSection_Manager::GetInstance()->Get_Cur_Section_Key();
            //    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
            //}

            //Event_DeleteObject(this);
            break;

        default:
            break;
        }
    }
}

HRESULT CGoblin::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == _eCoordinate)
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Animation2D::IDLE_DOWN);
    else
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Animation::IDLE);

    m_pFSM->Set_PatrolBound();

    return S_OK;
}

void CGoblin::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{

    if ((_uint)MONSTER_STATE::CHASE == m_iState)
    {
        if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
        {
            if ((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse
                && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse)
            {
                _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
                XMVectorSetY(vRepulse, -1.f);
                Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), Get_Stat().iDamg, vRepulse);
                Attack();
                m_isContactToTarget = true;
            }
        }
    }
    else 
    {
        __super::OnContact_Enter(_My, _Other, _ContactPointDatas);
    }

}

void CGoblin::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CGoblin::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if ((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse
            && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse)
        {
            if (true == m_isContactToTarget)
            {
                m_isContactToTarget = false;
            }
        }
    }
}

void CGoblin::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup && (_uint)SHAPE_USE::SHAPE_TRIGER == _My.pShapeUserData->iShapeUse)
    {
        if ((_uint)MONSTER_STATE::CHASE == m_iState)
        {
            _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
            XMVectorSetY(vRepulse, -1.f);
            Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), Get_Stat().iDamg, vRepulse);
            Attack();
        }

        __super::OnTrigger_Enter(_My, _Other);
    }
}

void CGoblin::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Stay(_My, _Other);
}

void CGoblin::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Exit(_My, _Other);
}

void CGoblin::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
    {
        if ((_uint)MONSTER_STATE::CHASE == m_iState)
        {
            Attack();
            m_isContactToTarget = true;
        }
    }

    __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
}

void CGoblin::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CGoblin::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
    {
        if (true == m_isContactToTarget)
        {
            m_isContactToTarget = false;
        }
    }
}

void CGoblin::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    __super::On_Hit(_pHitter, _iDamg, _vForce);

    m_pGameInstance->Start_SFX(_wstring(L"A_sfx_goblin_hit_") + to_wstring(rand() % 5), 50.f);
}

HRESULT CGoblin::Ready_ActorDesc(void* _pArg)
{
    CGoblin::MONSTER_DESC* pDesc = static_cast<CGoblin::MONSTER_DESC*>(_pArg);

    //if (COORDINATE_2D == pDesc->eStartCoord)
    //    return S_OK;

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
    ShapeDesc->fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    m_matQueryShapeOffset = ShapeData->LocalOffsetMatrix;

    m_fHalfBodySize = ShapeDesc->fRadius;

    //쿼리를 켜기 위한 트리거
    SHAPE_SPHERE_DESC* TriggerDesc = new SHAPE_SPHERE_DESC;
    TriggerDesc->fRadius = 0.6f; //pDesc->fAlertRange;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    ShapeData->pShapeDesc = TriggerDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, TriggerDesc->fRadius * 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
    ShapeData->FilterData.MyGroup = OBJECT_GROUP::RAY_TRIGGER;
    ShapeData->FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::INTERACTION_OBEJCT;

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);


    //공격용 구
    ///* 사용하려는 Shape의 형태를 정의 */
    //SHAPE_SPHERE_DESC* AttackShapeDesc = new SHAPE_SPHERE_DESC;
    //AttackShapeDesc->fRadius = 0.6f;

    ///* 해당 Shape의 Flag에 대한 Data 정의 */
    //ShapeData->pShapeDesc = AttackShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    //ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    //ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_CAPSULE; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    //ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    //ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    //XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, AttackShapeDesc->fRadius + 0.1f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    ///* 최종으로 결정 된 ShapeData를 PushBack */
    //ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER | OBJECT_GROUP::EXPLOSION | OBJECT_GROUP::DYNAMIC_OBJECT;

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



    if (false == Get_ControllerTransform()->Is_CoordChangeEnable() && COORDINATE_3D == Get_CurCoord())
        return S_OK;

    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = { 40.f };
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
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

    if (false == BodyDesc.isCoordChangeEnable)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            BodyDesc.strModelPrototypeTag_3D = TEXT("goblin_Rig");
            BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

            BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

            BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
            BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

            BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
            BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();
        }

        else
        {
            BodyDesc.strModelPrototypeTag_2D = TEXT("Goblin");
            BodyDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

            BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

            BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
            BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

            BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
            BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();
        }
    }
    else
    {
        BodyDesc.strModelPrototypeTag_3D = TEXT("goblin_Rig");
        BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

        BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

        BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

        BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
        BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

        BodyDesc.strModelPrototypeTag_2D = TEXT("Goblin");
        BodyDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

        BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

        BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

        BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
        BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();
    }

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
