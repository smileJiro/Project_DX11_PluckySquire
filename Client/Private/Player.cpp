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
#include "PlayerState_Roll.h"
#include "PlayerState_Clamber.h"
#include "PlayerState_ThrowSword.h"
#include "Actor_Dynamic.h"
#include "PlayerSword.h"    
#include "Section_Manager.h"    
#include "Collision_Manager.h"    

CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCharacter(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _Prototype)
    :CCharacter(_Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* _pArg)
{
    CPlayer::CONTAINEROBJ_DESC* pDesc = static_cast<CPlayer::CONTAINEROBJ_DESC*>(_pArg);
    pDesc->iNumPartObjects = CPlayer::PLAYER_PART_LAST;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 500.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720);
    pDesc->tTransform3DDesc.fSpeedPerSec = 8.f;

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
    CapsuleDesc.fHalfHeight = m_fCenterHeight - m_fFootLength;
	//SHAPE_BOX_DESC ShapeDesc = {};
	//ShapeDesc.vHalfExtents = { 0.5f, 1.f, 0.5f };

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &CapsuleDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData.eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData.eMaterial = ACTOR_MATERIAL::CHARACTER_CAPSULE;  // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData.iShapeUse = SHAPE_BODY;
    ShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, m_fCenterHeight + 0.1, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    //마찰용 박스
    SHAPE_BOX_DESC BoxDesc = {};
	_float fHalfWidth = CapsuleDesc.fRadius * cosf(XMConvertToRadians(45.f));
    BoxDesc.vHalfExtents = { fHalfWidth, CapsuleDesc.fRadius, fHalfWidth };
    SHAPE_DATA BoxShapeData;
    BoxShapeData.eShapeType = SHAPE_TYPE::BOX;
    BoxShapeData.pShapeDesc = &BoxDesc;
    XMStoreFloat4x4(&BoxShapeData.LocalOffsetMatrix,XMMatrixTranslation(0.0f, BoxDesc.vHalfExtents.y, 0.0f));
    BoxShapeData.iShapeUse = SHAPE_FOOT;
    BoxShapeData.isTrigger = false;                    
    BoxShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
    ActorDesc.ShapeDatas.push_back(BoxShapeData);

    //충돌 감지용 구 (트리거)
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.iShapeUse = SHAPE_TRIGER;
    ShapeData.isTrigger = true;                    
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0, 0.5, 0)); //여기임
    SHAPE_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 1.f;
    ShapeData.pShapeDesc = &SphereDesc;

    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::MONSTER | OBJECT_GROUP::INTERACTION_OBEJCT | OBJECT_GROUP::MONSTER_PROJECTILE;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;

    if (FAILED(__super::Initialize(pDesc)))
    {
        MSG_BOX("CPlayer super Initialize Failed");
        return E_FAIL;
    }

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

	m_tStat[COORDINATE_3D].fMoveSpeed = 10.f;
	m_tStat[COORDINATE_3D].fJumpPower = 10.f;	
    m_tStat[COORDINATE_2D].fMoveSpeed = 500.f;
	m_tStat[COORDINATE_2D].fJumpPower = 10.f;
     
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
    Bind_AnimEventFunc("Attack", bind(&CPlayer::Attack, this));

	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
	m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_GAMEPLAY, TEXT("Prototype_Component_PlayerAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenrator"), m_pAnimEventGenerator);

   /* Test 2D Collider */
   CCollider_Circle::COLLIDER_CIRCLE_DESC AABBDesc = {};
   AABBDesc.pOwner = this;
   AABBDesc.fRadius = 50.f;
   AABBDesc.vScale = { 1.0f, 1.0f };
   AABBDesc.vOffsetPosition = { 0.f,  AABBDesc.fRadius  };
   AABBDesc.isBlock = false;
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
       TEXT("Com_Collider_Test"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
       return E_FAIL; 

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
    BodyDesc.strModelPrototypeTag_2D = TEXT("Prototype_Component_player2DAnimation");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Latch_SkelMesh_NewRig");
    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;
    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_NONBLEND;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerBody"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
    {
        MSG_BOX("CPlayer Body Creation Failed");
        return E_FAIL;
    }

	//Part Sword
	CPlayerSword::PLAYER_SWORD_DESC SwordDesc{};
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
    SwordDesc.iPriorityID_3D = PR3D_NONBLEND;
    SwordDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
    SwordDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    m_PartObjects[PLAYER_PART_SWORD] = m_pSword = static_cast<CPlayerSword*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerSword"), &SwordDesc));
    if (nullptr == m_PartObjects[PLAYER_PART_SWORD])
    {
        MSG_BOX("CPlayer Sword Creation Failed");
        return E_FAIL;
    }
	m_PartObjects[PLAYER_PART_SWORD]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{1,0,0,0});
	Set_PartActive(PLAYER_PART_SWORD, false);

	//Part Glove
    BodyDesc.strModelPrototypeTag_3D = TEXT("latch_glove");
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
	BodyDesc.eActorType = ACTOR_TYPE::LAST;
	BodyDesc.pActorDesc = nullptr;
    m_PartObjects[PLAYER_PART_GLOVE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PLAYER_PART_GLOVE])
    {
        MSG_BOX("CPlayer Glove Creation Failed");
        return E_FAIL;
    }
    C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D));
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_GLOVE])->Set_SocketMatrix(p3DModel->Get_BoneMatrix("j_glove_hand_r")); /**/
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
    return S_OK;
}

void CPlayer::Priority_Update(_float _fTimeDelta)
{

    CContainerObject::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}


void CPlayer::Update(_float _fTimeDelta)
{
    Key_Input(_fTimeDelta);

    //// TestCode : 태웅
    _uint iSectionKey = RG_2D + PR2D_SECTION_START;
    CCollision_Manager::GetInstance()->Add_Collider(m_strSectionName, OBJECT_GROUP::PLAYER, m_pColliderCom);

    __super::Update(_fTimeDelta); /* Part Object Update */
    m_vLookBefore = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    m_bOnGround = false;
}

// 충돌 체크 후 container의 transform을 밀어냈어. 

void CPlayer::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CPlayer::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CPlayer::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    int a = 0;

}

void CPlayer::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
	SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::CPlayer::SHAPE_BODY:
        for (auto& pxPairData : _ContactPointDatas)
        {
            _vector vContactNormal = { pxPairData.normal.x,pxPairData.normal.y,pxPairData.normal.z };
            if (abs(pxPairData.normal.y) < m_fStepSlopeThreshold)
            {
                m_bContactWall = true;
            }
        }
        break;
    case Client::CPlayer::SHAPE_FOOT:
        for (auto& pxPairData : _ContactPointDatas)
        {
            _vector vMyPos = Get_FinalPosition();
            ////발 범위에 안닿으면 무시
            //if (vMyPos.m128_f32[1] + m_fFootLength < pxPairData.position.y
            //    || vMyPos.m128_f32[1] > pxPairData.position.y)
            //    continue;
            //천장이면 무시
            if (pxPairData.normal.y < 0)
                continue;
            //닿은 곳의 경사가 너무 급하면 무시
            if (pxPairData.normal.y < m_fStepSlopeThreshold)
                continue;
            m_bOnGround = true;
            return;
        }
        break;
    case Client::CPlayer::SHAPE_TRIGER:
        break;
    default:
        break;
    }

}

void CPlayer::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    int a = 0;

}

void CPlayer::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    int a = 0;
}

void CPlayer::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    int a = 0;
}

void CPlayer::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    int a = 0;
}

void CPlayer::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    int a = 0;
}

void CPlayer::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    int a = 0;
}

void CPlayer::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    int a = 0;
}

void CPlayer::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pStateMachine->Get_CurrentState()->On_AnimEnd(_eCoord, iAnimIdx);

}

HRESULT CPlayer::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == Get_CurCoord())
        Set_2DDirection(E_DIRECTION::DOWN);

    Set_State(IDLE);

    return S_OK;
}


void CPlayer::Attack()
{
    Stop_Move();
    Add_Impuls(Get_3DTargetDirection() * m_fAttackForwardingForce);
}

void CPlayer::Move(_vector _vForce, _float _fTimeDelta)
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
        m_pControllerTransform->Go_Direction(XMVector4Normalize(_vForce), _fTimeDelta);
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
        //m_f2DUpForce = m_tStat[COORDINATE_2D].fJumpPower;
    }
    else
    {
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        _vector vVelocity = pDynamicActor->Get_LinearVelocity();
        pDynamicActor->Add_Impulse(_float3{ 0, m_tStat[COORDINATE_3D].fJumpPower ,0 });
        //pDynamicActor->Set_LinearDamping(2);
    }
}




PLAYER_KEY_RESULT CPlayer::Player_KeyInput()
{
	PLAYER_KEY_RESULT tResult;
    fill(begin(tResult.bKeyStates), end(tResult.bKeyStates), false);


    if (Is_SwordEquiped())
    {
        //기본공격
        if (MOUSE_DOWN(MOUSE_KEY::LB))
			tResult.bKeyStates[PLAYER_KEY_ATTACK] = true;
        //칼 던지기
        else if (MOUSE_DOWN(MOUSE_KEY::RB))
			tResult.bKeyStates[PLAYER_KEY_THROWSWORD] = true;
    }
    //점프
    if (KEY_PRESSING(KEY::SPACE))
        tResult.bKeyStates[PLAYER_KEY_JUMP] = true;
    //구르기
    if (KEY_PRESSING(KEY::LSHIFT))
        tResult.bKeyStates[PLAYER_KEY_ROLL] = true;

    COORDINATE eCoord = Get_CurCoord();
    //이동
    if (KEY_PRESSING(KEY::W))
    {
        if (eCoord == COORDINATE_3D)
            tResult.vMoveDir += _vector{ 0.f, 0.f, 1.f,0.f };
        else
            tResult.vMoveDir += _vector{ 0.f, 1.f, 0.f,0.f };
        tResult.bKeyStates[PLAYER_KEY_MOVE] = true;
    }
    if (KEY_PRESSING(KEY::A))
    {
        tResult.vMoveDir += _vector{ -1.f, 0.f, 0.f,0.f };
        tResult.bKeyStates[PLAYER_KEY_MOVE] = true;
    }
    if (KEY_PRESSING(KEY::S))
    {
        if (eCoord == COORDINATE_3D)
            tResult.vMoveDir += _vector{ 0.f, 0.f, -1.f,0.f };
        else
            tResult.vMoveDir += _vector{ 0.f, -1.f, 0.f,0.f };
        tResult.bKeyStates[PLAYER_KEY_MOVE] = true;
    }
    if (KEY_PRESSING(KEY::D))
    {
        tResult.vMoveDir += _vector{ 1.f, 0.f, 0.f,0.f };
        tResult.bKeyStates[PLAYER_KEY_MOVE] = true;
    }
    if(tResult.bKeyStates[PLAYER_KEY_MOVE])
        tResult.vMoveDir = XMVector3Normalize(tResult.vMoveDir);
    return tResult;
}


_float CPlayer::Get_UpForce()
{
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        return -1.f;
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

_bool CPlayer::Is_SwordEquiped()
{
    return m_pSword->Is_Active() && (false == m_pSword->Is_Flying());
}

_bool CPlayer::Is_CarryingObject()
{
    return nullptr != m_pCarryingObject;
}

_vector CPlayer::Get_CenterPosition()
{
	if (COORDINATE_2D == Get_CurCoord())
		return Get_FinalPosition();
	else
        return Get_FinalPosition() + _vector{0,m_fCenterHeight, 0};
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
        return EDir_To_Vector(m_e2DDirection_E);
    else
        return XMVector4Normalize( m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
}

CPlayer::STATE CPlayer::Get_CurrentStateID()
{
	return m_pStateMachine->Get_CurrentState()->Get_StateID();
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
    case Client::CPlayer::ATTACK:
        m_pStateMachine->Transition_To(new CPlayerState_Attack(this));
        break;
    case Client::CPlayer::ROLL:
        m_pStateMachine->Transition_To(new CPlayerState_Roll(this, m_v3DTargetDirection));
		break;
    case Client::CPlayer::THROWSWORD:
        m_pStateMachine->Transition_To(new CPlayerState_ThrowSword(this));
        break;
        case Client::CPlayer::CLAMBER:
        m_pStateMachine->Transition_To(new CPlayerState_Clamber(this));
        break;
    case Client::CPlayer::STATE_LAST:
        break;
    default:
        break;
    }
}



void CPlayer::Set_2DDirection(E_DIRECTION _eEDir)
{
    m_e2DDirection_E = _eEDir;
	F_DIRECTION eFDir = EDir_To_FDir(m_e2DDirection_E);
    if (F_DIRECTION::LEFT ==  eFDir)
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
    }
    else if (F_DIRECTION::RIGHT == eFDir)
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
    }

    
}

void CPlayer::Set_3DTargetDirection(_fvector _vDir)
{
    m_v3DTargetDirection = XMVector4Normalize( _vDir);
}
void CPlayer::Equip_Part(PLAYER_PART _ePartId)
{
	for (_int i = PLAYER_PART_SWORD; i < PLAYER_PART_LAST; ++i)
		Set_PartActive((_uint)i, false);
	Set_PartActive(_ePartId, true);
}

void CPlayer::UnEquip_Part(PLAYER_PART _ePartId)
{
	Set_PartActive(_ePartId, false);
}

void CPlayer::ThrowSword()
{
    m_pSword->Throw(XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK)));

}



void CPlayer::Key_Input(_float _fTimeDelta)
{


    if (KEY_DOWN(KEY::F1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);
        if (iCurCoord == COORDINATE_2D)
            CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);
        else
            CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);

        Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);
        //Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
    }
    if (KEY_DOWN(KEY::F3))
    {
        m_pActorCom->Set_AllShapeEnable(false);

    }
    if (KEY_DOWN(KEY::B))
    {
        // 도형 크기 바꾸기
        SHAPE_CAPSULE_DESC Desc;
        Desc.fRadius = 1.f;
        Desc.fHalfHeight = 1.f;
        m_pActorCom->Set_ShapeGeometry(0, PxGeometryType::eCAPSULE, &Desc);
    }
    if (KEY_DOWN(KEY::F2))
    {
		if(Get_PartObject(PLAYER_PART_SWORD)->Is_Active())
		    UnEquip_Part(PLAYER_PART_SWORD);
        else
			Equip_Part(PLAYER_PART_SWORD);
    }

    if (KEY_DOWN(KEY::F4))
    {
        Event_DeleteObject(this);
    }
    if (KEY_DOWN(KEY::M))
    {
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->To_NextAnimation();
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
    Safe_Release(m_pColliderCom);

	Safe_Release(m_pStateMachine);
	Safe_Release(m_pAnimEventGenerator);
    __super::Free();
}
