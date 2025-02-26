#include "Light_Manager.h"

#include "GameInstance.h"
#include "Light.h"

BEGIN(Engine)
NLOHMANN_JSON_SERIALIZE_ENUM(LIGHT_TYPE, {
{LIGHT_TYPE::POINT, "POINT"},
{LIGHT_TYPE::DIRECTOINAL, "DIRECTOINAL"},

	})
END

CLight_Manager::CLight_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}


const CONST_LIGHT* CLight_Manager::Get_LightDesc_Ptr(_uint _iIndex) const
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < _iIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc_Ptr();
}

HRESULT CLight_Manager::Initialize()
{

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const CONST_LIGHT& LightDesc, LIGHT_TYPE _eType)
{
	CLight* pLight = CLight::Create(m_pDevice, m_pContext, LightDesc, _eType);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

void CLight_Manager::Update(_float _fTimeDelta)
{

}

HRESULT CLight_Manager::Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		pLight->Render(_pShader, _pVIBuffer);
	}

	return S_OK;
}

HRESULT CLight_Manager::Load_Lights(const _wstring& _strLightsJsonPath)
{
	// 1. 현재 존재하는 Lights를 전부 제거
	Clear();

	// 2. Json File을 읽어와 데이터 로드
	const std::string filePathLights = m_pGameInstance->WStringToString(_strLightsJsonPath);
	std::ifstream inputFile(filePathLights);
	if (!inputFile.is_open()) {
		throw std::runtime_error("파일을 열 수 없습니다: " + filePathLights);
		return E_FAIL;
	}
	json jsonLights;
	inputFile >> jsonLights;

	_uint iNumLights = (_uint)jsonLights.size();
	for (_uint i = 0; i < iNumLights; ++i)
	{
		LIGHT_TYPE eType = jsonLights[i]["eType"];
		CONST_LIGHT tConstLightData = {};
		tConstLightData.fFallOutEnd = jsonLights[i]["fFallOutEnd"];
		tConstLightData.fFallOutStart = jsonLights[i]["fFallOutStart"];
		tConstLightData.fSpotPower = jsonLights[i]["fSpotPower"];
		
		tConstLightData.vAmbient.x = jsonLights[i]["vAmbient"]["r"];
		tConstLightData.vAmbient.y = jsonLights[i]["vAmbient"]["g"];
		tConstLightData.vAmbient.z = jsonLights[i]["vAmbient"]["b"];
		tConstLightData.vAmbient.w = jsonLights[i]["vAmbient"]["a"];

		tConstLightData.vDiffuse.x = jsonLights[i]["vDiffuse"]["r"];
		tConstLightData.vDiffuse.y = jsonLights[i]["vDiffuse"]["g"];
		tConstLightData.vDiffuse.z = jsonLights[i]["vDiffuse"]["b"];
		tConstLightData.vDiffuse.w = jsonLights[i]["vDiffuse"]["a"];

		tConstLightData.vSpecular.x = jsonLights[i]["vSpecular"]["r"];
		tConstLightData.vSpecular.y = jsonLights[i]["vSpecular"]["g"];
		tConstLightData.vSpecular.z = jsonLights[i]["vSpecular"]["b"];
		tConstLightData.vSpecular.w = jsonLights[i]["vSpecular"]["a"];

		tConstLightData.vDirection.x = jsonLights[i]["vDirection"]["x"];
		tConstLightData.vDirection.y = jsonLights[i]["vDirection"]["y"];
		tConstLightData.vDirection.z = jsonLights[i]["vDirection"]["z"];

		tConstLightData.vPosition.x = jsonLights[i]["vPosition"]["x"];
		tConstLightData.vPosition.y = jsonLights[i]["vPosition"]["y"];
		tConstLightData.vPosition.z = jsonLights[i]["vPosition"]["z"];

		tConstLightData.vRadiance.x = jsonLights[i]["vRadiance"]["x"];
		tConstLightData.vRadiance.y = jsonLights[i]["vRadiance"]["y"];
		tConstLightData.vRadiance.z = jsonLights[i]["vRadiance"]["z"];

		if (FAILED(Add_Light(tConstLightData, eType)))
			return E_FAIL;
	}

	inputFile.close();

	return S_OK;
}

HRESULT CLight_Manager::Delete_Light(_uint _iLightIndex)
{
	if (true == m_Lights.empty())
		return E_FAIL;

	if (m_Lights.size() <= _iLightIndex)
		return E_FAIL;

	auto iter = m_Lights.begin();

	for (_uint i = 0; i < _iLightIndex; ++i)
		++iter;

	Safe_Release((*iter));
	m_Lights.erase(iter);

	return S_OK;
}

void CLight_Manager::Level_Exit()
{
	Clear();
}

void CLight_Manager::Clear()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

}

CLight_Manager* CLight_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Manager* pInstance = new CLight_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLight_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight_Manager::Free()
{

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	__super::Free();
}

