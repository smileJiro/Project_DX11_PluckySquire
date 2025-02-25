#include "stdafx.h"
#include "Spear_Soldier.h"
#include "GameInstance.h"
#include "FSM.h"
#include "Soldier_Spear.h"
#include "Soldier_Shield.h"
#include "DetectionField.h"

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
    CSpear_Soldier::MONSTER_DESC* pDesc = static_cast<CSpear_Soldier::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 2.f;
    pDesc->fAlert2DRange = 5.f;
    pDesc->fChase2DRange = 12.f;
    pDesc->fAttack2DRange = 4.f;
    pDesc->fFOVX = 90.f;
    pDesc->fFOVY = 30.f;

    pDesc->fCoolTime = 1.f;

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

    pModelObject->Register_OnAnimEndCallBack(bind(&CSpear_Soldier::Animation_End, this, placeholders::_1, placeholders::_2));

    Bind_AnimEventFunc("Attack", bind(&CSpear_Soldier::Attack, this));

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

void CSpear_Soldier::Priority_Update(_float _fTimeDelta)
{
    if (true == IsCool())
    {
        m_fAccTime += _fTimeDelta;
        if (m_fCoolTime <= m_fAccTime)
        {
            m_fAccTime = 0.f;
            CoolTime_Off();
        }
    }

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CSpear_Soldier::Update(_float _fTimeDelta)
{
    if (true == m_isDash)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            m_vDir.y = 0;
            Get_ActorCom()->Set_LinearVelocity(XMVector3Normalize(XMLoadFloat3(&m_vDir)), Get_ControllerTransform()->Get_SpeedPerSec() * 1.2f);
        }

        /*m_fAccDistance += Get_ControllerTransform()->Get_SpeedPerSec() * _fTimeDelta;

        if (m_fDashDistance <= m_fAccDistance)
        {
            m_fAccDistance = 0.f;
            m_isDash = false;
            Stop_MoveXZ();
        }*/
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

void CSpear_Soldier::Attack()
{
    if (false == m_isDash)
    {
        XMStoreFloat3(&m_vDir, Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
        m_isDash = true;
    }
}

void CSpear_Soldier::Change_Animation()
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
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DASH_ATTACK_RECOVERY);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(CHASE);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(DASH_ATTACK_STARTUP);
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

void CSpear_Soldier::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
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
        Set_AnimChangeable(true);
        CoolTime_Off();
        break;

    case DASH_ATTACK_RECOVERY:
        Set_AnimChangeable(true);
        break;

    default:
        break;
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
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, ShapeDesc->fHalfHeight + ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

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

    return S_OK;
}

HRESULT CSpear_Soldier::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("humgrump_troop_Rig_GT");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

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
    SpearDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

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
    static_cast<CPartObject*>(m_PartObjects[PART_RIGHT_WEAPON])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_hand_attach_r"));
    m_PartObjects[PART_RIGHT_WEAPON]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });
    //Set_PartActive(PART_RIGHT_WEAPON, false);


    CSoldier_Shield::SOLDIER_SHIELD_DESC ShieldDesc{};
    ShieldDesc.strModelPrototypeTag_3D = TEXT("Shield");
    ShieldDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

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
    m_PartObjects[PART_LEFT_WEAPON]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });

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
