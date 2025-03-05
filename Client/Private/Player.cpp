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
#include "PlayerState_LaydownObject.h"
#include "PlayerState_Roll.h"
#include "PlayerState_Clamber.h"
#include "PlayerState_ThrowSword.h"
#include "PlayerState_SpinAttack.h"
#include "PlayerState_PickUpObject.h"
#include "PlayerState_Die.h"
#include "PlayerState_StartPortal.h"
#include "PlayerState_IntoPortal.h"
#include "PlayerState_ExitPortal.h"
#include "PlayerState_TurnBook.h"
#include "PlayerState_Evict.h"
#include "PlayerState_LunchBox.h"
#include "PlayerState_Electric.h"
#include "PlayerState_Drag.h"
#include "PlayerState_Stamp.h"
#include "PlayerState_Bomber.h"
#include "PlayerState_ErasePalmDecal.h"
#include "PlayerState_GetItem.h"
#include "Actor_Dynamic.h"
#include "PlayerSword.h"    
#include "PlayerBody.h"
#include "StopStamp.h"
#include "BombStamp.h"
#include "Detonator.h"
#include "ZetPack.h"
#include "Section_Manager.h"
#include "UI_Manager.h"
#include "Effect2D_Manager.h"
#include "Effect_Manager.h"
#include "PlayerData_Manager.h"
    
#include "Collider_Fan.h"
#include "Collider_AABB.h"
#include "Interactable.h"
#include "CarriableObject.h"
#include "Blocker.h"
#include "NPC_Store.h"
#include "Portal.h"
#include "DraggableObject.h"
#include "Book.h"

CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCharacter(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _Prototype)
    :CCharacter(_Prototype)
{
    for (_uint i = 0; i < ATTACK_TYPE_LAST; i++)
    {
        for (_uint j = 0; j < (_uint)F_DIRECTION::F_DIR_LAST; j++)
        {
    		m_f2DAttackTriggerDesc[i][j] = _Prototype.m_f2DAttackTriggerDesc[i][j];
        }
    }
}

HRESULT CPlayer::Initialize_Prototype()
{
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::DOWN].vExtents = { 100.f, 60.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::DOWN].vOffset = { 0.f, 0.f};
	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::UP].vExtents = { 100.f, 60.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::UP].vOffset = { 0.f, 65.f  };
	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::RIGHT].vExtents = { 55.5f, 86.5f };
	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::RIGHT].vOffset = { 40.f, m_f2DCenterYOffset};
	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::LEFT].vExtents = { 55.5f, 86.5f };
	m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::LEFT].vOffset = { -40.f,m_f2DCenterYOffset };

    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::DOWN].vExtents = { 100.f, 60.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::DOWN].vOffset = { 0.f, 0.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::UP].vExtents = { 100.f, 60.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::UP].vOffset = { 0.f, 65.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::RIGHT].vExtents = { 55.5f, 86.5f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::RIGHT].vOffset = { 40.f, m_f2DCenterYOffset };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::LEFT].vExtents = { 55.5f, 86.5f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL2][(_uint)F_DIRECTION::LEFT].vOffset = { -40.f,m_f2DCenterYOffset };

    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::DOWN].vExtents = { 70.f, 70.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::DOWN].vOffset = { 0.f, -45.f  };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::UP].vExtents = { 70.f, 70.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::UP].vOffset = { 0.f, 150.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::RIGHT].vExtents = { 70.f, 70.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::RIGHT].vOffset = { 80.f, 0.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::LEFT].vExtents = { 70.f, 70.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL3][(_uint)F_DIRECTION::LEFT].vOffset = { -80.f,0.f };

    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::DOWN].vExtents = { 211.f, 211.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::DOWN].vOffset = { 0.f, m_f2DCenterYOffset };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::UP].vExtents = { 211.f, 211.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::UP].vOffset = { 0.f, m_f2DCenterYOffset};
    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::RIGHT].vExtents = { 211.f, 211.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::RIGHT].vOffset = { 0.f, m_f2DCenterYOffset};
    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::LEFT].vExtents = { 211.f, 211.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_SPIN][(_uint)F_DIRECTION::LEFT].vOffset = { 0.f, m_f2DCenterYOffset };
                           
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::DOWN].vExtents = { 146.5f, 74.5f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::DOWN].vOffset = { 0.f, -20.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::UP].vExtents = { 146.5f, 74.5f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::UP].vOffset = { 0.f, 60.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::RIGHT].vExtents = { 146.5f, 74.5f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::RIGHT].vOffset = { 50.f, 0.f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::LEFT].vExtents = { 146.5f, 74.5f };
    m_f2DAttackTriggerDesc[ATTACK_TYPE_JUMPATTACK][(_uint)F_DIRECTION::LEFT].vOffset = { -50.f, 0.f };


    return S_OK;
}

HRESULT CPlayer::Initialize(void* _pArg)
{
    CPlayer::CHARACTER_DESC* pDesc = static_cast<CPlayer::CHARACTER_DESC*>(_pArg);

    m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->_fStepHeightThreshold = 0.225f;
	pDesc->_fStepSlopeThreshold = 0.45f;

    pDesc->iNumPartObjects = CPlayer::PLAYER_PART_LAST;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 500.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720);
    pDesc->tTransform3DDesc.fSpeedPerSec = 8.f;
    
    pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER;
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

    m_tBodyShapeDesc.fRadius = m_fFootLength;
    m_tBodyShapeDesc.fHalfHeight = m_f3DCenterYOffset - m_fFootLength;
    //SHAPE_BOX_DESC ShapeDesc = {};
    //ShapeDesc.vHalfExtents = { 0.5f, 1.f, 0.5f };

    ActorDesc.ShapeDatas.resize(PLAYER_SHAPE_USE::PLAYER_SHAPE_USE_LAST);
    // 플레이어 몸통.
    m_tBodyShapeData.pShapeDesc = &m_tBodyShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    m_tBodyShapeData.eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    m_tBodyShapeData.eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT;  // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    m_tBodyShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    m_tBodyShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	m_tBodyShapeData.FilterData.MyGroup = OBJECT_GROUP::PLAYER;
	m_tBodyShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE | OBJECT_GROUP::TRIGGER_OBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::EXPLOSION; // Actor가 충돌을 감지할 그룹
    XMStoreFloat4x4(&m_tBodyShapeData.LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, m_f3DCenterYOffset /*+ 0.1f*/, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas[m_tBodyShapeData.iShapeUse] = m_tBodyShapeData;

    //주변 지형 감지용 구 (트리거)
    SHAPE_SPHERE_DESC SphereDesc = {};
    SphereDesc.fRadius = 2.5f;
    SHAPE_DATA ShapeData = {};
    ShapeData.pShapeDesc = &SphereDesc;
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
    ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    ShapeData.isTrigger = true;
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0, m_f3DCenterYOffset, 0));
    ShapeData.FilterData.MyGroup = OBJECT_GROUP::PLAYER_TRIGGER;
    ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::INTERACTION_OBEJCT;
    ActorDesc.ShapeDatas[ShapeData.iShapeUse] = ShapeData;

    //상호작용 구 (트리거)
    ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData.isTrigger = true;
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0, m_f3DCenterYOffset, m_f3DInteractLookOffset));
    SphereDesc.fRadius = m_f3DInteractRadius;
    ShapeData.pShapeDesc = &SphereDesc;
    ShapeData.iShapeUse = (_uint)PLAYER_SHAPE_USE::INTERACTION;
    ShapeData.FilterData.MyGroup = OBJECT_GROUP::PLAYER_TRIGGER;
    ShapeData.FilterData.OtherGroupMask =OBJECT_GROUP::INTERACTION_OBEJCT;
    ActorDesc.ShapeDatas[ShapeData.iShapeUse] = ShapeData;
    
    //공격시 몸통 가드
    SHAPE_SPHERE_DESC tGuardShapeDesc = {};
    tGuardShapeDesc.fRadius = 0.5f;
    SHAPE_DATA BodyGuardShapeData;
    BodyGuardShapeData.eShapeType = SHAPE_TYPE::SPHERE;
    BodyGuardShapeData.pShapeDesc = &tGuardShapeDesc;
    XMStoreFloat4x4(&BodyGuardShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.f, tGuardShapeDesc.fRadius, 0.0f));
    BodyGuardShapeData.iShapeUse = (_uint)PLAYER_SHAPE_USE::BODYGUARD;
    BodyGuardShapeData.isTrigger = false;
    BodyGuardShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
    BodyGuardShapeData.FilterData.MyGroup = OBJECT_GROUP::PLAYER;
    BodyGuardShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MONSTER; // Actor가 충돌을 감지할 그룹
    ActorDesc.ShapeDatas[BodyGuardShapeData.iShapeUse] = ShapeData;

    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE | OBJECT_GROUP::TRIGGER_OBJECT| OBJECT_GROUP::BLOCKER;

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

    Set_PlatformerMode(false);

	m_pActorCom->Set_ShapeEnable(PLAYER_SHAPE_USE::BODYGUARD,false);
    Set_State(CPlayer::IDLE);

    // PlayerData Manager 등록
    CPlayerData_Manager::GetInstance()->Register_Player(this);

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    /* Part Body */
    CPlayerBody::PLAYER_BODY_DESC BodyDesc{};
	BodyDesc.pPlayer = this;
    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
    BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
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
	Safe_AddRef(m_pBody);
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
    SwordDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
    SwordDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
    SwordDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    SwordDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    SwordDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    SwordDesc.tTransform2DDesc.fSpeedPerSec = 10.f;
    SwordDesc.iRenderGroupID_3D = RG_3D;
    SwordDesc.iPriorityID_3D = PR3D_GEOMETRY;
    SwordDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    SwordDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    m_PartObjects[PLAYER_PART_SWORD] = m_pSword = static_cast<CPlayerSword*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerSword"), &SwordDesc));
    Safe_AddRef(m_pSword);
    if (nullptr == m_PartObjects[PLAYER_PART_SWORD])
    {
        MSG_BOX("CPlayer Sword Creation Failed");
        return E_FAIL;
    }
    Set_PartActive(PLAYER_PART_SWORD, false);
    m_pSword->Switch_Grip(true);
    m_pSword->Set_AttackEnable(false);

    //Part Glove
	CModelObject::MODELOBJECT_DESC GloveDesc{};

    GloveDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    GloveDesc.iCurLevelID = m_iCurLevelID;
    GloveDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    GloveDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    GloveDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    GloveDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    GloveDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    GloveDesc.iRenderGroupID_3D = RG_3D;
    GloveDesc.iPriorityID_3D = PR3D_GEOMETRY;
    GloveDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
    GloveDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
    GloveDesc.strModelPrototypeTag_3D = TEXT("latch_glove");
    GloveDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    GloveDesc.eActorType = ACTOR_TYPE::LAST;
    GloveDesc.pActorDesc = nullptr;
    GloveDesc.isCoordChangeEnable = false;
    m_PartObjects[PLAYER_PART_GLOVE] = m_pGlove = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &GloveDesc));
	Safe_AddRef(m_pGlove);
    if (nullptr == m_PartObjects[PLAYER_PART_GLOVE])
    {
        MSG_BOX("CPlayer Glove Creation Failed");
        return E_FAIL;
    }
    C3DModel* p3DModel = static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D));
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_GLOVE])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_glove_hand_r")); /**/
    m_PartObjects[PLAYER_PART_GLOVE]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });
    Set_PartActive(PLAYER_PART_GLOVE, false);

    //Part STOP STAMP
    CStopStamp::STOP_STAMP_DESC StopStampDesc{};
    StopStampDesc.iCurLevelID = m_iCurLevelID;
    StopStampDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    m_PartObjects[PLAYER_PART_STOP_STMAP] = m_pStopStmap = static_cast<CStopStamp*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_StopStamp"), &StopStampDesc));
    Safe_AddRef(m_pStopStmap);
    if (nullptr == m_PartObjects[PLAYER_PART_STOP_STMAP])
    {
        MSG_BOX("CPlayer STOPSTAMP Creation Failed");
        return E_FAIL;
    }
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_STOP_STMAP])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_hand_attach_r")); /**/
    m_PartObjects[PLAYER_PART_STOP_STMAP]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });
    m_PartObjects[PLAYER_PART_STOP_STMAP]->Set_Position({0.f,-0.4f,0.f});
    Set_PartActive(PLAYER_PART_STOP_STMAP, false);

    //Part BOMB STAMP
	CBombStamp::BOMB_STAMP_DESC BombStampDesc{};
    BombStampDesc.iCurLevelID = m_iCurLevelID;
    BombStampDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    m_PartObjects[PLAYER_PART_BOMB_STMAP] = m_pBombStmap =  static_cast<CBombStamp*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_BombStamp"), &BombStampDesc));
    Safe_AddRef(m_pBombStmap);
    if (nullptr == m_PartObjects[PLAYER_PART_BOMB_STMAP])
    {
        MSG_BOX("CPlayer BOMBSTAMP Creation Failed");
        return E_FAIL;
    }
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_BOMB_STMAP])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_hand_attach_r")); /**/
    m_PartObjects[PLAYER_PART_BOMB_STMAP]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0,1,0,0 });
    m_PartObjects[PLAYER_PART_BOMB_STMAP]->Set_Position({ 0.f,-0.4f,0.f });
    Set_PartActive(PLAYER_PART_BOMB_STMAP, false);


	//Part DETONATOR
    CDetonator::DETONATOR_DESC tDetonatorDesc{};
    tDetonatorDesc.iCurLevelID = m_iCurLevelID;
    tDetonatorDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    m_PartObjects[PLAYER_PART_DETONATOR] = m_pDetonator = static_cast<CDetonator*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Detonator"), &tDetonatorDesc));
    Safe_AddRef(m_pDetonator);
    if (nullptr == m_PartObjects[PLAYER_PART_DETONATOR])
    {
        MSG_BOX("CPlayer BOMBSTAMP Creation Failed");
        return E_FAIL;
    }
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_DETONATOR])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_hand_attach_l")); /**/
    Set_PartActive(PLAYER_PART_DETONATOR, false);

    //Part ZETPACK
    CZetPack::ZETPACK_DESC tZetPackDesc{};
    tZetPackDesc.pPlayer = this;
    tZetPackDesc.iCurLevelID = m_iCurLevelID;
    tZetPackDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
    tZetPackDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    m_PartObjects[PLAYER_PART_ZETPACK]= m_pZetPack  = static_cast<CZetPack*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ZetPack"), &tZetPackDesc));
    if (nullptr == m_PartObjects[PLAYER_PART_ZETPACK])
    {
        MSG_BOX("CPlayer ZETPACK Creation Failed");
        return E_FAIL;
    }
    Safe_AddRef(m_pZetPack);
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_ZETPACK])->Set_SocketMatrix(COORDINATE_3D, p3DModel->Get_BoneMatrix("j_pelvis")); /**/
    static_cast<CPartObject*>(m_PartObjects[PLAYER_PART_ZETPACK])->Set_SocketMatrix(COORDINATE_2D, m_pBody->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_2D)); /**/
    m_PartObjects[PLAYER_PART_ZETPACK]->Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0.f,1.f,0.f,0.f });
    m_PartObjects[PLAYER_PART_ZETPACK]->Set_Position({ 0.f,-0.1f,0.5f });
    Set_PartActive(PLAYER_PART_ZETPACK, false);


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
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_3DAnimationTransitionTime((_uint)ANIM_STATE_3D::LATCH_TURN_LEFT, 0.35f);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_3DAnimationTransitionTime((_uint)ANIM_STATE_3D::LATCH_TURN_RIGHT, 0.35f);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_3DAnimationTransitionTime((_uint)ANIM_STATE_3D::LATCH_TURN_MID, 0.178f);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_3DAnimationTransitionTime((_uint)ANIM_STATE_3D::LATCH_ANIM_IDLE_NERVOUS_01_GT, 0.f);
    static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_3DAnimationTransitionTime((_uint)ANIM_STATE_3D::LATCH_ANIM_BOOKOUT_01_GT, 0.f);
    return S_OK;
}

void CPlayer::Enter_Section(const _wstring _strIncludeSectionName)
{
    /* 태웅 : */
    __super::Enter_Section(_strIncludeSectionName);
    for (auto& i : m_PartObjects)
    {
		if(nullptr == i)
			continue;
		i->Enter_Section(_strIncludeSectionName);
    }
    if (TEXT("Chapter2_P0102") == _strIncludeSectionName)
    {
        Set_Position(XMVectorSet(0.0f, 2800.f, 0.0f, 0.0f));
        Set_PlatformerMode(true);

    }
    else
        Set_PlatformerMode(false);

    if (TEXT("Chapter2_P1314") == _strIncludeSectionName)
    {
        static_cast<CCollider_Circle*>(m_pBody2DTriggerCom)->Set_Radius(9999.f);
    }
    else
    {
        static_cast<CCollider_Circle*>(m_pBody2DTriggerCom)->Set_Radius(m_f2DInteractRange);
    }
}


HRESULT CPlayer::Ready_Components()
{
    CStateMachine::STATEMACHINE_DESC tStateMachineDesc{};
    tStateMachineDesc.pOwner = this;
    
    m_pStateMachine = static_cast<CStateMachine*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), &tStateMachineDesc));
    m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
    Add_Component(TEXT("1StateMachine"), m_pStateMachine);
    Safe_AddRef(m_pStateMachine);

    Bind_AnimEventFunc("ThrowSword", bind(&CPlayer::ThrowSword, this));
    Bind_AnimEventFunc("ThrowObject", bind(&CPlayer::ThrowObject, this));
    Bind_AnimEventFunc("Attack", bind(&CPlayer::Move_Attack_3D, this));
    Bind_AnimEventFunc("StampSmash", bind(&CPlayer::StampSmash, this));
    Bind_AnimEventFunc("Detonate", bind(&CPlayer::Detonate, this));
    Bind_AnimEventFunc("ErasePalm", bind(&CPlayer::ErasePalm, this));
    Bind_AnimEventFunc("SpinAttack", bind(&CPlayer::SpinAttack, this));

	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
	CAnimEventGenerator* pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_PlayerAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenrator"), pAnimEventGenerator);
    
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
    pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_Player2DAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("2DAnimEventGenrator"), pAnimEventGenerator);


    // EffectAnimEvent
    Bind_AnimEventFunc("Dust_Walk", [this]() {CEffect_Manager::GetInstance()->Active_Effect(TEXT("Dust_Walk"), true, m_pControllerTransform->Get_WorldMatrix_Ptr() );});
    Bind_AnimEventFunc("Dust_Dodge", [this]() {CEffect_Manager::GetInstance()->Active_Effect(TEXT("Dust_Dodge"), true, m_pControllerTransform->Get_WorldMatrix_Ptr() );});
    
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D);
    pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_PlayeEffectEvent"), &tAnimEventDesc));
    Add_Component(TEXT("EffectEventGenerator"), pAnimEventGenerator);


	m_p2DColliderComs.resize(3);
   /* Test 2D Collider */
   CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
   CircleDesc.pOwner = this;
   CircleDesc.fRadius = 20.f;
   m_f2DColliderBodyRadius = CircleDesc.fRadius;
   CircleDesc.vScale = { 1.0f, 1.0f };
   CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius * 0.5f };
   CircleDesc.isBlock = false;
   CircleDesc.isTrigger = false;
   CircleDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER;
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
   CircleDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER_TRIGGER;
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
       TEXT("Com_Body2DTrigger"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &CircleDesc)))
       return E_FAIL;
   m_pBody2DTriggerCom = m_p2DColliderComs[1];
   Safe_AddRef(m_pBody2DTriggerCom);

   CCollider_AABB::COLLIDER_AABB_DESC BoxDesc = {};
   BoxDesc.pOwner = this;
   BoxDesc.vExtents = m_f2DAttackTriggerDesc[ATTACK_TYPE_NORMAL1][(_uint)F_DIRECTION::DOWN].vExtents;
   BoxDesc.vScale = { 1.0f, 1.0f };
   BoxDesc.vOffsetPosition = { 0.f,0.f };
   BoxDesc.isBlock = false;
   BoxDesc.isTrigger = true;
   BoxDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
       TEXT("Com_Attack2DTrigger"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[2]), &BoxDesc)))
       return E_FAIL;
   m_pAttack2DTriggerCom = m_p2DColliderComs[2];
   Safe_AddRef(m_pAttack2DTriggerCom);
   m_pAttack2DTriggerCom->Set_Active(false);

   /* Com_Gravity */
   CGravity::GRAVITY_DESC GravityDesc = {};
   GravityDesc.fGravity = 9.8f * 290.f;
   GravityDesc.vGravityDirection = { 0.0f, -1.0f, 0.0f };
   GravityDesc.pOwner = this;
   if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
       TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
       return E_FAIL;
   Safe_AddRef(m_pGravityCom);
   
   m_pGravityCom->Set_Active(false);
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

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}


void CPlayer::Update(_float _fTimeDelta)
{
  
    Key_Input(_fTimeDelta);
    COORDINATE eCoord  =  Get_CurCoord();

	if (m_bInvincible)
	{
        m_fInvincibleTImeAcc += _fTimeDelta;
        
		if (m_fInvincibleTIme <= m_fInvincibleTImeAcc)
		{
			m_bInvincible = false;
            m_fInvincibleTImeAcc = 0;
		}
	}
    __super::Update(_fTimeDelta); /* Part Object Update */
    if (m_pInteractableObject && false == dynamic_cast<CBase*>(m_pInteractableObject)->Is_Active())
        m_pInteractableObject = nullptr;
}

// 충돌 체크 후 container의 transform을 밀어냈어. 

void CPlayer::Late_Update(_float _fTimeDelta)
{            // cout << "Upforce :" << m_f2DUpForce << " Height : " << m_f2DHeight << endl;
    if (COORDINATE_2D == Get_CurCoord())
    {
        if (Is_PlatformerMode())
        {


        }
        else
        {
            _vector vUp = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_UP));
            m_pBody->Set_Position(vUp * m_f2DFloorDistance);
        }
    }
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */


    //cout << endl;

}

HRESULT CPlayer::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    if(m_pBody2DColliderCom->Is_Active())
        m_pBody2DColliderCom->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
    if(m_pBody2DTriggerCom->Is_Active())
        m_pBody2DTriggerCom->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
    if (m_pAttack2DTriggerCom->Is_Active())
        m_pAttack2DTriggerCom->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));


#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CPlayer::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    __super::OnContact_Enter(_My, _Other, _ContactPointDatas);
	m_pStateMachine->Get_CurrentState()->OnContact_Enter(_My, _Other, _ContactPointDatas);


}

void CPlayer::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    __super::OnContact_Stay(_My, _Other, _ContactPointDatas);
	m_pStateMachine->Get_CurrentState()->OnContact_Stay(_My, _Other, _ContactPointDatas);

}

void CPlayer::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    __super::OnContact_Exit(_My, _Other, _ContactPointDatas);
	m_pStateMachine->Get_CurrentState()->OnContact_Exit(_My, _Other, _ContactPointDatas);


}



void CPlayer::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Enter(_My, _Other);
	m_pStateMachine->Get_CurrentState()->OnTrigger_Enter(_My, _Other);
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_TRIGER:
        if (OBJECT_GROUP::MONSTER == _Other.pActorUserData->iObjectGroup)
            return;
        if (SHAPE_USE::SHAPE_BODY == (SHAPE_USE)_Other.pShapeUserData->iShapeUse)
            Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, true);
        break;
    }
}

void CPlayer::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Stay(_My, _Other);
	m_pStateMachine->Get_CurrentState()->OnTrigger_Stay(_My, _Other);
    if (PLAYER_SHAPE_USE::INTERACTION ==(PLAYER_SHAPE_USE)_My.pShapeUserData->iShapeUse)
    {
        OBJECT_GROUP eOtehrGroup = (OBJECT_GROUP)_Other.pActorUserData->pOwner->Get_ObjectGroupID();
        if (OBJECT_GROUP::INTERACTION_OBEJCT == eOtehrGroup)
        {
            IInteractable* pInteractable = dynamic_cast<IInteractable*> (_Other.pActorUserData->pOwner);
            if (Check_ReplaceInteractObject(pInteractable))
            {
				m_pInteractableObject = pInteractable;
            }

        }
    }


}

void CPlayer::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Exit(_My, _Other);
	m_pStateMachine->Get_CurrentState()->OnTrigger_Exit(_My, _Other);
    _uint iShapeUse = (_uint)_My.pShapeUserData->iShapeUse;
    switch (iShapeUse)
    {
    case (_uint)Client::SHAPE_USE::SHAPE_TRIGER:
        if (OBJECT_GROUP::MONSTER == _Other.pActorUserData->iObjectGroup)
            return;
        if (SHAPE_USE::SHAPE_BODY == (SHAPE_USE)_Other.pShapeUserData->iShapeUse)
            Event_SetSceneQueryFlag(_Other.pActorUserData->pOwner, _Other.pShapeUserData->iShapeIndex, false);
        break;
    case Client::CPlayer::PLAYER_SHAPE_USE::INTERACTION:
        if (m_pInteractableObject
            && dynamic_cast<CGameObject*>(m_pInteractableObject) == _Other.pActorUserData->pOwner
            && false == m_pInteractableObject->Is_Interacting())
            m_pInteractableObject = nullptr;
        break;
    }

}

void CPlayer::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
	m_pStateMachine->Get_CurrentState()->On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
    _uint iGroupID = _pOtherCollider->Get_CollisionGroupID();
    switch ((OBJECT_GROUP)iGroupID)
    {
    case Client::NONE:
        break;
    case Client::PLAYER:
        break;
    case Client::MONSTER:
        if (OBJECT_GROUP::PLAYER & _pMyCollider->Get_CollisionGroupID())
        {
			if (true == Is_PlatformerMode() && STATE::JUMP_DOWN == Get_CurrentStateID())
            {
                _float fAngle = atan2f(_pOtherObject->Get_FinalPosition().m128_f32[1] - Get_FinalPosition().m128_f32[1], _pOtherObject->Get_FinalPosition().m128_f32[0] - Get_FinalPosition().m128_f32[0]);
                fAngle = XMConvertToDegrees(fAngle);
                if (fAngle >= -170.f && fAngle <= -10.f)
                {
                    Attack(_pOtherObject);
                    //밟은 후 플레이어 띄우기
                    Event_KnockBack(this, XMVectorSet(0.f, 1.f, 0.f, 0.f), 2000.f);

                    //Effect
                    CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Hit_FX1"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), Get_ControllerTransform()->Get_WorldMatrix());
                }
            }  
        }
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

        if (true == Is_PlatformerMode())
        {
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
                    //Set_State(STATE::IDLE);
                    m_f2DUpForce = 0.f;

                }
                else if (-1.0f + fEpsilon >= fGdotC)
                {
                    m_pGravityCom->Set_GravityAcc(0.0f);
                    if (STATE::ROLL == Get_CurrentStateID())
                        break;
                    Set_State(STATE::JUMP_DOWN);
                    m_f2DUpForce = 0.f;
                }
            }
        }
       
        
    }
        break;
    case DOOR:
    {    
        break;
    }
    default:
        break;
    }

}

void CPlayer::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    __super::On_Collision2D_Stay(_pMyCollider, _pOtherCollider, _pOtherObject);
	m_pStateMachine->Get_CurrentState()->On_Collision2D_Stay(_pMyCollider, _pOtherCollider, _pOtherObject);
    OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherObject->Get_ObjectGroupID();
    if (_pMyCollider == m_pBody2DTriggerCom)
    {
        if (OBJECT_GROUP::INTERACTION_OBEJCT == eGroup)
        {
            IInteractable* pInteractable = dynamic_cast<IInteractable*> (_pOtherObject);
			if (nullptr != pInteractable && static_cast<CCollider_Circle*>( _pMyCollider)->Is_ContainsPoint(_pOtherCollider->Get_Position()))
            if (Check_ReplaceInteractObject(pInteractable))
            {
                m_pInteractableObject = pInteractable;
            }
        }
        
        if (OBJECT_GROUP::BLOCKER == eGroup)
        {
            if (true == _pMyCollider->Is_Trigger())
            {

            }
            else if (false == static_cast<CBlocker*>(_pOtherObject)->Is_Floor())
            {

            }
            else
            {
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
           
        }
    }
    else if (_pMyCollider == m_pAttack2DTriggerCom
        && OBJECT_GROUP::MONSTER == eGroup)
    {
        Attack(_pOtherObject);
    }
}

void CPlayer::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    __super::On_Collision2D_Exit(_pMyCollider, _pOtherCollider, _pOtherObject);
	m_pStateMachine->Get_CurrentState()->On_Collision2D_Exit(_pMyCollider, _pOtherCollider, _pOtherObject);
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
        {
            m_pGravityCom->Change_State(CGravity::STATE_FALLDOWN);
        }
    }
    break;    
    case DOOR:
    {
        break;
    }
    default:
        break;
    }

    if (OBJECT_GROUP::PLAYER_TRIGGER == _pMyCollider->Get_CollisionGroupID())
    {
        if (m_pInteractableObject
            && dynamic_cast<CGameObject*>(m_pInteractableObject) == _pOtherObject
            && false == m_pInteractableObject->Is_Interacting())
            m_pInteractableObject = nullptr;
    }
}

void CPlayer::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pStateMachine->Get_CurrentState()->On_AnimEnd(_eCoord, iAnimIdx);
}

void CPlayer::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    if (m_bInvincible)
    {
        cout << "no damg" << endl;
        return;
    }
    m_tStat.iHP -= _iDamg;
    cout << " Player HP" << m_tStat.iHP << endl;
	COORDINATE eCoord = Get_CurCoord();

    Uimgr->Set_PlayerOnHit(true);

    if (m_tStat.iHP <= 0)
    {
        m_tStat.iHP = 0;
        if (STATE::DIE != Get_CurrentStateID())
            Set_State(DIE);
        return;
    }

    KnockBack(_vForce);

    Start_Invinciblity();

    m_pGameInstance->Start_SFX(_wstring(L"A_sfx_jot_vocal_takedamage-") + to_wstring(rand() % 13), 25.f);
    m_pGameInstance->Start_SFX(_wstring(L"A_sfx_jot_lose_health_") + to_wstring(rand() % 3), 40.f);
}

HRESULT CPlayer::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;
    m_pInteractableObject = nullptr;
    if (Is_CarryingObject())
    {
        m_pCarryingObject->Change_Coordinate(_eCoordinate);
        if (COORDINATE_2D == _eCoordinate)
        {
            //m_pCarryingObject->Enter_Section(m_strSectionName);
            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, m_pCarryingObject);
        }
        else
        {
            CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(m_strSectionName, m_pCarryingObject);
        }

    }
    End_Attack();
    if (COORDINATE_2D == _eCoordinate)
    {
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
        break;
    case Client::CPlayer::PLAYER_MODE_SWORD:
		Equip_Part(PLAYER_PART_SWORD);
        break;
    case Client::CPlayer::PLAYER_MODE_SNEAK:
        UnEquip_Part(PLAYER_PART_SWORD);
        break;
    default:
        break;
    }
    m_pSword->Set_AttackEnable(false);

    return S_OK;
}

void CPlayer::On_Land()
{
    //cout << "Player Landed" << endl;
}


void CPlayer::Move_Attack_3D()
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        Stop_Move();
        Add_Impuls(Get_LookDirection() * m_fAttackForwardingForce);
    }
}

void CPlayer::StampSmash()
{
    if (COORDINATE_3D != Get_CurCoord())
        return;

	CBook* pBook = dynamic_cast<CBook*>(m_pInteractableObject);
    if (nullptr == pBook)
        return;

    _vector v2DPosition;
    if (FAILED(pBook->Convert_Position_3DTo2D(m_PartObjects[Get_CurrentStampType()]->Get_FinalPosition(),&v2DPosition)))
    {
        return;
    }
    if (PLAYER_PART::PLAYER_PART_BOMB_STMAP == m_eCurrentStamp)
    {
        m_pDetonator->Set_Bombable(m_pBombStmap->Place_Bomb(v2DPosition));
    }
    else if(PLAYER_PART::PLAYER_PART_STOP_STMAP == m_eCurrentStamp)
    {
	    _vector v2DDirection = Get_LookDirection();
        v2DDirection =XMVectorSetY(v2DDirection, XMVectorGetZ(v2DDirection));
	    v2DDirection = XMVector3Normalize(XMVectorSetZ( v2DDirection,0.f));
        m_pStopStmap->Place_PalmDecal(v2DPosition, v2DDirection);
    }

}

void CPlayer::Attack(CGameObject* _pVictim)
{
    if (m_AttckedObjects.find(_pVictim) != m_AttckedObjects.end())
        return;

    CCamera_Manager::CAMERA_TYPE eCameraType = (COORDINATE_2D == Get_CurCoord()) ? CCamera_Manager::TARGET_2D : CCamera_Manager::TARGET;
    CCamera_Manager::GetInstance()->Start_Shake_ByCount(eCameraType, 0.15f, 0.1f, 20, CCamera::SHAKE_XY);
    CCharacter* pCharacter = dynamic_cast<CCharacter*>(_pVictim);
    _vector vKnockBackForce = Get_LookDirection() * (COORDINATE_2D == Get_CurCoord() ? m_f2DKnockBackPower : m_f3DKnockBackPower);
    Event_Hit(this, pCharacter, m_tStat.iDamg, vKnockBackForce);

    m_AttckedObjects.insert(_pVictim);
}

void CPlayer::Detonate()
{
	if (nullptr == m_pDetonator)
		return;
	m_pDetonator->Detonate();
}

void CPlayer::ErasePalm()
{
	if (nullptr == m_pStopStmap)
		return;
	m_pStopStmap->Erase_PalmDecal();
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
        if (m_bPlatformerMode)
        {
            m_pGravityCom->Change_State(CGravity::STATE_FALLDOWN);
            m_f2DUpForce = m_f2DPlatformerJumpPower;
        }
        else
            m_f2DUpForce = m_f2DJumpPower;

    }
    else 
    {
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        if (pDynamicActor->Is_Kinematic())
        {

        }
        else
        {
            _vector vVelocity = pDynamicActor->Get_LinearVelocity();
            pDynamicActor->Set_LinearVelocity(vVelocity*0.5f);
            pDynamicActor->Add_Impulse(_float3{ 0, m_f3DJumpPower ,0 });
        }

    }
}



PLAYER_INPUT_RESULT CPlayer::Player_KeyInput()
{
	PLAYER_INPUT_RESULT tResult;
    fill(begin(tResult.bInputStates), end(tResult.bInputStates), false);
	if (m_bBlockInput)
		return tResult;
	if (STATE::DIE == Get_CurrentStateID())
    {
        if (KEY_DOWN(KEY::ENTER))
        {
			tResult.bInputStates[PLAYER_INPUT_REVIVE] = true;
        }
        return tResult;
    }


	_bool bCarrying = Is_CarryingObject();
    if (false == bCarrying && Is_SwordHandling())
    {
        //기본공격
        if (MOUSE_DOWN(MOUSE_KEY::LB))
        {
            tResult.bInputStates[PLAYER_INPUT_ATTACK] = true;
        }
        //칼 던지기
        else if (MOUSE_DOWN(MOUSE_KEY::RB))
        {
            tResult.bInputStates[PLAYER_INPUT_THROWSWORD] = true;
        }
        else if (Is_OnGround())
        {
            KEY_STATE eKeyState = m_pGameInstance->GetKeyState(KEY::Q);
            if (KEY_STATE::DOWN == eKeyState)
			    tResult.bInputStates[PLAYER_INPUT_SPINATTACK] = true;
            else if (KEY_STATE::PRESSING == eKeyState)
                tResult.bInputStates[PLAYER_INPUT_SPINCHARGING] = true;
            else if (KEY_STATE::UP == eKeyState)
                tResult.bInputStates[PLAYER_INPUT_SPINLAUNCH] = true;
        }
    }

    if (false == Has_InteractObject() && bCarrying)
    {
         //상호작용 오브젝트가 범위 안에 있으면 상호작용, 아니면 던지기
        if (KEY_DOWN(KEY::E))
            tResult.bInputStates[PLAYER_INPUT_THROWOBJECT] = true;
    }

    //점프
    if (KEY_DOWN(KEY::SPACE))
        tResult.bInputStates[PLAYER_INPUT_JUMP] = true;
    //구르기 & 잠입
    else if (false == bCarrying && KEY_PRESSING(KEY::LSHIFT))
    {
        if (Is_SneakMode())
            tResult.bInputStates[PLAYER_INPUT_SNEAK] = true;
        else
            tResult.bInputStates[PLAYER_INPUT_ROLL] = true;
    }

	if (Is_ZetPackMode())
	{
		if (KEY_PRESSING(KEY::SPACE))
			tResult.bInputStates[PLAYER_INPUT_ZETPROPEL] = true;
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

PLAYER_INPUT_RESULT CPlayer::Player_KeyInput_Stamp()
{
    PLAYER_INPUT_RESULT tResult;
    fill(begin(tResult.bInputStates), end(tResult.bInputStates), false);
    if (m_bBlockInput)
        return tResult;
    _bool bIsStamping = CPlayer::STAMP == m_pStateMachine->Get_CurrentState()->Get_StateID();
	if (false == bIsStamping) return tResult;

    if (KEY_DOWN(KEY::LSHIFT))
        tResult.bInputStates[PLAYER_INPUT_ROLL] = true;
    if (MOUSE_DOWN(MOUSE_KEY::LB)
        || MOUSE_DOWN(MOUSE_KEY::RB)
        ||KEY_DOWN(KEY::E))
        tResult.bInputStates[PLAYER_INPUT_CANCEL_STAMP] = true;

    if (KEY_DOWN(KEY::Q))
        tResult.bInputStates[PLAYER_INPUT_KEEP_STAMP] = true;
    else if (KEY_PRESSING(KEY::Q))
        tResult.bInputStates[PLAYER_INPUT_KEEP_STAMP] = true;


    //이동
    _vector vRight = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
    _vector vUp = XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_UP));
    if (KEY_PRESSING(KEY::W))
        tResult.vDir += _vector{ 0.f, 0.f, 1.f,0.f };
    if (KEY_PRESSING(KEY::A))
        tResult.vDir += _vector{ -1.f, 0.f, 0.f,0.f };
    if (KEY_PRESSING(KEY::S))
        tResult.vDir += _vector{ 0.f, 0.f, -1.f,0.f };
    if (KEY_PRESSING(KEY::D))
        tResult.vDir += _vector{ 1.f, 0.f, 0.f,0.f };
    tResult.vDir = XMVector3Normalize(tResult.vDir);
    tResult.vMoveDir = tResult.vDir;
    tResult.bInputStates[PLAYER_INPUT_MOVE] = false == XMVector3Equal(tResult.vMoveDir, XMVectorZero());

    return tResult;
}

PLAYER_INPUT_RESULT CPlayer::Player_KeyInput_ControlBook()
{
    PLAYER_INPUT_RESULT tResult;
    _bool bIsTurningBook = CPlayer::TURN_BOOK == m_pStateMachine->Get_CurrentState()->Get_StateID();
    if (m_bBlockInput)
        return tResult;
    if (false == bIsTurningBook)
        return tResult;
    if (KEY_PRESSING(KEY::A))
    {
        tResult.bInputStates[PLAYER_INPUT_TURNBOOK_LEFT] = true;
    }
    else if (KEY_PRESSING(KEY::D))
    {
        tResult.bInputStates[PLAYER_INPUT_TURNBOOK_RIGHT] = true;
    }
    else if (KEY_PRESSING(KEY::Z))
    {
        tResult.bInputStates[PLAYER_INPUT_TILTBOOK_LEFT] = true;
    }
    else if (KEY_PRESSING(KEY::X))
    {
        tResult.bInputStates[PLAYER_INPUT_TILTBOOK_RIGHT] = true;
    }
    else
    {
        if (MOUSE_DOWN(MOUSE_KEY::LB)
            || MOUSE_DOWN(MOUSE_KEY::RB)
            || KEY_DOWN(KEY::SPACE)
            || KEY_DOWN(KEY::LSHIFT)
            || KEY_DOWN(KEY::Q))
        {
            tResult.bInputStates[PLAYER_INPUT_TURNBOOK_END] = true;
        }
    }


    return tResult;
    
    return tResult;
}

void CPlayer::Revive()
{
	m_tStat.iHP = m_tStat.iMaxHP;
	Set_State(IDLE);
}

void CPlayer::ReFuel()
{
    m_pZetPack->ReFuel();
}

void CPlayer::ZetPropel(_float _fTimeDelta)
{
	if (Is_ZetPackMode() )
	{
		m_pZetPack->Propel(_fTimeDelta);
	}
}


//아무런 상호작용 중이 아닐 때에는 그냥 가장 가까운 애로 교체.
//포탈, 끌고다니기 등 상호작용 중일 때는 교체 불가.
_bool CPlayer::Check_ReplaceInteractObject(IInteractable* _pObj)
{
    if(nullptr == _pObj)
		return false;
    if (false == _pObj->Is_Interactable(this))
        return false;
    if (m_pInteractableObject == _pObj)
        return false;
    if (m_pCarryingObject ==_pObj )
        return false;
    if(nullptr == m_pInteractableObject)
		return true;
	COORDINATE eCoord = Get_CurCoord();
    if (m_pInteractableObject->Get_Distance(eCoord, this) > _pObj->Get_Distance(eCoord, this))
        return true;
    return false;
}


void CPlayer::Start_Portal(CPortal* _pPortal)
{
    Set_State(START_PORTAL);
}

void CPlayer::JumpTo_Portal(CPortal* _pPortal)
{
	Set_State(JUMPTO_PORTAL);
}

void CPlayer::Set_PlayingAnim(_bool _bPlaying)
{
    m_pBody->Set_PlayingAnim(_bPlaying);
}

void CPlayer::Start_Invinciblity()
{
	m_bInvincible = true;
	m_fInvincibleTImeAcc = 0;
    //m_pActorCom->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, false);
	//m_pBody2DColliderCom->Set_Active(false);
}

INTERACT_RESULT CPlayer::Try_Interact(_float _fTimeDelta)
{
    //이미 인터렉터블 오브젝트가 있다? 
    // -> 버튼만 눌러주면 바로 상호작용 OK
    if (false == Has_InteractObject())
    {
        return INTERACT_RESULT::FAIL;
    }
    
    IInteractable::INTERACT_TYPE eInteractType = m_pInteractableObject->Get_InteractType();
    KEY eInteractKey = m_pInteractableObject->Get_InteractKey();

    if (KEY_CHECK(eInteractKey, KEY_STATE::NONE))
    {
        m_pInteractableObject->Set_Interacting(false);
        return INTERACT_RESULT::NO_INPUT;
    }

    switch (eInteractType)
    {
    case Client::IInteractable::NORMAL:
        if (KEY_DOWN(eInteractKey))
        {
			m_pInteractableObject->Start_Interact(this);
            m_pInteractableObject->Interact(this);
			m_pInteractableObject->End_Interact(this);
            return INTERACT_RESULT::SUCCESS;
        }
        break;
    case Client::IInteractable::CHARGE:
        if (KEY_DOWN(eInteractKey))
        {
            m_pInteractableObject->Start_Interact(this);
            return INTERACT_RESULT::INTERACT_START;
        }
		else if (KEY_PRESSING(eInteractKey))
		{
			m_pInteractableObject->Pressing(this, _fTimeDelta);
            if (m_pInteractableObject->Is_ChargeCompleted())
            {
                m_pInteractableObject->Interact(this);
				m_pInteractableObject->End_Interact(this);
                return INTERACT_RESULT::SUCCESS;
            }
            return INTERACT_RESULT::CHARGING;
		}
		else if (KEY_UP(eInteractKey))
		{
			m_pInteractableObject->End_Interact(this);
            return INTERACT_RESULT::CHARGE_CANCEL;
		}
        break;
    case Client::IInteractable::HOLDING:
        if (KEY_DOWN(eInteractKey))
        {
            m_pInteractableObject->Start_Interact(this);
            return INTERACT_RESULT::INTERACT_START;
        }
        else if (KEY_PRESSING(eInteractKey))
        {
            m_pInteractableObject->Pressing(this, _fTimeDelta);
            m_pInteractableObject->Interact(this);
            return INTERACT_RESULT::SUCCESS;
        }
        else if (KEY_UP(eInteractKey))
        {
            m_pInteractableObject->End_Interact(this);
            return INTERACT_RESULT::INTERACT_END;
        }
        break;
    default:
        break;
    }
	return INTERACT_RESULT::FAIL;
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

_bool CPlayer::Is_DetonationMode()
{
    return m_pDetonator->Is_DetonationMode();
}

_bool CPlayer::Is_ZetPackMode()
{
    return m_PartObjects[PLAYER_PART_ZETPACK]->Is_Active(); 
}

_bool CPlayer::Is_PlayingAnim()
{
    return m_pBody->Is_PlayingAnim();
}

_float CPlayer::Get_UpForce()
{
    COORDINATE eCoord = Get_CurCoord();

    if (COORDINATE_2D == eCoord)
    {
        if (Is_PlatformerMode())
            return -m_pGravityCom->Get_GravityAcc();
        else
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

_float CPlayer::Get_AnimationTIme()
{
    return m_pBody->Get_Model(Get_CurCoord())->Get_AnimationTime();
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

_vector CPlayer::Get_2DELookDirection()
{
    return _vector();
}

_vector CPlayer::Get_2DFLookDirection()
{
    return _vector();
}

_vector CPlayer::Get_BodyPosition()
{
    return m_pBody->Get_FinalPosition(); 
}

CPlayer::STATE CPlayer::Get_CurrentStateID()
{
	return m_pStateMachine->Get_CurrentState()->Get_StateID();
}



const _float4x4* CPlayer::Get_BodyWorldMatrix_Ptr() const
{
    return m_pBody->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
}

const _float4x4* CPlayer::Get_BodyWorldMatrix_Ptr(COORDINATE eCoord) const
{
    return m_pBody->Get_ControllerTransform()->Get_Transform(eCoord)->Get_WorldMatrix_Ptr();
}

_vector CPlayer::Get_RootBonePosition()
{
    _float4x4 matRootBone =*static_cast<C3DModel*>( m_pBody->Get_Model(COORDINATE_3D))->Get_BoneMatrix("j_pelvis");
    _vector vPos = { matRootBone._41, matRootBone._42, matRootBone._43 };
    _vector vDirection = XMVector3Normalize(vPos);
	_float fDistance = XMVectorGetX( XMVector3Length(vPos ));
    _vector vLook = Get_LookDirection(COORDINATE_3D);
	vDirection.m128_f32[0] = vLook.m128_f32[0];
	vDirection.m128_f32[2] = vLook.m128_f32[2];
	vPos = vDirection * fDistance;
    vPos += Get_FinalPosition();
    return vPos;

}




CActor_Dynamic* CPlayer::Get_ActorDynamic()
{
    return static_cast<CActor_Dynamic*>(m_pActorCom);
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
    //cout << "eState: " << _eState << endl;
    //_uint iAnimIdx;
    switch (_eState)
    {
    case Client::CPlayer::IDLE:
        //cout << "IDLE" << endl;
        m_pStateMachine->Transition_To(new CPlayerState_Idle(this));
        if (Is_ZetPackMode())
            m_pZetPack->Switch_State(CZetPack::STATE_IDLE);
        break;
    case Client::CPlayer::RUN:
        //cout << "Run" << endl;
        m_pStateMachine->Transition_To(new CPlayerState_Run(this));
        break;
    case Client::CPlayer::JUMP_UP:
        //cout << "JUMPUP" << endl;
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
    case Client::CPlayer::LAYDOWNOBJECT:
		m_pStateMachine->Transition_To(new CPlayerState_LaydownObject(this));
		break;
    case Client::CPlayer::DIE:
		m_pStateMachine->Transition_To(new CPlayerState_Die(this));
		break;
    case Client::CPlayer::START_PORTAL:
		m_pStateMachine->Transition_To(new CPlayerState_StartPortal(this));
		break;
    case Client::CPlayer::JUMPTO_PORTAL:
        m_pStateMachine->Transition_To(new CPlayerState_JumpToPortal(this));
		break;
    case Client::CPlayer::EXIT_PORTAL:
        m_pStateMachine->Transition_To(new CPlayerState_ExitPortal(this));
		break;
    case Client::CPlayer::TURN_BOOK:
        m_pStateMachine->Transition_To(new CPlayerState_TurnBook(this));
		break;
    case Client::CPlayer::EVICT:
        m_pStateMachine->Transition_To(new CPlayerState_Evict(this));
        break;
    case Client::CPlayer::LUNCHBOX:
        m_pStateMachine->Transition_To(new CPlayerState_LunchBox(this));
        break;
    case Client::CPlayer::ELECTRIC:
        m_pStateMachine->Transition_To(new CPlayerState_Electric(this));
        break;
    case Client::CPlayer::DRAG:
        m_pStateMachine->Transition_To(new CPlayerState_Drag(this));
        break;
    case Client::CPlayer::STAMP:
        m_pStateMachine->Transition_To(new CPlayerState_Stamp(this));
        break;
    case Client::CPlayer::BOMBER:
        m_pStateMachine->Transition_To(new CPlayerState_Bomber(this));
        break;
    case Client::CPlayer::ERASE_PALM:
        m_pStateMachine->Transition_To(new CPlayerState_ErasePalmDecal(this));
        break;
    case Client::CPlayer::GET_ITEM:
        m_pStateMachine->Transition_To(new CPlayerState_GetItem(this));
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
    _vector vTmpDir = EDir_To_Vector(m_e2DDirection_E);
    _vector  vDir = vTmpDir * m_f2DInteractOffset;
    m_pBody2DTriggerCom->Set_Offset({ XMVectorGetX(vDir),XMVectorGetY(vDir) +  m_f2DCenterYOffset});

    
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
void CPlayer::Set_PlatformerMode(_bool _bPlatformerMode)
{
    m_bPlatformerMode = _bPlatformerMode;
    if (true == _bPlatformerMode)
    {
        Event_SetActive(m_pGravityCom, true);
        m_pGravityCom->Set_Active(false);
        m_pGravityCom->Change_State(CGravity::STATE_FALLDOWN);
        CCollider_Circle* pCollider = static_cast<CCollider_Circle*>(m_pBody2DColliderCom);

        pCollider->Set_Radius(m_f2DColliderBodyRadius * 2.f);
        pCollider->Set_Offset(_float2(0.0f, m_f2DColliderBodyRadius * 2.0f * 1.0f));
    }
    else
    {
        Event_SetActive(m_pGravityCom, false);
        m_pGravityCom->Set_Active(true);
        m_pGravityCom->Change_State(CGravity::STATE_FLOOR);
        CCollider_Circle* pCollider = static_cast<CCollider_Circle*>(m_pBody2DColliderCom);
        pCollider->Set_Radius(m_f2DColliderBodyRadius);
        pCollider->Set_Offset(_float2(0.0f, m_f2DColliderBodyRadius * 0.5f));
    }
}

void CPlayer::Set_ScrollingMode(_bool _bScrollingMode)
{
    m_bScrollingMode = _bScrollingMode;
}

void CPlayer::Set_Upforce(_float _fForce)
{
    if (COORDINATE_2D == Get_CurCoord())
    {
        if (Is_PlatformerMode())
            m_pGravityCom->Set_GravityAcc(_fForce);
        else
            m_f2DUpForce = _fForce;
    }
    else
    {
        CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        _vector vVelocity = pDynamicActor->Get_LinearVelocity();

        pDynamicActor->Set_LinearVelocity(XMVectorSetY(vVelocity, _fForce));
    }

}
HRESULT CPlayer::CarryObject(CCarriableObject* _pCarryingObject)
{
    if (Is_CarryingObject())
        return E_FAIL;
    Set_CarryingObject(_pCarryingObject);

    Set_State(PICKUPOBJECT);
    return S_OK;
}
HRESULT CPlayer::LayDownObject()
{
    if (false == Is_CarryingObject())
        return E_FAIL;
    Set_State(LAYDOWNOBJECT);
    Set_CarryingObject(nullptr);
    return S_OK;
}
void CPlayer::Set_CarryingObject(CCarriableObject* _pCarryingObject)
{
    //손 비우기
    if (nullptr == _pCarryingObject)
    {
        if (Is_CarryingObject())
        {
            Safe_Release(m_pCarryingObject);
            m_pCarryingObject = nullptr;
        }

    }
    //손에 물건 들리기
    else
    {
        if (false == Is_CarryingObject())
        {
            m_pCarryingObject = _pCarryingObject;
            Safe_AddRef(m_pCarryingObject);
        }

    }
    return;
}

void CPlayer::Set_GravityCompOn(_bool _bOn)
{
	m_pGravityCom->Set_Active(_bOn);
    m_pGravityCom->Change_State(_bOn ?  CGravity::STATE_FALLDOWN : CGravity::STATE_FLOOR);
}

void CPlayer::Start_Attack(ATTACK_TYPE _eAttackType)
{

	m_eCurAttackType = _eAttackType;
	if (COORDINATE_2D == Get_CurCoord())
	{
        m_pAttack2DTriggerCom->Set_Active(true);
        F_DIRECTION eFDir = EDir_To_FDir(m_e2DDirection_E);
        const ATTACK_TRIGGER_DESC& tDsc = m_f2DAttackTriggerDesc[m_eCurAttackType][(_uint)eFDir];
        CCollider_AABB* pAttackTrigger =static_cast<CCollider_AABB*>(m_pAttack2DTriggerCom);
        pAttackTrigger->Set_Extents(tDsc.vExtents);
		pAttackTrigger->Set_Offset(tDsc.vOffset);

	}
	else
	{
        m_pActorCom->Set_ShapeEnable(PLAYER_SHAPE_USE::BODYGUARD, true);
		m_pSword->Set_AttackEnable(true, _eAttackType);
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
        m_pActorCom->Set_ShapeEnable(PLAYER_SHAPE_USE::BODYGUARD, false);
        m_pSword->Set_AttackEnable(false);
    }

}

void CPlayer::Equip_Part(PLAYER_PART _ePartId)
{
	if (PLAYER_PART_BODY == _ePartId)
		return;
    _bool bMainEquip = false;
    for (_uint i = 0; i < PLAYER_MAIN_EQUIP::PLAYER_MAIN_EQUIP_LAST; i++)
    {
        if (i == _ePartId)
        {
            for (_int i = PLAYER_MAIN_EQUIP_SWORD; i < PLAYER_MAIN_EQUIP_LAST; ++i)
                Set_PartActive((_uint)i, false);
            break;
        }
    }
    if(COORDINATE_3D == Get_CurCoord())
	    Set_PartActive(_ePartId, true);
}

void CPlayer::UnEquip_Part(PLAYER_PART _ePartId)
{
    if (PLAYER_PART_BODY == _ePartId)
        return;
    if(COORDINATE_3D == Get_CurCoord() && Is_SwordHandling())
		Set_PartActive(PLAYER_PART_SWORD, true);
	Set_PartActive(_ePartId, false);
}

void CPlayer::UnEquip_All()
{
    for (_int i = PLAYER_PART_SWORD; i < PLAYER_PART_LAST; ++i)
        Set_PartActive((_uint)i, false);
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
    
	CCarriableObject* pObj = static_cast<CCarriableObject*>(m_pCarryingObject);
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

void CPlayer::SpinAttack()
{
    End_Attack();
    Start_Attack(CPlayer::ATTACK_TYPE_SPIN);
}

void CPlayer::Add_Upforce(_float _fForce)
{
    if (COORDINATE_2D == Get_CurCoord())
    {
		m_f2DUpForce += _fForce;

	}
	else
	{
		CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
        if (false == pDynamicActor->Is_Kinematic())
        {
            _vector vVelocity = pDynamicActor->Get_LinearVelocity();
            pDynamicActor->Add_Force(_float3{ 0, _fForce ,0 });
        }

    }

}

void CPlayer::Key_Input(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::F1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);
        _vector vPos = Get_FinalPosition((COORDINATE)iCurCoord);

		XMStoreFloat3(&vNewPos, vPos);


        if (iCurCoord == COORDINATE_2D)
        {
            CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_OBJECT);
        }
        else
        {
            CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
        }

        Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);
        //Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
    }
    if (KEY_DOWN(KEY::F3))
    {

        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        _float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);
        if (iCurCoord == COORDINATE_2D)
            CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(L"Chapter2_SKSP_05",this, SECTION_2D_PLAYMAP_OBJECT);
        else
            CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(L"Chapter2_SKSP_05",this);

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
            Set_PlatformerMode(!m_bPlatformerMode);
        else
        {
            Set_PlatformerMode(false);
            Set_Kinematic(false == m_pActorCom->Is_Kinematic());
        }
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
    if (KEY_DOWN(KEY::Z))
    {
        //COORDINATE eCoord =Get_CurCoord();
        //if (COORDINATE_3D == eCoord)
        //{
            //근처 포탈
            //static_cast<CActor_Dynamic*>(Get_ActorCom())->Start_ParabolicTo(_vector{ -46.9548531, 0.358914316, -11.1276035 }, XMConvertToRadians(45.f), 9.81f * 3.0f);
            //포탈 4 0x00000252f201def0 {52.1207695, 2.48441672, 13.1522322, 1.00000000}
            //도미노 { 6.99342966, 5.58722591, 21.8827782 }
            //static_cast<CActor_Dynamic*>(Get_ActorCom())->Start_ParabolicTo(_vector{ 6.99342966, 5.58722591, 21.8827782 }, XMConvertToRadians(45.f), 9.81f * 3.0f);
            //주사위 2 (48.73f, 2.61f, -5.02f);
            //static_cast<CActor_Dynamic*>(Get_ActorCom())->Start_ParabolicTo(_vector{ 48.73f, 2.61f, -5.02f }, XMConvertToRadians(45.f), 9.81f * 3.0f);
            //static_cast<CActor_Dynamic*>(Get_ActorCom())->Start_ParabolicTo(_vector{ 15.f, 10.f, 21.8827782 }, XMConvertToRadians(45.f), 9.81f * 3.0f);

       // }
        //static_cast<CModelObject*>(m_PartObjects[PART_BODY])->To_NextAnimation();
		Set_BlockPlayerInput(!Is_PlayerInputBlocked());
    }
    if (m_pActorCom->Is_Kinematic())
    {
        if (KEY_PRESSING(KEY::SPACE))
        {
            Move(_vector{0.f,5.f,0.f}, _fTimeDelta);
        }
    }
    if (KEY_DOWN(KEY::NUM1))
    {
        if (false == m_pDetonator->Is_DetonationMode())
        {
            Set_CurrentStampType(PLAYER_PART_STOP_STMAP);
            if (STATE::STAMP == Get_CurrentStateID())
                Equip_Part(PLAYER_PART_STOP_STMAP);
        }
    }
    else if (KEY_DOWN(KEY::NUM2))
    {
		Set_CurrentStampType(PLAYER_PART_BOMB_STMAP);
        if (STATE::STAMP == Get_CurrentStateID())
            Equip_Part(PLAYER_PART_BOMB_STMAP);
    }
    //if (KEY_DOWN(KEY::H))
    //{
    //    m_pActorCom->Set_GlobalPose(_float3(-31.f, 6.56f, 22.5f));
    //    //m_pActorCom->Set_GlobalPose(_float3(23.5f, 20.56f, 22.5f));
    //    //m_pActorCom->Set_GlobalPose(_float3(42.f, 8.6f, 20.f));
    //    //m_pActorCom->Set_GlobalPose(_float3(40.f, 0.35f, -7.f));
    //}
    //if (KEY_DOWN(KEY::J))
    //{
    //    Set_State(CPlayer::EVICT);
    //}
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

    Safe_Release(m_pBody2DColliderCom);
    Safe_Release(m_pBody2DTriggerCom);
    Safe_Release(m_pAttack2DTriggerCom);
	Safe_Release(m_pStateMachine);
    Safe_Release(m_pGravityCom);

	Safe_Release(m_pSword);
	Safe_Release(m_pBody);
	Safe_Release(m_pGlove);
	Safe_Release(m_pBombStmap);
	Safe_Release(m_pStopStmap);
	Safe_Release(m_pDetonator);
	Safe_Release(m_pZetPack);

    Safe_Release(m_pCarryingObject);
    
    __super::Free();
}
