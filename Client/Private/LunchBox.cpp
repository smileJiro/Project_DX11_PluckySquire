#include "stdafx.h"
#include "LunchBox.h"
#include "Player.h"
#include "AnimEventGenerator.h"
#include "AnimEventReceiver.h"
#include "GameInstance.h"
#include "RabbitLunch.h"

CLunchBox::CLunchBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CLunchBox::CLunchBox(CLunchBox& _Prototype)
	:CModelObject(_Prototype)
{
}


HRESULT CLunchBox::Initialize(void* _pArg)
{
    m_fInteractChargeTime = 1.f;
    m_eInteractType = INTERACT_TYPE::CHARGE;

    CModelObject::MODELOBJECT_DESC* pDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

    m_iCurLevelID = pDesc->iCurLevelID;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->iModelPrototypeLevelID_2D = pDesc->iCurLevelID;
    pDesc->iModelPrototypeLevelID_3D = pDesc->iCurLevelID;

    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->iPriorityID_3D = PR3D_GEOMETRY;
    pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

    pDesc->isCoordChangeEnable = false;
    pDesc->iModelPrototypeLevelID_3D = pDesc->iCurLevelID;
    pDesc->strModelPrototypeTag_3D = TEXT("Lunch_Box_Rig_GT");
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->iPriorityID_3D = PR3D_GEOMETRY;

    pDesc->tTransform3DDesc.vInitialPosition = _float3(-34.5f, 3.40f, -7.8f);
    pDesc->tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    pDesc->tTransform3DDesc.vInitialRotation = _float3(XMConvertToRadians(90.f), XMConvertToRadians(180.f), 0.f);


    CActor::ACTOR_DESC ActorDesc;
    ActorDesc.pOwner = this;
    ActorDesc.FreezeRotation_XYZ[0] = true;
    ActorDesc.FreezeRotation_XYZ[1] = true;
    ActorDesc.FreezeRotation_XYZ[2] = true;
    ActorDesc.FreezePosition_XYZ[0] = true;
    ActorDesc.FreezePosition_XYZ[1] = true;
    ActorDesc.FreezePosition_XYZ[2] = true;

    //블락용
    SHAPE_BOX_DESC ShapeDesc = {};
    ShapeDesc.vHalfExtents = { 4.75f,1.6f ,3.0f };
    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &ShapeDesc;
    ShapeData.eShapeType = SHAPE_TYPE::BOX;
    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
    ShapeData.isTrigger = false;
    ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    ShapeData.FilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
    ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc.vHalfExtents.y, 0.f));
    ActorDesc.ShapeDatas.push_back(ShapeData);

    //상호작용용
    SHAPE_SPHERE_DESC ShapeDesc2 = {};
	ShapeDesc2.fRadius = 2.f;
    SHAPE_DATA ShapeData2;
    ShapeData2.pShapeDesc = &ShapeDesc2;
    ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
    ShapeData2.isTrigger = true;
    ShapeData2.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    ShapeData2.FilterData.MyGroup =  OBJECT_GROUP::MAPOBJECT;
    ShapeData2.FilterData.OtherGroupMask =OBJECT_GROUP::PLAYER;
    XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.f, ShapeDesc.vHalfExtents.y*2, ShapeDesc.vHalfExtents.z));
    ActorDesc.ShapeDatas.push_back(ShapeData2);

    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER;

    pDesc->pActorDesc = &ActorDesc;
    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;


    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

	//Get_ControllerTransform()->Rotation(XMConvertToRadians(180.f), _vector{ 0.f,1.f,0.f,0.f });
    Set_PlayingAnim(false);
    
    m_vInteractionPointOffset = { 0.f, -ShapeDesc.vHalfExtents.z , -ShapeDesc.vHalfExtents.y *2.f - 0.7f};



    Bind_AnimEventFunc("LunchBoxOpen", bind(&CLunchBox::LunchBoxOpen, this));

    CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
    tAnimEventDesc.pReceiver = this;
    tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(this)->Get_Model(COORDINATE_3D);
    CAnimEventGenerator* pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_LunchBoxAnimEvent"), &tAnimEventDesc));
    Add_Component(TEXT("AnimEventGenrator"), pAnimEventGenerator);


	return S_OK;
}

void CLunchBox::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CLunchBox::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CLunchBox::Render()
{
    return __super::Render();
}

void CLunchBox::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_TRIGER:
        if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup
            && (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            m_bPlayerInRange = true;
        }
        break;
    }
}

void CLunchBox::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CLunchBox::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_TRIGER:
        if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup
            && (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            m_bPlayerInRange = false;
        }
        break;
    }
}

CLunchBox* CLunchBox::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLunchBox* pInstance = new CLunchBox(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : LuncBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLunchBox::Clone(void* _pArg)
{
    CLunchBox* pInstance = new CLunchBox(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : LuncBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLunchBox::Free()
{
    __super::Free();
}

void CLunchBox::Interact(CPlayer* _pUser)
{
    int a = 0;
    m_bOpened = true;
}

_bool CLunchBox::Is_Interactable(CPlayer* _pUser)
{
    return m_bPlayerInRange && false == m_bOpened && false == _pUser->Is_CarryingObject();
}

_float CLunchBox::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}

void CLunchBox::On_Touched(CPlayer* _pPlayer)
{
    _pPlayer->Set_State(CPlayer::LUNCHBOX);
}

void CLunchBox::LunchBoxOpen()
{
    _float3 vBoxPos;
    XMStoreFloat3(&vBoxPos, Get_FinalPosition());
	vBoxPos.z += m_vInteractionPointOffset.z - 1.2f;

	CRabbitLunch::RABBITLUNCH_DESC tRabbitLunchDesc{};
    //  도시락 위치 : pDesc->tTransform3DDesc.vInitialPosition = _float3(-34.5f, 3.40f, -7.8f);
	tRabbitLunchDesc.eLunchType = CRabbitLunch::LUNCH_TYPE::CARROT_1;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition = vBoxPos ;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.x += m_pGameInstance->Compute_Random(-2.5f, 2.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.y += m_pGameInstance->Compute_Random(-1.5f, 1.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.y += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.x += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.z += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.iCurLevelID = m_iCurLevelID;
    _vector vForce{ 0.f,0.f,-10.0f };
    CActorObject* pDynamicActor;
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_RabbitLunch"), m_iCurLevelID, TEXT("Layer_LunchBox"), (CGameObject**)&pDynamicActor ,& tRabbitLunchDesc)))
        return ;
    pDynamicActor->Add_Impuls(XMVectorSetZ( vForce, m_pGameInstance->Compute_Random(-2.5f, 2.5f)));

	tRabbitLunchDesc.eLunchType = CRabbitLunch::LUNCH_TYPE::CARROT_2;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition = vBoxPos;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.x += m_pGameInstance->Compute_Random(-2.5f,2.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.y += m_pGameInstance->Compute_Random(-1.5f, 1.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.y += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.x += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.z += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_RabbitLunch"), m_iCurLevelID, TEXT("Layer_LunchBox"), (CGameObject**)&pDynamicActor ,&tRabbitLunchDesc)))
        return ;
    pDynamicActor->Add_Impuls(XMVectorSetZ(vForce, m_pGameInstance->Compute_Random(-2.5f, 2.5f)));

	tRabbitLunchDesc.eLunchType = CRabbitLunch::LUNCH_TYPE::CARROT_3;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition = vBoxPos;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.x += m_pGameInstance->Compute_Random(-2.5f, 2.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.y += m_pGameInstance->Compute_Random(-1.5f, 1.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.y += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.x += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.z += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_RabbitLunch"), m_iCurLevelID, TEXT("Layer_LunchBox"), (CGameObject**)&pDynamicActor, &tRabbitLunchDesc)))
        return ;
    pDynamicActor->Add_Impuls(XMVectorSetZ(vForce, m_pGameInstance->Compute_Random(-2.5f, 2.5f)));

    tRabbitLunchDesc.eLunchType = CRabbitLunch::LUNCH_TYPE::GRAPE_1;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition = vBoxPos;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.x += m_pGameInstance->Compute_Random(-2.5f, 2.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.y += m_pGameInstance->Compute_Random(-1.5f, 1.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.y += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.x += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.z += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_RabbitLunch"), m_iCurLevelID, TEXT("Layer_LunchBox"), (CGameObject**)&pDynamicActor, &tRabbitLunchDesc)))
        return;
    pDynamicActor->Add_Impuls(XMVectorSetZ(vForce, m_pGameInstance->Compute_Random(-2.5f, 2.5f)));

    tRabbitLunchDesc.eLunchType = CRabbitLunch::LUNCH_TYPE::GRAPE_2;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition = vBoxPos;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.x += -2.5f;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.x += m_pGameInstance->Compute_Random(-2.5f, 2.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.y += m_pGameInstance->Compute_Random(-1.5f, 1.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.y += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.x += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.z += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_RabbitLunch"), m_iCurLevelID, TEXT("Layer_LunchBox"), (CGameObject**)&pDynamicActor, &tRabbitLunchDesc)))
        return;
    pDynamicActor->Add_Impuls(XMVectorSetZ(vForce, m_pGameInstance->Compute_Random(-2.5f, 2.5f)));

    tRabbitLunchDesc.eLunchType = CRabbitLunch::LUNCH_TYPE::GRAPE_3;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition = vBoxPos;
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.x += m_pGameInstance->Compute_Random(-2.5f, 2.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialPosition.y += m_pGameInstance->Compute_Random(-1.5f, 1.5f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.y += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.x += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    tRabbitLunchDesc.tTransform3DDesc.vInitialRotation.z += m_pGameInstance->Compute_Random(-3.14f, 3.14f);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_RabbitLunch"), m_iCurLevelID, TEXT("Layer_LunchBox"), (CGameObject**)&pDynamicActor, &tRabbitLunchDesc)))
        return;
    pDynamicActor->Add_Impuls(XMVectorSetZ(vForce, m_pGameInstance->Compute_Random(-2.5f, 2.5f)));

    
}
