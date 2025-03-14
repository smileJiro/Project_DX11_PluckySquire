#include "stdafx.h"
#include "BombableBox.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "SlipperyObject.h"
#include "Section_Manager.h"

CBombableBox::CBombableBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C2DMapWordObject(_pDevice, _pContext)
{
}

CBombableBox::CBombableBox(const CBombableBox& _Prototype)
    :C2DMapWordObject(_Prototype)
{
}

HRESULT CBombableBox::Initialize(void* _pArg)
{

    MODELOBJECT_DESC* pBodyDesc = static_cast<MODELOBJECT_DESC*>(_pArg);
    pBodyDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = pBodyDesc->iCurLevelID;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("BoomBox");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    _float fBodyXColliderSize = 0.f;
    _float fBodyYColliderSize = 0.f;
    _float fBodyXColliderOffset = 0.f;
    _float fBodyYColliderOffset = 0.f;

    /* BODY 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
    AABBDesc.pOwner = this;
    AABBDesc.vExtents = { fBodyXColliderSize, fBodyYColliderSize };
    AABBDesc.vScale = { 1.0f, 1.0f };
    AABBDesc.vOffsetPosition = { fBodyXColliderOffset, fBodyYColliderOffset };
    AABBDesc.isBlock = true;
    AABBDesc.isTrigger = true;
    AABBDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;
    AABBDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
        return E_FAIL;

    _float fPusherXColliderSize = 0.f;
    _float fPusherYColliderSize = 0.f;
    _float fPusherXOffset = 0.f;
    _float fPusherYOffset = 0.f;
  

    Register_OnAnimEndCallBack(bind(&CBombableBox::On_AnimEnd, this, std::placeholders::_1, std::placeholders::_2));
    return S_OK;
}

void CBombableBox::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}


void CBombableBox::On_Detonated()
{
    Set_State(STATE_EXPLODE);
}

void CBombableBox::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    switch (iAnimIdx)
    {
    case EXPLODE:
        Event_DeleteObject(this);
    default:
        break;
    }
}

void CBombableBox::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

_bool CBombableBox::Action_Execute(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex)
{
    _bool isExecute = false;
    for_each(m_Actions.begin(), m_Actions.end(), [this, &isExecute, &_iControllerIndex, &_iContainerIndex, &_iWordIndex]
    (WORD_ACTION& tAction) {
            if (tAction.iControllerIndex == _iControllerIndex
                && tAction.iContainerIndex == _iContainerIndex
                && tAction.iWordType == _iWordIndex)
            {
                isExecute = true;
                m_iContainerIndex = _iContainerIndex;
                m_iControllerIndex = _iControllerIndex;
                m_iWordIndex = _iWordIndex;
                switch (tAction.eAction)
                {
                case ANIMATION_CHANGE:
                {
                    _uint iAnimIndex = any_cast<_uint>(tAction.anyParam);
                    Set_AnimationLoop(COORDINATE_2D, iAnimIndex, true);
                    //Set_Animation(iAnimIndex);
					Set_State((STATE)iAnimIndex);
                }
                break;
                break;
                default:
                    assert(nullptr);
                    break;
                }

            }
        });
    return isExecute;
}

HRESULT CBombableBox::Render()
{
#ifdef _DEBUG
    if (m_p2DColliderComs[0]->Is_Active())
        m_p2DColliderComs[0]->Render();
#endif // _DEBUG
    return __super::Render();
   
}

void CBombableBox::Set_State(STATE _eState)
{
    if (m_eState == _eState)
        return;
    m_eState = _eState;
    Switch_Animation(_eState);
}

void CBombableBox::Detonate()
{
	if (STATE_BOMBIDLE == m_eState)
	    On_Detonated();
}


CBombableBox* CBombableBox::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBombableBox* pInstance = new CBombableBox(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : BombableBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBombableBox::Clone(void* _pArg)
{
    CBombableBox* pInstance = new CBombableBox(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : BombableBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBombableBox::Free()
{
    __super::Free();
}
