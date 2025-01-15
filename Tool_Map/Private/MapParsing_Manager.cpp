#include "stdafx.h"
#include "MapParsing_Manager.h"
#include "GameInstance.h"
#include "MapObject.h"


CMapParsing_Manager::CMapParsing_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice),
	m_pContext(_pContext),
	m_pGameInstance(Engine::CGameInstance::GetInstance())

{
}

HRESULT CMapParsing_Manager::Initialize(CImguiLogger* _pLogger)
{
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
		string arrRotateKey[3] = {"Roll","Pitch","Yaw"};

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

									fValue /= 150.f;
									memcpy(((&tMapData.fPos.x) + i), &fValue, sizeof(_float));
								}
							}

							if (!jsonObj.value()["Properties"]["RelativeRotation"].is_null())
							{
								for (_uint i = 0; i < 3; i++)
								{
									_float fValue = jsonObj.value()["Properties"]["RelativeRotation"].at(arrRotateKey[i]);
									if (arrRotateKey[i] == "Yaw")
										fValue *= -1.f;
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

HRESULT CMapParsing_Manager::Parsing(const string& _strParsingName)
{

	///* Read json Standard Stat Data */
	const std::string filePathDialog = _strParsingName;
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
		//for_each(jsonDialogs.array().begin(), jsonDialogs.array().end(), [](json jsonObj) {;
		//    
		//    });
		//
		//_int iNumDialogs = jsonDialogs.array().size();
		//for (_int i = 0; i < iNumDialogs; ++i)
		//{
		//    jsonDialogs.array().
		//    int a = 1;
		//}

	}

	inputFile.close();


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
			//pGameObject->Get_ControllerTransform()->RotationXYZ(pair.second.fRotate);
			pGameObject->Set_Position(XMLoadFloat3(&pair.second.fPos));
			pGameObject->Set_Scale(pair.second.fScale.x, pair.second.fScale.y, pair.second.fScale.z);
		}
		else
		{
			LOG_TYPE("Model Parsing Faile - [ " + pair.first +" ]", LOG_ERROR);
		}
	}



	int a = 1;

	return S_OK;

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
}
