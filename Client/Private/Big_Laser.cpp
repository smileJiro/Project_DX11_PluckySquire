#include "stdafx.h"
#include "Big_Laser.h"
#include "Shader.h"
#include "Section_Manager.h"
#include "GameInstance.h"
#include "Character.h"

CBig_Laser::CBig_Laser(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CBig_Laser::CBig_Laser(const CBig_Laser& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CBig_Laser::Initialize(void* _pArg)
{
    CModelObject::MODELOBJECT_DESC Desc = {};
    
    
    Desc.Build_2D_Model(m_pGameInstance->Get_CurLevelID(), L"EnergyBeam", L"Prototype_Component_Shader_VtxPosTex");
    Desc.Build_2D_Transform({-1400.f,25.f});

    HRESULT hr = __super::Initialize(&Desc);

    Register_OnAnimEndCallBack(bind(&CBig_Laser::Anim_End, this, placeholders::_1, placeholders::_2));
    
    
    Ready_Collider();
    
    Switch_Animation(LASER_STOP);
    __super::Update(0.f);
    return hr;
}

void CBig_Laser::Update(_float _fTimeDelta)
{
    if (true == m_isMove)
    {
        _vector vTargetPos = XMLoadFloat2(&m_fTargetPos);
        _vector vDir = XMVectorSetW(XMVector2Normalize(vTargetPos - Get_FinalPosition()),0.f);
        Get_ControllerTransform()->Go_Direction(vDir, _fTimeDelta);
        if (Get_ControllerTransform()->Compute_Distance(vTargetPos) < 5.f)
        {
            Set_Beam_Collider(false);
            m_isMove = false;
            if (Get_CurrentAnimIndex() == LASER_LOOP)
            {
                m_bReverseAnimation = true;
				Switch_Animation(LASER_START);
            }
        }
    }

    __super::Update(_fTimeDelta);
}


void CBig_Laser::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CBig_Laser::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
#ifdef _DEBUG

    if (COORDINATE_2D == Get_CurCoord())
    {
        for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
        {
            m_p2DColliderComs[i]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
        }
    }
#endif // _DEBUG
    return S_OK;
}

void CBig_Laser::Anim_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    switch (iAnimIdx)
    {
    case Client::CBig_Laser::LASER_START:
        if (false == m_bReverseAnimation)
        {
            Set_AnimationLoop(COORDINATE_2D, LASER_LOOP, true);
            Switch_Animation(LASER_LOOP);
        }
        else
            Switch_Animation(LASER_STOP);
        break;
    }
}

void CBig_Laser::Move_Start(_float _fMovePosX, _float _fSpeed)
{
    _vector vPos = Get_FinalPosition();
    XMStoreFloat2(&m_fTargetPos, XMVectorSetX(vPos, XMVectorGetX(vPos) + _fMovePosX));
    Get_ControllerTransform()->Set_SpeedPerSec(_fSpeed);
    m_isMove = true;
}

void CBig_Laser::Set_Beam_Collider(_bool _isBeamCollider)
{
    if (false == m_p2DColliderComs.empty())
    {
        for (auto pCollider : m_p2DColliderComs)
            pCollider->Set_Active(_isBeamCollider);
    }
}

void CBig_Laser::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (PLAYER & _pOtherCollider->Get_CollisionGroupID())
        Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 99, XMVectorZero());
}

HRESULT CBig_Laser::Ready_Collider()
{
    m_p2DColliderComs.resize(1);
    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
    AABBDesc.pOwner = this;
    AABBDesc.vExtents = {1.f,1.f};
    AABBDesc.vScale = { 60.0f, 500.0f };
    AABBDesc.vOffsetPosition = {0.f,-300.f};
    AABBDesc.isBlock = false;
    AABBDesc.isTrigger = true;
    AABBDesc.iCollisionGroupID = MONSTER_PROJECTILE;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Trigger"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
        return E_FAIL;

    return S_OK;
}

CModelObject* CBig_Laser::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBig_Laser* pInstance = new CBig_Laser(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBig_Laser::Clone(void* _pArg)
{
    CBig_Laser* pInstance = new CBig_Laser(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBig_Laser");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBig_Laser::Free()
{
    __super::Free();
}
