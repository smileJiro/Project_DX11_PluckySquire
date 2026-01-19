#include "Light_Manager.h"

#include "GameInstance.h"


BEGIN(Engine)
NLOHMANN_JSON_SERIALIZE_ENUM(LIGHT_TYPE, {
{LIGHT_TYPE::POINT, "POINT"},
{LIGHT_TYPE::DIRECTOINAL, "DIRECTOINAL"},
{LIGHT_TYPE::SPOT, "SPOT"},
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

HRESULT CLight_Manager::Add_Light_Target(const CONST_LIGHT& LightDesc, LIGHT_TYPE _eType, CGameObject* _pGameObject, const _float3& _vOffsetPosition, CLight_Target** _ppOut, _bool _isNotClear)
{
	CLight_Target* pLight_Target = CLight_Target::Create(m_pDevice, m_pContext, LightDesc, _eType, _pGameObject, _vOffsetPosition, _isNotClear);

	if (nullptr == pLight_Target)
		return E_FAIL;

	m_Lights.push_back(pLight_Target);

	if (nullptr != _ppOut)
		*_ppOut = pLight_Target;

	return S_OK;
}

void CLight_Manager::Update(_float _fTimeDelta)
{
	for (auto& pLight : m_Lights)
	{
		if(true == pLight->Is_Active())
			pLight->Update(_fTimeDelta);
	}
}

HRESULT CLight_Manager::Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		if (true == pLight->Is_Active())
			pLight->Render_Light(_pShader, _pVIBuffer);
	}

	return S_OK;
}

HRESULT CLight_Manager::Load_Lights(const _wstring& _strLightsJsonPath)
{
	// 1. 현재 존재하는 Lights를 전부 제거// 2. 셰도우 라이트가있다면 그것도 제거해야한다.
	Clear_Load();
	

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

		if (jsonLights[i].contains("isShadow") && eType != LIGHT_TYPE::POINT)
		{
			tConstLightData.isShadow = jsonLights[i]["isShadow"];
		}
		else
		{
			tConstLightData.isShadow = false;
		}

		if (jsonLights[i].contains("fShadowFactor") && eType != LIGHT_TYPE::POINT)
		{
			tConstLightData.fShadowFactor = jsonLights[i]["fShadowFactor"];
		}
		else
		{
			tConstLightData.fShadowFactor = 1.0f;
		}

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

HRESULT CLight_Manager::Delete_Light(list<CLight*>::iterator& _iter)
{
	if (true == m_Lights.empty())
		return E_FAIL;

	if (_iter == m_Lights.end())
		return E_FAIL;

	_iter = m_Lights.erase(_iter);

	return S_OK;
}

void CLight_Manager::Level_Exit()
{
	Clear();
}

void CLight_Manager::Clear_Load()
{

	vector<CLight*> NotClearLights;

	for (auto& pLight : m_Lights)
	{
		if (true == pLight->Is_NotClear())
		{
			NotClearLights.push_back(pLight);
			continue;
		}

		CONST_LIGHT tLightDesc = pLight->Get_LightDesc();
		if (true == (_bool)tLightDesc.isShadow)
		{
			_int iLightID = pLight->Get_LightID();
			m_pGameInstance->Remove_ShadowLight(iLightID);
		}
		Safe_Release(pLight);
	}
	m_Lights.clear();

	/* 라이트 로드종료 후, */
	for (_uint i = 0; i < NotClearLights.size(); ++i)
	{
		m_Lights.push_back(NotClearLights[i]);
	}
	NotClearLights.clear();

}

void CLight_Manager::Clear()
{
	for (auto& pLight : m_Lights)
	{
		CONST_LIGHT tLightDesc = pLight->Get_LightDesc();
		if (true == (_bool)tLightDesc.isShadow)
		{
			_int iLightID = pLight->Get_LightID();(_bool)
			m_pGameInstance->Remove_ShadowLight(iLightID);
		}
		Safe_Release(pLight);
	}



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

