#include "Model.h"

CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const CModel& _Prototype)
	:CComponent(_Prototype)
	, m_eModelType{ _Prototype.m_eModelType }
{
}

void CModel::Free()
{
	__super::Free();
}
