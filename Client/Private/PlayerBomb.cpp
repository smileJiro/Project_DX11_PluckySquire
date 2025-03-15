#include "stdafx.h"
#include "PlayerBomb.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"

CPlayerBomb::CPlayerBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CPlayerBomb::CPlayerBomb(const CPlayerBomb& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CPlayerBomb::Initialize(void* _pArg)
{
    CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
  
    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("playerbomb");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    m_p2DColliderComs.resize(1);
    /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 100.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius * 0.5f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;
    m_pExplosionCollider = m_p2DColliderComs[0];
    Safe_AddRef(m_pExplosionCollider);
	m_pExplosionCollider->Set_Active(false);

    Switch_Animation(IDLE);

   Register_OnAnimEndCallBack(bind(&CPlayerBomb::On_AnimEnd, this, placeholders::_1, placeholders::_2));

    return S_OK;
}



CPlayerBomb* CPlayerBomb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayerBomb* pInstance = new CPlayerBomb(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : PlayerBomb");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerBomb::Clone(void* _pArg)
{
    CPlayerBomb* pInstance = new CPlayerBomb(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : PlayerBomb");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerBomb::Free()
{
	Safe_Release(m_pExplosionCollider);
	__super::Free();
}

void CPlayerBomb::Detonate()
{
	Switch_Animation(EXPLODE);
	m_pExplosionCollider->Set_Active(true);
}

void CPlayerBomb::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (iAnimIdx == EXPLODE)
	{
		Event_DeleteObject(this);
	}
}

void CPlayerBomb::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

    if (OBJECT_GROUP::MONSTER == _pOtherCollider->Get_CollisionGroupID()
        || OBJECT_GROUP::GIMMICK_OBJECT == _pOtherCollider->Get_CollisionGroupID())
    {
        if (m_AttckedObjects.find(_pOtherObject) != m_AttckedObjects.end())
            return;

       // CCharacter* pCharacter = dynamic_cast<CCharacter*>(_pOtherObject);
        _vector vKnockBackDir = XMVectorSetW(XMVectorSetZ( XMVector3Normalize( Get_FinalPosition() - _pOtherObject->Get_FinalPosition()),0.f),0.f);
        _vector vKnockBackForce = vKnockBackDir * m_f2DKnockBackPower;
        _pOtherObject->On_Hit(this, m_iAttackDamg, vKnockBackForce);


        m_AttckedObjects.insert(_pOtherObject);
    }


}
