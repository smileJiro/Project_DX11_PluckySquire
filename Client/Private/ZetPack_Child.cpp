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
}

void CZetPack_Child::Update(_float fTimeDelta)
{
}

void CZetPack_Child::Late_Update(_float _fTimeDelta)
{
}

HRESULT CZetPack_Child::Render()
{
	__super::Render();
    return S_OK;
}

void CZetPack_Child::State_Change()
{
}

void CZetPack_Child::State_Change_Idle()
{
}

void CZetPack_Child::State_Change_Talk()
{
}

void CZetPack_Child::Action_State(_float _fTimeDelta)
{
}

void CZetPack_Child::Action_State_Idle(_float _fTimeDelta)
{
}

void CZetPack_Child::Action_State_Talk(_float _fTimeDelta)
{
}

void CZetPack_Child::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
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
