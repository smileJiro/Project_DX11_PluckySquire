#include "stdafx.h"
#include "EvictedSword.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"

void CEvictedSword::Interact(CPlayer* _pUser)
{
    _pUser->Set_Position(Get_FinalPosition());
    _pUser->RetrieveSword();
	Set_Active(false); 
}

_bool CEvictedSword::Is_Interactable(CPlayer* _pUser)
{
	return Get_Distance(COORDINATE_2D, _pUser) < 30.f;
}

_float CEvictedSword::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
    return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
        - _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

}

CEvictedSword::CEvictedSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CEvictedSword::CEvictedSword(const CEvictedSword& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CEvictedSword::Initialize(void* _pArg)
{

    CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);
	pBodyDesc->iCurLevelID = LEVEL_CHAPTER_2;
    pBodyDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_CHAPTER_2;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("EvictSword");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
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
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;
    m_p2DColliderComs[0]->Set_Active(false);

    Register_OnAnimEndCallBack(bind(&CEvictedSword::On_AnimEnd, this, placeholders::_1, placeholders::_2));


    Switch_Animation(EVICT);

    return S_OK;
}

void CEvictedSword::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

HRESULT CEvictedSword::Render()
{
#ifdef _DEBUG
    if (m_p2DColliderComs[0]->Is_Active())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));

#endif // _DEBUG
    return __super::Render();
}

void CEvictedSword::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (EVICT ==iAnimIdx )
	{
		m_p2DColliderComs[0]->Set_Active(true);
	}
}

CEvictedSword* CEvictedSword::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CEvictedSword* pInstance = new CEvictedSword(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : EvictedSword");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEvictedSword::Clone(void* _pArg)
{
    CEvictedSword* pInstance = new CEvictedSword(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : EvictedSword");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEvictedSword::Free()
{
	__super::Free();
}
