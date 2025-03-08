#include "stdafx.h"
#include "ZetPack_Child.h"
#include "GameInstance.h"
#include "Player.h"

CZetPack_Child::CZetPack_Child(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CZetPack_Child::CZetPack_Child(const CZetPack_Child& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CZetPack_Child::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CZetPack_Child::Initialize(void* _pArg)
{
    ZETPACK_CHILD_DESC* pDesc = static_cast<ZETPACK_CHILD_DESC*>(_pArg);
    if (nullptr == pDesc->pPlayer)
        return E_FAIL;
    m_pPlayer = pDesc->pPlayer;
    Safe_AddRef(m_pPlayer);
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CZetPack_Child::Priority_Update(_float _fTimeDelta)
{
	if (true == m_pPlayer->Is_Dead())
	{
		Safe_Release(m_pPlayer);
		m_pPlayer = nullptr;
	}

	__super::Priority_Update(_fTimeDelta);
}

void CZetPack_Child::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CZetPack_Child::Late_Update(_float _fTimeDelta)
{
	State_Change();

	__super::Late_Update(_fTimeDelta);
}

HRESULT CZetPack_Child::Render()
{
	__super::Render();
    return S_OK;
}

void CZetPack_Child::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CZetPack_Child::STATE_IDLE:
		State_Change_Idle();
		break;
	case Client::CZetPack_Child::STATE_TALK:
		State_Change_Talk();
		break;
	case Client::CZetPack_Child::STATE_CHASE:
		State_Change_Chase();
		break;
	case Client::CZetPack_Child::STATE_PORTALOUT:
		State_Change_PortalOut();
		break;
	case Client::CZetPack_Child::STATE_PORTALIN:
		State_Change_PortalIn();
		break;
	default:
		break;
	}
}

void CZetPack_Child::State_Change_Idle()
{
}

void CZetPack_Child::State_Change_Talk()
{
}

void CZetPack_Child::State_Change_Chase()
{
}

void CZetPack_Child::State_Change_PortalOut()
{
}

void CZetPack_Child::State_Change_PortalIn()
{
}

void CZetPack_Child::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CZetPack_Child::STATE_IDLE:
		Action_State_Idle(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_TALK:
		Action_State_Talk(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_CHASE:
		Action_State_Chase(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_PORTALOUT:
		Action_State_PortalOut(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_PORTALIN:
		Action_State_PortalIn(_fTimeDelta);
		break;

	default:
		break;
	}
}

void CZetPack_Child::Action_State_Idle(_float _fTimeDelta)
{
}

void CZetPack_Child::Action_State_Talk(_float _fTimeDelta)
{
}

void CZetPack_Child::Action_State_Chase(_float _fTimeDelta)
{
}

void CZetPack_Child::Action_State_PortalOut(_float _fTimeDelta)
{
}

void CZetPack_Child::Action_State_PortalIn(_float _fTimeDelta)
{
}

void CZetPack_Child::ChaseToTarget(_float _fTimeDelta)
{
	if (nullptr == m_pPlayer)
		return;

	_vector vPos = Get_FinalPosition();
	_vector vPlayerPos = m_pPlayer->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);
	_vector vDir = vPlayerPos - vPos;
	_float fDiff = XMVectorGetX(XMVector3Length(vDir));
	vDir /= fDiff;
	
	/* 1. 외적을 통한 방향 판별 */
	_vector vUpVector = { 0.0f, 1.0f, 0.0f, 0.0f };
	_vector vCrossResult = XMVector3Cross(vUpVector, vDir);


}


CZetPack_Child* CZetPack_Child::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CZetPack_Child* pInstance = new CZetPack_Child(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CZetPack_Child");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CZetPack_Child* CZetPack_Child::Clone(void* _pArg)
{
	CZetPack_Child* pInstance = new CZetPack_Child(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CZetPack_Child");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZetPack_Child::Free()
{
    Safe_Release(m_pPlayer);

    __super::Free();
}
