#include "stdafx.h"
#include "GameEventExecuter_C8.h"
#include "Trigger_Manager.h"


CGameEventExecuter_C8::CGameEventExecuter_C8(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameEventExecuter(_pDevice, _pContext)
{
}


HRESULT CGameEventExecuter_C8::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	// 초기 설정 
	switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
	{
	case Client::CTrigger_Manager::C02P0910_LIGHTNING_BOLT_SPAWN:
		break;
	default:
		break;
	}

	return S_OK;
}

void CGameEventExecuter_C8::Priority_Update(_float _fTimeDelta)
{
	_wstring strEventTag;
}

void CGameEventExecuter_C8::Update(_float _fTimeDelta)
{
	if (!Is_Dead())
	{
		switch ((CTrigger_Manager::EVENT_EXECUTER_ACTION_TYPE)m_iEventExcuterAction)
		{
		case Client::CTrigger_Manager::C02P0910_LIGHTNING_BOLT_SPAWN:
			break;
		default:
			break;
		}
	}

}

void CGameEventExecuter_C8::Late_Update(_float _fTimeDelta)
{
}


CGameEventExecuter_C8* CGameEventExecuter_C8::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CGameEventExecuter_C8* pInstance = new CGameEventExecuter_C8(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameEventExecuter_C8");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameEventExecuter_C8::Clone(void* _pArg)
{
	CGameEventExecuter_C8* pInstance = new CGameEventExecuter_C8(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameEventExecuter_C8");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameEventExecuter_C8::Free()
{
	__super::Free();
}


HRESULT CGameEventExecuter_C8::Cleanup_DeadReferences()
{
	return S_OK;
}
