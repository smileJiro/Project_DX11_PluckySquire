#include "stdafx.h"
#include "Test3DModel.h"

CTest3DModel::CTest3DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: C3DModel(_pDevice, _pContext)
{
}

CTest3DModel::CTest3DModel(const CTest3DModel& _Prototype)
	: C3DModel(_Prototype)
{
}

HRESULT CTest3DModel::Export_Model(ofstream& _outfile)
{
	return S_OK;
}

void CTest3DModel::Get_TextureNames(set<wstring>& _outTextureNames)
{

}


CTest3DModel* CTest3DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CTest3DModel* pInstance = new CTest3DModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : Test3DModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTest3DModel::Clone(void* _pArg)
{
	CTest3DModel* pInstance = new CTest3DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Test3DModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTest3DModel::Free()
{
	__super::Free();
}
