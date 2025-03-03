#include "stdafx.h"
#include "TiltSwapPusher.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "SlipperyObject.h"

CTiltSwapPusher::CTiltSwapPusher(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CBombableObject(_pDevice, _pContext)
{
}

CTiltSwapPusher::CTiltSwapPusher(const CTiltSwapPusher& _Prototype)
	:CBombableObject(_Prototype)
{
}

HRESULT CTiltSwapPusher::Initialize(void* _pArg)
{

    TILTSWAPPUSHER_DESC* pBodyDesc = static_cast<TILTSWAPPUSHER_DESC*>(_pArg);
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
    switch (m_eDirection)
    {
    case Client::F_DIRECTION::LEFT:
    case Client::F_DIRECTION::RIGHT:
        Switch_Animation(ANIM_IDLE_RIGHT);
        break;
    case Client::F_DIRECTION::UP:
    case Client::F_DIRECTION::DOWN:
        Switch_Animation(ANIM_IDLE_DOWN);
        break;

    default:
        break;
    }

	Set_Direction(pBodyDesc->eDirection);

    m_p2DColliderComs.resize(1);
    /* Test 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
    AABBDesc.pOwner = this;
    AABBDesc.vExtents = { 100.f,100.f };
    AABBDesc.vScale = { 1.0f, 1.0f };
    AABBDesc.vOffsetPosition = { 0.f, AABBDesc.vExtents.y * 0.5f };
    AABBDesc.isBlock = true;
    AABBDesc.isTrigger = true;
    AABBDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
        return E_FAIL;



    return S_OK;
}

void CTiltSwapPusher::Detonate()
{
    switch (m_eDirection)
    {
    case Client::F_DIRECTION::LEFT:
    case Client::F_DIRECTION::RIGHT:
        Switch_Animation(ANIM_EXPLODE_RIGHT);
        break;
    case Client::F_DIRECTION::UP:
    case Client::F_DIRECTION::DOWN:
        Switch_Animation(ANIM_EXPLODE_DOWN);
        break;
    case Client::F_DIRECTION::F_DIR_LAST:
        break;
    default:
        break;
    }
}

void CTiltSwapPusher::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	switch (iAnimIdx)
	{
	case ANIM_EXPLODE_DOWN:
	case ANIM_EXPLODE_RIGHT:
		break;
	default:
		break;
	}
}

void CTiltSwapPusher::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::SLIPPERY == _pOtherObject->Get_ObjectGroupID())
    {
		CSlipperyObject* pSlippery = dynamic_cast<CSlipperyObject*>(_pOtherObject);
		if (nullptr == pSlippery)

        pSlippery->Start_Slip(FDir_To_Vector(m_eDirection), m_f2DKnockBackPower);
    }
}

void CTiltSwapPusher::Set_Direction(F_DIRECTION _eDirection)
{

	m_eDirection = _eDirection;

    switch (m_eDirection)
    {
    case Client::F_DIRECTION::LEFT:
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
        break;
    }
    case Client::F_DIRECTION::RIGHT:
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
        break;
    }
    case Client::F_DIRECTION::UP:
    {
        _vector vUp = m_pControllerTransform->Get_State(CTransform::STATE_UP);
        Get_ControllerTransform()->Set_State(CTransform::STATE_UP, -XMVectorAbs(vUp));
        break;
    }
    case Client::F_DIRECTION::DOWN:
    {
        _vector vUp = m_pControllerTransform->Get_State(CTransform::STATE_UP);
        Get_ControllerTransform()->Set_State(CTransform::STATE_UP, XMVectorAbs(vUp));
        break;
    }
    default:
        break;
    }

}

CTiltSwapPusher* CTiltSwapPusher::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTiltSwapPusher* pInstance = new CTiltSwapPusher(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : TiltSwapPusher");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTiltSwapPusher::Clone(void* _pArg)
{
    CTiltSwapPusher* pInstance = new CTiltSwapPusher(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : TiltSwapPusher");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTiltSwapPusher::Free()
{
    __super::Free();
}
