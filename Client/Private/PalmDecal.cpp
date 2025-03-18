#include "stdafx.h"
#include "PalmDecal.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"
#include "Stoppable.h"

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
	m_pSectionMgr = CSection_Manager::GetInstance();
    m_eInteractID = INTERACT_ID::PALMDECAL;
    m_eInteractKey = KEY::E;

    CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

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

    SHAPE_SPHERE_DESC ShapeDesc2 = {};
    ShapeDesc2.fRadius = 0.25f;
    SHAPE_DATA ShapeData2;
    ShapeData2.pShapeDesc = &ShapeDesc2;
    ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
    ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
    ShapeData2.isTrigger = false;
    ShapeData2.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
    ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER_TRIGGER;
    XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
    ActorDesc.ShapeDatas.push_back(ShapeData2);

    ActorDesc.tFilterData.MyGroup = m_iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_TRIGGER;
    pBodyDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    pBodyDesc->pActorDesc = &ActorDesc;
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;
	m_pControllerTransform->Create_Transform(COORDINATE_3D);

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
    if(m_bPlacedFrame)
    {
        Event_SetActive(m_p2DColliderComs[0], false);
        m_bPlacedFrame = false;
    }
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

_matrix CPalmDecal::Get_FinalWorldMatrix()
{
    return Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_WorldMatrix();
}

void CPalmDecal::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    IStoppable* pStop = dynamic_cast<IStoppable*>(_pOtherObject);
    if (pStop)
    {
        pStop->Stop();
        m_StoppedObjects.insert(_pOtherObject);
    }



}

void CPalmDecal::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

}

void CPalmDecal::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

}

void CPalmDecal::Place(_fvector _v2DPos, _fvector _v2DDir)
{   
    Erase();

  
    m_pSectionMgr->Remove_GameObject_FromSectionLayer(m_strSectionName,this);
	m_strSectionName = m_pSectionMgr->Get_Cur_Section_Key();
    m_pSectionMgr->Add_GameObject_ToSectionLayer(m_strSectionName, this, SECTION_2D_PLAYMAP_STAMP);
    static_cast<CTransform_2D*>(m_pControllerTransform->Get_Transform(COORDINATE_2D))->LookDir(_v2DDir);
    Set_Position({ XMVectorGetX(_v2DPos), XMVectorGetY(_v2DPos), 0.0f });


    _vector v3DPos = m_pSectionMgr->Get_WorldPosition_FromWorldPosMap(m_strSectionName, { XMVectorGetX(_v2DPos),XMVectorGetY(_v2DPos) });
    m_pControllerTransform->Get_Transform(COORDINATE_3D)->Set_State(CTransform::STATE_POSITION, v3DPos);
    m_pActorCom->Update(0.f);
    m_bPlacedFrame = true;
    m_p2DColliderComs[0]->Set_Active(true);
}

void CPalmDecal::Erase()
{
    Set_Active(false);
    m_p2DColliderComs[0]->Set_Active(false);
    //지우기
    for (auto& pStopped : m_StoppedObjects)
    {
        IStoppable* pStoppable = dynamic_cast<IStoppable*>(pStopped);
        pStoppable->UnStop();
    }
	m_StoppedObjects.clear();
}


void CPalmDecal::Interact(CPlayer* _pUser)
{
    //지워지기
	_pUser->Set_State(CPlayer::ERASE_PALM);

}
/*Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_WorldMatrix();*/
_bool CPalmDecal::Is_Interactable(CPlayer* _pUser)
{
    return (COORDINATE_3D == _pUser->Get_CurCoord()) 
        && (false == _pUser->Is_CarryingObject()
        && m_strSectionName == m_pSectionMgr->Get_Cur_Section_Key());
}

_float CPalmDecal::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}

void CPalmDecal::Active_OnEnable()
{
    __super::Active_OnEnable();
    m_pActorCom->Set_Active(true);

}

void CPalmDecal::Active_OnDisable()
{
    __super::Active_OnDisable();
    m_pActorCom->Set_Active(true);
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

