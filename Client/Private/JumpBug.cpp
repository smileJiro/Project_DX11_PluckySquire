#include "stdafx.h"
#include "JumpBug.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"
#include "Pooling_Manager.h"
#include "Section_Manager.h"

CJumpBug::CJumpBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CJumpBug::CJumpBug(const CJumpBug& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CJumpBug::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CJumpBug::Initialize(void* _pArg)
{
    CJumpBug::MONSTER_DESC* pDesc = static_cast<CJumpBug::MONSTER_DESC*>(_pArg);
    pDesc->isCoordChangeEnable = true;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;
    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 30.f;

    pDesc->fAlertRange = 0.f;
    pDesc->fChaseRange = 0.f;
    pDesc->fAttackRange = 0.f;
    pDesc->fAlert2DRange = 0.f;
    pDesc->fChase2DRange = 0.f;
    pDesc->fAttack2DRange = 0.f;

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

    m_pFSM->Add_Neutral_State(true);
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_DOWN, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_RIGHT, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE_UP, true);

    if (COORDINATE_3D == Get_CurCoord())
        pModelObject->Set_Animation(Animation::IDLE);
    else if (COORDINATE_2D == Get_CurCoord())
    {
        Set_2D_Direction(F_DIRECTION::DOWN);
        pModelObject->Set_Animation(Animation2D::IDLE_DOWN);
    }

    pModelObject->Register_OnAnimEndCallBack(bind(&CJumpBug::Animation_End, this, placeholders::_1, placeholders::_2));

    //Bind_AnimEventFunc("Monster_Move", bind(&CJumpBug::Monster_Move, this));

    ///* Com_AnimEventGenerator */
    //CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
    //tAnimEventDesc.pReceiver = this;
    //tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
    //m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_JumpBugJumpEvent"), &tAnimEventDesc));
    //Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

    //tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
    //m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_BarfBug2DAttackAnimEvent"), &tAnimEventDesc));
    //Add_Component(TEXT("2DAnimEventGenerator"), m_pAnimEventGenerator);

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CJumpBug::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CJumpBug::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CJumpBug::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CJumpBug::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    __super::Render();
#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CJumpBug::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            switch (MONSTER_STATE(m_iState))
            {
            case MONSTER_STATE::IDLE:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
                break;

            case MONSTER_STATE::PATROL:
                static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(JUMP_UP);
                m_isJump = true;
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

        else if (COORDINATE_2D == Get_CurCoord())
        {
            CJumpBug::Animation2D eAnim = ANIM2D_LAST;
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
                    eAnim = JUMP_RISE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = JUMP_RISE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = JUMP_RISE_RIGHT;
                m_isJump = true;
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
                    eAnim = DIE_UP;
                else if (F_DIRECTION::DOWN == m_e2DDirection)
                    eAnim = DIE_DOWN;
                else if (F_DIRECTION::RIGHT == m_e2DDirection || F_DIRECTION::LEFT == m_e2DDirection)
                    eAnim = DIE_RIGHT;
                break;
            default:
                break;
            }

            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(eAnim);
        }
    }
}

void CJumpBug::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    if(COORDINATE_3D == Get_CurCoord())
    {
        switch ((CJumpBug::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
        {
        case JUMP_UP:
            pModelObject->Switch_Animation(JUMP_DOWN);
            m_isJump = false;
            break;

        case JUMP_DOWN:
            //Set_AnimChangeable(true);
            break;

        case HIT:
            Set_AnimChangeable(true);
            break;

        case DIE:
            Set_AnimChangeable(true);
            break;

        default:
            break;
        }
    }

    else  if (COORDINATE_2D == Get_CurCoord())
    {
        switch ((CJumpBug::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
        {
        case JUMP_RISE_DOWN:
            pModelObject->Switch_Animation(JUMP_FALL_DOWN);
            m_isJump = false;
            break;
        case JUMP_RISE_RIGHT:
            pModelObject->Switch_Animation(JUMP_FALL_RIGHT);
            m_isJump = false;
            break;
        case JUMP_RISE_UP:
            pModelObject->Switch_Animation(JUMP_FALL_UP);
            m_isJump = false;
            break;

        case JUMP_FALL_DOWN:
            pModelObject->Switch_Animation(JUMP_LAND_DOWN);
            break;
        case JUMP_FALL_RIGHT:
            pModelObject->Switch_Animation(JUMP_LAND_RIGHT);
            break;
        case JUMP_FALL_UP:
            pModelObject->Switch_Animation(JUMP_LAND_UP);
            break;
            

        case HIT_DOWN:
        case HIT_UP:
        case HIT_RIGHT:
            Set_AnimChangeable(true);
            break;

        case DIE_DOWN:
        case DIE_UP:
        case DIE_RIGHT:
            Set_AnimChangeable(true);

            CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Death_Burst"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), Get_ControllerTransform()->Get_WorldMatrix());

            //확률로 전구 생성
            if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
            {
                _float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
                _wstring strCurSection = CSection_Manager::GetInstance()->Get_Cur_Section_Key();
                CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
            }

            Event_DeleteObject(this);

            break;


        default:
            break;
        }
    }
}

HRESULT CJumpBug::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
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

void CJumpBug::Turn_Animation(_bool _isCW)
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

void CJumpBug::Monster_Move(_fvector _vDirection)
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        //_float3 vForce = { 0.f,2000.f,0.f };
        //Get_ActorCom()->Add_Force(vForce);

        if (true == m_isJump)
        {
            _vector vDirection = _vDirection + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
            Get_ActorCom()->Set_LinearVelocity(XMVector3Normalize(vDirection), Get_ControllerTransform()->Get_SpeedPerSec());
        }
        else
        {
            _vector vDirection = _vDirection + XMVectorSet(0.f, -1.f, 0.f, 0.f);
            Get_ActorCom()->Set_LinearVelocity(XMVector3Normalize(_vDirection), Get_ControllerTransform()->Get_SpeedPerSec());
        }
    }

    else if (COORDINATE_2D == Get_CurCoord())
    {

    }
}

HRESULT CJumpBug::Ready_ActorDesc(void* _pArg)
{
    CJumpBug::MONSTER_DESC* pDesc = static_cast<CJumpBug::MONSTER_DESC*>(_pArg);

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
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

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

HRESULT CJumpBug::Ready_Components()
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

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &FSMDesc)))
        return E_FAIL;

    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 30.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius - 10.f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CJumpBug::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("jumpBug_Rig");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.strModelPrototypeTag_2D = TEXT("JumpBug");
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

CJumpBug* CJumpBug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CJumpBug* pInstance = new CJumpBug(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CJumpBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CJumpBug::Clone(void* _pArg)
{
    CJumpBug* pInstance = new CJumpBug(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CJumpBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJumpBug::Free()
{

    __super::Free();
}
