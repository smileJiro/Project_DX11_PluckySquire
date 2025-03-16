#include "stdafx.h"
#include "TiltSwapPusher.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "SlipperyObject.h"
#include "Section_Manager.h"

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
    m_eDirection = pBodyDesc->eLookDirection;
    pBodyDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = pBodyDesc->iCurLevelID;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("tiltswappusher");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

	Set_Direction(pBodyDesc->eLookDirection);
    m_p2DColliderComs.resize(2);

    _float fBodyXColliderSize = 0.f;
    _float fBodyYColliderSize = 0.f;
    _float fBodyXColliderOffset = 0.f;
    _float fBodyYColliderOffset = 0.f;

    switch (m_eDirection)
    {
    case Client::F_DIRECTION::LEFT:
    case Client::F_DIRECTION::RIGHT:
        fBodyXColliderSize = 83.f;
        fBodyYColliderSize = 160.f;
        fBodyXColliderOffset = 25.f;
        fBodyYColliderOffset = 10.f;
        Switch_Animation(ANIM_IDLE_RIGHT);
        break;
    case Client::F_DIRECTION::UP:
    case Client::F_DIRECTION::DOWN:
        fBodyXColliderSize = 140.f;
        fBodyYColliderSize = 90.f;
        fBodyXColliderOffset = 0.f;
        fBodyYColliderOffset = -50.f;
        Switch_Animation(ANIM_IDLE_DOWN);
        break;

    default:
        break;
    }
    if (Client::F_DIRECTION::LEFT == m_eDirection)
    {
        fBodyXColliderOffset *= -1.f;
    }

    /* BODY 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
    AABBDesc.pOwner = this;
    AABBDesc.vExtents = { fBodyXColliderSize, fBodyYColliderSize };
    AABBDesc.vScale = { 1.0f, 1.0f };
    AABBDesc.vOffsetPosition = {fBodyXColliderOffset, fBodyYColliderOffset };
    AABBDesc.isBlock = true;
    AABBDesc.isTrigger = true;
    AABBDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;
    AABBDesc.iColliderUse =(_uint) COLLIDER2D_USE::COLLIDER2D_BODY;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
        return E_FAIL;

    _float fPusherXColliderSize = 0.f;
    _float fPusherYColliderSize = 0.f;
    _float fPusherXOffset = 0.f;
    _float fPusherYOffset = 0.f;
    switch (m_eDirection)
    {
    case Client::F_DIRECTION::LEFT:
    case Client::F_DIRECTION::RIGHT:
        fPusherXColliderSize = 90.f;
        fPusherYColliderSize = 160.f;
        fPusherXOffset = abs(fPusherXColliderSize) + abs(AABBDesc.vExtents.x) + abs(AABBDesc.vOffsetPosition.x);
        fPusherYOffset = AABBDesc.vOffsetPosition.y;
        break;
    case Client::F_DIRECTION::UP:
    case Client::F_DIRECTION::DOWN:
        fPusherXColliderSize = 140.f;
        fPusherYColliderSize = 90.f;
        fPusherXOffset = AABBDesc.vOffsetPosition.x;
        fPusherYOffset = -abs(fPusherYColliderSize) - abs( AABBDesc.vExtents.y) - abs(AABBDesc.vOffsetPosition.y);
        break;
        break;
    default:
        break;
    }
    if (Client::F_DIRECTION::LEFT == m_eDirection)
    {
        fPusherXOffset *= -1.f;
    }


    /* PUSHER 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc2 = {};
    AABBDesc2.pOwner = this;
    AABBDesc2.vExtents = { fPusherXColliderSize, fPusherYColliderSize };
    AABBDesc2.vScale = { 1.0f, 1.0f };
    AABBDesc2.vOffsetPosition = { fPusherXOffset, fPusherYOffset };
    AABBDesc2.isBlock = false;
    AABBDesc2.isTrigger = true;
    AABBDesc2.iCollisionGroupID = OBJECT_GROUP::TRIGGER_OBJECT;
    AABBDesc2.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_TRIGGER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_PushCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &AABBDesc2)))
        return E_FAIL;
    m_p2DColliderComs[1]->Set_Active(false);


    Register_OnAnimEndCallBack(bind(&CTiltSwapPusher::On_AnimEnd,this,std::placeholders::_1, std::placeholders::_2));
    return S_OK;
}

void CTiltSwapPusher::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}


void CTiltSwapPusher::On_Detonated()
{
	Set_State(STATE_EXPLODE);

}

void CTiltSwapPusher::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	switch (iAnimIdx)
	{
	case ANIM_EXPLODE_DOWN:
	case ANIM_EXPLODE_RIGHT:

        Set_State(STATE_IDLE);
		break;
	case ANIM_DROP_RIGHT:
	case ANIM_DROP_DOWN:
		Set_State(STATE_BOMBIDLE);
		break;
	default:
		break;
	}
}

void CTiltSwapPusher::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::SLIPPERY == _pOtherObject->Get_ObjectGroupID()
        &&(_uint)COLLIDER2D_USE::COLLIDER2D_TRIGGER == _pMyCollider->Get_ColliderUse())
    {
		CSlipperyObject* pSlippery = dynamic_cast<CSlipperyObject*>(_pOtherObject);
        if (nullptr == pSlippery)
            return;
        if (m_PushedObjects.end() == m_PushedObjects.find(pSlippery))
        {
            m_PushedObjects.insert(pSlippery);
            pSlippery->Start_Slip(FDir_To_Vector(m_eDirection), m_f2DKnockBackPower);
        }
    }
}

HRESULT CTiltSwapPusher::Render()
{
#ifdef _DEBUG
   if (m_p2DColliderComs[0]->Is_Active())
       m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
    if (m_p2DColliderComs[1]->Is_Active())
        m_p2DColliderComs[1]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));

#endif // _DEBUG
    return __super::Render();
}

void CTiltSwapPusher::Set_State(STATE _eState)
{
	if (m_eState == _eState)
		return;
	m_eState = _eState;
    switch (_eState)
    {
    case Client::CTiltSwapPusher::STATE_IDLE:
		if (Client::F_DIRECTION::LEFT == m_eDirection
            || Client::F_DIRECTION::RIGHT == m_eDirection)
			Switch_Animation(ANIM_IDLE_RIGHT);
		else
			Switch_Animation(ANIM_IDLE_DOWN);
        m_PushedObjects.clear();
        m_p2DColliderComs[1]->Set_Active(false);
        break;
    case Client::CTiltSwapPusher::STATE_DROP:
        if (Client::F_DIRECTION::LEFT == m_eDirection
            || Client::F_DIRECTION::RIGHT == m_eDirection)
            Switch_Animation(ANIM_DROP_RIGHT);
        else
            Switch_Animation(ANIM_DROP_DOWN);
        break;
    case Client::CTiltSwapPusher::STATE_EXPLODE:
        if (Client::F_DIRECTION::LEFT == m_eDirection
            || Client::F_DIRECTION::RIGHT == m_eDirection)
            Switch_Animation(ANIM_EXPLODE_RIGHT);
        else
            Switch_Animation(ANIM_EXPLODE_DOWN);
        m_p2DColliderComs[1]->Set_Active(true);
        break;
    case Client::CTiltSwapPusher::STATE_BOMBIDLE:
        if (Client::F_DIRECTION::LEFT == m_eDirection
            || Client::F_DIRECTION::RIGHT == m_eDirection)
            Switch_Animation(ANIM_BOMBIDLE_RIGHT);
        else
            Switch_Animation(ANIM_BOMBIDLE_DOWN);
        break;

    default:
        break;
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

void CTiltSwapPusher::On_BombInstalled()
{
	Set_State(STATE_DROP);
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
