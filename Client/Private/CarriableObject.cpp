#include "stdafx.h"
#include "CarriableObject.h"
#include "Player.h"

CCarriableObject::CCarriableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CCarriableObject::CCarriableObject(const CCarriableObject& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CCarriableObject::Render()
{
	return __super::Render();
}

HRESULT CCarriableObject::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CCarriableObject::Set_Carrier(CPlayer* _pCarrier)
{
	m_pCarrier = _pCarrier;
	//m_pParentMatrices = m_pCarrier->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_3D);
	m_pSocketMatrix =  m_pCarrier->Get_CarryingOffset3D_Ptr();
	return S_OK;
}

CCarriableObject* CCarriableObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCarriableObject* pInstance = new CCarriableObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CarriableObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCarriableObject::Clone(void* _pArg)
{
	CCarriableObject* pInstance = new CCarriableObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CarriableObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarriableObject::Free()
{
		__super::Free();
}
