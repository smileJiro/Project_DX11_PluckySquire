#include "stdafx.h"
#include "Meteor.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"
#include "Effect2D_Manager.h"

CMeteor::CMeteor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CMeteor::CMeteor(const CMeteor& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CMeteor::Initialize(void* _pArg)
{
    METEOR_DESC* pBodyDesc = static_cast<METEOR_DESC*>(_pArg);

    m_eDir = pBodyDesc->eDir;
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = pBodyDesc->iCurLevelID;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("meteor");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

	_float3 fGravityDir = { 0.5f, -0.5f, 0.f };

    switch (m_eDir)
    {
    case Client::F_DIRECTION::LEFT:
        break;
    case Client::F_DIRECTION::RIGHT:
    {
        _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
        Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -1.f * XMVectorAbs(vRight));
		fGravityDir.x = -0.5f;
    }
        break;
    default:
        break;
    }



    m_p2DColliderComs.resize(1);
    /* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 30.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;
    m_pExplosionCollider = m_p2DColliderComs[0];
    Safe_AddRef(m_pExplosionCollider);
    m_pExplosionCollider->Set_Active(false);

    Switch_Animation(IDLE);

    /* Com_Gravity */
    CGravity::GRAVITY_DESC GravityDesc = {};
    GravityDesc.fGravity = 9.8f * 25.f;
    GravityDesc.vGravityDirection = fGravityDir;
    GravityDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
        TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
        return E_FAIL;
    Safe_AddRef(m_pGravityCom);

    m_pGravityCom->Set_Active(true);
    m_pGravityCom->Change_State(CGravity::STATE_FALLDOWN);


    return S_OK;
}

HRESULT CMeteor::Render()
{
#ifdef _DEBUG
    if (!m_p2DColliderComs.empty())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG
    return __super::Render();
}

void CMeteor::Active_OnEnable()
{
    __super::Active_OnEnable();
    m_pGravityCom->Set_GravityAcc(0.f);
}



CMeteor* CMeteor::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMeteor* pInstance = new CMeteor(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Meteor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMeteor::Clone(void* _pArg)
{
    CMeteor* pInstance = new CMeteor(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : Meteor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMeteor::Free()
{
    Safe_Release(m_pExplosionCollider);
    __super::Free();
}


void CMeteor::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

    if (OBJECT_GROUP::PLAYER == _pOtherCollider->Get_CollisionGroupID()
        || OBJECT_GROUP::BLOCKER == _pOtherCollider->Get_CollisionGroupID()
        || OBJECT_GROUP::BLOCKER_JUMPPASS== _pOtherCollider->Get_CollisionGroupID())
    {

        CEffect2D_Manager::GetInstance()->Play_Effect(L"meteor_explosion", m_strSectionName, Get_ControllerTransform()->Get_WorldMatrix());

        Event_DeleteObject(this);
        if (OBJECT_GROUP::PLAYER == _pOtherCollider->Get_CollisionGroupID())
        {
            Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 2, _vector{ 0.f, 0.f, 0.f });
        }
    }


}
