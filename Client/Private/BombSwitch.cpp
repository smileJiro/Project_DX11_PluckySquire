#include "stdafx.h"
#include "BombSwitch.h"
#include "Collider_Circle.h"
#include "Section_Manager.h"

CBombSwitch::CBombSwitch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CBombSwitch::CBombSwitch(const CBombSwitch& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CBombSwitch::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;
    BOMB_SWITCH_DESC* pBodyDesc = static_cast<BOMB_SWITCH_DESC*>(_pArg);
    m_iCurLevelID = pBodyDesc->iCurLevelID;
	m_pReceiver = pBodyDesc->pReceiver;
    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("BombSwitch");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_p2DColliderComs.resize(1);
    ///* Test 2D Collider */
    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 30.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius};
    CircleDesc.isBlock = false;
    CircleDesc.isTrigger = true;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::GIMMICK_OBJECT;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    Set_SwitchState(pBodyDesc->eStartState);
    return S_OK;
}

HRESULT CBombSwitch::Render()
{

#ifdef _DEBUG
    if (m_p2DColliderComs[0]->Is_Active())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));

#endif // _DEBUG
    return __super::Render();
}



void CBombSwitch::On_Hit(CGameObject* _pHitter, _int _fDamg, _fvector _vForce)
{
    Set_SwitchState(OFF == m_eBombSwitchOn ? ON : OFF);


}

void CBombSwitch::Set_SwitchState(BOMB_SWITCH_STATE _eState)
{
	if (m_eBombSwitchOn == _eState)
		return;
    m_eBombSwitchOn = _eState;
    if (m_pReceiver)
        m_pReceiver->Switch_Bomb(ON == m_eBombSwitchOn );
    Switch_Animation(m_eBombSwitchOn);
}

CBombSwitch* CBombSwitch::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBombSwitch* pInstance = new CBombSwitch(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : BombSwitch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBombSwitch::Clone(void* _pArg)
{
	CBombSwitch* pInstance = new CBombSwitch(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : BombSwitch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBombSwitch::Free()
{
	__super::Free();
}
