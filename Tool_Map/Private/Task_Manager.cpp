#include "stdafx.h"
#include "Task_Manager.h"
#include "GameInstance.h"
#include "MapObject.h"
#include "Layer.h"
#include "CriticalSectionGuard.h"
#include "Model_PackagingObject.h"
#include <commdlg.h>


_uint APIENTRY ParsingMain(void* pArg)
{
	CTask_Manager* pManager = static_cast<CTask_Manager*>(pArg);

	if (FAILED(pManager->Parsing()))
		return 1;

	return 0;
}

_uint APIENTRY RePackagingMain(void* pArg)
{
	CTask_Manager* pManager = static_cast<CTask_Manager*>(pArg);

	if (FAILED(pManager->RePackaging()))
		return 1;

	return 0;
}


HRESULT CTask_Manager::Update()
{
	if (m_isLoading)
	{
		if (m_isLoadComp)
		{
			DeleteObject(m_hThread);
			DeleteCriticalSection(&m_Critical_Section);
			m_isLoading = false;
		}
		else
			return S_OK;
	}
	if (!m_LoadTasks.empty())
	{
		switch (m_LoadTasks.front().eType)
		{
		case Map_Tool::CTask_Manager::MAPDATA_PARSING:
			Open_Parsing();
			break;
		case Map_Tool::CTask_Manager::MODEL_RE_PACKAGING:
			Open_RePackaging();
			break;
		default:
			break;
		}
	}
	return S_OK;
}


CTask_Manager::CTask_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice),
	m_pContext(_pContext),
	m_pGameInstance(Engine::CGameInstance::GetInstance())

{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTask_Manager::Initialize(CImguiLogger* _pLogger)
{
	m_EgnoreObjectNames.push_back("Cube");
	Safe_AddRef(m_pLogger);
	m_pLogger = _pLogger;
	return S_OK;
}

HRESULT CTask_Manager::Open_ParsingDialog(const wstring& _strLayerName)
{
	_tchar originalDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalDir);

	OPENFILENAME ofn = {};
	_tchar szName[MAX_PATH] = {};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szName;
	ofn.nMaxFile = sizeof(szName);
	ofn.lpstrFilter = L".json\0*.json\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	wstring strPath = m_strUmapJsonPath;
	ofn.lpstrInitialDir = strPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		SetCurrentDirectory(originalDir);
		const _string strFilePath = m_pGameInstance->WStringToString(szName);
		Register_Parsing(strFilePath, _strLayerName);
		Parsing();
	}

	return S_OK;
}

HRESULT CTask_Manager::Parsing(json _jsonObj)
{
	if (_jsonObj.is_array())
	{
		string arrAxisKey[3] = { "X","Z","Y" };
		string arrRotateKey[3] = { "Roll","Yaw","Pitch" };

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
						if (!jsonObj.value()["Properties"]["StaticMesh"].is_null()
							&&
							!jsonObj.value()["Properties"]["StaticMesh"]["ObjectName"].is_null()
							)
						{
							// 유효성 체크 END



							string strModelKey = jsonObj.value()["Properties"]["StaticMesh"].at("ObjectName");
							strModelKey = strModelKey.substr(strModelKey.find("'") + 1, strModelKey.size() - strModelKey.find("'") - 2);



							// 필요없는 오브젝트 거르기
							if (Check_EgnoreObject(strModelKey))
								continue;

							MAP_DATA tMapData = {};
							ZeroMemory(&tMapData, sizeof tMapData);

							// 위치 정보가 있는가?
							if (!jsonObj.value()["Properties"]["RelativeLocation"].is_null())
							{
								for (_uint i = 0; i < 3; i++)
								{
									_float fValue = jsonObj.value()["Properties"]["RelativeLocation"].at(arrAxisKey[i]);
									if (i == 2)
										fValue *= -1.f;

									fValue /= 150.f;
									memcpy(((&tMapData.fPos.x) + i), &fValue, sizeof(_float));
								}
							}

							// 회전 정보가 있는가?
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

							// 크기 정보가 있는가?
							if (!jsonObj.value()["Properties"]["RelativeScale3D"].is_null())
							{
								for (_uint i = 0; i < 3; i++)
								{
									_float fValue = jsonObj.value()["Properties"]["RelativeScale3D"].at(arrAxisKey[i]);

									memcpy(((&tMapData.fScale.x) + i), &fValue, sizeof(_float));
								}
							}
							else
								tMapData.fScale = { 1.f,1.f,1.f };

							// override material 정보가 있는가?
							if (!jsonObj.value()["Properties"]["OverrideMaterials"].is_null())
							{
								if (jsonObj.value()["Properties"]["OverrideMaterials"].is_array())
								{
									auto obj = jsonObj.value()["Properties"]["OverrideMaterials"].begin();
									if (obj.value().contains("ObjectName"))
									{
										string strMaterialKey = obj.value().at("ObjectName");
										strMaterialKey = strMaterialKey.substr(strMaterialKey.find("'") + 1, strMaterialKey.size() - strMaterialKey.find("'") - 2);
										strcpy_s(tMapData.szMaterialKey, strMaterialKey.c_str());
										tMapData.isOverrideMaterial = true;
									}
								}
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


HRESULT CTask_Manager::Parsing()
{
	m_Models.clear();
	m_MapObjectNames.clear();
	m_RepackNeededModels.clear();
	string strFileFullPath = m_LoadTasks.front().strFilePath;
	string strFileName = strFileFullPath.substr(strFileFullPath.rfind("/") + 1, strFileFullPath.size() - strFileFullPath.rfind("/") - 1);
	string strFilePath = strFileFullPath.substr(0, strFileFullPath.rfind("/"));

	wstring strLayerTag = m_LoadTasks.front().strLayerName;

	if (m_LoadTasks.front().isLayerClear)
	{
		CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_3D_MAP, strLayerTag);

		if (pLayer)
		{
			LOG_TYPE("Layer Clear - " + m_pGameInstance->WStringToString(strLayerTag), LOG_LOAD);

			for (auto _pGameObject : pLayer->Get_GameObjects())
			{
				Event_DeleteObject(_pGameObject);
			}
		}

	}

	m_LoadTasks.pop();
	//CCriticalSectionGuard csGuard(&m_Critical_Section);


	LOG_TYPE("Model Parsing Start - [ " + strFileFullPath + " ]", LOG_LOAD);

	///* Read json Standard Stat Data */
	const std::string filePathDialog = strFileFullPath;
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

	vector<pair<_string, MAP_DATA>> OverrideMaterialModels;
	LOG_TYPE("Model Create Start", LOG_LOAD);
	_uint iCompCnt = 0;
	_uint iFailedCnt = 0;

	_wstring strMaterialPath = L"../Bin/json/Material/";

	for (auto pair : m_Models)
	{

		CMapObject::MAPOBJ_DESC NormalDesc = {};

		NormalDesc.eCreateType = CMapObject::OBJ_CREATE;
		lstrcpy(NormalDesc.szModelName, m_pGameInstance->StringToWString(pair.first).c_str());
		NormalDesc.iCurLevelID = LEVEL_TOOL_3D_MAP;
		NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TOOL_3D_MAP;
		CGameObject* pGameObject = nullptr;
		m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
			LEVEL_TOOL_3D_MAP, strLayerTag, &pGameObject, (void*)&NormalDesc);
		if (pGameObject)
		{
			iCompCnt++;
			pGameObject->Get_ControllerTransform()->RotationXYZ(pair.second.fRotate);
			pGameObject->Set_Position(XMLoadFloat3(&pair.second.fPos));
			pGameObject->Set_Scale(pair.second.fScale.x, pair.second.fScale.y, pair.second.fScale.z);
			CMapObject* pMapObject = static_cast<CMapObject*>(pGameObject);
			pMapObject->Create_Complete();
			_bool isStatic_override = false;
			if (ContainString(pair.first,"ToyBrick_"))
				isStatic_override = true;
			if (isStatic_override || pair.second.isOverrideMaterial)
			{
				_string strMaterialKey = pair.second.szMaterialKey;
				// override material 목록 출력용 벡터 삽입

				_wstring strModelName = pMapObject->Get_ModelName();
				if (strMaterialKey != "" || !isStatic_override)
					OverrideMaterialModels.push_back(pair);
				else
					strMaterialKey = "MI_ToyBricks_6";

				//TODO :: 페이퍼클립의 경우, 마테리얼에 색상정보가 없어서!!!!!!
				// 일단 파일이름 보고 걍 대충 디폴트색상만 지정해둠... .orz
				if (ContainString(WSTRINGTOSTRING(strModelName), "paperclip_01"))
				{
					_float4 fColor = {};

					if (ContainString(strMaterialKey, "blue"))
					{
						fColor = _float4(0.608f, 0.796f, 0.906f, 1.f);

					}
					else if (ContainString(strMaterialKey, "green"))
					{
						fColor = _float4(0.663f, 0.922f, 0.647f, 1.f);
					}
					else if (ContainString(strMaterialKey, "red"))
					{
						fColor = _float4(0.949f, 0.063f, 0.561f, 1.f);
					}
					else if (ContainString(strMaterialKey, "metal"))
					{
						fColor = _float4(0.682f, 0.663f, 0.675f, 1.f);
					}
					else if (ContainString(strMaterialKey, "yellow"))
					{
						fColor = _float4(0.961f, 0.773f, 0.369f, 1.f);
					}
					if (fColor.x != 0.f)
					{
						pMapObject->Set_Color_Shader_Mode(0,C3DModel::COLOR_DEFAULT);
						pMapObject->Set_Diffuse_Color(0,fColor);
					}

					continue;
				}

				std::string filePathDialog = WstringToString(strMaterialPath);
				if (strMaterialKey == "M_WC_09")
				{
					int a = 1;
				}
				filePathDialog += strMaterialKey;
				filePathDialog += ".json";

				std::ifstream inputFile(filePathDialog);
				if (!inputFile.is_open())
				{
					LOG_TYPE("json file not exist-> " + filePathDialog, LOG_ERROR);

				}
				else
				{
					json jsonDialogs;
					inputFile >> jsonDialogs;

					// 1차.
					if (FAILED(Find_Override_Material_Texture(pMapObject, jsonDialogs)))
					{
						LOG_TYPE("data not exist -> " + filePathDialog, LOG_ERROR);
					}
					if (FAILED(Find_Override_Material_Color(pMapObject, jsonDialogs)))
					{
						LOG_TYPE("data not exist -> " + filePathDialog, LOG_ERROR);
					}
					inputFile.close();
				}
			}



		}
		else
		{
			LOG_TYPE("Model Parsing Failed - [ " + pair.first + " ]", LOG_ERROR);
			iFailedCnt++;
		}
	}



	if (!m_RepackNeededModels.empty())
	{
		for (auto& PackPair : m_RepackNeededModels)
		{
			_wstring strFilePath = STATIC_3D_MODEL_FILE_PATH;
			strFilePath += L"NonAnim/";
			strFilePath += PackPair.first;
			strFilePath += L"/";
			strFilePath += PackPair.first;
			strFilePath += L".model";
			Register_RePackaging(strFilePath, PackPair.second);
		}
	}


	string strLogging = "Model Create End - [Complete : " + std::to_string(iCompCnt) + ", Failed : " + std::to_string(iFailedCnt) + " ]";

	LOG_TYPE(strLogging, LOG_ERROR);

	vector<_string> ExportStrings;

	for (auto ModelInfoPair : OverrideMaterialModels)
	{
		_string strModelName = ModelInfoPair.first;
		_string strModelTextureName = ModelInfoPair.second.szMaterialKey;
		ExportStrings.push_back(strModelName + " - " + strModelTextureName);
	}


	wstring strResultFileFath = L"../Bin/json/Result/";
	
	auto filrPair = Get_FileName_From_Path(StringToWstring(strFileName));

	if (FAILED(Export_Result(strResultFileFath, filrPair.first, ExportStrings)))
		LOG_TYPE("Export Result Save Error... ", LOG_ERROR);


	//CoUninitialize();
	m_isLoadComp = true;

	return S_OK;
}

HRESULT CTask_Manager::Find_Override_Material_Texture(CMapObject* _pMapObject, json _jsonObj)
{
	//_uint iTextureType[4];
	const _string strTextureTag = "ReferencedTextures";
	if (_jsonObj.is_object())
	{
		if (_jsonObj.contains(strTextureTag))
		{
			LOG_TYPE("Material Texture On", LOG_SAVE);

			if (_jsonObj[strTextureTag].is_array())
			{
				vector<_string> vecNames;
				for (const auto& item : _jsonObj[strTextureTag])
				{
					if (item.is_object() == item.contains("ObjectName"))
					{
						vecNames.push_back(item["ObjectName"]);
					}
				}


				for (_string strTextureFullName : vecNames)
				{
					_int iTexIdx = -1;

					// 지우고, 
					_string strTextureName =
						strTextureFullName.substr(strTextureFullName.find("'") + 1, strTextureFullName.size() - strTextureFullName.find("'") - 2);
					// 내리고
					_string strCheckTextureName = strTextureFullName;

					std::transform(strCheckTextureName.begin(), strCheckTextureName.end(), strCheckTextureName.begin(), [](unsigned char c) {
						return std::tolower(c);
						});

					if (ContainString(strCheckTextureName, "basecolor") ||
						ContainString(strCheckTextureName, "albedo'") ||
						EndString(strCheckTextureName, "_d\'")
						)
					{
						LOG_TYPE("diffuse Contect", LOG_SAVE);
						iTexIdx = aiTextureType_DIFFUSE;
					}
					else if (ContainString(strCheckTextureName, "normal") || EndString(strCheckTextureName, "_n'"))
					{
						LOG_TYPE("normal Contect", LOG_SAVE);
						iTexIdx = aiTextureType_NORMALS;

					}
					else if (ContainString(strCheckTextureName, "occlusionroughnessmetallic") || ContainString(strCheckTextureName, "orm"))
					{
						LOG_TYPE("ORM Contect", LOG_SAVE);
						iTexIdx = aiTextureType_BASE_COLOR;
					}
					else if (ContainString(strCheckTextureName, "_a"))
					{
						LOG_TYPE("Ab Contect", LOG_SAVE);
						iTexIdx = aiTextureType_AMBIENT;
					}


					if (-1 != iTexIdx && string::npos != strCheckTextureName.find("'"))
					{


						if (SUCCEEDED(_pMapObject->Push_Texture(strTextureName, iTexIdx)))
						{
							_wstring strModelName = _pMapObject->Get_ModelName();

							auto iter = find_if(m_RepackNeededModels.begin(), m_RepackNeededModels.end(),
								[&strModelName](const pair<_wstring, CMapObject*> RepackNeededModelPair)->_bool {

									return RepackNeededModelPair.first == strModelName;
								});

							if (iter == m_RepackNeededModels.end())
							{
								m_RepackNeededModels.push_back(make_pair(strModelName, _pMapObject));
							}
						}
					}

				}
			}
			return S_OK;
		}
		else
			for (const auto& [key, value] : _jsonObj.items())
				if (SUCCEEDED(Find_Override_Material_Texture(_pMapObject, value)))
					return S_OK;
	}
	else if (_jsonObj.is_array())
		for (const auto& item : _jsonObj)
			if (SUCCEEDED(Find_Override_Material_Texture(_pMapObject, item)))
				return S_OK;

	return S_OK;

}

HRESULT CTask_Manager::Find_Override_Material_Color(CMapObject* _pMapObject, json _jsonObj)
{
	string arrColorKey[4] = { "R","G","B","A" };
	const _string strColorTag = "VectorParameterValues";
	/*if (_jsonObj.is_array())
	{
		if (_jsonObj.contains("Properties") || _jsonObj.contains(strColorTag))
		{
			if (_jsonObj.contains(strColorTag))
			{
				LOG_TYPE("Material On", LOG_SAVE);

				if (_jsonObj[strColorTag].is_array())
				{
					vector<_string> vecNames;
					for (const auto& item : _jsonObj[strColorTag])
					{
						if (item.is_object() == item.contains("ParameterInfo"))
						{
							if (item["ParameterInfo"]["Name"] == "BaseColour_Vector3")
							{

								if (item["ParameterInfo"].contains("ParameterValue"))
								{
									LOG_TYPE("Material Override Color Contect! ", LOG_SAVE);
									auto ParameterValue = item["ParameterInfo"]["ParameterValue"];
									_float4 fColor = {};
									for (_uint i = 0; i < 4; i++)
									{
										_float fValue = ParameterValue[arrColorKey[i]];

										fValue /= 150.f;
										memcpy(((&fColor.x) + i), &fValue, sizeof(_float));
									}
									_pMapObject->Set_Diffuse_Color(fColor);
									_pMapObject->Set_Color_Shader_Mode(CMapObject::MIX_DIFFUSE);
								}
							}
						}
					}
					return S_OK;
				}
			}
			else
			for (const auto& [key, value] : _jsonObj.items())
				if (SUCCEEDED(Find_Override_Material_Color(_pMapObject, value)))
					return S_OK;
		}
		else
			for (const auto& [key, value] : _jsonObj.items())
				if (SUCCEEDED(Find_Override_Material_Color(_pMapObject, value)))
					return S_OK;
	}
	else if (_jsonObj.is_array())
		for (const auto& item : _jsonObj)
			if (SUCCEEDED(Find_Override_Material_Color(_pMapObject, item)))
				return S_OK;*/

	if (_jsonObj.is_object()) 
	{
		if (_jsonObj.contains(strColorTag)) 
		{
			const auto& vectorParams = _jsonObj[strColorTag];
			if (vectorParams.is_array()) 
			{
				for (const auto& param : vectorParams) 
				{
					if (param.contains("ParameterInfo") && param["ParameterInfo"].contains("Name")) 
					{
						std::string name = param["ParameterInfo"]["Name"];
						if (name == "BaseColour_Vector3" && param.contains("ParameterValue")) 
						{
							LOG_TYPE("Material Override Color Contect! ", LOG_SAVE);
							auto ParameterValue = param["ParameterValue"];
							_float4 fColor = {};
							for (_uint i = 0; i < 4; i++)
							{
								_float fValue = ParameterValue[arrColorKey[i]];

								memcpy(((&fColor.x) + i), &fValue, sizeof(_float));
							}
							_pMapObject->Set_Color_Shader_Mode(0,C3DModel::COLOR_DEFAULT);
							_pMapObject->Set_Diffuse_Color(0,fColor);
						}
					}
				}
			}
		}

		for (const auto& [key, value] : _jsonObj.items()) {
			Find_Override_Material_Color(_pMapObject, value);
		}
	}
	else if (_jsonObj.is_array())
	{
		for (const auto& element : _jsonObj) {
			Find_Override_Material_Color(_pMapObject, element);
		}
	}


	return S_OK;

}

HRESULT CTask_Manager::RePackaging()
{
	CCriticalSectionGuard csGuard(&m_Critical_Section);


	_string strFilePath = m_LoadTasks.front().strFilePath;
	CMapObject* pTargetObject = m_LoadTasks.front().pTargetObject;


	LOG_TYPE("Model RePackaging Start... ", LOG_SAVE);


	CModel_PackagingObject* pObj = CModel_PackagingObject::Create(strFilePath);

	if (pObj == nullptr)
	{
		LOG_TYPE("Model Re-Packaging Fail... ", LOG_ERROR);
		LOG_TYPE("Error Type : Packaging Object Import Failed", LOG_ERROR);
		return E_FAIL;
	}

	if (FAILED(pObj->Set_Sync_Material(pTargetObject)))
	{
		LOG_TYPE("Model Re-Packaging Fail... ", LOG_ERROR);
		LOG_TYPE("Error Type : Material Sync Failed", LOG_ERROR);
		return E_FAIL;
	}

	if (FAILED(pObj->Export(strFilePath)))
	{
		LOG_TYPE("Model Re-Packaging Fail... ", LOG_ERROR);
		LOG_TYPE("Error Type : Packaging Object Export Failed", LOG_ERROR);
		return E_FAIL;
	}



	Safe_Release(pObj);


	LOG_TYPE("Model RePackaging Complete!!! \n [" + strFilePath + "]", LOG_SAVE);
	LOG_TYPE("File Fath : [" + strFilePath + "]", LOG_SAVE);



	CoUninitialize();
	m_isLoadComp = true;
	m_LoadTasks.pop();
	return S_OK;
}




HRESULT CTask_Manager::Export_SaveResult_ToJson(const _wstring _strFIlePath, const vector<_string>& _SaveModelProtoNames, _bool isStatic)
{

	wstring strResultFileFath = _strFIlePath;

	strResultFileFath = strResultFileFath.substr(0, strResultFileFath.rfind(L"."));
	strResultFileFath += L".json";


	json jAddFile;
	jAddFile["IsStatic"].push_back(isStatic);

	for (auto strModelName : _SaveModelProtoNames)
	{
		jAddFile["data"].push_back(strModelName);
	}

	std::ofstream file(strResultFileFath);
	if (file.is_open()) {
		file << jAddFile.dump(1);
		file.close();
		return S_OK;
	}
	else {
		return E_FAIL;
	}


}

HRESULT CTask_Manager::Export_Result(const _wstring& _strFilePath, const _wstring& _strFileName, vector<_string> _ExportStrings)
{
	wstring strResultFileFath = _strFilePath;
	strResultFileFath += _strFileName;
	strResultFileFath += L"_Result.json";

	json jAddFile;
	for (auto strExport : _ExportStrings)
		jAddFile["ExportData"].push_back(strExport);


	std::ofstream file(strResultFileFath);
	if (file.is_open()) {
		file << jAddFile.dump(1);
		file.close();
	}
	else {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTask_Manager::Cooking(const _string& _strCookingPath, CMapObject* _pMapObject, COOKING_MODE _eMode)
{
	PxCooking* pCooking = m_pGameInstance->Get_Cooking();
	PxPhysics* pPhysics = m_pGameInstance->Get_Physics();



	if (pCooking == nullptr || pPhysics == nullptr)
		return E_FAIL;


	PxShapeFlags ShapeFlags = PxShapeFlag::eSIMULATION_SHAPE;
	PxMaterial* pShapeMaterial = m_pGameInstance->Get_Material(ACTOR_MATERIAL::DEFAULT);


	CModel* _pModel = _pMapObject->Get_Model(COORDINATE_3D);
	if (nullptr == _pModel)
		return E_FAIL;
	C3DModel* pModel = static_cast<C3DModel*>(_pModel);

	_uint iNumMeshes = pModel->Get_NumMeshes();

	_string strCookingPath = _strCookingPath;

	switch (_eMode)
	{
	case Map_Tool::CTask_Manager::COOKING_CONVEX:
	{
		CMesh* pMesh = pModel->Get_Mesh(0);

		PxConvexMeshDesc  meshDesc;
		if (FAILED(pMesh->Cooking(meshDesc)))
			return E_FAIL;
		
		PxDefaultMemoryOutputStream writeBuffer;
		PxCookingParams params(pPhysics->getTolerancesScale());
		pCooking->setParams(params);
		
		
		if (!pCooking->cookConvexMesh(meshDesc, writeBuffer))
			return E_FAIL;

		strCookingPath += ".modelcoll";
		ofstream file;
		file.open(strCookingPath, ios::binary);
		if (!file)
			return E_FAIL;

		_uint iMeshDataSize = writeBuffer.getSize();

		file.write((_char*)&iMeshDataSize, sizeof(_uint));
		file.write((_char*)writeBuffer.getData(), iMeshDataSize);

		file.close();
	}

		break;
	case Map_Tool::CTask_Manager::COOKING_TRI:
	{

		CMesh* pMesh = pModel->Get_Mesh(0);

		PxTriangleMeshDesc meshDesc;
		if (FAILED(pMesh->Cooking(meshDesc)))
			return E_FAIL;
		PxDefaultMemoryOutputStream writeBuffer;
		PxCookingParams params(pPhysics->getTolerancesScale());
		pCooking->setParams(params);

		if (!pCooking->cookTriangleMesh(meshDesc, writeBuffer))
			return E_FAIL;

		strCookingPath = std::filesystem::path(_strCookingPath).replace_extension("").string();
		strCookingPath += ".modelTriColl";
		ofstream file;
		file.open(strCookingPath, ios::binary);
		if (!file)
			return E_FAIL;

		_uint iMeshDataSize = writeBuffer.getSize();

		file.write((_char*)&iMeshDataSize, sizeof(_uint));
		file.write((_char*)writeBuffer.getData(), iMeshDataSize);

		file.close();
	}


		break;
	case Map_Tool::CTask_Manager::COOKING_MULTI_CONVEX:

	{

#pragma region 복수메쉬

		_uint iMeshMaxSize = 0;

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			CMesh* pMesh = pModel->Get_Mesh(i);

			PxConvexMeshDesc meshDesc;
			pMesh->Cooking(meshDesc);

			PxDefaultMemoryOutputStream writeBuffer;
			PxCookingParams params(pPhysics->getTolerancesScale());
			pCooking->setParams(params);

			if (!pCooking->cookConvexMesh(meshDesc, writeBuffer))
				return E_FAIL;


			string strMultiMeshPath = std::filesystem::path(strCookingPath).replace_extension("").string();
			strMultiMeshPath += "_";
			strMultiMeshPath += std::to_string(i);
			strMultiMeshPath += ".modelMultiConvex";
			ofstream file;
			//file.open(strMultiMeshPath, ios::binary | std::ios::app);
			file.open(strMultiMeshPath, ios::binary);
			if (!file)
				return E_FAIL;

			_uint iMeshDataSize = writeBuffer.getSize();

			file.write((_char*)&iMeshDataSize, sizeof(_uint));
			file.write((_char*)writeBuffer.getData(), iMeshDataSize);

			file.close();
		}

#pragma endregion

	}
	break;
	case Map_Tool::CTask_Manager::COOKING_MULTI_TRI:

	{

#pragma region 복수메쉬

		_uint iMeshMaxSize = 0;

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			CMesh* pMesh = pModel->Get_Mesh(i);

			PxTriangleMeshDesc meshDesc;
			if (FAILED(pMesh->Cooking(meshDesc)))
				return E_FAIL;
			PxDefaultMemoryOutputStream writeBuffer;
			PxCookingParams params(pPhysics->getTolerancesScale());
			pCooking->setParams(params);

			if (!pCooking->cookTriangleMesh(meshDesc, writeBuffer))
				return E_FAIL;


			string strMultiMeshPath = std::filesystem::path(strCookingPath).replace_extension("").string();
			strMultiMeshPath += "_";
			strMultiMeshPath += std::to_string(i);
			strMultiMeshPath += ".modelMultiTri";
			ofstream file;
			//file.open(strMultiMeshPath, ios::binary | std::ios::app);
			file.open(strMultiMeshPath, ios::binary);
			if (!file)
				return E_FAIL;

			_uint iMeshDataSize = writeBuffer.getSize();

			file.write((_char*)&iMeshDataSize, sizeof(_uint));
			file.write((_char*)writeBuffer.getData(), iMeshDataSize);

			file.close();
		}
		break;
#pragma endregion

	}
		break;
	case Map_Tool::CTask_Manager::COOKING_LAST:
		break;
	default:
		break;

	}

	return S_OK;

}

_bool CTask_Manager::Check_EgnoreObject(_string _strModelName)
{
	auto iter = find_if(m_EgnoreObjectNames.begin(), m_EgnoreObjectNames.end(), [&_strModelName]
	(_string strEgnoreModelName)->_bool {
			return strEgnoreModelName == _strModelName;
		});

	return iter != m_EgnoreObjectNames.end();
}
void CTask_Manager::Open_Parsing()
{
	m_isLoading = true;
	m_isLoadComp = false;

	InitializeCriticalSection(&m_Critical_Section);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ParsingMain, this, 0, nullptr);

}

void CTask_Manager::Open_RePackaging()
{
	m_isLoading = true;
	m_isLoadComp = false;

	InitializeCriticalSection(&m_Critical_Section);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, RePackagingMain, this, 0, nullptr);

}


void CTask_Manager::Register_Parsing(const string& _strParsingFileName, const wstring& _strLayerName, _bool _isClear)
{
	TASK tWork = {};

	tWork.eType = MAPDATA_PARSING;
	tWork.isLayerClear = _isClear;
	tWork.strFilePath = _strParsingFileName;
	tWork.strLayerName = _strLayerName;
	m_LoadTasks.push(tWork);
}


void CTask_Manager::Register_RePackaging(const wstring& _strRepackagingFilePath, CMapObject* pMapObject)
{
	TASK tWork = {};

	tWork.eType = MODEL_RE_PACKAGING;
	tWork.strFilePath = WstringToString(_strRepackagingFilePath);
	tWork.pTargetObject = pMapObject;
	m_LoadTasks.push(tWork);
}


CTask_Manager* CTask_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	CTask_Manager* pInstance = new CTask_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : CTask_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_Manager::Free()
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
