#include "stdafx.h"
#include "Bomb_Soldier.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "DetectionField.h"
#include "Pooling_Manager.h"
#include "Bomb.h"

CBomb_Soldier::CBomb_Soldier(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CBomb_Soldier::CBomb_Soldier(const CBomb_Soldier& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CBomb_Soldier::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBomb_Soldier::Initialize(void* _pArg)
{
    CBomb_Soldier::MONSTER_DESC* pDesc = static_cast<CBomb_Soldier::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 10.f;
    pDesc->fAlert2DRange = 5.f;
    pDesc->fChase2DRange = 12.f;
    pDesc->fAttack2DRange = 10.f;

    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 60.f;

    pDesc->fDelayTime = 1.f;

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

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, WALK, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, CHASE, true);
    pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CBomb_Soldier::Animation_End, this, placeholders::_1, placeholders::_2));

    Bind_AnimEventFunc("Attack", bind(&CBomb_Soldier::Attack, this));

    /* Com_AnimEventGenerator */
    CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
    m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_SoldierAttackEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CBomb_Soldier::Priority_Update(_float _fTimeDelta)
{
    if(true == m_isDelay)
    {
        m_fAccTime += _fTimeDelta;
        if (m_fDelayTime <= m_fAccTime)
        {
            Delay_Off();
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CBomb_Soldier::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CBomb_Soldier::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CBomb_Soldier::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    if (COORDINATE_3D == Get_CurCoord())
    {
        m_pDetectionField->Render();
    }
#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CBomb_Soldier::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    __super::On_Hit(_pHitter, _iDamg, _vForce);

    if (nullptr != m_PartObjects[PART_RIGHT_WEAPON])
    {
        Event_DeleteObject(m_PartObjects[PART_RIGHT_WEAPON]);
    }
}

void CBomb_Soldier::Attack()
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        Set_PreAttack(false);

        // 다이나믹으로 전환하고 레이어에 넣기
        m_PartObjects[PART_RIGHT_WEAPON]->Get_ActorCom()->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, true);
        m_PartObjects[PART_RIGHT_WEAPON]->Get_ControllerTransform()->Rotation(XMConvertToRadians(0.f), _vector{ 1.f,0.f,0.f,0.f });
        CCarriableObject* pBomb = static_cast<CCarriableObject*>(m_PartObjects[PART_RIGHT_WEAPON]);
        pBomb->Set_Kinematic(false);
        m_pGameInstance->Add_GameObject_ToLayer(Get_CurLevelID(), TEXT("Layer_Monster_Projectile"), m_PartObjects[PART_RIGHT_WEAPON]);
        

        //던지기
        CActor_Dynamic* pDynamic = static_cast<CActor_Dynamic*>(pBomb->Get_ActorCom());
        _float3 vForce;
		XMStoreFloat3(&vForce, m_pTarget->Get_FinalPosition() - Get_FinalPosition());
        //pDynamic->Add_Force(vForce);
        //pDynamic->Set_Gravity(true);
        pDynamic->Start_ParabolicTo(m_pTarget->Get_FinalPosition(), XMConvertToRadians(60.f));

        //파트에서 빼기
		pBomb->Set_ParentMatrix(COORDINATE_3D, nullptr);
		pBomb->Set_SocketMatrix(COORDINATE_3D, nullptr);
        m_PartObjects[PART_RIGHT_WEAPON] = nullptr;
    }

    Delay_On();
}

void CBomb_Soldier::Change_Animation()
{
    if(m_iState != m_iPreState)
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
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(CHASE);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(BOMB_OUT);
            Create_Bomb();
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
}

void CBomb_Soldier::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CBomb_Soldier::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case ALERT:
        Set_AnimChangeable(true);
        break;

    case BOMB_OUT:
        pModelObject->Switch_Animation(BOMB_THROW);
        break;

    case BOMB_THROW:
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

void CBomb_Soldier::Create_Bomb()
{
    if(COORDINATE_3D == Get_CurCoord())
    {
        C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D));

        _matrix HandMatrix = XMLoadFloat4x4(p3DModel->Get_BoneMatrix("j_hand_attach_r"));

        _float3 vPosition;
        _float4 vRotation;

        m_pGameInstance->MatrixDecompose(nullptr, &vRotation, &vPosition, HandMatrix);

        //CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &vPosition, &vRotation);
        //CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &vPosition);

        CGameObject* pObject = nullptr;

        CBomb::CARRIABLE_DESC BombDesc;
        BombDesc.iCurLevelID = Get_CurLevelID();
        BombDesc.eStartCoord = COORDINATE_3D;
        BombDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);

        BombDesc.pParentMatrices[COORDINATE_3D] = Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_3D);

        m_PartObjects[PART_RIGHT_WEAPON] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Bomb"), &BombDesc));
        if (nullptr == m_PartObjects[PART_RIGHT_WEAPON])
            return;

        m_PartObjects[PART_RIGHT_WEAPON]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_hand_attach_r"));
        m_PartObjects[PART_RIGHT_WEAPON]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 1.f,0.f,0.f,0.f });
        static_cast<CCarriableObject*>(m_PartObjects[PART_RIGHT_WEAPON])->Set_Kinematic(true);
        m_PartObjects[PART_RIGHT_WEAPON]->Get_ActorCom()->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, false);
    }
}

HRESULT CBomb_Soldier::Ready_ActorDesc(void* _pArg)
{
    CBomb_Soldier::MONSTER_DESC* pDesc = static_cast<CBomb_Soldier::MONSTER_DESC*>(_pArg);

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
    ShapeDesc->fHalfHeight = 0.2f;
    ShapeDesc->fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, ShapeDesc->fHalfHeight + ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

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

HRESULT CBomb_Soldier::Ready_Components()
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

HRESULT CBomb_Soldier::Ready_PartObjects()
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

    return S_OK;
}

CBomb_Soldier* CBomb_Soldier::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBomb_Soldier* pInstance = new CBomb_Soldier(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBomb_Soldier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBomb_Soldier::Clone(void* _pArg)
{
    CBomb_Soldier* pInstance = new CBomb_Soldier(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBomb_Soldier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBomb_Soldier::Free()
{

    __super::Free();
}
