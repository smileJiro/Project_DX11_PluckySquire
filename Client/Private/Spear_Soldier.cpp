#include "stdafx.h"
#include "Spear_Soldier.h"
#include "GameInstance.h"
#include "FSM.h"
#include "Soldier_Spear.h"
#include "Soldier_Shield.h"
#include "DetectionField.h"
#include "Sneak_DetectionField.h"
#include "Effect2D_Manager.h"

CSpear_Soldier::CSpear_Soldier(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CSpear_Soldier::CSpear_Soldier(const CSpear_Soldier& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CSpear_Soldier::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpear_Soldier::Initialize(void* _pArg)
{
    CSpear_Soldier::SPEARSOLDIER_DESC* pDesc = static_cast<CSpear_Soldier::SPEARSOLDIER_DESC*>(_pArg);
    //pDesc->isCoordChangeEnable = true;

    if(false == pDesc->isCoordChangeEnable)
    {
        if (COORDINATE_3D == pDesc->eStartCoord)
        {
            pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
            pDesc->tTransform3DDesc.fSpeedPerSec = 4.f;
        }
        else
        {
            pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
            pDesc->tTransform2DDesc.fSpeedPerSec = 150.f;
        }
    }
    else
    {
        pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
        pDesc->tTransform3DDesc.fSpeedPerSec = 4.f;
        pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
        pDesc->tTransform2DDesc.fSpeedPerSec = 150.f;
    }

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 2.f;
    pDesc->fAlert2DRange = 300.f;
    pDesc->fChase2DRange = 700.f;
    pDesc->fAttack2DRange = 250.f;
    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 30.f;

    pDesc->fCoolTime = 2.f;

    pDesc->_tStat.iHP = 5;
    pDesc->_tStat.iMaxHP = 5;
    pDesc->_tStat.iDamg = 1;

    if (true == pDesc->isSneakMode)
    {
        m_fAttackRange = m_fAlertRange;
    }


    /* Create Test Actor (Desc를 채우는 함수니까. __super::Initialize() 전에 위치해야함. )*/
    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;


	if (true == pDesc->isC6BossMode)
    {
        m_isC6BossMode = pDesc->isC6BossMode;
        Set_2DDirection(pDesc->eDirection);
        m_fChase2DRange *= 3.f;
        m_fCoolTime = 1.5f;
    }

    if (COORDINATE_3D == Get_CurCoord())
    {
        m_fDashDistance = 10.f;
    }
    else if (COORDINATE_2D == Get_CurCoord())
    {
        m_fDashDistance = 200.f;
    }

    if(false == m_isSneakMode)
    {
        if(false == m_isC6BossMode)
        {
            m_pFSM->Add_State((_uint)MONSTER_STATE::IDLE);
            m_pFSM->Add_State((_uint)MONSTER_STATE::PATROL);
            m_pFSM->Add_State((_uint)MONSTER_STATE::ALERT);
            m_pFSM->Add_State((_uint)MONSTER_STATE::STANDBY);
            m_pFSM->Add_State((_uint)MONSTER_STATE::CHASE);
            m_pFSM->Add_State((_uint)MONSTER_STATE::ATTACK);
            m_pFSM->Add_State((_uint)MONSTER_STATE::HIT);
            m_pFSM->Add_State((_uint)MONSTER_STATE::DEAD);
            m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
        }
        else
        {
            //move의 경우 일단 방향으로 쭉 이동하다 블록커와 충돌하면 상태전환하는거로
            //점프 장소까지 이동 후 블록커와 충돌을 순간 끄고 점프가 끝나면 다시 켜는 작업을 수행
            m_pFSM->Add_C6_State();
            m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
            //Event_ChangeMonsterState(MONSTER_STATE::JUMP, m_pFSM);
            Event_ChangeMonsterState(MONSTER_STATE::MOVE, m_pFSM);
        }
    }
    else if(true == m_isSneakMode)
    {
        m_pFSM->Add_SneakState();
        m_pFSM->Set_State((_uint)MONSTER_STATE::SNEAK_IDLE);
    }

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    if (COORDINATE_3D == Get_CurCoord())
    {
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, WALK, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, FAST_WALK, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, CHASE, true);

        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, BLOCK_HOLD_LOOP, true);

        pModelObject->Set_Animation(IDLE);
    }
    else if (COORDINATE_2D == Get_CurCoord())
    {
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_DOWN, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_UP, true);

        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_DOWN, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_RIGHT, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, CHASE_UP, true);

        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, WALK_DOWN, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, WALK_RIGHT, true);
        pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, WALK_UP, true);

        pModelObject->Set_Animation(IDLE_DOWN);
    }

    pModelObject->Register_OnAnimEndCallBack(bind(&CSpear_Soldier::Animation_End, this, placeholders::_1, placeholders::_2));

    Bind_AnimEventFunc("Attack", bind(&CSpear_Soldier::Attack, this));

    CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};

    if(COORDINATE_3D==Get_CurCoord())
    /* Com_AnimEventGenerator */
    {
        tAnimEventDesc.pReceiver = this;
        tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
        m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_SoldierAttackAnimEvent"), &tAnimEventDesc));
        Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);
    }

    else if (COORDINATE_2D == Get_CurCoord())
    {
        /* Com_AnimEventGenerator */
        tAnimEventDesc.pReceiver = this;
        tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
        m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_SoldierAttack2DAnimEvent"), &tAnimEventDesc));
        Add_Component(TEXT("2DAnimEventGenerator"), m_pAnimEventGenerator);
    }

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CSpear_Soldier::Priority_Update(_float _fTimeDelta)
{
    if (true == IsCool())
    {
        m_fAccTime += _fTimeDelta;
        if (m_fCoolTime <= m_fAccTime)
        {
            CoolTime_Off();
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CSpear_Soldier::Update(_float _fTimeDelta)
{
    if (true == m_isDead)
    {
        m_isDash = false;
    }

    if (true == m_isDash)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            m_vDir.y = 0;
            Get_ActorCom()->Set_LinearVelocity(XMVector3Normalize(XMLoadFloat3(&m_vDir)), Get_ControllerTransform()->Get_SpeedPerSec() * 2.f);
        }
        else if (COORDINATE_2D == Get_CurCoord())
        {
            Get_ControllerTransform()->Go_Direction(XMLoadFloat3(&m_vDir), Get_ControllerTransform()->Get_SpeedPerSec() * 5.f, _fTimeDelta);
        }
        /*m_fAccDistance += Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta;

        if (m_fDashDistance <= m_fAccDistance)
        {
            m_fAccDistance = 0.f;
            m_isDash = false;
            Stop_MoveXZ();
        }*/
    }

    if (true == m_isJump)
    {
        m_pGameInstance->Erase_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::BLOCKER);
    }

    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CSpear_Soldier::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CSpear_Soldier::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    if (COORDINATE_3D == Get_CurCoord())
    {
        m_pDetectionField->Render();

        if (true == m_isSneakMode)
            m_pSneak_DetectionField->Render();
    }
#endif // _DEBUG

    __super::Render();

    /* Font Render */

    return S_OK;
}

void CSpear_Soldier::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CSpear_Soldier::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CSpear_Soldier::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CSpear_Soldier::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
}

void CSpear_Soldier::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    if(false == m_isC6BossMode)
    {
        __super::On_Hit(_pHitter, _iDamg, _vForce);
    }

    //챕터 6 보스전에서는 hit 상태와 넉백 없음
    else
    {
        if ((_uint)MONSTER_STATE::DEAD == m_iState)
            return;

        m_tStat.iHP -= _iDamg;
        if (m_tStat.iHP < 0)
        {
            m_tStat.iHP = 0;
        }
        cout << m_tStat.iHP << endl;
        if (0 >= m_tStat.iHP)
        {
            Set_AnimChangeable(true);
            if (0 < m_p2DColliderComs.size())
            {
                m_p2DColliderComs[0]->Set_Active(false);
            }

            m_isDash = false;
            Event_ChangeMonsterState(MONSTER_STATE::DEAD, m_pFSM);
        }
        else
        {
            _matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();

            _wstring strFXTag = L"Hit_FX";
            strFXTag += to_wstring((_int)ceil(m_pGameInstance->Compute_Random(0.f, 5.f)));
            CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, Get_Include_Section_Name(), matFX);
            matFX.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
            switch ((_uint)ceil(m_pGameInstance->Compute_Random(0.f, 4.f)))
            {
            case 1:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words1"), Get_Include_Section_Name(), matFX);
                break;

            case 2:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words2"), Get_Include_Section_Name(), matFX);
                break;

            case 3:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words4"), Get_Include_Section_Name(), matFX);
                break;

            case 4:
                CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_Words5"), Get_Include_Section_Name(), matFX);
                break;
            }
        }
    }
}

void CSpear_Soldier::Attack()
{
    if (false == m_isDash)
    {
        if(COORDINATE_3D==Get_CurCoord())
        {
            Stop_Rotate();
            XMStoreFloat3(&m_vDir, Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
            m_isDash = true;
            Set_PreAttack(false);
        }
        else if (COORDINATE_2D == Get_CurCoord())
        {
            XMStoreFloat3(&m_vDir, XMVector3Normalize(m_pTarget->Get_FinalPosition() - Get_FinalPosition()));
            //Change_Dir(false);
            m_isDash = true;
        }
    }
}

void CSpear_Soldier::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        if(COORDINATE_3D == Get_CurCoord())
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
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(BLOCK_HOLD_UP);
                break;

            case MONSTER_STATE::CHASE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(CHASE);
                break;

            case MONSTER_STATE::ATTACK:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DASH_ATTACK_STARTUP);
                break;

            case MONSTER_STATE::SNEAK_IDLE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
                break;

            case MONSTER_STATE::SNEAK_PATROL:
            case MONSTER_STATE::SNEAK_BACK:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
                break;

            case MONSTER_STATE::SNEAK_AWARE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
                Set_AnimChangeable(true);
                break;

            case MONSTER_STATE::SNEAK_INVESTIGATE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FAST_WALK);
                break;

            case MONSTER_STATE::SNEAK_ALERT:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
                break;

            case MONSTER_STATE::SNEAK_CHASE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(CHASE);
                break;

            case MONSTER_STATE::SNEAK_ATTACK:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ARREST);
                break;

            case MONSTER_STATE::HIT:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(HIT_FRONT);
                break;

            case MONSTER_STATE::DEAD:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DEATH_02_EDIT);
                break;

            default:
                break;
            }
        }

        else if (COORDINATE_2D == Get_CurCoord())
        {
            CSpear_Soldier::Animation2D eAnim = ANIM2D_LAST;
            switch (MONSTER_STATE(m_iState))
            {
            case MONSTER_STATE::IDLE:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = IDLE_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = IDLE_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = IDLE_RIGHT;
                break;

            case MONSTER_STATE::PATROL:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = WALK_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = WALK_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = WALK_RIGHT;
                break;

            case MONSTER_STATE::ALERT:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = ALERT_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = ALERT_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = ALERT_RIGHT;
                break;

            case MONSTER_STATE::STANDBY:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = IDLE_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = IDLE_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = IDLE_RIGHT;
                break;

            case MONSTER_STATE::CHASE:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = CHASE_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = CHASE_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = CHASE_RIGHT;
                break;

            case MONSTER_STATE::MOVE:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim =WALK_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim =WALK_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim =WALK_RIGHT;
                break;

            case MONSTER_STATE::JUMP:
                if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = JUMP_RISE_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = JUMP_RISE_RIGHT;

                m_isJump = true;
                break;

            case MONSTER_STATE::ATTACK:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = DASHATTACK_INTO_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = DASHATTACK_INTO_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = DASHATTACK_INTO_RIGHT;
                break;

            case MONSTER_STATE::HIT:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = HIT_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = HIT_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
                    eAnim = HIT_RIGHT;
                break;

            case MONSTER_STATE::DEAD:
                if (E_DIRECTION::UP == Get_2DDirection())
                    eAnim = DEATH_UP;
                else if (E_DIRECTION::DOWN == Get_2DDirection())
                    eAnim = DEATH_DOWN;
                else if (E_DIRECTION::RIGHT == Get_2DDirection() || E_DIRECTION::LEFT == Get_2DDirection())
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

void CSpear_Soldier::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    if (COORDINATE_3D == Get_CurCoord())
    {
        switch ((CSpear_Soldier::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
        {
        case ALERT:
            Set_AnimChangeable(true);
            break;

        case DASH_ATTACK_STARTUP:
            pModelObject->Switch_Animation(DASH_ATTACK_LOOP);
            break;

        case DASH_ATTACK_LOOP:
            m_isDash = false;
            Stop_MoveXZ();
            pModelObject->Switch_Animation(DASH_ATTACK_RECOVERY);
            CoolTime_On();
            break;

        case DASH_ATTACK_RECOVERY:
            Set_AnimChangeable(true);
            break;

        case BLOCK_HOLD_UP:
            pModelObject->Switch_Animation(BLOCK_HOLD_LOOP);
            break;

        case ARREST:
            Set_AnimChangeable(true);
            break;

        case HIT_FRONT:
            Set_AnimChangeable(true);
            break;

        case DEATH_02_EDIT:
            Monster_Death();
            break;

        default:
            break;
        }
    }
    else if (COORDINATE_2D == Get_CurCoord())
    {
        switch ((CSpear_Soldier::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
        {
            //if (MONSTER_STATE::MOVE == MONSTER_STATE(m_iState))
            //{
            //    case WALK_DOWN:
            //    case WALK_RIGHT:
            //    case WALK_UP:
            //        Set_AnimChangeable(true);
            //        break;
            //}

        case ALERT_DOWN:
        case ALERT_RIGHT:
        case ALERT_UP:
            Set_AnimChangeable(true);
            break;

        case DASHATTACK_INTO_DOWN:
        case DASHATTACK_INTO_RIGHT:
        case DASHATTACK_INTO_UP:
            switch (Get_2DDirection())
            {
            case E_DIRECTION::LEFT:
            case E_DIRECTION::RIGHT:
                pModelObject->Switch_Animation(DASHATTACK_LOOP_RIGHT);
                break;
            case E_DIRECTION::UP:
                pModelObject->Switch_Animation(DASHATTACK_LOOP_UP);
                break;
            case E_DIRECTION::DOWN:
                pModelObject->Switch_Animation(DASHATTACK_LOOP_DOWN);
                break;
            default:
                break;
            }
            break;

        case DASHATTACK_LOOP_DOWN:
            m_isDash = false;
            CoolTime_On();
            pModelObject->Switch_Animation(DASHATTACK_OUT_DOWN);
            break;
        case DASHATTACK_LOOP_RIGHT:
            m_isDash = false;
            CoolTime_On();
            pModelObject->Switch_Animation(DASHATTACK_OUT_RIGHT);
            break;
        case DASHATTACK_LOOP_UP:
            //애니메이션 없어서 그냥 바로 idle 애니메이션으로 전환함
            m_isDash = false;
            CoolTime_On();
            pModelObject->Switch_Animation(IDLE_UP);
            Set_AnimChangeable(true);
            break;

        case DASHATTACK_OUT_DOWN:
            Set_AnimChangeable(true);
            break;
        case DASHATTACK_OUT_RIGHT:
            Set_AnimChangeable(true);
            break;

        case JUMP_RISE_DOWN:
            pModelObject->Switch_Animation(JUMP_FALL_DOWN);
            break;
        case JUMP_RISE_RIGHT:
            pModelObject->Switch_Animation(JUMP_FALL_RIGHT);
            break;

        case JUMP_FALL_DOWN:
            Set_AnimChangeable(true);
            m_isJump = false;
            m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::BLOCKER);
            break;
        case JUMP_FALL_RIGHT:
            Set_AnimChangeable(true);
            m_isJump = false;
            m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::BLOCKER);
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
            break;

        default:
            break;
        }
    }
}

HRESULT CSpear_Soldier::Ready_ActorDesc(void* _pArg)
{
    CSpear_Soldier::MONSTER_DESC* pDesc = static_cast<CSpear_Soldier::MONSTER_DESC*>(_pArg);

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
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, ShapeDesc->fHalfHeight + ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    m_matQueryShapeOffset = ShapeData->LocalOffsetMatrix;

    m_fHalfBodySize = ShapeDesc->fRadius;


    if(false == pDesc->isSneakMode)
    {
        //쿼리를 켜기 위한 트리거
        SHAPE_CAPSULE_DESC* TriggerDesc = new SHAPE_CAPSULE_DESC;
        TriggerDesc->fRadius = 0.3f;
        TriggerDesc->fHalfHeight = 0.6f;

        /* 해당 Shape의 Flag에 대한 Data 정의 */
        ShapeData->pShapeDesc = TriggerDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
        ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
        ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
        ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
        ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
        XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, TriggerDesc->fHalfHeight + TriggerDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
        ShapeData->FilterData.MyGroup = OBJECT_GROUP::RAY_TRIGGER;
        ShapeData->FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::INTERACTION_OBEJCT;

        /* 최종으로 결정 된 ShapeData를 PushBack */
        ActorDesc->ShapeDatas.push_back(*ShapeData);
    }
    else if(true == pDesc->isSneakMode)
    {
        m_vRayOffset.y = ShapeDesc->fRadius - 0.1f;
        m_vRayOffset.z = ShapeDesc->fRadius - 0.1f;

        m_fRayHalfWidth = ShapeDesc->fRadius;

        //닿은 물체의 씬 쿼리를 켜는 트리거
        SHAPE_BOX_DESC* RayBoxDesc = new SHAPE_BOX_DESC;
        //RayBoxDesc->vHalfExtents = { pDesc->fAlertRange * tanf(XMConvertToRadians(pDesc->fFOVX * 0.5f)) * 0.5f, 1.f, pDesc->fAlertRange * 0.5f };
        RayBoxDesc->vHalfExtents = { pDesc->fAlertRange * tanf(XMConvertToRadians(pDesc->fFOVX * 0.5f)), 1.f, pDesc->fAlertRange };
        //RayBoxDesc->vHalfExtents = { ShapeDesc->fRadius, 0.1f, pDesc->fAlertRange * 0.5f };

        /* 해당 Shape의 Flag에 대한 Data 정의 */
        //SHAPE_DATA* ShapeData = new SHAPE_DATA;
        ShapeData->pShapeDesc = RayBoxDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
        ShapeData->eShapeType = SHAPE_TYPE::BOX;     // Shape의 형태.
        ShapeData->eMaterial = ACTOR_MATERIAL::NORESTITUTION; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
        ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
        //XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.f, RayBoxDesc->vHalfExtents.y, RayBoxDesc->vHalfExtents.z)); // Shape의 LocalOffset을 행렬정보로 저장.
        XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.f, RayBoxDesc->vHalfExtents.y, 0.f)); // Shape의 LocalOffset을 행렬정보로 저장.

        /* 최종으로 결정 된 ShapeData를 PushBack */
        ActorDesc->ShapeDatas.push_back(*ShapeData);
    }

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER| OBJECT_GROUP::EXPLOSION;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CSpear_Soldier::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC FSMDesc;
    FSMDesc.fAlertRange = m_fAlertRange;
    FSMDesc.fChaseRange = m_fChaseRange;
    FSMDesc.fAttackRange = m_fAttackRange;
    FSMDesc.fAlert2DRange = m_fAlert2DRange;
    FSMDesc.fChase2DRange = m_fChase2DRange;
    FSMDesc.fAttack2DRange = m_fAttack2DRange;
    FSMDesc.isMelee = true;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;
    FSMDesc.eWayIndex = m_eWayIndex;
    FSMDesc.eMoveNextState = MONSTER_STATE::JUMP;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &FSMDesc)))
        return E_FAIL;


    if(true == Get_ControllerTransform()->Is_CoordChangeEnable() || COORDINATE_3D==Get_CurCoord())
    {
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

        if (true == m_isSneakMode)
        {
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
        }
    }

    if (true == Get_ControllerTransform()->Is_CoordChangeEnable() || COORDINATE_2D == Get_CurCoord())
    {
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
    }
    return S_OK;
}

HRESULT CSpear_Soldier::Ready_PartObjects()
{
	if (COORDINATE_3D == Get_CurCoord())
    {
        /* Part Body */
        CModelObject::MODELOBJECT_DESC BodyDesc{};

        BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
        BodyDesc.iCurLevelID = m_iCurLevelID;
        BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

        BodyDesc.strModelPrototypeTag_3D = TEXT("humgrump_troop_Rig_GT");
        BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

        BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

        BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
        BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
        BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
        if (nullptr == m_PartObjects[PART_BODY])
            return E_FAIL;


        /* Part_Weapon */
        CSoldier_Spear::SOLDIER_SPEAR_DESC SpearDesc{};
        SpearDesc.strModelPrototypeTag_3D = TEXT("Spear");
        SpearDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

        SpearDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");

        SpearDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

        SpearDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

        SpearDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        SpearDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
        SpearDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
        SpearDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

        SpearDesc.iRenderGroupID_3D = RG_3D;
        SpearDesc.iPriorityID_3D = PR3D_GEOMETRY;

        SpearDesc.pParent = this;

        m_PartObjects[PART_RIGHT_WEAPON] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_Spear"), &SpearDesc));
        if (nullptr == m_PartObjects[PART_RIGHT_WEAPON])
            return E_FAIL;

        C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D));
        m_PartObjects[PART_RIGHT_WEAPON]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_hand_attach_r"));
        m_PartObjects[PART_RIGHT_WEAPON]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });
        //Set_PartActive(PART_RIGHT_WEAPON, false);


        CSoldier_Shield::SOLDIER_SHIELD_DESC ShieldDesc{};
        ShieldDesc.strModelPrototypeTag_3D = TEXT("Shield");
        ShieldDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

        ShieldDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");

        ShieldDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

        ShieldDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

        ShieldDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        ShieldDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
        ShieldDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
        ShieldDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

        ShieldDesc.iRenderGroupID_3D = RG_3D;
        ShieldDesc.iPriorityID_3D = PR3D_GEOMETRY;

        ShieldDesc.pParent = this;

        m_PartObjects[PART_LEFT_WEAPON] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_Shield"), &ShieldDesc));
        if (nullptr == m_PartObjects[PART_LEFT_WEAPON])
            return E_FAIL;

        static_cast<CPartObject*>(m_PartObjects[PART_LEFT_WEAPON])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_hand_attach_l"));
        m_PartObjects[PART_LEFT_WEAPON]->Get_ControllerTransform()->RotationXYZ(_float3{ XMConvertToRadians(-90.f), XMConvertToRadians(-90.f), 0.f });
    }

    else if (COORDINATE_2D == Get_CurCoord())
    {
        /* Part Body */
        CModelObject::MODELOBJECT_DESC BodyDesc{};

        BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
        BodyDesc.iCurLevelID = m_iCurLevelID;
        BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

        BodyDesc.strModelPrototypeTag_2D = TEXT("Spear_Soldier");
        BodyDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

        BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

        BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
        BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
        BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();

        m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
        if (nullptr == m_PartObjects[PART_BODY])
            return E_FAIL;
    }

    return S_OK;
}

CSpear_Soldier* CSpear_Soldier::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSpear_Soldier* pInstance = new CSpear_Soldier(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSpear_Soldier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSpear_Soldier::Clone(void* _pArg)
{
    CSpear_Soldier* pInstance = new CSpear_Soldier(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CSpear_Soldier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpear_Soldier::Free()
{

    __super::Free();
}
