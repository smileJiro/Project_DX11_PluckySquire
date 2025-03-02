#include "stdafx.h"
#include "ButterGrump.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Projectile_BarfBug.h"
#include "Boss_HomingBall.h"
#include "Boss_EnergyBall.h"
#include "Boss_YellowBall.h"
#include "Boss_PurpleBall.h"
#include "FSM_Boss.h"
#include "ButterGrump_LeftEye.h"
#include "ButterGrump_RightEye.h"
#include "ButterGrump_Tongue.h"
#include "ButterGrump_Shield.h"

CButterGrump::CButterGrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CButterGrump::CButterGrump(const CButterGrump& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CButterGrump::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CButterGrump::Initialize(void* _pArg)
{
    CButterGrump::MONSTER_DESC* pDesc = static_cast<CButterGrump::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;

    pDesc->iNumPartObjects = BOSSPART_END;

    pDesc->iObjectGroupID = OBJECT_GROUP::BOSS;

    m_fDelayTime = 0.5f;
    m_fCoolTime = 3.f;


    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(CContainerObject::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;
    
    m_pBossFSM->Add_State((_uint)BOSS_STATE::SCENE);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::IDLE);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::ENERGYBALL);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::HOMINGBALL);
    m_pBossFSM->Add_State((_uint)BOSS_STATE::YELLOWBALL);

    m_pBossFSM->Set_State((_uint)BOSS_STATE::IDLE);

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CButterGrump::Animation_End, this, placeholders::_1, placeholders::_2));

    //m_pControllerTransform->Rotation(XMConvertToRadians(180.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }
    Safe_Delete(pDesc->pActorDesc);

    static_cast<CActor_Dynamic*>(Get_ActorCom())->Set_Gravity(false);

    m_PartObjects[BOSSPART_SHIELD]->Set_Active(false);
    m_PartObjects[BOSSPART_SHIELD]->Get_ControllerTransform()->RotationXYZ(_float3(0.f, 90.f, 0.f));

    //플레이어 위치 가져오기
    m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Player"), 0);
    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "BOSSINIT : NO PLAYER" << endl;
#endif // _DEBUG
        return S_OK;
    }

    Safe_AddRef(m_pTarget);

    return S_OK;
}

void CButterGrump::Priority_Update(_float _fTimeDelta)
{
    //if (true == m_isDelay)
    //{
    //    m_fAccTime += _fTimeDelta;

    //    if (m_fDelayTime <= m_fAccTime)
    //    {
    //        Delay_Off();
    //        if (3 <= m_iAttackCount)
    //        {
    //            CoolTime_On();
    //        }
    //    }
    //}

    //if (true == m_isCool)
    //{
    //    m_fAccTime += _fTimeDelta;

    //    if (m_fCoolTime <= m_fAccTime)
    //    {
    //        CoolTime_Off();
    //        m_iAttackCount = 0;
    //    }
    //}

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CButterGrump::Update(_float _fTimeDelta)
{
#ifdef _DEBUG

    //if ((KEY_DOWN(KEY::NUMPAD7)))
    //{
    //    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH01);
    //    Set_AnimChangeable(true);
    //    m_pBossFSM->Change_State((_uint)BOSS_STATE::SCENE);
    //}
    //if ((KEY_DOWN(KEY::NUMPAD8)))
    //{
    //    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(EXPLOSION_INTO);
    //    Set_AnimChangeable(true);
    //    m_pBossFSM->Change_State((_uint)BOSS_STATE::SCENE);
    //}
    //if ((KEY_DOWN(KEY::NUMPAD9)))
    //{
    //    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(TRANSITION_PHASE2);
    //    Set_AnimChangeable(true);
    //    m_pBossFSM->Change_State((_uint)BOSS_STATE::SCENE);
    //}
    if (KEY_DOWN(KEY::F5))
    {
        m_isInvincible ^= 1;
        m_PartObjects[BOSSPART_SHIELD]->Set_Active(m_isInvincible);
    }

#endif // _DEBUG

    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CButterGrump::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CButterGrump::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CButterGrump::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        switch ((BOSS_STATE)m_iState)
        {
        case BOSS_STATE::SCENE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH01);
            break;

        case BOSS_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case BOSS_STATE::ENERGYBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            break;

        case BOSS_STATE::HOMINGBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            break;

        case BOSS_STATE::YELLOWBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            break;

        case BOSS_STATE::PURPLEBALL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(FIREBALL_SPIT_SMALL);
            break;

        default:
            break;
        }
    }
}

void CButterGrump::Attack()
{
    _float3 vScale, vPosition;
    _float4 vRotation;
    _matrix matMuzzle = XMLoadFloat4x4(static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[BOSSPART_BODY])->Get_Model(COORDINATE_3D))->Get_BoneMatrix("buttergrump_rigtonsils_01_01"));
    if (false == m_pGameInstance->MatrixDecompose(&vScale, &vRotation, &vPosition, matMuzzle))
        return;
    switch ((BOSS_STATE)m_iState)
    {
    case BOSS_STATE::ENERGYBALL:
    {
        //vPosition.y += vScale.y * 0.5f;
        //vPosition.x += m_pGameInstance->Compute_Random(-5.f, 5.f);
        //vPosition.y += m_pGameInstance->Compute_Random(-5.f, 5.f);
        //vPosition.z += m_pGameInstance->Compute_Random(-5.f, 5.f);
        //XMQuaternionMultiply(XMLoadFloat4(&vRotation), )
        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_EnergyBall"), COORDINATE_3D,  &vPosition, &vRotation);

        break;
    }

    case BOSS_STATE::HOMINGBALL:
    {
        _vector Rot = XMLoadFloat4(&vRotation);

        Rot = XMQuaternionMultiply(Rot, XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)));
        XMStoreFloat4(&vRotation, Rot);
        //vPosition.y += vScale.y * 0.5f;

        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_HomingBall"), COORDINATE_3D, &vPosition, &vRotation);

        break;
    }

    case BOSS_STATE::YELLOWBALL:
    {
        _vector Rot;

        vPosition.y += vScale.y * 0.5f;

        float Array[8] = { 1,0,-1,0,1,1,-1,-1 };
        float Array2[8] = { 0,1,0,-1,1,-1,1,-1 };
        for (_uint i = 0; i < 8; i++)
        {
            _float4 vRot;
            _float3 vPos=vPosition;
            Rot = XMQuaternionMultiply(XMLoadFloat4(&vRotation), XMQuaternionRotationAxis(XMVectorSet(Array[i], Array2[i], 0.f, 0.f), XMConvertToRadians(20.f)));
            XMStoreFloat4(&vRot, Rot);
            /*vPos.x += 2.f * Array[i];
            vPos.y += 2.f * Array2[i];*/
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_YellowBall"), COORDINATE_3D, &vPos, &vRot);
        }

        break;
    }

    case BOSS_STATE::PURPLEBALL:
    {
        _vector Rot;

        vPosition.y += vScale.y * 0.5f;

        float Array[8] = { 1,0,-1,0,1,1,-1,-1 };
        float Array2[8] = { 0,1,0,-1,1,-1,1,-1 };
        for (_uint i = 0; i < 8; i++)
        {
            _float4 vRot;
            _float3 vPos = vPosition;
            Rot = XMQuaternionMultiply(XMLoadFloat4(&vRotation), XMQuaternionRotationAxis(XMVectorSet(Array[i], Array2[i], 0.f, 0.f), XMConvertToRadians(20.f)));
            XMStoreFloat4(&vRot, Rot);
            /*vPos.x += 2.f * Array[i];
            vPos.y += 2.f * Array2[i];*/
            CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_PurpleBall"),COORDINATE_3D ,&vPos, &vRot);
        }
        break;
    }

    default:
        break;
    }
}

void CButterGrump::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    //딜레이 동안은 애니 전환 안됨. 따라서 상태 전환도 불가
    switch ((CButterGrump::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case LB_INTRO_SH01 :
        pModelObject->Switch_Animation(LB_INTRO_SH04);
        break;

    case LB_INTRO_SH04 :
        Set_AnimChangeable(true);
        break;

    case EXPLOSION_INTO:
        pModelObject->Switch_Animation(EXPLOSION_OUT);
        break;

    case EXPLOSION_OUT:
        Set_AnimChangeable(true);
        break;

    case TRANSITION_PHASE2:
        Set_AnimChangeable(true);
        break;

    case FIREBALL_SPIT_SMALL:
        Set_AnimChangeable(true);
        break;

    default:
        break;
    }
}

//void CButterGrump::Intro_First_End(COORDINATE _eCoord, _uint iAnimIdx)
//{
//    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH04);
//}
//
//void CButterGrump::Intro_Second_End(COORDINATE _eCoord, _uint iAnimIdx)
//{
//    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LB_INTRO_SH04);
//}

void CButterGrump::Play_Intro()
{
}

HRESULT CButterGrump::Ready_ActorDesc(void* _pArg)
{
    CButterGrump::MONSTER_DESC* pDesc = static_cast<CButterGrump::MONSTER_DESC*>(_pArg);

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
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
    ShapeDesc->fRadius = 1.f;
    ShapeDesc->fHalfHeight = 1.f;

    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fHalfHeight + ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);


    ///* 사용하려는 Shape의 형태를 정의 */
    //SHAPE_SPHERE_DESC* ShieldShapeDesc = new SHAPE_SPHERE_DESC;
    //ShieldShapeDesc->fRadius = 15.f;

    //ShapeData->pShapeDesc = ShieldShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    //ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    //ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    //ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    //ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    //XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    ///* 최종으로 결정 된 ShapeData를 PushBack */
    //ActorDesc->ShapeDatas.push_back(*ShapeData);


       /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::BOSS;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::EXPLOSION;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CButterGrump::Ready_Components()
{
    /* Com_FSM */
    CFSM_Boss::FSMBOSSDESC FSMDesc;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM_Boss"),
        TEXT("Com_FSM_Boss"), reinterpret_cast<CComponent**>(&m_pBossFSM), &FSMDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CButterGrump::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("Prototype_Model_ButterGrump");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;
    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    m_PartObjects[BOSSPART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[BOSSPART_BODY])
        return E_FAIL;


    /* Parts */

    CButterGrump_LeftEye::BUTTERGRUMP_LEFTEYE_DESC LeftEyeDesc{};

    LeftEyeDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    LeftEyeDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    LeftEyeDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    LeftEyeDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    LeftEyeDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    LeftEyeDesc.pParent = this;

    m_PartObjects[BOSSPART_LEFTEYE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_LeftEye"), &LeftEyeDesc));
    if (nullptr == m_PartObjects[BOSSPART_LEFTEYE])
        return E_FAIL;

    C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[BOSSPART_BODY])->Get_Model(COORDINATE_3D));
    m_PartObjects[BOSSPART_LEFTEYE]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_left_eye"));


    CButterGrump_RightEye::BUTTERGRUMP_RIGHTEYE_DESC RightEyeDesc{};

    RightEyeDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    RightEyeDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    RightEyeDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    RightEyeDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    RightEyeDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    RightEyeDesc.pParent = this;

    m_PartObjects[BOSSPART_RIGHTEYE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_RightEye"), &RightEyeDesc));
    if (nullptr == m_PartObjects[BOSSPART_RIGHTEYE])
        return E_FAIL;

    m_PartObjects[BOSSPART_RIGHTEYE]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_right_eye"));


    CButterGrump_Tongue::BUTTERGRUMP_TONGUE_DESC TongueDesc{};

    TongueDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    TongueDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    TongueDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    TongueDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    TongueDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    TongueDesc.pParent = this;

    m_PartObjects[BOSSPART_TONGUE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_Tongue"), &TongueDesc));
    if (nullptr == m_PartObjects[BOSSPART_TONGUE])
        return E_FAIL;

    m_PartObjects[BOSSPART_TONGUE]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_tongue_01"));


    CButterGrump_Shield::BUTTERGRUMP_SHIELD_DESC ShieldDesc{};

    ShieldDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_HalfSphere_01_2");
    ShieldDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    ShieldDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    ShieldDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    ShieldDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::ALPHABLEND;

    ShieldDesc.tTransform3DDesc.vInitialPosition = _float3(-7.0f, 0.0f, -5.0f);
    ShieldDesc.tTransform3DDesc.vInitialScaling = _float3(25.0f, 25.0f, 25.0f);
    ShieldDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    ShieldDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    ShieldDesc.iRenderGroupID_3D = RG_3D;
    ShieldDesc.iPriorityID_3D = PR3D_GEOMETRY;

    ShieldDesc.pParent = this;

    m_PartObjects[BOSSPART_SHIELD] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_ButterGrump_Shield"), &ShieldDesc));
    if (nullptr == m_PartObjects[BOSSPART_SHIELD])
        return E_FAIL;

    m_PartObjects[BOSSPART_SHIELD]->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("buttergrump_righead_nose"));

    return S_OK;
}

HRESULT CButterGrump::Ready_Projectiles()
{
    Pooling_DESC Pooling_Desc;
    Pooling_Desc.iPrototypeLevelID = m_iCurLevelID;
    Pooling_Desc.strLayerTag = TEXT("Layer_Monster_Projectile");
    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_HomingBall");

    CBoss_HomingBall::PROJECTILE_MONSTER_DESC* pHomingBallDesc = new CBoss_HomingBall::PROJECTILE_MONSTER_DESC;
    pHomingBallDesc->fLifeTime = 4.f;
    pHomingBallDesc->eStartCoord = COORDINATE_3D;
    pHomingBallDesc->isCoordChangeEnable = false;
    pHomingBallDesc->iCurLevelID = m_iCurLevelID;

    pHomingBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(30.f);
    pHomingBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_HomingBall"), Pooling_Desc, pHomingBallDesc);


    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_EnergyBall");

    CBoss_EnergyBall::PROJECTILE_MONSTER_DESC* pEnergyBallDesc = new CBoss_EnergyBall::PROJECTILE_MONSTER_DESC;
    pEnergyBallDesc->fLifeTime = 5.f;
    pEnergyBallDesc->eStartCoord = COORDINATE_3D;
    pEnergyBallDesc->isCoordChangeEnable = false;
    pEnergyBallDesc->iCurLevelID = m_iCurLevelID;

    pEnergyBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pEnergyBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_EnergyBall"), Pooling_Desc, pEnergyBallDesc);

    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_YellowBall");

    CBoss_YellowBall::PROJECTILE_MONSTER_DESC* pYellowBallDesc = new CBoss_YellowBall::PROJECTILE_MONSTER_DESC;
    pYellowBallDesc->fLifeTime = 5.f;
    pYellowBallDesc->eStartCoord = COORDINATE_3D;
    pYellowBallDesc->isCoordChangeEnable = false;
    pYellowBallDesc->iCurLevelID = m_iCurLevelID;

    pYellowBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pYellowBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_YellowBall"), Pooling_Desc, pYellowBallDesc);

    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Boss_PurpleBall");

    CBoss_PurpleBall::PROJECTILE_MONSTER_DESC* pPurpleBallDesc = new CBoss_PurpleBall::PROJECTILE_MONSTER_DESC;
    pPurpleBallDesc->fLifeTime = 5.f;
    pPurpleBallDesc->eStartCoord = COORDINATE_3D;
    pPurpleBallDesc->isCoordChangeEnable = false;
    pPurpleBallDesc->iCurLevelID = m_iCurLevelID;

    pPurpleBallDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pPurpleBallDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Boss_PurpleBall"), Pooling_Desc, pPurpleBallDesc);

    return S_OK;
}

CButterGrump* CButterGrump::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CButterGrump* pInstance = new CButterGrump(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CButterGrump");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButterGrump::Clone(void* _pArg)
{
    CButterGrump* pInstance = new CButterGrump(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CButterGrump");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButterGrump::Free()
{
    Safe_Release(m_pBossFSM);
    
    __super::Free();
}
