#include "stdafx.h"
#include "BigPressurePlate.h"
#include "Collider_Circle.h"

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
    m_iCurLevelID = pBodyDesc->iCurLevelID;
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("BigPressurePad");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_p2DColliderComs.resize(1);
    ///* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 50.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    return S_OK;
}

void CBigPressurePlate::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();

    if ( OBJECT_GROUP::GIMMICK_OBJECT == eGroup)
    {
        
    }
}

void CBigPressurePlate::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

CBigPressurePlate* CBigPressurePlate::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBigPressurePlate* pInstance = new CBigPressurePlate(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : BigPressurePlate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBigPressurePlate::Clone(void* _pArg)
{
    CBigPressurePlate* pInstance = new CBigPressurePlate(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : BigPressurePlate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBigPressurePlate::Free()
{
    __super::Free();
}