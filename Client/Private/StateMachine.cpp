#include "stdafx.h"
#include "StateMachine.h"


CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine& Prototype)
	: CComponent(Prototype)
{
}

HRESULT CStateMachine::Initialize_Prototype()
{
	return S_OK;
}

void CStateMachine::Update(_float fTimeDelta)
{
	Get_CurrentState()->Update(fTimeDelta);
}



HRESULT CStateMachine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	STATEMACHINE_DESC* pDesc = static_cast<STATEMACHINE_DESC*>(pArg);
	m_pOwner = pDesc->pOwner;
	

	return S_OK;
}


void CStateMachine::Transition_To(CPlayerState* _pNewState)
{
	if (nullptr != m_pState)
		Safe_Release(m_pState);
	m_pState = _pNewState;
	for (auto& callback : m_listStateChangeCallback)
		callback(_pNewState);
}


CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}


CComponent* CStateMachine::Clone(void* pArg)
{
	CStateMachine* pInstance = new CStateMachine(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();
}