#include "stdafx.h"
#include "MapParsing_Manager.h"
#include "GameInstance.h"
#include "MapObject.h"
#include "CriticalSectionGuard.h"


_uint APIENTRY ParsingMain(void* pArg)
{
	CMapParsing_Manager* pManager = static_cast<CMapParsing_Manager*>(pArg);

	if (FAILED(pManager->Parsing()))
		return 1;

	return 0;
}


HRESULT CMapParsing_Manager::Update()
{
	if (m_isLoading && m_isLoadComp)
	{
		DeleteObject(m_hThread);
		DeleteCriticalSection(&m_Critical_Section);
		m_isLoading = false;
	}
	return S_OK;
}


CMapParsing_Manager::CMapParsing_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice),
	m_pContext(_pContext),
	m_pGameInstance(Engine::CGameInstance::GetInstance())

{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMapParsing_Manager::Initialize(CImguiLogger* _pLogger)
{
	Safe_AddRef(m_pLogger);
	m_pLogger = _pLogger;
	return S_OK;
}

HRESULT CMapParsing_Manager::Open_ParsingDialog()
{
	return S_OK;
}
HRESULT CMapParsing_Manager::Parsing(json _jsonObj)
{
	if (_jsonObj.is_array())
	{
		string arrAxisKey[3] = {"X","Z","Y"};
		string arrRotateKey[3] = {"Roll","Yaw","Pitch"};

		_int iNumDialogs = (_int)_jsonObj.size();
		for (auto jsonObj : _jsonObj.items())
		{
			if (jsonObj.value().is_object())
			{
				if (jsonObj.value()["Class"].is_string())
				{
					string strComponentKey = jsonObj.value()["Class"];
					if (strComponentKey == "UScriptClass'StaticMeshComponent'")
					{
						auto test = jsonObj.value()["Properties"];
						if (!jsonObj.value()["Properties"]["StaticMesh"].is_null()
							&&
							!jsonObj.value()["Properties"]["StaticMesh"]["ObjectName"].is_null()
							)
						{
							string strModelKey = jsonObj.value()["Properties"]["StaticMesh"].at("ObjectName");

							strModelKey = strModelKey.substr(strModelKey.find("'") + 1 , strModelKey.size() - strModelKey.find("'") -2);
							
							MAP_DATA tMapData = {};
							ZeroMemory(&tMapData, sizeof tMapData);
							
							if (!jsonObj.value()["Properties"]["RelativeLocation"].is_null())
							{
								for (_uint i = 0; i < 3; i++)
								{
									_float fValue = jsonObj.value()["Properties"]["RelativeLocation"].at(arrAxisKey[i]);
									if (i == 0)
									{
										fValue *= -1.f;
									}

									fValue /= 150.f;
									memcpy(((&tMapData.fPos.x) + i), &fValue, sizeof(_float));
								}
							}

							if (!jsonObj.value()["Properties"]["RelativeRotation"].is_null())
							{
								for (_uint i = 0; i < 3; i++)
								{
									_float fValue = jsonObj.value()["Properties"]["RelativeRotation"].at(arrRotateKey[i]);
									if (i == 0)
									{
										fValue *= -1.f;
									}

									fValue = XMConvertToRadians(fValue);
									memcpy(((&tMapData.fRotate.x) + i), &fValue, sizeof(_float));
								}
							}

							if (!jsonObj.value()["Properties"]["RelativeScale3D"].is_null())
							{
								for (_uint i = 0; i < 3; i++)
								{
									_float fValue = jsonObj.value()["Properties"]["RelativeScale3D"].at(arrAxisKey[i]);

									memcpy(((&tMapData.fScale.x) + i), &fValue, sizeof(_float));
								}
							}
							else 
							{
								tMapData.fScale = { 1.f,1.f,1.f };
							}

							
							m_Models.push_back(make_pair(strModelKey, tMapData));
						}
					}
				}
			}
		
		}
	}
	return S_OK;

}

HRESULT CMapParsing_Manager::Parsing()
{

	CCriticalSectionGuard csGuard(&m_Critical_Section);


	LOG_TYPE("Model Parsing Start - [ " + m_strParsingName + " ]", LOG_LOAD);

	///* Read json Standard Stat Data */
	const std::string filePathDialog = m_strParsingName;
	std::ifstream inputFile(filePathDialog);
	if (!inputFile.is_open()) {
		throw std::runtime_error("json Error :  " + filePathDialog);
		return E_FAIL;
	}

	json jsonDialogs;
	inputFile >> jsonDialogs;
	if (jsonDialogs.is_array())
	{
		Parsing(jsonDialogs);
	}

	inputFile.close();

	string strLog = std::to_string(m_Models.size());

	LOG_TYPE("Model Parsing End - [ count : " + strLog + " ]", LOG_LOAD);
	LOG_TYPE("Model Create Start", LOG_LOAD);
	_uint iCompCnt = 0;
	_uint iFailedCnt = 0;
	for (auto pair : m_Models)
	{
		CMapObject::MAPOBJ_DESC NormalDesc = {};

		NormalDesc.eCreateType = CMapObject::OBJ_CREATE;
		lstrcpy(NormalDesc.szModelName, m_pGameInstance->StringToWString(pair.first).c_str());

		CGameObject* pGameObject = nullptr;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_MAP, TEXT("Prototype_GameObject_MapObject"),
			LEVEL_TOOL_MAP, L"Layer_GameObject", &pGameObject, (void*)&NormalDesc);
		if (pGameObject)
		{
			iCompCnt++;
			pGameObject->Get_ControllerTransform()->RotationXYZ(pair.second.fRotate);
			pGameObject->Set_Position(XMLoadFloat3(&pair.second.fPos));
			pGameObject->Set_Scale(pair.second.fScale.x, pair.second.fScale.y, pair.second.fScale.z);
			static_cast<CMapObject*>(pGameObject)->Create_Complete();
		}
		else
		{
			LOG_TYPE("Model Parsing Failed - [ " + pair.first +" ]", LOG_ERROR);
			iFailedCnt++;
		}
	}

	LOG_TYPE("Model Create End - [Complete : " + std::to_string(iCompCnt)  +", Failed : "+ std::to_string(iFailedCnt) +" ]", LOG_ERROR);
	
	CoUninitialize();
	m_isLoadComp = true;
	return S_OK;
}



void CMapParsing_Manager::Open_Parsing(const string& _strParsingFileName)
{
	m_strParsingName = _strParsingFileName;
	m_isLoading = true;
	m_isLoadComp = false;
	InitializeCriticalSection(&m_Critical_Section);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ParsingMain, this, 0, nullptr);
}


CMapParsing_Manager* CMapParsing_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	CMapParsing_Manager* pInstance = new CMapParsing_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : CMapParsing_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapParsing_Manager::Free()
{
	if (m_isLoading)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		DeleteObject(m_hThread);
		DeleteCriticalSection(&m_Critical_Section);
	}

	Safe_Release(m_pLogger);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	
	__super::Free();
}
