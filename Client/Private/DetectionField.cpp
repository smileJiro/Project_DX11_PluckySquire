#include "stdafx.h"
#include "DetectionField.h"

CDetectionField::CDetectionField(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CDetectionField::CDetectionField(const CDetectionField& _Prototype)
	: CComponent(_Prototype)
{
}

HRESULT CDetectionField::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDetectionField::Initialize(void* _pArg)
{
	DETECTIONFIELDDESC* pDesc = static_cast<DETECTIONFIELDDESC*>(_pArg);
	m_fRadius = pDesc->fRadius;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CDetectionField::Render()
{
	return S_OK;
}
#endif

CDetectionField* CDetectionField::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDetectionField* pInstance = new CDetectionField(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDetectionField");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDetectionField* CDetectionField::Clone(void* _pArg)
{
	CDetectionField* pInstance = new CDetectionField(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CDetectionField");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDetectionField::Free()
{
	__super::Free();
}
