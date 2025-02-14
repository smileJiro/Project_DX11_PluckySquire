#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "StateMachine.h"
#include "ModelObject.h"
#include "Controller_Transform.h"
#include "PlayerState_Idle.h"
#include "PlayerState_Run.h"
#include "PlayerState_Attack.h"
#include "PlayerState_JumpUp.h"
#include "PlayerState_JumpDown.h"
#include "PlayerState_JumpAttack.h"
#include "PlayerState_ThrowObject.h"
#include "PlayerState_Roll.h"
#include "PlayerState_Clamber.h"
#include "PlayerState_ThrowSword.h"
#include "PlayerState_SpinAttack.h"
#include "PlayerState_PickUpObject.h"
#include "Actor_Dynamic.h"
#include "PlayerSword.h"    
#include "Section_Manager.h"    
    
#include "Collider_Fan.h"
#include "Interactable.h"
#include "CarriableObject.h"
#include "Blocker.h"

CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCharacter(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _Prototype)
    :CCharacter(_Prototype)
	, m_mat3DCarryingOffset(_Prototype.m_mat3DCarryingOffset)
	, m_mat2DCarryingOffset(_Prototype.m_mat2DCarryingOffset)
{
    for (_uint i = 0; i < ATTACK_TYPE_LAST; i++)
    {
		m_f2DAttackTriggerDesc[i] = _Prototype.m_f2DAttackTriggerDesc[i];
    }
}

HRESULT CPlayer::Initialize_Prototype()
{
	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1].fRadius = 93.f;
	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1].fRadianAngle = XMConvertToRadians(120.f);
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1].fOffset = {0.f, m_f2DCenterYOffset };

	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2].fRadius = 93.f;
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2].fRadianAngle = XMConvertToRadians(120.f);
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2].fOffset = { 0.f, m_f2DCenterYOffset };

    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3].fRadius = 110.f;
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3].fRadianAngle = XMConvertToRadians(60.f);
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3].fOffset = { 0.f, m_f2DCenterYOffset };

	m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN].fRadius = 110.f;
	m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN].fRadianAngle = XMConvertToRadians(360.f);
	m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN].fOffset = { 0.f, m_f2DCenterYOffset };

	m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK].fRadius = 93.f;
	m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK].fRadianAngle = XMConvertToRadians(360.f);
	m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK].fOffset = { 0.f, 0.f };

    XMStoreFloat4x4(&m_mat3DCarryingOffset ,XMMatrixTranslation(0.f, 2.f, 0.f));
    XMStoreFloat4x4(&m_mat2DCarryingOffset ,XMMatrixTranslation(0.f, 100.f, 0.f));

    return S_OK;
}

HRESULT CPlayer::Initialize(void* _pArg)
{
    CPlayer::CONTAINEROBJ_DESC* pDesc = static_cast<CPlayer::CONTAINEROBJ_DESC*>(_pArg);

    m_iCurLevelID = pDesc->iCurLevelID;

    pDesc->iNumPartObjects = CPlayer::PLAYER_PART_LAST;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 500.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720);
    pDesc->tTransform3DDesc.fSpeedPerSec = 8.f;
    
    pDesc->iCollisionGroupID = OBJECT_GROUP::PLAYER;
    /* 너무 길어서 함수로 묶고싶다 하시는분들은 주소값 사용하는 데이터들 동적할당 하셔야합니다. 아래처럼 지역변수로 하시면 날라가니 */
    /* Create Test Actor (Desc를 채우는 함수니까. __super::Initialize() 전에 위치해야함. )*/
    pDesc->eActorType = ACTOR_TYPE::DYNAMIC;
    CActor::ACTOR_DESC ActorDesc;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc.pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc.FreezeRotation_XYZ[0] = true;
    ActorDesc.FreezeRotation_XYZ[1] = false;
    ActorDesc.FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc.FreezePosition_XYZ[0] = false;
    ActorDesc.FreezePosition_XYZ[1] = false;
    ActorDesc.FreezePosition_XYZ[2] = false;

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_CAPSULE_DESC CapsuleDesc = {};
    CapsuleDesc.fRadius = m_fFootLength;
    CapsuleDesc.fHalfHeight = m_f3DCenterYOffset - m_fFootLength;
    //SHAPE_BOX_DESC ShapeDesc = {};
    //ShapeDesc.vHalfExtents = { 0.5f, 1.f, 0.5f };

    /* 해당 Shape의 Flag에 대한 Data 정의 */

    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &CapsuleDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData.eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData.eMaterial = ACTOR_MATERIAL::CHARACTER_CAPSULE;  // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    ShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, m_f3DCenterYOffset + 0.1f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    //마찰용 박스
    SHAPE_BOX_DESC BoxDesc = {};
    _float fHalfWidth = CapsuleDesc.fRadius * cosf(XMConvertToRadians(45.f));
    BoxDesc.vHalfExtents = { fHalfWidth, CapsuleDesc.fRadius, fHalfWidth };
    SHAPE_DATA BoxShapeData;
    BoxShapeData.eShapeType = SHAPE_TYPE::BOX;
    BoxShapeData.pShapeDesc = &BoxDesc;
    XMStoreFloat4x4(&BoxShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, BoxDesc.vHalfExtents.y, 0.0f));
    BoxShapeData.iShapeUse =(_uint)SHAPE_USE::SHAPE_FOOT;
    BoxShapeData.isTrigger = false;
    BoxShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
    ActorDesc.ShapeDatas.push_back(BoxShapeData);

    //마찰용 캡슐로 테스트해봤어요
    //SHAPE_CAPSULE_DESC capDesc = {};
    //_float fHalfWidth = CapsuleDesc.fRadius * cosf(XMConvertToRadians(45.f));
    //capDesc.fHalfHeight = fHalfWidth;
    //capDesc.fRadius = CapsuleDesc.fRadius;
    //SHAPE_DATA capShapeData;
    //capShapeData.eShapeType = SHAPE_TYPE::CAPSULE;
    //capShapeData.pShapeDesc = &capDesc;
    //XMStoreFloat4x4(&capShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, capDesc.fRadius, 0.0f));
    //capShapeData.iShapeUse = SHAPE_FOOT;
    //capShapeData.isTrigger = false;
    //capShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
    //ActorDesc.ShapeDatas.push_back(capShapeData);

    //주변 지형 감지용 구 (트리거)
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    ShapeData.isTrigger = true;
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0, m_f3DCenterYOffset, 0)); //여기임
    SHAPE_SPHERE_DESC SphereDesc = {};
    SphereDesc.fRadius = 2.5f;
    ShapeData.pShapeDesc = &SphereDesc;

    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::MONSTER | OBJECT_GROUP::INTERACTION_OBEJCT | OBJECT_GROUP::MONSTER_PROJECTILE | OBJECT_GROUP::TRIGGER_OBJECT;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;
    

    if (FAILED(__super::Initialize(pDesc)))
    {
        MSG_BOX("CPlayer super Initialize Failed");
        return E_FAIL;
    }
    static_cast<CActor_Dynamic*> (m_pActorCom)->Set_SleepThreshold(0);
    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
     
	m_ePlayerMode = PLAYER_MODE_NORMAL;
    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.strModelPrototypeTag_2D = TEXT("player");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Latch_SkelMesh_NewRig");
    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;
    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    m_PartObjects[PART_BODY] = m_pBody = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerBody"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
    {
        MSG_BOX("CPlayer Body Creation Failed");
        return E_FAIL;
    }
    //Part Sword
    CPlayerSword::PLAYER_SWORD_DESC SwordDesc{};
    SwordDesc.isCoordChangeEnable = true;
    SwordDesc.pParent = this;
    SwordDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    SwordDesc.iCurLevelID = m_iCurLevelID;
    SwordDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();
    SwordDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    SwordDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    SwordDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    SwordDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    SwordDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    SwordDesc.tTransform2DDesc.fSpeedPerSec = 10.f;
    SwordDesc.iRenderGroupID_3D = RG_3D;
    SwordDesc.iPriorityID_3D = PR3D_GEOMETRY;
    SwordDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    SwordDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    m_PartObjects[PLAYER_PART_SWORD] = m_pSword = static_cast<CPlayerSword*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerSword"), &SwordDesc));
    if (nullptr == m_PartObjects[PLAYER_PART_SWORD])
    {
        MSG_BOX("CPlayer Sword Creation Failed");
        return E_FAIL;
    }
    m_PartObjects[PLAYER_PART_SWORD]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 1,0,0,0 });
    Set_PartActive(PLAYER_PART_SWORD, false);
    m_pSword->Switch_Grip(true);
    m_pSword->Set_AttackEnable(false);

    //Part Glove
    BodyDesc.strModelPrototypeTag_3D = TEXT("latch_glove");
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    BodyDesc.eActorType = ACTOR_TYPE::LAST;
    BodyDesc.pActorDesc = nullptr;
    m_PartObjects[PLAYER_PART_GLOVE] = m_pGlove = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PLAYER_PART_GLOVE])
    {
        MSG_BOX("CPlayer Glove Creation Failed");
        return E_FAIL;
    }
    C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D));
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_GLOVE])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_glove_hand_r")); /**/
    m_PartObjects[PLAYER_PART_GLOVE]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });
    Set_PartActive(PLAYER_PART_GLOVE, false);

    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CPlayer::On_AnimEnd, this, placeholders::_1, placeholders::_2));
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_IDLE_RIGHT, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_IDLE_UP, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_IDLE_DOWN, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_RUN_RIGHT, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_RUN_UP, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_RUN_DOWN, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, (_uint)ANIM_STATE_3D::LATCH_PICKUP_IDLE_GT, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, (_uint)ANIM_STATE_3D::LATCH_ANIM_RUN_01_GT, true);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_3D, (_uint)ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_SPIN_LOOP_GT, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL1, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL2, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL3, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_DOWN_LVL4, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL1, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL2, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL3, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_UP_LVL4, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL1, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL2, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL3, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_AnimationLoop(COORDINATE::COORDINATE_2D, (_uint)ANIM_STATE_2D::PLAYER_ATTACKV02_SPIN_RIGHT_LVL4, false);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_3DAnimationTransitionTime((_uint)ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_SPIN_LOOP_GT, 0);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_3DAnimationTransitionTime((_uint)ANIM_STATE_3D::LATCH_ANIM_SPIN_ATTACK_OUT_GT, 0);
    return S_OK;
}


HRESULT CPlayer::Ready_Components()
{
    CStateMachine::STATEMACHINE_DESC tStateMachineDesc{};
    tStateMachineDesc.pOwner = this;
    
    m_pStateMachine = static_cast<CStateMachine*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), &tStateMachineDesc));
    m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
    Add_Component(TEXT("StateMachine"), m_pStateMachine);

    Bind_AnimEventFunc("ThrowSword", bind(&CPlayer::ThrowSword, this));
    Bind_AnimEventFunc("ThrowObject", bind(&CPlayer::ThrowObject, this));
    Bind_AnimEventFunc("Attack", bind(&CPlayer::Move_Attack_3D, this));

	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
	m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_PlayerAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenrator"), m_pAnimEventGenerator);
    
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
    m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_Player2DAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("2DAnimEventGenrator"), m_pAnimEventGenerator);

	m_p2DColliderComs.resize(3);
   /* Test 2D Collider */
   CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
   CircleDesc.pOwner = this;
   CircleDesc.fRadius = 20.f;
   CircleDesc.vScale = { 1.0f, 1.0f };
   CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius*0.5f };
   CircleDesc.isBlock = false;
   CircleDesc.isTrigger = false;
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
       TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
       return E_FAIL; 
   m_pBody2DColliderCom = m_p2DColliderComs[0];
   Safe_AddRef(m_pBody2DColliderCom);

   CircleDesc.pOwner = this;
   CircleDesc.fRadius = m_f2DInteractRange;
   CircleDesc.vScale = { 1.0f, 1.0f };
   CircleDesc.vOffsetPosition = { 0.f, m_f2DCenterYOffset };
   CircleDesc.isBlock = false;
   CircleDesc.isTrigger = true; 
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
       TEXT("Com_Body2DTrigger"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
       return E_FAIL;
   m_pBody2DTriggerCom = m_p2DColliderComs[1];
   Safe_AddRef(m_pBody2DTriggerCom);

   CCollider_Fan::COLLIDER_FAN_DESC FanDesc = {};
   FanDesc.pOwner = this;
   FanDesc.fRadius = m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1].fRadius;
   FanDesc.fRadianAngle = m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1].fRadianAngle;
   FanDesc.vDirection = { 0,-1 };
   FanDesc.vScale = { 1.0f, 1.0f };
   FanDesc.vOffsetPosition = m_f2DAttackTriggerDesc->fOffset;
   FanDesc.isBlock = false;
   FanDesc.isTrigger = true;
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Fan"),
       TEXT("Com_Attack2DTrigger"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[2]), &FanDesc)))
       return E_FAIL;
   m_pAttack2DTriggerCom = m_p2DColliderComs[2];
   Safe_AddRef(m_pAttack2DTriggerCom);
   m_pAttack2DTriggerCom->Set_Active(false);

   /* Com_Gravity */
   CGravity::GRAVITY_DESC GravityDesc = {};
   GravityDesc.fGravity = 9.8f * 100.f;
   GravityDesc.vGravityDirection = { 0.0f, -1.0f, 0.0f };
   GravityDesc.pOwner = this;
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
       TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
       return E_FAIL;

    return S_OK;
}


void CPlayer::Priority_Update(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::K))
    {
        CGravity::STATE eCurState = m_pGravityCom->Get_CurState();
        _int iState = eCurState;
        iState ^= 1;
        m_pGravityCom->Change_State((CGravity::STATE)iState);
    }

    CContainerObject::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}


void CPlayer::Update(_float _fTimeDelta)
{
    Key_Input(_fTimeDelta);
    COORDINATE eCoord  =  Get_CurCoord();
    if (Is_Sneaking())
        int a = 0;
  //  if (COORDINATE_2D == eCoord)
  //  {
  //      //// TestCode : 태웅
  //      _uint iSectionKey = RG_2D + PR2D_SECTION_START;
  //      if(m_pBody2DColliderCom->Is_Active())
  //          m_pGameInstance->Add_Collider(m_strSectionName, OBJECT_GROUP::PLAYER, m_pBody2DColliderCom);
		//if (m_pBody2DTriggerCom->Is_Active())
  //          m_pGameInstance->Add_Collider(m_strSectionName, OBJECT_GROUP::PLAYER_TRIGGER, m_pBody2DTriggerCom);
		//if (m_pAttack2DTriggerCom->Is_Active())
		//	m_pGameInstance->Add_Collider(m_strSectionName, OBJECT_GROUP::PLAYER_PROJECTILE, m_pAttack2DTriggerCom);
  //  }

   //cout << "Sneak" << Is_Sneaking() << endl;
    __super::Update(_fTimeDelta); /* Part Object Update */
    m_vLookBefore = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    if (COORDINATE_3D == eCoord)
    {
        _bool bSleep = static_cast<CActor_Dynamic*>(m_pActorCom)->Is_Sleeping();
        if (false == bSleep)
        {
            m_bOnGround = false;
        }
    }


}

// 충돌 체크 후 container의 transform을 밀어냈어. 

void CPlayer::Late_Update(_float _fTimeDelta)
{
    COORDINATE eCoord = Get_CurCoord();
    if (COORDINATE_2D == eCoord)
    {
        if (m_bPlatformerMode)
        {

        }
        else
        {
            m_f2DUpForce -= 9.8f * _fTimeDelta * 300;

            m_f2DFloorDistance += m_f2DUpForce * _fTimeDelta;
            if (0 > m_f2DFloorDistance)
            {
                m_f2DFloorDistance = 0;
                m_bOnGround = true;
                m_f2DUpForce = 0;
            }
            else if (0 == m_f2DFloorDistance)
                m_bOnGround = true;
            else
                m_bOnGround = false;
            // cout << "Upforce :" << m_f2DUpForce << " Height : " << m_f2DHeight << endl;
            _vector vUp = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_UP));
            m_pBody->Set_Position(vUp * m_f2DFloorDistance);
        }

    }
    else
    {
		_vector vPlayerPos = Get_FinalPosition();
        _float3 vOrigin;
        XMStoreFloat3(&vOrigin, vPlayerPos);
        _float3 vRayDir = { 0,-1,0 };
        list<CActorObject*> hitActors;
        list<RAYCASTHIT> raycasthits;
        _float fFloorHeihgt = -1;
        if (m_pGameInstance->RayCast(vOrigin, vRayDir, 100, hitActors, raycasthits))
        {
            //닿은 것들 중에서 가장 높은 것을 찾기
            auto& iterHitPoint = raycasthits.begin();
            for (auto& pActor : hitActors)
            {
                if ( pActor != this )//맵과 닿음.
				{
					if (iterHitPoint->vNormal.y > m_fStepSlopeThreshold)//닿은 곳의 경사가 너무 급하지 않으면
					{
						fFloorHeihgt = max(fFloorHeihgt, iterHitPoint->vPosition.y);
					}
                }
                iterHitPoint++;
            }
        }
		if (fFloorHeihgt > -1)
		    m_f3DFloorDistance = XMVectorGetY(vPlayerPos) - fFloorHeihgt;
        else
			m_f3DFloorDistance = -1;
    }
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
    //cout << endl;
}

HRESULT CPlayer::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    if(m_pBody2DColliderCom->Is_Active())
        m_pBody2DColliderCom->Render();
    if(m_pBody2DTriggerCom->Is_Active())
        m_pBody2DTriggerCom->Render();
    if (m_pAttack2DTriggerCom->Is_Active())
        m_pAttack2DTriggerCom->Render();
#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CPlayer::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_BODY:

        break;
    case Client::SHAPE_USE::SHAPE_FOOT:
        //cout << "   COntatct Enter";

        //TestCode
    //    for (auto& pxPairData : _ContactPointDatas)
    //    {
    //        if (OBJECT_GROUP::MAPOBJECT == _Other.pActorUserData->iObjectGroup)
    //        {
    //            //경사로 벽인지 판단하는데, 낮은 높이애들만 할 것이므로 안씀
    //            //if (abs(pxPairData.normal.y) < m_fFootSlopeThreshold)
    //            //높이가 한계점 이하이면
				//if (Get_FinalPosition().m128_f32[1] < pxPairData.position.y && pxPairData.position.y - m_fFootHeightThreshold <= Get_FinalPosition().m128_f32[1])
    //            {
    //                Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, true);
    //            }
    //        }
    //    }

        break;
	case Client::SHAPE_USE::SHAPE_TRIGER:
		break;
    }

}

void CPlayer::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_BODY:
        break;
    case Client::SHAPE_USE::SHAPE_FOOT:
        for (auto& pxPairData : _ContactPointDatas)
        {
            //_vector vMyPos = Get_FinalPosition();
            ////천장이면 무시
            //if (pxPairData.normal.y < 0)
            //    continue;
            //닿은 곳의 경사가 너무 급하면 무시
            if (abs(pxPairData.normal.y) < m_fStepSlopeThreshold)
                continue;
            m_bOnGround = true;
            ////cout << "  Contact";
            return;
        }
        break;
    case Client::SHAPE_USE::SHAPE_TRIGER:

        break;
    default:
        break;
    }

}

void CPlayer::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
 
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_BODY:

        break;
    case Client::SHAPE_USE::SHAPE_FOOT:
        //cout << "   COntatct Exit";

         //TestCode
        //for (auto& pxPairData : _ContactPointDatas)
        //{
        //    if (OBJECT_GROUP::MAPOBJECT == _Other.pActorUserData->iObjectGroup)
        //    {
        //        //경사로 벽인지 판단하는데, 낮은 높이애들만 할 것이므로 안씀
        //        //if (abs(pxPairData.normal.y) < m_fFootSlopeThreshold)
        //        //높이가 한계점 이하이면
        //        if (!(Get_FinalPosition().m128_f32[1] < pxPairData.position.y && pxPairData.position.y - m_fFootHeightThreshold <= Get_FinalPosition().m128_f32[1]))
        //        {
        //            Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, false);
        //        }
        //    }
        //}
        break;
    case Client::SHAPE_USE::SHAPE_TRIGER:
        break;
    }


}

void CPlayer::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_TRIGER:
        Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, true);
        break;
    }

}

void CPlayer::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (SHAPE_USE::SHAPE_TRIGER ==(SHAPE_USE)_My.pShapeUserData->iShapeUse)
    {
        if (OBJECT_GROUP::INTERACTION_OBEJCT == _Other.pActorUserData->iObjectGroup)
        {
            PLAYER_INPUT_RESULT tKeyResult = Player_KeyInput();
            if (tKeyResult.bInputStates[PLAYER_KEY_INTERACT])
            {
                IInteractable* pInteractable = dynamic_cast<IInteractable*> (_Other.pActorUserData->pOwner);
                if (pInteractable && pInteractable->Is_Interactable(this))
                {
                    pInteractable->Interact(this);
                }
            }
        }
    }


}

void CPlayer::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_TRIGER:
        Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, false);
        break;
    }
}

void CPlayer::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    _uint iGroupID = _pOtherCollider->Get_CollisionGroupID();
    switch ((OBJECT_GROUP)iGroupID)
    {
    case Client::NONE:
        break;
    case Client::PLAYER:
        break;
    case Client::MONSTER:
        break;
    case Client::MAPOBJECT:
        break;
    case Client::INTERACTION_OBEJCT:
        break;
    case Client::PLAYER_PROJECTILE:
        break;
    case Client::MONSTER_PROJECTILE:
        break;
    case Client::TRIGGER_OBJECT:
        break;
    case Client::RAY_OBJECT:
        break;
    case Client::PLAYER_TRIGGER:
        break;
    case Client::BLOCKER:
    {
        if (true == _pMyCollider->Is_Trigger())
            break;
        if (false == static_cast<CBlocker*>(_pOtherObject)->Is_Floor())
            break;

        /* 1. Blocker은 항상 AABB임을 가정. */

        /* 2. 나의 Collider 중점 기준, AABB에 가장 가까운 점을 찾는다. */
        _bool isResult = false;
        _float fEpsilon = 0.01f;
        _float2 vContactVector = {};
        isResult = static_cast<CCollider_Circle*>(_pMyCollider)->Compute_NearestPoint_AABB(static_cast<CCollider_AABB*>(_pOtherCollider), nullptr, &vContactVector);
        if (true == isResult)
        {
            /* 3. 충돌지점 벡터와 중력벡터를 내적하여 그 결과를 기반으로 Floor 인지 체크. */
            _float3 vGravityDir = m_pGravityCom->Get_GravityDirection();
            _float2 vGravityDirection = _float2(vGravityDir.x, vGravityDir.y);
            _float fGdotC = XMVectorGetX(XMVector2Dot(XMLoadFloat2(&vGravityDirection), XMVector2Normalize(XMLoadFloat2(&vContactVector))));
            if (1.0f - fEpsilon <= fGdotC)
            {
                /* 결과가 1에 근접한다면 이는 floor로 봐야겠지. */
                m_pGravityCom->Change_State(CGravity::STATE_FLOOR);
            }
        }
        
    }
        break;
    default:
        break;
    }



    if (_pMyCollider == m_pAttack2DTriggerCom
        && OBJECT_GROUP::MONSTER == _pOtherCollider->Get_CollisionGroupID())
    {
        Attack(_pOtherObject);
    }
}

void CPlayer::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (_pMyCollider == m_pBody2DTriggerCom)
    {
        PLAYER_INPUT_RESULT tKeyResult = Player_KeyInput();
        if (tKeyResult.bInputStates[PLAYER_KEY_INTERACT])
        {
            IInteractable* pInteractable = dynamic_cast<IInteractable*> (_pOtherObject);
            if (pInteractable && pInteractable->Is_Interactable(this))
            {
                pInteractable->Interact(this);
            }
        }
    }
    else if (_pMyCollider == m_pAttack2DTriggerCom
        && OBJECT_GROUP::MONSTER == _pOtherCollider->Get_CollisionGroupID())
    {
        Attack(_pOtherObject);
    }
}

void CPlayer::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    _uint iGroupID = _pOtherCollider->Get_CollisionGroupID();
    switch ((OBJECT_GROUP)iGroupID)
    {
    case Client::NONE:
        break;
    case Client::PLAYER:
        break;
    case Client::MONSTER:
        break;
    case Client::MAPOBJECT:
        break;
    case Client::INTERACTION_OBEJCT:
        break;
    case Client::PLAYER_PROJECTILE:
        break;
    case Client::MONSTER_PROJECTILE:
        break;
    case Client::TRIGGER_OBJECT:
        break;
    case Client::RAY_OBJECT:
        break;
    case Client::PLAYER_TRIGGER:
        break;
    case Client::BLOCKER:
    {
        if (true == _pMyCollider->Is_Trigger())
            break;

        if (static_cast<CBlocker*>(_pOtherObject)->Is_Floor())
            m_pGravityCom->Change_State(CGravity::STATE_FALLDOWN);
    }
    break;
    default:
        break;
    }

}

void CPlayer::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pStateMachine->Get_CurrentState()->On_AnimEnd(_eCoord, iAnimIdx);
}

void CPlayer::On_Hit(CGameObject* _pHitter, _float _fDamg)
{
    //cout << " Player Get Damg" << _fDamg << endl;
    m_tStat.fHP -= _fDamg;
    if (m_tStat.fHP < 0)
    {
        m_tStat.fHP = 0;
        Set_State(DIE);
    }
}

HRESULT CPlayer::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == Get_CurCoord()) {
        Set_2DDirection(E_DIRECTION::DOWN);
        CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET_2D, true, 1.f);
    }
    else
    {
        CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET, true, 1.f);
		m_bPlatformerMode = false;
    }

    switch (m_ePlayerMode)
    {
    case Client::CPlayer::PLAYER_MODE_NORMAL:
		UnEquip_Part(PLAYER_PART_SWORD);
        m_pSword->Set_AttackEnable(false);
        break;
    case Client::CPlayer::PLAYER_MODE_SWORD:
		Equip_Part(PLAYER_PART_SWORD);
        m_pSword->Set_AttackEnable(true);
        break;
    case Client::CPlayer::PLAYER_MODE_SNEAK:
        UnEquip_Part(PLAYER_PART_SWORD);
        m_pSword->Set_AttackEnable(false);
        break;
    default:
        break;
    }
    Set_State(IDLE);

    return S_OK;
}


void CPlayer::Move_Attack_3D()
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        Stop_Move();
        Add_Impuls(Get_LookDirection() * m_fAttackForwardingForce);
    }
}

void CPlayer::Attack(CGameObject* _pVictim)
{
    if (m_AttckedObjects.find(_pVictim) != m_AttckedObjects.end())
        return;
    Event_Hit(this, _pVictim, m_tStat.fDamg);
    _float3 vRepulse; XMStoreFloat3(&vRepulse, 10.f * Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
    static_cast<CActorObject*>(_pVictim)->Get_ActorCom()->Add_Impulse(vRepulse);
    m_AttckedObjects.insert(_pVictim);
}

void CPlayer::Move(_fvector _vForce, _float _fTimeDelta)
{
    ACTOR_TYPE eActorType = Get_ActorType();

    if (COORDINATE_3D == Get_CurCoord())
    {
        m_v3DTargetDirection = XMVector4Normalize(_vForce);
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        _vector vVeclocity = _vForce /** m_tStat[COORDINATE_3D].fMoveSpeed*/  /** fDot*/;

        vVeclocity = XMVectorSetY(vVeclocity, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));
        pDynamicActor->Set_LinearVelocity(vVeclocity);
    }
    else
    {
        m_pControllerTransform->Go_Direction(_vForce, XMVectorGetX( XMVector3Length(_vForce)), _fTimeDelta);
    }
}

void CPlayer::Move_Forward(_float fVelocity, _float _fTimeDelta)
{
    ACTOR_TYPE eActorType = Get_ActorType();
    if (ACTOR_TYPE::DYNAMIC == eActorType)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);

            _vector vLook = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
            vLook = XMVectorSetY(vLook * fVelocity /** _fTimeDelta*/, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));
            pDynamicActor->Set_LinearVelocity(vLook);
        }
        else
        {
            _vector vDir = EDir_To_Vector(m_e2DDirection_E);
            m_pControllerTransform->Go_Direction(vDir, fVelocity, _fTimeDelta);
        }
    }

}

void CPlayer::Jump()
{		/* Test Jump */
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        m_f2DUpForce = m_f2DJumpPower;
    }
    else
    {
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        _vector vVelocity = pDynamicActor->Get_LinearVelocity();
        pDynamicActor->Add_Impulse(_float3{ 0, m_f3DMoveSpeed ,0 });
        //pDynamicActor->Set_LinearDamping(2);
    }
}




PLAYER_INPUT_RESULT CPlayer::Player_KeyInput()
{
	PLAYER_INPUT_RESULT tResult;
    fill(begin(tResult.bInputStates), end(tResult.bInputStates), false);

    if (Is_SwordHandling())
    {
        //기본공격
        if (MOUSE_DOWN(MOUSE_KEY::LB))
        {
            tResult.bInputStates[PLAYER_INPUT_ATTACK] = true;
        }
        //칼 던지기
        else if (MOUSE_DOWN(MOUSE_KEY::RB))
        {
            tResult.bInputStates[PLAYER_KEY_THROWSWORD] = true;
        }
        else if (Is_OnGround())
        {
            KEY_STATE eKeyState = m_pGameInstance->GetKeyState(KEY::Q);
            if (KEY_STATE::DOWN == eKeyState)
			    tResult.bInputStates[PLAYER_KEY_SPINATTACK] = true;
            else if (KEY_STATE::PRESSING == eKeyState)
                tResult.bInputStates[PLAYER_KEY_SPINCHARGING] = true;
            else if (KEY_STATE::UP == eKeyState)
                tResult.bInputStates[PLAYER_KEY_SPINLAUNCH] = true;
        }
    }
    if (Is_CarryingObject())
    {
        //던지기
        if (KEY_DOWN(KEY::E))
            tResult.bInputStates[PLAYER_KEY_THROWOBJECT] = true;
    }
    else
    {
        //상호작용
        if (KEY_DOWN(KEY::E))
            tResult.bInputStates[PLAYER_KEY_INTERACT] = true;
    }

    //점프
    if (KEY_DOWN(KEY::SPACE))
        tResult.bInputStates[PLAYER_INPUT_JUMP] = true;
    //구르기 & 잠입
    else if (KEY_PRESSING(KEY::LSHIFT))
    {
        if (Is_SneakMode())
            tResult.bInputStates[PLAYER_KEY_SNEAK] = true;
        else
            tResult.bInputStates[PLAYER_KEY_ROLL] = true;
    }

    COORDINATE eCoord = Get_CurCoord();
    //이동
	_vector vRight = XMVector3Normalize( m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_UP));
    if (KEY_PRESSING(KEY::W))
    {
        if (eCoord == COORDINATE_3D)
            tResult.vDir += _vector{ 0.f, 0.f, 1.f,0.f };
        else
            //tResult.vMoveDir += _vector{ 0.f, 1.f, 0.f,0.f };
            tResult.vDir += vUp;
    }
    if (KEY_PRESSING(KEY::A))
    {
        if (eCoord == COORDINATE_3D)
            tResult.vDir += _vector{ -1.f, 0.f, 0.f,0.f };
        else
           // tResult.vMoveDir += _vector{ -1.f, 0.f, 0.f,0.f };
            tResult.vDir -= vRight;
    }
    if (KEY_PRESSING(KEY::S))
    {
        if (eCoord == COORDINATE_3D)
            tResult.vDir += _vector{ 0.f, 0.f, -1.f,0.f };
        else
            //tResult.vMoveDir += _vector{ 0.f, -1.f, 0.f,0.f };
            tResult.vDir -= vUp;
    }
    if (KEY_PRESSING(KEY::D))
    {
        if (eCoord == COORDINATE_3D)
            tResult.vDir += _vector{ 1.f, 0.f, 0.f,0.f };
        else
            //tResult.vMoveDir += _vector{ 1.f, 0.f, 0.f,0.f };
			tResult.vDir += vRight;
    }
    tResult.vDir = XMVector3Normalize(tResult.vDir);
    tResult.vMoveDir = tResult.vDir;
    if (m_bPlatformerMode)
    {
        //Up 방향 제거하기
        tResult.vMoveDir = XMVector2Normalize(XMVectorMultiply(tResult.vMoveDir, _vector{ 1,1 } - XMVectorAbs(vUp)));
    }
    tResult.bInputStates[PLAYER_INPUT_MOVE] =false ==  XMVector3Equal(tResult.vMoveDir, XMVectorZero());
    
    return tResult;
}


_bool CPlayer::Is_Sneaking()
{
    STATE eState = Get_CurrentStateID();
    if (Is_SneakMode())
    {
        if (STATE::IDLE == eState
            || STATE::JUMP_DOWN == eState)
            return true;
        else if (STATE::RUN == eState)
            return  static_cast<CPlayerState_Run*>(m_pStateMachine->Get_CurrentState())->Is_Sneaking();
        else
            return false;
	}
	else
	{
		return false;
	}

}

_bool CPlayer::Is_AttackTriggerActive()
{
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        return m_pAttack2DTriggerCom->Is_Active();
    }
    else
    {
        return m_pSword->Is_AttackEnable();
    }
}

_float CPlayer::Get_UpForce()
{
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        return m_f2DUpForce;
    }
    else
    {
        return XMVectorGetY( static_cast<CActor_Dynamic*>(m_pActorCom)->Get_LinearVelocity());
    }

}

_float CPlayer::Get_AnimProgress()
{
    CModel* pModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(Get_CurCoord());
	if (nullptr != pModel)
	{
		return pModel->Get_CurrentAnimProgeress();
	}
    return 0;
}

_bool CPlayer::Is_SwordHandling()
{
    return Is_SwordMode()&& (m_pSword->Is_SwordHandling());
}

_vector CPlayer::Get_CenterPosition()
{
	if (COORDINATE_2D == Get_CurCoord())
		return Get_FinalPosition() + _vector{ 0,m_f2DCenterYOffset, 0 };
	else
        return Get_FinalPosition() + _vector{0,m_f3DCenterYOffset, 0};
}

_vector CPlayer::Get_HeadPosition()
{
    if (COORDINATE_2D == Get_CurCoord())
        return Get_FinalPosition();
    else
        return Get_FinalPosition() + _vector{ 0,m_fHeadHeight, 0 };
}

_vector CPlayer::Get_LookDirection()
{
	COORDINATE eCoord = Get_CurCoord();
    if (COORDINATE_2D == eCoord)
        return FDir_To_Vector(EDir_To_FDir( m_e2DDirection_E));
    else
        return XMVector4Normalize( m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
}

_vector CPlayer::Get_LookDirection(COORDINATE _eCoord)
{
    if (COORDINATE_2D == _eCoord)
        return FDir_To_Vector(EDir_To_FDir(m_e2DDirection_E));
    else
        return XMVector4Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
}

CPlayer::STATE CPlayer::Get_CurrentStateID()
{
	return m_pStateMachine->Get_CurrentState()->Get_StateID();
}

CCarriableObject* CPlayer::Get_CarryingObject()
{
    { return static_cast<CCarriableObject*>(m_PartObjects[PLAYER_PART_CARRYOBJ]); }
}




void CPlayer::Switch_Animation(_uint _iAnimIndex)
{

	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(_iAnimIndex);
}

void CPlayer::Set_Animation(_uint _iAnimIndex)
{
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(_iAnimIndex);
}

void CPlayer::Set_State(STATE _eState)
{
    //_uint iAnimIdx;
    switch (_eState)
    {
    case Client::CPlayer::IDLE:
        m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
        break;
    case Client::CPlayer::RUN:
        m_pStateMachine->Transition_To(new CPlayerState_Run(this));
        break;
    case Client::CPlayer::JUMP_UP:
        m_pStateMachine->Transition_To(new CPlayerState_JumpUp(this));
        break;
    case Client::CPlayer::JUMP_DOWN:
        m_pStateMachine->Transition_To(new CPlayerState_JumpDown(this));
        break;
    case Client::CPlayer::JUMP_ATTACK:
        m_pStateMachine->Transition_To(new CPlayerState_JumpAttack(this));
        break;
    case Client::CPlayer::ATTACK:
        m_pStateMachine->Transition_To(new CPlayerState_Attack(this));
        break;
    case Client::CPlayer::ROLL:
        m_pStateMachine->Transition_To(new CPlayerState_Roll(this));
		break;
    case Client::CPlayer::THROWSWORD:
        m_pStateMachine->Transition_To(new CPlayerState_ThrowSword(this));
        break;
    case Client::CPlayer::CLAMBER:
        m_pStateMachine->Transition_To(new CPlayerState_Clamber(this));
        break;
    case Client::CPlayer::SPINATTACK:
        m_pStateMachine->Transition_To(new CPlayerState_SpinAttack(this));
        break;
    case Client::CPlayer::PICKUPOBJECT:
		m_pStateMachine->Transition_To(new CPlayerState_PickUpObject(this));
        break;
    case Client::CPlayer::THROWOBJECT:
		m_pStateMachine->Transition_To(new CPlayerState_ThrowObject(this));
		break;
    case Client::CPlayer::STATE_LAST:
        break;
    default:
        break;
    }
}

void CPlayer::Set_Mode(PLAYER_MODE _eNewMode)
{
    if (m_ePlayerMode != _eNewMode)
    {
        m_ePlayerMode = _eNewMode;
        switch (m_ePlayerMode)
        {
        case Client::CPlayer::PLAYER_MODE::PLAYER_MODE_NORMAL:
			UnEquip_Part(PLAYER_PART_SWORD);
            break;
        case Client::CPlayer::PLAYER_MODE::PLAYER_MODE_SWORD:
			Equip_Part(PLAYER_PART_SWORD);
            break;
        case Client::CPlayer::PLAYER_MODE::PLAYER_MODE_SNEAK:
            UnEquip_Part(PLAYER_PART_SWORD);
            break;
        default:
            break;
        }
    }
}





void CPlayer::Set_2DDirection(E_DIRECTION _eEDir)
{

    m_e2DDirection_E = _eEDir;
    switch (m_e2DDirection_E)
    {
    case Client::E_DIRECTION::LEFT:
    case Client::E_DIRECTION::LEFT_UP:
    case Client::E_DIRECTION::LEFT_DOWN:
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        m_pBody->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
        break;
    }
    case Client::E_DIRECTION::RIGHT:
    case Client::E_DIRECTION::RIGHT_UP:
    case Client::E_DIRECTION::RIGHT_DOWN:
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        m_pBody->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
        break;
    }
    default:
        break;
    }

    
}

void CPlayer::Set_3DTargetDirection(_fvector _vDir)
{
    m_v3DTargetDirection = XMVector4Normalize( _vDir);
}
void CPlayer::Set_SwordGrip(_bool _bForehand)
{
	m_pSword->Switch_Grip(_bForehand);
}
void CPlayer::Set_Kinematic(_bool _bKinematic)
{
	CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
	if (_bKinematic)
    {
        pDynamicActor->Late_Update(0);
        pDynamicActor->Set_Kinematic();

    }
	else
    {
        pDynamicActor->Update(0);
        pDynamicActor->Set_Dynamic();
    }
}
HRESULT CPlayer::Set_CarryingObject(CCarriableObject* _pCarryingObject)
{
    //손 비우기
    if (nullptr == _pCarryingObject)
    {
        if (Is_CarryingObject())
        {

            Safe_Release(m_PartObjects[PLAYER_PART_CARRYOBJ]);
            m_PartObjects[PLAYER_PART_CARRYOBJ] = nullptr;
        }
        return S_OK;
    }
    //손에 물건 들리기
    else
    {
        if (Is_CarryingObject())
            return E_FAIL;
        m_PartObjects[PLAYER_PART_CARRYOBJ] = _pCarryingObject;
        Safe_AddRef(m_PartObjects[PLAYER_PART_CARRYOBJ]);

        Set_State(PICKUPOBJECT);
    }

    return S_OK;
}
void CPlayer::Start_Attack(ATTACK_TYPE _eAttackType)
{
	m_eCurAttackType = _eAttackType;
	if (COORDINATE_2D == Get_CurCoord())
	{
        m_pAttack2DTriggerCom->Set_Active(true);
		static_cast<CCollider_Circle*>( m_pAttack2DTriggerCom)->Set_Radius(m_f2DAttackTriggerDesc[_eAttackType].fRadius);
        m_pAttack2DTriggerCom->Set_Offset(m_f2DAttackTriggerDesc[_eAttackType].fOffset);
	}
	else
	{
		m_pSword->Set_AttackEnable(true);
	}
}

void CPlayer::End_Attack()
{
    Flush_AttckedSet();
    if (COORDINATE_2D == Get_CurCoord())
    {
        m_pAttack2DTriggerCom->Set_Active(false);
    }
    else
    {
        m_pSword->Set_AttackEnable(false);
    }
}

void CPlayer::Equip_Part(PLAYER_PART _ePartId)
{
	for (_int i = PLAYER_PART_SWORD; i < PLAYER_PART_LAST; ++i)
		Set_PartActive((_uint)i, false);
    if(COORDINATE_3D == Get_CurCoord())
	    Set_PartActive(_ePartId, true);
}

void CPlayer::UnEquip_Part(PLAYER_PART _ePartId)
{
	Set_PartActive(_ePartId, false);
}

void CPlayer::ThrowSword()
{
	if (COORDINATE_3D == Get_CurCoord())
        m_pSword->Throw(XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK)));
    else
    {
		m_pSword->Throw(EDir_To_Vector(m_e2DDirection_E));
    }

}

void CPlayer::ThrowObject()
{
	assert(Is_CarryingObject());


	COORDINATE eCoord = Get_CurCoord();
    _vector vForce = { 0,0,0 };
    if (COORDINATE_3D == eCoord)
    {
        vForce = XMVector3Normalize(XMVectorSetY(m_pControllerTransform->Get_State(CTransform::STATE_LOOK), 0.5)) * m_f3DThrowObjectPower;
    }
    else
    {
        vForce = XMVector2Normalize(EDir_To_Vector( Get_2DDirection())) * m_f2DThrowObjectPower;
		vForce = XMVectorSetW(XMVectorSetZ(vForce, 0),0);
    }

	CCarriableObject* pObj = static_cast<CCarriableObject*>(m_PartObjects[PLAYER_PART_CARRYOBJ]);
    pObj->Set_Carrier(nullptr);
    if (COORDINATE_3D == Get_CurCoord())
    {
        pObj->Set_Kinematic(false);
    }
    else
    {
        pObj->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, Get_FinalPosition());
    }
	pObj->Throw(vForce);
	Set_CarryingObject(nullptr);
}

void CPlayer::Key_Input(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::F1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);
        if (iCurCoord == COORDINATE_2D)
            CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_OBJECT);
        else
            CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);

        Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);
        //Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
    }
    if (KEY_DOWN(KEY::F3))
    {

        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);
        if (iCurCoord == COORDINATE_2D)
            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(L"Chapter2_SKSP_01",this, SECTION_2D_PLAYMAP_OBJECT);
        else
            CSection_Manager::GetInstance()->Remove_GameObject_ToSectionLayer(L"Chapter2_SKSP_01",this);

        Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);

        //m_pActorCom->Set_AllShapeEnable(false);

    }
    if (KEY_DOWN(KEY::V))
    {
        // 도형 크기 바꾸기
        //SHAPE_CAPSULE_DESC Desc;
        //Desc.fRadius = 1.f;
        //Desc.fHalfHeight = 1.f;
        //m_pActorCom->Set_ShapeGeometry(0, PxGeometryType::eCAPSULE, &Desc);
		COORDINATE eCurCoord = Get_CurCoord();
        if (COORDINATE_2D == eCurCoord)
            m_bPlatformerMode = !m_bPlatformerMode;
        else
            m_bPlatformerMode = false;
        //m_pControllerTransform->Rotation(XMConvertToRadians(m_bPlatformerMode ? 90 : 0), {0,0,1});
    }
    if (KEY_DOWN(KEY::F2))
    {
        PLAYER_MODE eNextMode = (PLAYER_MODE)((m_ePlayerMode + 1) % PLAYER_MODE_LAST);
        Set_Mode(eNextMode);
    }

    if (KEY_DOWN(KEY::F4))
    {
        Event_DeleteObject(this);
    }
    if (KEY_DOWN(KEY::M))
    {
        //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->To_NextAnimation();

    }

    if (KEY_DOWN(KEY::TAB))
    {
        //m_pActorCom->Set_GlobalPose(_float3(-31.f, 6.56f, 22.5f));
        //m_pActorCom->Set_GlobalPose(_float3(23.5f, 20.56f, 22.5f));
        //m_pActorCom->Set_GlobalPose(_float3(42.f, 8.6f, 20.f));
        m_pActorCom->Set_GlobalPose(_float3(40.f, 0.35f, -7.f));
        //m_pActorCom->Set_GlobalPose(_float3(0.f, 0.35f, 0.f));
    }

}


CPlayer* CPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayer* pInstance = new CPlayer(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* _pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    // test
    Safe_Release(m_pBody2DColliderCom);
    Safe_Release(m_pBody2DTriggerCom);
    Safe_Release(m_pAttack2DTriggerCom);

	Safe_Release(m_pStateMachine);
	Safe_Release(m_pAnimEventGenerator);
    if((_int)(m_PartObjects.size() -1 )>= (_int)PLAYER_PART_CARRYOBJ)
        Safe_Release(m_PartObjects[PLAYER_PART_CARRYOBJ]);
    __super::Free();
}
