#include "stdafx.h"
#include "BigPressurePlate.h"

CBigPressurePlate::CBigPressurePlate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CBigPressurePlate::CBigPressurePlate(const CBigPressurePlate& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CBigPressurePlate::Initialize(void* _pArg)
{

    if (nullptr == _pArg)
        return E_FAIL;
    CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("Pressure_Plate");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    //m_p2DColliderComs.resize(1);
    /////* Test 2D Collider */
    //CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

    //CircleDesc.pOwner = this;
    //CircleDesc.fRadius = 50.f;
    //CircleDesc.vScale = { 1.0f, 1.0f };
    //CircleDesc.vOffsetPosition = { 0.f, 0.f };
    //CircleDesc.isBlock = false;
    //CircleDesc.isTrigger = true;
    //CircleDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;

    //if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
    //    TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
    //    return E_FAIL;


    //Set_AnimationLoop(COORDINATE::COORDINATE_2D, DOWN, false);
    //Set_AnimationLoop(COORDINATE::COORDINATE_2D, DOWN_IDLE, true);
    //Set_AnimationLoop(COORDINATE::COORDINATE_2D, IDLE, true);
    //Set_AnimationLoop(COORDINATE::COORDINATE_2D, UP, false);

    //Switch_Animation(IDLE);
    //Register_OnAnimEndCallBack(bind(&CPressure_Plate::On_AnimEnd, this, placeholders::_1, placeholders::_2));


    return S_OK;
}

void CBigPressurePlate::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CBigPressurePlate::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

CBigPressurePlate* CBigPressurePlate::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CGameObject* CBigPressurePlate::Clone(void* _pArg)
{
	return nullptr;
}

void CBigPressurePlate::Free()
{
}
