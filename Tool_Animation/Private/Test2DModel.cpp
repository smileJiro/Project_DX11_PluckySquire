#include "stdafx.h"
#include "Test2DModel.h"

CTest2DModel::CTest2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:C2DModel(_pDevice, _pContext)
{
}

CTest2DModel::CTest2DModel(const CTest2DModel& _Prototype)
	:C2DModel(_Prototype)
{
}



HRESULT CTest2DModel::Initialize_Prototype_FromModelFile(const _char* _pModelFilePath)
{
	return S_OK;
}

CTest2DModel* CTest2DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _bool _bRawData, const _char* _pPath)
{
	CTest2DModel* pInstance = new CTest2DModel(pDevice, pContext);

	if (_bRawData)
	{
		if (FAILED(pInstance->Initialize_Prototype(_pPath)))
		{
			MSG_BOX("Failed to Created : Test2DModel");
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Initialize_Prototype_FromModelFile(_pPath)))
		{
			MSG_BOX("Failed to Created : Test2DModel");
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}

CComponent* CTest2DModel::Clone(void* _pArg)
{
	CTest2DModel* pInstance = new CTest2DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Test2DModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest2DModel::Free()
{
	__super::Free();
}
