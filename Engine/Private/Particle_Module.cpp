#include "Particle_Module.h"


CParticle_Module::CParticle_Module()
{
}

HRESULT CParticle_Module::Initialize(const json& _jsonModuleInfo)
{
	if (false == _jsonModuleInfo.contains("Type"))
		return E_FAIL;

	m_eModuleType = _jsonModuleInfo["Type"];

	if (false == _jsonModuleInfo.contains("Order"))
		return E_FAIL;
	m_iOrder = _jsonModuleInfo["Order"];

	if (false == _jsonModuleInfo.contains("Init"))
		return E_FAIL;
	m_isInit = _jsonModuleInfo["Init"];


	switch (m_eModuleType)
	{
	case MODULENONE:
		break;
	case INIT_VELOCITY:
	{
		m_eApplyData = VELOCITY;

		if (false == _jsonModuleInfo.contains("Mode"))
			return E_FAIL;
		m_eDataType = _jsonModuleInfo["Mode"];
		
		if (POINT == m_eDataType)
		{
			if (false == _jsonModuleInfo.contains("Amount"))
				return E_FAIL;
			m_FloatDatas.emplace("Amount", _jsonModuleInfo["Amount"]);

			if (false == _jsonModuleInfo.contains("Origin"))
				return E_FAIL;
			if (FAILED(Add_Float3("Origin", _jsonModuleInfo)))
				return E_FAIL;			
		}


		break;
	}
	default:
		break;
	}


    return S_OK;
}

void CParticle_Module::Init_Data(_float4* _pPosition, _float3* _pVelocity, _float3* _pForce, _float4* _pColor)
{
	switch (m_eModuleType)
	{
	case INIT_VELOCITY:
		if (POINT == m_eDataType)
		{
			XMStoreFloat3(_pVelocity, XMLoadFloat4(_pPosition) - XMLoadFloat3(&m_Float3Datas["Origin"]) * m_FloatDatas["Amount"]);
		}
		break;
	}
}





HRESULT CParticle_Module::Add_Float3(const _char* _szTag, const json& _jsonInfo)
{
	if (3 > _jsonInfo[_szTag].size())
		return E_FAIL;
	
	_float3 vAdd;
	for (_int i = 0; i < 3; ++i)
		*((_float*)(&vAdd) + i) = _jsonInfo[_szTag][i];

	m_Float3Datas.emplace(_szTag, vAdd);



	return S_OK;
}

CParticle_Module* CParticle_Module::Create(const json& _jsonModuleInfo)
{
	CParticle_Module* pInstance = new CParticle_Module();

	if (FAILED(pInstance->Initialize(_jsonModuleInfo)))
	{
		MSG_BOX("Failed to Created : CParticle_Module");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Module::Free()
{
	m_Float3Datas.clear();
	m_FloatDatas.clear();

    __super::Free();

}
