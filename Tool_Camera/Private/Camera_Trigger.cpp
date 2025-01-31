#include "stdafx.h"
#include "Camera_Trigger.h"

CCamera_Trigger::CCamera_Trigger(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CTriggerObject(_pDevice, _pContext)
{
}

CCamera_Trigger::CCamera_Trigger(const CCamera_Trigger& _Prototype)
	:CTriggerObject(_Prototype)
{
}

HRESULT CCamera_Trigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Trigger::Initialize(void* _pArg)
{
	CAMERA_TRIGGER_DESC* pDesc = static_cast<CAMERA_TRIGGER_DESC*>(_pArg);

	m_iCameraTriggerType = pDesc->iCameraTriggerType;
	m_szEventTag = pDesc->szEventTag;
	m_isReturn = pDesc->isReturn;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Trigger::Priority_Update(_float _fTimeDelta)
{
}

void CCamera_Trigger::Update(_float _fTimeDelta)
{
}

void CCamera_Trigger::Late_Update(_float _fTimeDelta)
{
	CTriggerObject::Late_Update(_fTimeDelta);
}

void CCamera_Trigger::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CCamera_Trigger::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CCamera_Trigger::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

CCamera_Trigger* CCamera_Trigger::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Trigger* pInstance = new CCamera_Trigger(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Trigger::Clone(void* _pArg)
{
	CCamera_Trigger* pInstance = new CCamera_Trigger(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Trigger::Free()
{
	__super::Free();
}
