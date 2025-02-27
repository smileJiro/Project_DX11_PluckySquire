#include "stdafx.h"
#include "PalmDecal.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"

CPalmDecal::CPalmDecal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CPalmDecal::CPalmDecal(const CPalmDecal& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CPalmDecal::Initialize(void* _pArg)
{
    CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(0.8f, 0.8f, 0.8f);

    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("stamp_icon_stop_01_Sprite");
	pBodyDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;


    CActor::ACTOR_DESC ActorDesc;
    ActorDesc.pOwner = this;
    ActorDesc.FreezeRotation_XYZ[0] = true;
    ActorDesc.FreezeRotation_XYZ[1] = true;
    ActorDesc.FreezeRotation_XYZ[2] = true;
    ActorDesc.FreezePosition_XYZ[0] = false;
    ActorDesc.FreezePosition_XYZ[1] = false;
    ActorDesc.FreezePosition_XYZ[2] = false;

    SHAPE_SPHERE_DESC tShapeDesc = {};
	tShapeDesc.fRadius = 0.2f;
    SHAPE_DATA tShapeData = {};
    ActorDesc.ShapeDatas.resize(1);
    tShapeData.pShapeDesc = &tShapeDesc;            
    tShapeData.eShapeType = SHAPE_TYPE::SPHERE;    
    tShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
    tShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    tShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    tShapeData.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
    tShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_TRIGGER;
    XMStoreFloat4x4(&tShapeData.LocalOffsetMatrix, XMMatrixIdentity());

    ActorDesc.ShapeDatas[tShapeData.iShapeUse] = tShapeData;
    ActorDesc.tFilterData.MyGroup = m_iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_TRIGGER;


    pBodyDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    pBodyDesc->pActorDesc = &ActorDesc;
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    m_p2DColliderComs.resize(1);
    /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 50.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;
    m_p2DColliderComs[0]->Set_Active(false);
    Get_ControllerTransform()->Rotation(XMConvertToRadians(90.f), _vector{ 0.f, 0.f, 1.f, 0.f });

    return S_OK;
}

void CPalmDecal::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);

}

void CPalmDecal::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CPalmDecal::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);

}

HRESULT CPalmDecal::Render()
{
#ifdef _DEBUG

     m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));


#endif // _DEBUG

    return __super::Render();
}

void CPalmDecal::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    int a = 0;
    Event_SetActive( m_p2DColliderComs[0], false);
}

void CPalmDecal::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    OBJECT_GROUP eOtherGroup = (OBJECT_GROUP)_pOtherObject->Get_ObjectGroupID();
    switch (eOtherGroup)
    {
    case Client::MONSTER:
    case Client::INTERACTION_OBEJCT:
    case Client::MAPOBJECT:
    {
        m_StoppedObjects.insert(_pOtherObject);
        break;
    }
    case Client::NONE:
    case Client::PLAYER:
    case Client::PLAYER_TRIGGER:
    case Client::PLAYER_PROJECTILE:
    case Client::MONSTER_PROJECTILE:
    case Client::TRIGGER_OBJECT:
    case Client::RAY_OBJECT:
    case Client::BLOCKER:
    case Client::BOOK_3D:
    case Client::WORD_GAME:
    case Client::FALLINGROCK_BASIC:
    case Client::EFFECT2D:
    case Client::DYNAMIC_OBJECT:
    case Client::NPC_EVENT:
    case Client::EXPLOSION:
        break;
    default:
        break;
    }
}

void CPalmDecal::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    int a = 0;
}

void CPalmDecal::Place(_fvector _v2DPos, _fvector _v2DDir)
{    //3D 상에서의 위치 잡아주기
    //방향도 받아서 돌려주기
    Set_Position({ XMVectorGetX(_v2DPos), XMVectorGetY(_v2DPos), 0.0f });
    static_cast<CTransform_2D*>(m_pControllerTransform->Get_Transform(COORDINATE_2D))->LookDir(_v2DDir);

    Event_SetActive(m_p2DColliderComs[0], true);
}

void CPalmDecal::Erase()
{
    //지우기
}


void CPalmDecal::Interact(CPlayer* _pUser)
{
    //지워지기
    Erase();
}

_bool CPalmDecal::Is_Interactable(CPlayer* _pUser)
{
    return (COORDINATE_3D == _pUser->Get_CurCoord()) 
        && (false == _pUser->Is_CarryingObject());
}

_float CPalmDecal::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}

CPalmDecal* CPalmDecal::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPalmDecal* pInstance = new CPalmDecal(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created PalmDecal Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPalmDecal::Clone(void* _pArg)
{
    CPalmDecal* pInstance = new CPalmDecal(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone PalmDecal Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPalmDecal::Free()
{
    __super::Free();
}

