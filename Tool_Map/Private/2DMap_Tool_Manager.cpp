#include "stdafx.h"
#include "2DMap_Tool_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Transform.h"
#include "VIBuffer_Terrain.h"
#include "MapObject.h"
#include <DirectXCollision.h>
#include <filesystem>
#include "gizmo/ImGuizmo.h"
#include "Imgui_Manager.h"
#include "NavigationVertex.h"
#include "EditableCell.h"
//#include "CellContainor.h"
#include "Event_Manager.h"
#include "Task_Manager.h"
#include "Engine_Defines.h"
#include "2DTile_RenderObject.h"
#include <commdlg.h>
using namespace std::filesystem;




C2DMap_Tool_Manager::C2DMap_Tool_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext) :
	m_pDevice(_pDevice),
	m_pContext(_pContext), m_pGameInstance(Engine::CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT C2DMap_Tool_Manager::Initialize(CImguiLogger* _pLogger)
{
	// Logger 등록
	m_pLogger = _pLogger;
	Safe_AddRef(m_pLogger);

	ZeroMemory(m_szSaveFileName,sizeof(m_szSaveFileName));
	ZeroMemory(m_szImportLayerTag,sizeof(m_szSaveFileName));

	// 임구이 크기 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(600, 1200), ImVec2(FLT_MAX, FLT_MAX));

	// 모델 리스트 불러오기
	Load_SaveFileList();

	m_DefaultEgnoreLayerTags.push_back(L"Layer_Cell");
	m_DefaultEgnoreLayerTags.push_back(L"Layer_Camera");

	m_pGameInstance->Set_DebugRender(false);
	
	m_pMapParsingManager = CTask_Manager::Create(m_pDevice, m_pContext, m_pLogger);
	if (nullptr == m_pMapParsingManager)
		return E_FAIL;

	m_pMapParsingManager->Register_Parsing("..\\Bin\\json\\Persistent_Room.json",L"Layer_Room_Environment");
	
	
	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DDefaultRenderObject"),
		LEVEL_TOOL_2D_MAP, L"Layer_Default", &pGameObject, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT C2DMap_Tool_Manager::Render()
{
	return S_OK;
}

void C2DMap_Tool_Manager::Update_Tool()
{
	// 임구이 화면 구성
	Update_Imgui_Logic();

	m_pMapParsingManager->Update();
}


void C2DMap_Tool_Manager::Update_Imgui_Logic()
{
	Object_Create_Imgui();
	SaveLoad_Imgui();
}



void C2DMap_Tool_Manager::Input_Object_Tool_Mode()
{
	HWND hWnd = GetFocus();
	auto& io = ImGui::GetIO();

	if (nullptr != hWnd)
	{
	}


	if (nullptr != hWnd && !io.WantCaptureMouse)
	{
		

	}

}

void C2DMap_Tool_Manager::Object_Create_Imgui(_bool _bLock)
{
	ImGui::Begin("Object");
	
	if(ImGui::Button("GO"))
	{
		string arrAxisKey[3] = { "X","Y","Z" };
		string arrColorKey[4] = { "R","G","B","A"};

		const std::string filePathDialog = "../Bin/json/2DMapTestJson/C01_P1718.json";
		std::ifstream inputFile(filePathDialog);
		if (!inputFile.is_open()) {
			throw std::runtime_error("json Error :  " + filePathDialog);
			return;
		}

		vector<pair<_string, _float3>> testVector;
		vector<pair<_string, _float4>> testVector2;
		json jsonDialogs;
		inputFile >> jsonDialogs;
		if (jsonDialogs.is_array())
		{
			for (auto ChildJson : jsonDialogs)
			{
				if (
					ChildJson.is_object() &&
					ChildJson.contains("Type") &&
					ChildJson.contains("Properties") &&
					ChildJson.contains("Outer") &&
					ChildJson["Type"] == "CapsuleComponent")
				{
					if (ChildJson["Properties"].contains("RelativeLocation"))
					{
						_string strText = ChildJson["Outer"];
						auto TargetJson = ChildJson["Properties"];
						_float3 fValuePos = {};
						for (_uint i = 0; i < 3; i++)
						{

							_float fValue = TargetJson["RelativeLocation"].at(arrAxisKey[i]);
							memcpy((&fValuePos.x) + i, &fValue, sizeof(_float));
						}

							testVector.push_back(make_pair(strText, fValuePos));
					}
				
				}
				else if (
					ChildJson.is_object() &&
					ChildJson.contains("Type") &&
					ChildJson.contains("Properties") &&
					ChildJson.contains("Outer") &&
					ChildJson["Type"] == "MaterialInstanceDynamic")
				{
					if (ChildJson["Properties"].contains("VectorParameterValues"))
					{
						auto TargetJson = ChildJson["Properties"];
						if (TargetJson["VectorParameterValues"].is_array())
						{
							for (auto& values : TargetJson["VectorParameterValues"])
							{
								if (values.contains("ParameterValue"))
								{
									_string strText = ChildJson["Outer"];

									_float4 fValueColor = {};
									for (_uint i = 0; i < 4; i++)
									{
										_float fValue = values["ParameterValue"].at(arrColorKey[i]);
										memcpy((&fValueColor.x) + i, &fValue, sizeof(_float));
									}
									testVector2.push_back(make_pair(strText, fValueColor));
								}
							}
						}

					}

				}
			}
		}

		inputFile.close();
	


		if (!testVector.empty())
		{
			int a = 1;
		
		}

	}


	if (ImGui::Button("Go2"))
	{
		_uint iTileXSize = 0;
		_uint iTileYSize = 0;
		_uint iTileWidthInTiles = 0;
		_uint iTileHeightInTiles = 0;
		string arrAxisKey[2] = { "X","Y" };

		const std::string filePathDialog = "../Bin/json/2DMapTestJson/Tile/C01_P1718_TD_Tilemap.json";
		std::ifstream inputFile(filePathDialog);
		if (!inputFile.is_open()) {
			throw std::runtime_error("json Error :  " + filePathDialog);
			return;
		}

		vector<vector<_float2>> testVector;
		vector<pair<_string, _float4>> testVector2;
		json jsonDialogs;
		inputFile >> jsonDialogs;
		if (jsonDialogs.is_array())
		{
			for (auto& ChildJson : jsonDialogs)
			{
				if (
					ChildJson.is_object() &&
					ChildJson.contains("Type") &&
					ChildJson.contains("Properties") &&
					ChildJson["Type"] == "PaperTileSet")
				{
					auto PropertiesJson = ChildJson["Properties"];
					if (PropertiesJson.contains("TileSize"))
					{
						iTileXSize = PropertiesJson["TileSize"]["X"];
						iTileYSize = PropertiesJson["TileSize"]["Y"];
					}

					if (PropertiesJson.contains("WidthInTiles"))
						iTileWidthInTiles = PropertiesJson["WidthInTiles"];
					if (PropertiesJson.contains("HeightInTiles"))
						iTileHeightInTiles = PropertiesJson["HeightInTiles"];
					if (PropertiesJson.contains("PerTileData"))
					{

						for (auto& MapData : PropertiesJson["PerTileData"])
						{
							if (MapData.contains("CollisionData")
								&&
								MapData["CollisionData"].contains("Shapes")
								&&
								MapData["CollisionData"]["Shapes"].is_array()
								)
							{
								for (auto& Shape : MapData["CollisionData"]["Shapes"])
								{
								
									if (Shape.contains("Vertices"))
									{
										vector<_float2> test;
										for (auto Vertices : Shape["Vertices"])
										{
											_float2 fTarget = {};

											for (size_t i = 0; i < 2; i++)
											{
												_float fValue = Vertices.at(arrAxisKey[i]);
												memcpy((&fTarget.x) + i, &fValue, sizeof(_float));
											}
												test.push_back(fTarget);
										}
										testVector.push_back(test);
									}

								}

							}
						}
					}

				}
			}
		}

		inputFile.close();



		if (!testVector.empty())
		{
			int a = 1;

		}

	}
	if (ImGui::Button("Go3"))
	{
		_uint iTileXSize = 0;
		_uint iTileYSize = 0;
		_uint iTileWidthInTiles = 0;
		_uint iTileHeightInTiles = 0;
		string arrAxisKey[2] = { "X","Y" };

		const std::string filePathDialog = "../Bin/json/2DMapTestJson/Tile/C01_P1718_TD_Tilemap.json";
		std::ifstream inputFile(filePathDialog);
		if (!inputFile.is_open()) {
			throw std::runtime_error("json Error :  " + filePathDialog);
			return;
		}

		vector<vector<_float2>> testVector;
		vector<pair<_string, _int>> testVector2;
		json jsonDialogs;
		inputFile >> jsonDialogs;
		if (jsonDialogs.is_array())
		{
			for (auto& ChildJson : jsonDialogs)
			{
				if (
					ChildJson.is_object() &&
					ChildJson.contains("Type") &&
					ChildJson.contains("Properties") &&
					ChildJson["Type"] == "PaperTileMap")
				{
					auto PropertiesJson = ChildJson["Properties"];

					if (PropertiesJson.contains("TileWidth"))
					{
						iTileXSize = PropertiesJson["TileWidth"];
						iTileYSize = PropertiesJson["TileHeight"];
					}

					if (PropertiesJson.contains("MapWidth"))
						iTileWidthInTiles = PropertiesJson["MapWidth"];
					if (PropertiesJson.contains("MapHeight"))
						iTileHeightInTiles = PropertiesJson["MapHeight"];

				}
				if (
					ChildJson.is_object() &&
					ChildJson.contains("Type") &&
					ChildJson.contains("Properties") &&
					ChildJson["Type"] == "PaperTileLayer")
				{
					auto PropertiesJson = ChildJson["Properties"];

					
					if (
						PropertiesJson.contains("LayerName")
						&&
						PropertiesJson["LayerName"].contains("SourceString")
						)
					{
						if (!(PropertiesJson["LayerName"]["SourceString"] == "CollidingLayer" || 
							PropertiesJson["LayerName"]["SourceString"] == "PlayableFront"
							))
							continue;
					if (PropertiesJson.contains("AllocatedCells")
						&&
						PropertiesJson["AllocatedCells"].is_array()
						)
					{
						for (auto& IndexData : PropertiesJson["AllocatedCells"])
						{
							if (
								IndexData.contains("PackedTileIndex")
								&&
								IndexData.contains("TileSet")
								)
							{
								if (IndexData["TileSet"].is_null())
								{
									testVector2.push_back(make_pair("", -1));
								}
								else
								{
									_string txt = IndexData["TileSet"]["ObjectName"];
									_uint iIndex = IndexData["PackedTileIndex"];
									testVector2.push_back(make_pair(txt, iIndex));
								}
							}
						}
					}
				}
				}
			}
		}

		inputFile.close();



		if (!testVector2.empty())
		{
			CGameObject* pGameObject = nullptr;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DTile_RenderObject"),
				LEVEL_TOOL_2D_MAP, L"Layer_TileObject", &pGameObject, nullptr)))
				return;

			if (pGameObject)
				m_pTileRenderObject = static_cast<C2DTile_RenderObject*>(pGameObject);


			C2DTile_RenderObject::TILE_INFO tInfo;
			tInfo.iWidthSIze = iTileXSize;
			tInfo.iHeightSIze = iTileYSize;
			tInfo.iWidthCount = iTileWidthInTiles;
			tInfo.iHeightCount = iTileHeightInTiles;
			ID3D11ShaderResourceView* pSRV = { nullptr };
			HRESULT		hr = E_FAIL;
			hr = CreateDDSTextureFromFile(m_pDevice, L"../Bin/json/2DMapTestJson/Tile/CO1_P0910_TD_Tilemap.dds", nullptr, &pSRV);

			if (SUCCEEDED(hr))
			{
				tInfo.pTexture = pSRV;
				tInfo.strTextureName = L"CO1_P0910_TD_Tilemap.dds";
				m_pTileRenderObject->Set_TileTexture(tInfo);

				for (_uint i = 0 ; i < (_uint)testVector2.size(); ++i)
				{
					m_pTileRenderObject->Set_Index(i, testVector2[i].second);
				}
			}

		}

	}

	ImGui::End();
}

void C2DMap_Tool_Manager::SaveLoad_Imgui(_bool _bLock)
{
	// Always center this window when appearing
	//ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	//ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Save & Load");
	{
		ImGui::SeparatorText("Save File List");
		if (ImGui::BeginListBox("##Save File List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const auto& fileName : m_SaveFileLists) {
				_char szName[MAX_PATH] = {};

				WideCharToMultiByte(CP_ACP, 0, fileName.c_str(), -1, szName, (_int)(lstrlen(fileName.c_str())), NULL, NULL);
				if (ImGui::Selectable(szName, fileName == m_arrSelectName[SAVE_LIST])) {
					if (m_arrSelectName[SAVE_LIST] == fileName)
						m_arrSelectName[SAVE_LIST] = L"";
					else
					{
						wstring filename;
						filename.assign(fileName.begin(), fileName.end());
						m_arrSelectName[SAVE_LIST] = filename;
					}
				}
			}
			ImGui::EndListBox();
		}

		Begin_Draw_ColorButton("Save_FileStyle", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
		if (ImGui::Button("Map Binary Save"))
			Save_Popup();
		ImGui::SameLine();
		if (ImGui::BeginPopup("Save_Popup"))
		{

			auto pLayerMaps = m_pGameInstance->Get_Layers_Ptr();

			vector<wstring> strSaveLayerTags;

			ImGui::SeparatorText("Save Layers");
			for (auto& Pair : pLayerMaps[LEVEL_TOOL_3D_MAP])
			{
				wstring strLayerTag = Pair.first;
				auto iter = find_if(m_DefaultEgnoreLayerTags.begin(), m_DefaultEgnoreLayerTags.end(), [&strLayerTag]
				(const wstring& strEgnoreLayerTag)->_bool {
						return strEgnoreLayerTag == strLayerTag;
					});
				if (iter != m_DefaultEgnoreLayerTags.end())
					continue;
				else
					strSaveLayerTags.push_back(strLayerTag);
			}

			if (strSaveLayerTags.empty())
				ImGui::CloseCurrentPopup();
			else 
			{
				for (auto& strSaveLayerTag : strSaveLayerTags)
				{
					auto iter = find_if(m_EgnoreLayerTags.begin(), m_EgnoreLayerTags.end(), [&strSaveLayerTag](const wstring& strAlreadyEgnoreTag)->_bool {
						return strAlreadyEgnoreTag == strSaveLayerTag;
						});
					_bool isEgnored = iter != m_EgnoreLayerTags.end();
					if (ImGui::Selectable(m_pGameInstance->WStringToString(strSaveLayerTag).c_str(), !isEgnored, ImGuiSelectableFlags_DontClosePopups))
					{
						if (!isEgnored)
							m_EgnoreLayerTags.push_back(strSaveLayerTag);
						else
							m_EgnoreLayerTags.erase(iter);
					}
				}
			}





			ImGui::SeparatorText("Save File Name");

			ImGui::InputText("##Save File Name", m_szSaveFileName, MAX_PATH);

			Begin_Draw_ColorButton("Save_", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("Save"))
			{
				Save();
			}
			End_Draw_ColorButton();

			ImGui::EndPopup();
		}
		End_Draw_ColorButton();
		Begin_Draw_ColorButton("Load_FileStyle", (ImVec4)ImColor::HSV(0.7f, 0.6f, 0.6f));
		if (ImGui::Button("Map Binary Load"))
		{
			if (m_arrSelectName[SAVE_LIST] != L"")
				ImGui::OpenPopup("Load_Popup");
		}
		if (ImGui::BeginPopup("Load_Popup"))
		{
			ImGui::Text("All those Mesh Object will be deleted.");
			ImGui::Text("Do you want to proceed?");
			Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("OK"))
			{
				Load();
				ImGui::CloseCurrentPopup();
			}
			End_Draw_ColorButton();
			ImGui::SameLine();
			Begin_Draw_ColorButton("NO_Style", (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
			if (ImGui::Button("NO"))
			{
				ImGui::CloseCurrentPopup();
			}
			End_Draw_ColorButton();

			ImGui::EndPopup();
		}
		End_Draw_ColorButton();


	}

	ImGui::End();
}


void C2DMap_Tool_Manager::Save(_bool _bSelected)
{
	string filename = m_szSaveFileName;
	string log = "";


	// 1. 레이어 검사 후 저장할 레이어 벡터에 삽입

	vector<pair<wstring, CLayer*>> strSaveLayerTag;
	if (FAILED(Setting_Action_Layer(strSaveLayerTag) || strSaveLayerTag.empty()))
	{
		log = "Save Failed... Save Layer Empty! ";
		LOG_TYPE(log, LOG_ERROR);
		return;
	}
	// 1. 레이어 검사 후 저장할 레이어 벡터에 삽입 END
	

	// 2. 저장 파일 경로 및 네이밍 무결성 검사 & 핸들 오픈 


	if (!Path_String_Validation_Check(filename))
	{
		if (_bSelected)
		{
			log = "Save Failed... file Name Error - File Name : ";
			log += filename;
			if (_bSelected)
			LOG_TYPE(log, LOG_ERROR);
		}

		return;
	}

	_wstring strFullFilePath = L"";
	strFullFilePath = (m_strMapBinaryPath  + m_pGameInstance->StringToWString(filename) + L".mchc");
	log = "Save Start... File Name : ";
	log += filename;
	LOG_TYPE(log, LOG_SAVE);

	HANDLE	hFile = CreateFile(strFullFilePath.c_str(),
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		log = "Save Failed... file Open Error - File Name : ";
		log += filename;
		LOG_TYPE(log, LOG_ERROR);
		return;
	}


	// 2. 저장 파일 경로 및 네이밍 무결성 검사 END

	//
	//
	//
	//
	//
	// ProtoFileName ? 
	// Proto-MapFile 간 동기화 작업 -> Proto-Static Checkbox
	// MapFile Object는 그냥 그 level에 만들면 되는데, 프로토로 받아올떄의 문제
	// 그냥 Loader에 플래그를 줄까 <- 이걸 어떻게?

	// json 양식을 그럼 static flag - _bool, 파일경로 이렇게 가자





	vector<_string> vecSaveModelProtos;

	// 3. 세이브
	DWORD	dwByte(0);

	_uint iLayerCount = (_uint)strSaveLayerTag.size();
	////	세이브 파라미터 1. 레이어 갯수
	WriteFile(hFile, &iLayerCount, sizeof(_uint), &dwByte, nullptr);

	for (auto& LayerPair : strSaveLayerTag)
	{
		_uint		iCount = 0;
		_char		szSaveMeshName[MAX_PATH];
		_char		szLayerTag[MAX_PATH];
		string		strLayerTag = m_pGameInstance->WStringToString(LayerPair.first).c_str();
		CLayer*		pLayer = LayerPair.second;



		strcpy_s(szLayerTag, strLayerTag.c_str());

		//	세이브 파라미터 1. 레이어 태그
		WriteFile(hFile, &szLayerTag, (DWORD)(sizeof(_char)* MAX_PATH), &dwByte, nullptr);

		_uint iAllCount = 0;
		if (LayerPair.second != nullptr)
		{
			auto ObjList = pLayer->Get_GameObjects();
			iAllCount += (_uint)ObjList.size();
		}
		//	세이브 파라미터 2. 레이어 오브젝트 갯수
		WriteFile(hFile, &iAllCount, sizeof(_uint), &dwByte, nullptr);

		log = "=======Layer Save Start...[";
		log += strLayerTag;
		log += "] (";
		log += std::to_string(iAllCount);
		log += ")=======";
		if (_bSelected)
			LOG_TYPE(log, LOG_SAVE);


		// 세이브 파라미터 2. 갯수만큼 반복 3.메쉬이름 - 4.월드 메트릭스
		if (pLayer != nullptr)
		{
			auto ObjList = pLayer->Get_GameObjects();

			for_each(ObjList.begin(), ObjList.end(), [&vecSaveModelProtos ,&_bSelected, &log, &iCount, &hFile, &dwByte, &szSaveMeshName, this](CGameObject* pGameObject)
				{
					CMapObject* pObject = static_cast<CMapObject*>(pGameObject);
					_string strModelName = m_pGameInstance->WStringToString(pObject->Get_ModelName()).c_str();
					strcpy_s(szSaveMeshName, strModelName.c_str());

					auto iter = find_if(vecSaveModelProtos.begin(), vecSaveModelProtos.end(), [&strModelName](const string& _strName)->_bool {
						return _strName == strModelName; });
					if (iter == vecSaveModelProtos.end())
						vecSaveModelProtos.push_back(strModelName);
					
					
					//	세이브 파라미터 3. 메쉬 이름
					WriteFile(hFile, &szSaveMeshName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
					//	세이브 파라미터 4. 월드 매트릭스
					WriteFile(hFile, &pObject->Get_WorldMatrix(), sizeof(_float4x4), &dwByte, nullptr);
					// 세이브 파라미터 5. 마테리얼 오버라이드
					pObject->Save_Override_Material(hFile);
					iCount++;
					log = "Save... Save Object Count :  ";
					log += std::to_string(iCount);
					if (_bSelected)
						LOG_TYPE(log, LOG_SAVE);
				});

		}
	}

	CloseHandle(hFile);
	// 3. 세이브 END

	log = "Model File Save Complete! FileName : ";
	log += m_szSaveFileName;
	log += ".mchc";
	if (_bSelected)
		LOG_TYPE(log, LOG_SAVE);

	if (_bSelected)
		LOG_TYPE("Model Prototype Tag Save Start...", LOG_SAVE);

	if (SUCCEEDED(
		m_pMapParsingManager->
		Export_SaveResult_ToJson(strFullFilePath, vecSaveModelProtos, false)
	))
	{
		log = "Model Prototype Tag Save Complete! FileName :  ";
		log += m_szSaveFileName;
		log += ".json";
		if (_bSelected)
			LOG_TYPE(log, LOG_SAVE);
	}
	else
	{
		log = "Model Prototype Tag Save Error...  FileName : ";
		log += m_szSaveFileName;
		log += ".json";
		if (_bSelected)
			LOG_TYPE(log, LOG_ERROR);
	}

	Load_SaveFileList();
}

void C2DMap_Tool_Manager::Init_Egnore_Layer()
{

	m_EgnoreLayerTags.clear();
	m_EgnoreLayerTags.insert(m_EgnoreLayerTags.end(), m_DefaultEgnoreLayerTags.begin(), m_DefaultEgnoreLayerTags.end());
}


HRESULT C2DMap_Tool_Manager::Setting_Action_Layer(vector<pair<wstring, CLayer*>>& _TargetLayerPairs)
{
	_TargetLayerPairs.clear();

	auto pLayerMaps = m_pGameInstance->Get_Layers_Ptr();

	if (nullptr == pLayerMaps)
		return E_FAIL;

	for (auto& Pair : pLayerMaps[LEVEL_TOOL_3D_MAP])
	{
		wstring strLayerTag = Pair.first;
		auto iter = find_if(m_EgnoreLayerTags.begin(), m_EgnoreLayerTags.end(), [&strLayerTag]
		(const wstring& strEgnoreLayerTag)->_bool {
				return strEgnoreLayerTag == strLayerTag;
			});
		if (iter != m_EgnoreLayerTags.end())
			continue;
		else
		{
			CLayer* arrLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_3D_MAP, strLayerTag);
			_TargetLayerPairs.push_back(make_pair(strLayerTag, arrLayer));
		}
	}
	return S_OK;
}

void C2DMap_Tool_Manager::Load(_bool _bSelected)
{
	//Object_Clear(false);
	
	string filename = m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]);
	string log = "";

	log = "Load Start... File Name : ";
	log += filename;
	_wstring strFullFilePath = (m_strMapBinaryPath + L"/" + m_arrSelectName[SAVE_LIST] + L".mchc");


	if (!Path_String_Validation_Check(filename))
	{
		if (_bSelected)
		{
			log = "Load Failed... File Path Error - File Path : ";
			log += filename;
			if (_bSelected)
				LOG_TYPE(log, LOG_ERROR);
		}

		return;
	}



	HANDLE	hFile = CreateFile(strFullFilePath.c_str(),
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		log = "Load Failed... File Open Error - File Path : ";
		log += filename;
		if (_bSelected)
			LOG_TYPE(log, LOG_ERROR);
		return;
	}
	_uint iCount = 0;
	log = "Load... Load Object Count :  ";
	log += std::to_string(iCount);
	if (_bSelected)
		LOG_TYPE(log, LOG_LOAD);

	DWORD	dwByte(0);
	_uint iLayerCount = 0;

	ReadFile(hFile, &iLayerCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iLayerCount; i++)
	{
		_uint		iObjectCnt = 0;
		_char		szLayerTag[MAX_PATH];
		wstring		strLayerTag;

		ReadFile(hFile, &szLayerTag, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
		ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

		strLayerTag = m_pGameInstance->StringToWString(szLayerTag);
			
		for (size_t i = 0; i < iObjectCnt; i++)
		{
			_char		szSaveMeshName[MAX_PATH];
			_float4x4	vWorld = {};


			ReadFile(hFile, &szSaveMeshName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
			ReadFile(hFile, &vWorld, sizeof(_float4x4), &dwByte, nullptr);

			CMapObject::MAPOBJ_DESC NormalDesc = {};
			lstrcpy(NormalDesc.szModelName, m_pGameInstance->StringToWString(szSaveMeshName).c_str());
			NormalDesc.eCreateType = CMapObject::OBJ_LOAD;
			NormalDesc.matWorld = vWorld;



			CGameObject* pGameObject = nullptr;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_3D_MAP, TEXT("Prototype_GameObject_MapObject"),
				LEVEL_TOOL_3D_MAP,
				strLayerTag,
				&pGameObject,
				(void*)&NormalDesc);

			if (pGameObject)
			{
				static_cast<CMapObject*>(pGameObject)->Load_Override_Material(hFile);

				iCount++;
				log = "Load... Loading Object Count :  ";
				log += std::to_string(iCount);
				if (_bSelected)
					LOG_TYPE(log, LOG_SAVE);
			}
			else
			{
				log = "Load Fail ! Model Prototype does not exist... : ";
				log += std::to_string(iCount);
				LOG_TYPE(log, LOG_ERROR);

			}
		}
	}

	

	
	CloseHandle(hFile);
	log = "Load Complete! FileName : ";
	log += m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]);
	if (_bSelected)
		LOG_TYPE(log, LOG_LOAD);
}


void C2DMap_Tool_Manager::Object_Clear(_bool _bSelected)
{
	if (!_bSelected)
		Init_Egnore_Layer();

	vector<pair<wstring, CLayer*>> vecSaveLayerPairs;
	Setting_Action_Layer(vecSaveLayerPairs);

	if (_bSelected)
		LOG_TYPE("Object Clear", LOG_DELETE);

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_3D_MAP, L"Layer_MapObject");
	DWORD	dwByte(0);
	for (auto LayerPair : vecSaveLayerPairs)
	{
		CLayer* pLayer = LayerPair.second;
		if (pLayer != nullptr)
		{
			auto ObjList = pLayer->Get_GameObjects();
			for_each(ObjList.begin(), ObjList.end(), [](CGameObject* pGameObject)
				{
				OBJECT_DESTROY(pGameObject);
				});
		}
	}
	Init_Egnore_Layer();
}


void C2DMap_Tool_Manager::Save_Popup()
{
	Init_Egnore_Layer();
	// default EgnoreLayerTag 구성 !

	ImGui::OpenPopup("Save_Popup");
	strcpy_s(m_szSaveFileName, m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]).c_str());
}


HRESULT C2DMap_Tool_Manager::Picking_On_Terrain(_float3* fPickingPos, CMapObject** ppMap)
{
	_float3			vRayPos, vRayDir;

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fCursorPos = { (_float)ptMouse.x,(_float)ptMouse.y };
	Compute_World_PickingLay(&vRayPos, &vRayDir);

	auto pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_3D_MAP, L"Layer_Environment");
	if (!pLayer)
		return E_FAIL;
	auto List = pLayer->Get_GameObjects();
	_float	 fDist = 0.f, fNewDist = 0.f;
	_float3  vReturnPos = {};
	_float3  vReturnNewPos = {};
	CMapObject* pReturnObject = nullptr;
	for_each(List.begin(), List.end(), [this, &fCursorPos,  &fDist, &fNewDist, &vReturnNewPos, &vReturnPos, &pReturnObject](CGameObject* pGameObject)
		{
			CMapObject* pMapObject = dynamic_cast<CMapObject*>(pGameObject);
			if (pMapObject)
			{
				//bool bRange = pMapObject->Check_Picking(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir), &vReturnNewPos, &fNewDist);
				_bool bRange = pMapObject->Is_PickingCursor_Model(fCursorPos, fNewDist);
				if (bRange)
				{
					int a = 1;
				}
				if (bRange && (fNewDist < fDist || fDist == 0.f))
				{
					pReturnObject = pMapObject;
					fDist = fNewDist;
				}
			}
		});

	if (pReturnObject)
	{
		*ppMap = pReturnObject;

		_vector vWorldRayPos = XMLoadFloat3(&vRayPos) + (XMLoadFloat3(&vRayDir) * fDist);
		XMStoreFloat3(fPickingPos, vWorldRayPos);

		return S_OK;
	}
	else
		return E_FAIL;

}

HRESULT C2DMap_Tool_Manager::Picking_On_Terrain(_float3* fPickingPos)
{
	HRESULT hr = {};
	CMapObject* pToolObj = nullptr;

	if (SUCCEEDED(Picking_On_Terrain(fPickingPos, &pToolObj)))
		return S_OK;
	else
		return E_FAIL;
}

CMapObject* C2DMap_Tool_Manager::Picking_On_Object()
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fCursorPos = { (_float)ptMouse.x,(_float)ptMouse.y };

	auto pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_3D_MAP, m_strPickingLayerTag);
	if (!pLayer)
		return nullptr;

	auto List = pLayer->Get_GameObjects();
	_float	 fDist = 0.f, fNewDist = 0.f;
	_float3  vReturnPos = {};
	CMapObject* pReturnObject = nullptr;
	for_each(List.begin(), List.end(), [this, &fCursorPos, &fDist, &vReturnPos, &fNewDist, &pReturnObject](CGameObject* pGameObject)
		{
			CMapObject* pMapObject = dynamic_cast<CMapObject*>(pGameObject);
			if (pMapObject)
			{
				bool bRange = pMapObject->Is_PickingCursor_Model(fCursorPos, fNewDist);
				//bool bRange = pMapObject->Check_Picking(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir), &vReturnPos, &fNewDist);
				if (bRange && (fNewDist < fDist || fDist == 0.f))
				{
					pReturnObject = pMapObject;
					fDist = fNewDist;
				}
			}
		});

	return pReturnObject;
}

HRESULT C2DMap_Tool_Manager::Compute_World_PickingLay(_float3* pLayPos, _float3* pLayDir)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_vector	vMousePos;

	_uint		iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	vMousePos = XMVectorSet(ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f
		, ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f
		, 0.f
		, 1.f
	);
	_matrix		matProj;
	matProj = m_pGameInstance->Get_TransformInverseMatrix(Engine::CPipeLine::D3DTS_PROJ);
	vMousePos = XMVector3TransformCoord(vMousePos, matProj);

	_vector	vRayDir, vRayPos;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMousePos - vRayPos;
	XMVectorSetW(vRayDir, 0.f);

	_matrix		matView;
	matView = m_pGameInstance->Get_TransformInverseMatrix(Engine::CPipeLine::D3DTS_VIEW);
	vRayPos = XMVector3TransformCoord(vRayPos, matView);
	vRayDir = XMVector3TransformNormal(vRayDir, matView);
	vRayDir = XMVector3Normalize(vRayDir);


	XMStoreFloat3(pLayDir, vRayDir);
	XMStoreFloat3(pLayPos, vRayPos);

	return S_OK;
}

void C2DMap_Tool_Manager::Load_2DModelList()
{
	m_ObjectFileLists.clear();
	_wstring strPath 
		= TEXT("../../Client/Bin/resources/Models/");

	for (const auto& entry : ::recursive_directory_iterator(strPath))
	{
		if (is_directory(entry))
		{
			for (const auto& file : ::recursive_directory_iterator(entry))
			{
				if (file.path().extension() == ".model")
				{
					_wstring strName = file.path().stem().wstring();
					_wstring strPath = file.path().wstring();
					m_ObjectFileLists.push_back(make_pair(strName, strPath));
				}
			}
		}
	}
}

void C2DMap_Tool_Manager::Load_SaveFileList()
{
	m_SaveFileLists.clear();
	for (const auto& entry : ::recursive_directory_iterator(m_strMapBinaryPath))
	{
			if (entry.path().extension() == ".mchc")
			{
				wstring strKey = entry.path().stem().wstring();
				m_SaveFileLists.push_back(strKey);
			}
	}
}


C2DMap_Tool_Manager* C2DMap_Tool_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	C2DMap_Tool_Manager* pInstance = new C2DMap_Tool_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : C2DMap_Tool_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DMap_Tool_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pLogger);
	Safe_Release(m_pMapParsingManager);

	__super::Free();
}
