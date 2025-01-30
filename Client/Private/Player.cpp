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
#include "PlayerState_Jump.h"
#include "PlayerState_Roll.h"
#include "Actor_Dynamic.h"
#include "PlayerSword.h"    

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
    SHAPE_CAPSULE_DESC ShapeDesc = {};
    ShapeDesc.fHalfHeight = 0.25f;
    ShapeDesc.fRadius = 0.25f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData.eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;  // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 만약, Shape을 여러개 사용하고싶다면, 아래와 같이 별도의 Shape에 대한 정보를 추가하여 push_back() */
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.isTrigger = true;                     // Trigger로 사용하겠다.
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

	m_tStat[COORDINATE_3D].fMoveSpeed = 500.f;
	m_tStat[COORDINATE_3D].fJumpPower = 17.f;	
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

    Bind_AnimEventFunc("Someting", bind(&CPlayer::Someting, this, 1));
    Bind_AnimEventFunc("Someting2", bind(&CPlayer::Someting2, this, 0.1f));
    Bind_AnimEventFunc("Someting3", bind(&CPlayer::Someting3, this));

	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
	m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_GAMEPLAY, TEXT("Prototype_Component_PlayerAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenrator"), m_pAnimEventGenerator);
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
    BodyDesc.iRenderGroupID_2D = RG_3D;
    BodyDesc.iPriorityID_2D = PR3D_BOOK2D;
    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_NONBLEND;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
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
    SwordDesc.iRenderGroupID_2D = RG_3D;
    SwordDesc.iPriorityID_2D = PR3D_BOOK2D;
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
    __super::Update(_fTimeDelta); /* Part Object Update */
    m_vLookBefore = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
}

void CPlayer::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CPlayer::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

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
    int a = 0;
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


void CPlayer::Move(_vector _vDir, _float _fTimeDelta)
{
    ACTOR_TYPE eActorType = Get_ActorType();
    if (ACTOR_TYPE::KINEMATIC == eActorType)
    {
        if (COORDINATE_3D ==Get_CurCoord())
        {
            m_pControllerTransform->Set_AutoRotationYDirection(_vDir);
            m_pControllerTransform->Update_AutoRotation(_fTimeDelta);
        }
        else
        {

        }
        m_pControllerTransform->Go_Direction(_vDir, m_tStat[COORDINATE_2D].fMoveSpeed,_fTimeDelta);
    }
    else if (ACTOR_TYPE::DYNAMIC == eActorType)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            CActor_Dynamic* pDynamicActor =static_cast<CActor_Dynamic*>(m_pActorCom);

            
            _vector vLook = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
            _float3 vLookDiff; XMStoreFloat3( &vLookDiff,_vDir - vLook);
            _float3 vLookDiffBefore; XMStoreFloat3(&vLookDiffBefore, _vDir - m_vLookBefore);
			if (XMVector3Equal(_vDir, XMVectorZero()))
            {
				_vDir = vLook;
            }
            if (XMVector3NearEqual( _vDir ,vLook, XMVectorReplicate(1e-6f)))
            {
                pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
            }
            else if ((vLookDiff.x * vLookDiffBefore.x) < 0 
                || (vLookDiff.z * vLookDiffBefore.z) < 0)
            {
                pDynamicActor->Set_Rotation(_vDir);
                pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
            }
            else
            {
                _vector vAxis = XMVector3Normalize(XMVector3Cross(vLook, _vDir));
                if(XMVector3Equal(vAxis, XMVectorZero()))
					vAxis = XMVectorSet(0, 1, 0, 0);
                pDynamicActor->Set_AngularVelocity(vAxis * XMConvertToRadians(720));

            }

            _float fDot = abs( XMVectorGetX(XMVector3Dot(vLook, _vDir)));
            _vector vVeclocity = _vDir* m_tStat[COORDINATE_3D].fMoveSpeed * _fTimeDelta * fDot;
            vVeclocity = XMVectorSetY(vVeclocity, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));
            pDynamicActor->Set_LinearVelocity(vVeclocity);

        }
        else
        {
            m_pControllerTransform->Go_Direction(_vDir, _fTimeDelta);
        }
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
            vLook = XMVectorSetY(vLook * fVelocity * _fTimeDelta, XMVectorGetY(pDynamicActor->Get_LinearVelocity()));
            pDynamicActor->Set_LinearVelocity(vLook);
        }
        else
        {
            _vector vDir = EDir_To_Vector(m_e2DDirection_E);
            m_pControllerTransform->Go_Direction(vDir, fVelocity,_fTimeDelta);
        }
    }

}

void CPlayer::Stop_Rotate()
{

    ACTOR_TYPE eActorType = Get_ActorType();
    if (ACTOR_TYPE::DYNAMIC == eActorType)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
            pDynamicActor->Set_AngularVelocity(_vector{ 0,0,0,0 });
        }
    }
}

void CPlayer::Stop_Move()
{
    ACTOR_TYPE eActorType = Get_ActorType();
    if (ACTOR_TYPE::DYNAMIC == eActorType)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
            pDynamicActor->Set_LinearVelocity(_vector{ 0,0,0,0 });
        }
        else
        {

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
        m_pActorCom->Add_Impulse(_float3(0.0f, m_tStat[COORDINATE_3D].fJumpPower, 0.0f));
}



_bool CPlayer::Is_OnGround()
{
    if (XMVectorGetY(m_pControllerTransform->Get_State(CTransform::STATE_POSITION)) <= 0.8f)
		return true;
    else
        return false;
}

_float CPlayer::Get_UpForce()
{
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {

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
    case Client::CPlayer::JUMP:
        m_pStateMachine->Transition_To(new CPlayerState_Jump(this));
        break;
    case Client::CPlayer::ATTACK:
        m_pStateMachine->Transition_To(new CPlayerState_Attack(this, m_e2DDirection_E));
        break;
    case Client::CPlayer::ROLL:
        m_pStateMachine->Transition_To(new CPlayerState_Roll(this));
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


void CPlayer::Someting(int a)
{
    cout << "Someting" << endl;
    int b = a;
}

void CPlayer::Someting2(float a)
{
    cout << "Someting2" << endl;
    float b = a;
}

_int CPlayer::Someting3()
{
    int a = 0;
    cout << "Someting3" << endl;
    return a;
}

void CPlayer::Key_Input(_float _fTimeDelta)
{


    if (KEY_DOWN(KEY::F1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);
        Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);
        //Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
    }
    if (KEY_DOWN(KEY::F3))
    {
        //static_cast<CActor_Dynamic*>(m_pActorCom)->On_Kinematic();
        //m_pActorCom->Set_ShapeLocalOffsetPosition(0, _float3(0.0f, 0.5f, 0.0f));
        m_pActorCom->Set_ShapeLocalOffsetPitchYawRoll(0, _float3(XMConvertToRadians(0.f), XMConvertToRadians(00.f), XMConvertToRadians(60.f)));
        //m_pActorCom->Delete_Shape(1);
        //m_pActorCom->Set_ShapeLocalOffsetMatrix(0, XMMatrixRotationZ(XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.0f, -1.0f, 0.0f));
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
    if (Is_SwordEquiped() && MOUSE_DOWN(MOUSE_KEY::RB))
    {
         _vector vLook =  XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
        m_pSword->Throw(vLook);
        return;
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
	Safe_Release(m_pStateMachine);
	Safe_Release(m_pAnimEventGenerator);
    __super::Free();
}
