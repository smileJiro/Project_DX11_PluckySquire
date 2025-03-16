#include "stdafx.h"
#include "BombSwitchStopper.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Section_Manager.h"

CBombSwitchStopper::CBombSwitchStopper(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CBombSwitchStopper::CBombSwitchStopper(const CBombSwitchStopper& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CBombSwitchStopper::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;
    BOMB_SWITCH_STOPPER_DESC* pBodyDesc = static_cast<BOMB_SWITCH_STOPPER_DESC*>(_pArg);
    m_iCurLevelID = pBodyDesc->iCurLevelID;
	pBodyDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("BombSwitchStopper");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_p2DColliderComs.resize(1);
    ///* Test 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC tAABBDEsc = {};
    tAABBDEsc.pOwner = this;
    tAABBDEsc.vExtents = { 125.f, 150.f };
    tAABBDEsc.vScale = { 1.0f, 1.0f };
    tAABBDEsc.vOffsetPosition = { 0.f, tAABBDEsc.vExtents.y };
    tAABBDEsc.isBlock = true;
    tAABBDEsc.isTrigger = false;
    tAABBDEsc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &tAABBDEsc)))
        return E_FAIL;
    Register_OnAnimEndCallBack(bind(&CBombSwitchStopper::On_AnimEnd, this, placeholders::_1, placeholders::_2));
    Switch_Animation(SQUARE_UP);
    return S_OK;
}

HRESULT CBombSwitchStopper::Render()
{
#ifdef _DEBUG
    if (m_p2DColliderComs[0]->Is_Active())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

    return __super::Render();
}

void CBombSwitchStopper::On_BombSwitch(_bool _bOn)
{

    if (SQUARE_DOWN == Get_CurrentAnimIndex())
    {
        Switch_Animation(SQUARE_MOVE_UP);
        m_p2DColliderComs[0]->Set_Active(true);
        SECTION_MGR->Change_GameObject_LayerIndex(m_strSectionName, this, SECTION_2D_PLAYMAP_OBJECT);
    }
    else if (SQUARE_UP == Get_CurrentAnimIndex())
        Switch_Animation(SQUARE_MOVE_DOWN);


}

void CBombSwitchStopper::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
    if (SQUARE_MOVE_UP == _iAnimIdx)
    {
        Switch_Animation(SQUARE_UP);
    }
    else if (SQUARE_MOVE_DOWN == _iAnimIdx)
    {
        Switch_Animation(SQUARE_DOWN);
        SECTION_MGR->Change_GameObject_LayerIndex(m_strSectionName, this, SECTION_2D_PLAYMAP_BACKGROUND);
        m_p2DColliderComs[0]->Set_Active(false);
    }
}


CBombSwitchStopper* CBombSwitchStopper::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBombSwitchStopper* pInstance = new CBombSwitchStopper(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : BombSwitchStopper");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBombSwitchStopper::Clone(void* _pArg)
{
    CBombSwitchStopper* pInstance = new CBombSwitchStopper(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : BombSwitchStopper");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBombSwitchStopper::Free()
{
	__super::Free();
}

