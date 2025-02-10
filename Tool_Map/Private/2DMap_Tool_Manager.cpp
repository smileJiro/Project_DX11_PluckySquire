#include "stdafx.h"
#include "2DMap_Tool_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Transform.h"
#include "VIBuffer_Terrain.h"
#include "MapObject.h"
#include <filesystem>
#include "gizmo/ImGuizmo.h"
#include "Imgui_Manager.h"
#include "NavigationVertex.h"
#include "Event_Manager.h"
#include "Task_Manager.h"
#include "Engine_Defines.h"
#include "2DTile_RenderObject.h"
#include "2DDefault_RenderObject.h"
#include "2DMapObject.h"
#include "2DTrigger_Sample.h"
#include "RenderGroup.h"
#include "RenderGroup_2D.h"
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

	ZeroMemory(m_szSaveFileName, sizeof(m_szSaveFileName));


	// 임구이 크기 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(600, 1200), ImVec2(FLT_MAX, FLT_MAX));

	// 모델 리스트 불러오기
	Load_SaveFileList();
	Load_2DModelList();
	Load_String();

	m_pGameInstance->Set_DebugRender(false);

	m_pTaskManager = CTask_Manager::Create(m_pDevice, m_pContext, m_pLogger);
	if (nullptr == m_pTaskManager)
		return E_FAIL;

	m_arrSelectName[SAVE_LIST] = L"Chapter_04_Default_Desk";
	Load_3D_Map(false);


	m_Egnore2DObjectrTags.push_back("InvisibleCollision");
	m_Egnore2DObjectrTags.push_back("Event");
	m_Egnore2DObjectrTags.push_back("Goblin");

	CGameObject* pGameObject = nullptr;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DDefaultRenderObject"),
		LEVEL_TOOL_2D_MAP, L"Layer_Default", &pGameObject, nullptr)))
		return E_FAIL;
	if (nullptr != pGameObject)
	{
		m_DefaultRenderObject = static_cast<C2DDefault_RenderObject*>(pGameObject);
	}


	CModelObject::MODELOBJECT_DESC Desc = {};

	Desc.iCurLevelID = LEVEL_TOOL_2D_MAP;
	Desc.iModelPrototypeLevelID_3D = LEVEL_TOOL_2D_MAP;

	Change_RenderGroup(_float2((_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_SampleBook"),
		LEVEL_TOOL_2D_MAP, L"Layer_Default", &pGameObject, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT C2DMap_Tool_Manager::Render()
{
	return S_OK;
}

void C2DMap_Tool_Manager::Update_Tool()
{
	Input_Logic();

	// 임구이 화면 구성
	Update_Imgui_Logic();

	m_pTaskManager->Update();
}


void C2DMap_Tool_Manager::Update_Imgui_Logic()
{
	Map_Import_Imgui();
	Model_Edit_Imgui();
	SaveLoad_Imgui();
	TriggerSetting_Imgui();
	TriggerEvent_Imgui();
}



void C2DMap_Tool_Manager::Input_Logic()
{
	HWND hWnd = GetFocus();
	auto& io = ImGui::GetIO();

	if (nullptr != hWnd && !io.WantCaptureMouse)
	{

		if (((ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt)) && ImGui::IsKeyPressed(ImGuiKey_MouseLeft)))
		{
			C2DTrigger_Sample::TRIGGER_2D_DESC TriggerDesc = {};
			TriggerDesc.tTransform2DDesc.vInitialPosition = { 0.f,0.f,1.f };
			TriggerDesc.tTransform2DDesc.vInitialScaling = { 100.f,100.f,1.f };
			//TriggerDesc.fRenderTargetSize = { (_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y };
			TriggerDesc.iCurLevelID = LEVEL_TOOL_2D_MAP;
			TriggerDesc.eStartCoord = COORDINATE_2D;
			TriggerDesc.isCoordChangeEnable = false;


			CGameObject* pGameObject = nullptr;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DTrigger"),
				LEVEL_TOOL_2D_MAP,
				L"Layer_Trigger",
				&pGameObject,
				(void*)&TriggerDesc)))
			{
				//_string strNotExistTextureMessage = WstringToString(_wstring(L"2D Texture Info not Exist ->") + NormalDesc.strProtoTag);
				//LOG_TYPE(strNotExistTextureMessage, LOG_ERROR);
				//NotExistTextures.push_back(strNotExistTextureMessage);
			}

		}
		if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft) && nullptr != m_DefaultRenderObject && m_DefaultRenderObject->Is_2DMode())
		{
			C2DMapObject* pPickingObject = Picking_2DMap();
			m_pPickingObject = pPickingObject;
		}
		if (!io.WantCaptureKeyboard && ImGui::IsKeyPressed(ImGuiKey_C) && nullptr != m_DefaultRenderObject)
		{
			_bool is2DMode = m_DefaultRenderObject->Toggle_Mode();
			CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(LEVEL_TOOL_2D_MAP, L"Layer_Camera", 0);
			if (nullptr != pGameObject)

				pGameObject->Set_Active(!is2DMode);
		}
		if (ImGui::IsKeyPressed(ImGuiKey_Delete) && nullptr != m_DefaultRenderObject && m_DefaultRenderObject->Is_2DMode())
		{
			if (nullptr != m_pPickingObject)
			{
				Event_DeleteObject(m_pPickingObject);
				m_pPickingObject = nullptr;
			}
		}
	}


	if (nullptr != hWnd && !io.WantCaptureMouse)
	{


	}

}


void C2DMap_Tool_Manager::Map_Import_Imgui(_bool _bLock)
{
	ImGui::Begin("Map");
	static _float4 fColor = { 1.0f, 0.0f, 1.0f, 1.0f }; // 초기 색상 (RGBA)

	if (ImGui::Button("Import 2D .umap file"))
	{
		_tchar originalDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, originalDir);

		_wstring strModelPath = L"../Bin/json/2DMapJson/";

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
		wstring strPath = strModelPath;
		ofn.lpstrInitialDir = strPath.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileName(&ofn))
		{
			Object_Clear(false);
			m_fOffsetPos = { 0.f,0.f };
			LOG_TYPE(_wstring(L"=====  2D Map Read Start  -> ") + szName, LOG_LOAD);

			//const std::string filePathDialog = "../Bin/json/2DMapJson/C01_P1718.json";
			//const std::string filePathDialog = "../Bin/json/2DMapJson/C01_P1112.json";
			const std::string filePathDialog = WstringToString(szName);
			const string strFileName = WstringToString(Get_FileName_From_Path(StringToWstring(filePathDialog)).first);
			std::ifstream inputFile(filePathDialog);
			if (!inputFile.is_open()) {
				throw std::runtime_error("json Error :  " + filePathDialog);
				return;
			}

			json jsonDialogs;
			inputFile >> jsonDialogs;

			Import(strFileName, jsonDialogs, &fColor);

			inputFile.close();

		}

	}

	ImGui::Text("Offset: %.2f, %.2f", m_fOffsetPos.x, m_fOffsetPos.y);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(50.0f);
	if (ImGui::DragFloat("##X", &m_fOffsetPos.x, 10.f))
	{
		auto Layer = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_2DMapObject");
		if (nullptr != Layer)
		{
			auto GameObjects = Layer->Get_GameObjects();
			for (auto& pGameObject : GameObjects)
			{
				static_cast<C2DMapObject*>(pGameObject)->Set_OffsetPos(m_fOffsetPos);
			}
		}
	}

	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);
	if (ImGui::DragFloat("##Y", &m_fOffsetPos.y, 10.f))
	{
		auto Layer = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_2DMapObject");
		if (nullptr != Layer)
		{
			auto GameObjects = Layer->Get_GameObjects();
			for (auto& pGameObject : GameObjects)
			{
				static_cast<C2DMapObject*>(pGameObject)->Set_OffsetPos(m_fOffsetPos);
			}
		}
	}
	// 1. RGB 슬라이더 (3채널)
	ImGui::Text("RGB Color Picker:");
	if (ImGui::ColorEdit3("Color (RGB)", &fColor.x))
		m_DefaultRenderObject->Set_Color(fColor);
	if (ImGui::Button("Out 2D Map Texture"))
	{

		_tchar originalDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, originalDir);

		_wstring strModelPath = L"../../Client/Bin/Resources/Textures/Map";

		OPENFILENAME ofn = {};
		_tchar szName[MAX_PATH] = L"NewFile.dds";
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szName;
		ofn.nMaxFile = sizeof(szName);
		ofn.lpstrFilter = L".dds\0*.dds\0";
		ofn.nFilterIndex = 0;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		wstring strPath = strModelPath;
		ofn.lpstrInitialDir = strPath.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
		if (GetSaveFileName(&ofn))
		{
			m_pTileRenderObject->Set_OutputPath(szName);
		}
	}

	if (ImGui::Button("SampleBook Mode Toggle"))
	{
		_bool is2DMode = m_DefaultRenderObject->Toggle_Mode();
		CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(LEVEL_TOOL_2D_MAP, L"Layer_Camera", 0);
		if (nullptr != pGameObject)

			pGameObject->Set_Active(!is2DMode);
	}

	if (nullptr != m_pPickingObject)
	{
		_wstring strKey = m_pPickingObject->Get_Key();
		_wstring strModelName = m_pPickingObject->Get_ModelName();
		_float2 fPos = m_pPickingObject->Get_DefaultPosition();
		//m_pPickingObject->Get_FinalPosition();
		ImGui::Text("Model SearchKey : %s", WstringToString(strKey).c_str());
		ImGui::Text("Model Name : %s", WstringToString(strModelName).c_str());
		ImGui::Text("Model Load : %s", m_pPickingObject->Is_ModelLoad() ? "On" : "Off");
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::DragFloat("##ObjectPosX", &fPos.x, 1.f, -FLT_MAX, FLT_MAX, "x:%.1f"))
		{
			m_pPickingObject->Set_DefaultPosition(fPos);
			static_cast<C2DMapObject*>(m_pPickingObject)->Set_OffsetPos(m_fOffsetPos);
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::DragFloat("##ObjectPosY", &fPos.y, 1.f, -FLT_MAX, FLT_MAX, "y:%.1f"))
		{
			m_pPickingObject->Set_DefaultPosition(fPos);
			static_cast<C2DMapObject*>(m_pPickingObject)->Set_OffsetPos(m_fOffsetPos);
		}
		ImGui::SameLine();
		ImGui::Text("Position");

	}




	ImGui::End();
}
void C2DMap_Tool_Manager::Model_Edit_Imgui(_bool _bLock)
{
	ImGui::Begin("2D Model Edit");
	{
		ImGui::SeparatorText("Model List");

#pragma region CHECK_BOX_FILTER
		static char sz2DModelSearchBuffer[128] = ""; // 검색어 입력을 위한 버퍼

		// 검색어에 따라 필터링된 항목을 저장할 임시 벡터
		static std::vector<C2DMapObjectInfo*> filteredItems;

		// 검색 입력 필드
		ImGui::SetNextItemWidth(-FLT_MIN - 50.f);

		ImGui::InputText("Filter", sz2DModelSearchBuffer, IM_ARRAYSIZE(sz2DModelSearchBuffer));
		filteredItems.clear();
		_string searchTerm = sz2DModelSearchBuffer;
		std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
		for (const auto& item : m_ObjectInfoLists)
		{
			_string strSearchTag = item->Get_SearchTag();
			std::transform(strSearchTag.begin(), strSearchTag.end(), strSearchTag.begin(), ::tolower);
			if (strSearchTag.find(searchTerm) != _string::npos)
				filteredItems.push_back(item);
		}
#pragma endregion



		if (ImGui::BeginListBox("##Model List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& pInfo : filteredItems) {
				_char szName[MAX_PATH] = {};
				_string strSearchTag = pInfo->Get_SearchTag();
				_bool isCurTag = StringToWstring(strSearchTag) == m_arrSelectName[MODEL_LIST];
				if (strSearchTag == "")
					strSearchTag = "empty";
				if (ImGui::Selectable(strSearchTag.c_str(), isCurTag))
				{
					if (isCurTag)
					{
						m_arrSelectName[MODEL_LIST] = L"";
						m_pPickingInfo = nullptr;
					}
					else
					{
						wstring filename = StringToWstring(strSearchTag);
						m_arrSelectName[MODEL_LIST] = filename;
						m_pPickingInfo = pInfo;
					}
				}
			}
			ImGui::EndListBox();
		}
		Begin_Draw_ColorButton("##Add_Model_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
		if (StyleButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE::MINI, "Add Model Info"))
		{
			_string strOriginNewTag = "NewModel_";
			_uint iNewTagNumber = 1;
			_string strNumberingNewTag = strOriginNewTag + std::to_string(iNewTagNumber);
			for (auto iter = m_ObjectInfoLists.begin(); iter != m_ObjectInfoLists.end();)
			{
				if (ContainString((*iter)->Get_SearchTag(), strNumberingNewTag))
				{
					iter = m_ObjectInfoLists.begin();
					iNewTagNumber++;
					strNumberingNewTag = strOriginNewTag + std::to_string(iNewTagNumber);
					continue;
				}
				else
					++iter;
			}
			C2DMapObjectInfo* pInfo = C2DMapObjectInfo::Create();
			pInfo->Set_SearchTag(strNumberingNewTag);
			m_ObjectInfoLists.push_back(pInfo);
			m_pPickingInfo = pInfo;
			m_arrSelectName[MODEL_LIST] = StringToWstring(strNumberingNewTag);
		}
		End_Draw_ColorButton();

		ImGui::SameLine();

		Begin_Draw_ColorButton("##Delete_Model_Style", (ImVec4)ImColor::HSV(0.6f, 0.6f, 0.6f));
		if (StartPopupButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE::MINI, "Delete Model Info"))
		{
			ImGui::Text("Really?");

			Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("OK"))
			{
				if (m_pPickingInfo)
				{
					auto iter = find_if(m_ObjectInfoLists.begin(), m_ObjectInfoLists.end(), [this](C2DMapObjectInfo* _pModelInfo) {
						return _pModelInfo->Get_SearchTag() == m_pPickingInfo->Get_SearchTag();
						});
					if (iter != m_ObjectInfoLists.end())
					{
						_uint iIndx = m_pPickingInfo->Get_ModelIndex();

						iter = m_ObjectInfoLists.erase(iter);

						for (; iter != m_ObjectInfoLists.end(); ++iter)
							(*iter)->Set_ModelIndex((*iter)->Get_ModelIndex() - 1);


						m_pPickingInfo->Set_Delete(true);

						if (FAILED(Update_Model_Index()))
						{
							MSG_BOX("Model Index Update Error - Check");
						}

						Safe_Release(m_pPickingInfo);
					}

				}
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

		ImGui::SameLine();
		Begin_Draw_ColorButton("##Output_Model_Style", (ImVec4)ImColor::HSV(0.6f, 1.f, 0.6f));
		if (StartPopupButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE::MINI, "Output .json ModelList"))
		{
			ImGui::Text("Really?");

			Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("OK"))
			{
				Save_2DModelList();
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


		if (nullptr != m_pPickingInfo)
		{
			ImGui::SeparatorText("Model Info");

			_string strSearchTag = m_pPickingInfo->Get_SearchTag().c_str();
			_string strTextureName = m_pPickingInfo->Get_ModelName().c_str();
			_bool isCollider = m_pPickingInfo->Is_Collider();
			_bool isActive = m_pPickingInfo->Is_Active();
			_bool isSorting = m_pPickingInfo->Is_Sorting();
			_bool isModelLoad = m_pPickingInfo->Is_ModelLoad();
			_bool isToolRendering = m_pPickingInfo->Is_ToolRendering();
			_bool isBackGround = m_pPickingInfo->Is_BackGround();

			if (isToolRendering)
			{
				_float2 fDefaultSize = { 128.f, 128.f };
				_float2 fOffSize = { 128.f, 128.f };
				auto pSRV = m_pPickingInfo->Get_SRV(&fOffSize);
				if (nullptr != pSRV)
				{
					if (fOffSize.x != -1)
					{
						//TODO :: 이미지해상도 조절하게될지도?
						//_float fRatio = fDefaultSize.x / fOffSize.x;
						//fDefaultSize.x = fOffSize.x * fRatio;
						//fDefaultSize.y = fOffSize.y * fRatio;
					}
					ImGui::Image((ImTextureID)pSRV,
						ImVec2(fDefaultSize.x, fDefaultSize.y)
					);
				}
				ImGui::SameLine();
			}
			ImGui::BeginGroup();
			{
				CModel::ANIM_TYPE eType = (CModel::ANIM_TYPE)m_pPickingInfo->Get_ModelType();


				ImGui::Text("Model Search Tag");
				ImGui::SetNextItemWidth(200.0f);
				if (InputText("##Model Search Tag", strSearchTag))
					m_pPickingInfo->Set_SearchTag(strSearchTag);

				ImGui::SetNextItemWidth(200.0f);
				ImGui::Text("Model Load :");
				ImGui::SameLine();
				if (isModelLoad)
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "OK");
				else
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "NO");

				if (!isModelLoad)
				{
					if (ImGui::Button("Add .model2d"))
					{
						_tchar originalDir[MAX_PATH];
						GetCurrentDirectory(MAX_PATH, originalDir);

						_wstring strModelPath = L"../../Client/Bin/Resources/Models/2DMapObject";

						OPENFILENAME ofn = {};
						_tchar szName[MAX_PATH] = {};
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = g_hWnd;
						ofn.lpstrFile = szName;
						ofn.nMaxFile = sizeof(szName);
						ofn.lpstrFilter = L".model2d\0*.model2d\0";
						ofn.nFilterIndex = 0;
						ofn.lpstrFileTitle = nullptr;
						ofn.nMaxFileTitle = 0;
						wstring strPath = strModelPath;
						ofn.lpstrInitialDir = strPath.c_str();
						ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

						if (GetOpenFileName(&ofn))
						{
							const string strFileName = WstringToString(Get_FileName_From_Path(szName).first);
							const string strPath = WstringToString(szName);
							CBase* pProtoModel = m_pGameInstance->Find_Prototype(LEVEL_TOOL_2D_MAP, StringToWstring(strFileName));
							if (pProtoModel == nullptr)
							{
								CModel* pModel = C2DModel::Create(m_pDevice, m_pContext, strPath.c_str());

								if (nullptr == pModel)
								{
									MSG_BOX("2D Model Create Failed");
								}
								else
								{
									m_pGameInstance->Add_Prototype(LEVEL_TOOL_2D_MAP, StringToWstring(strFileName.c_str()), pModel);
									C2DModel* p2DModel = static_cast<C2DModel*>(pModel);
									m_pPickingInfo->Set_Model(p2DModel);
									m_pPickingInfo->Set_ModelName(strFileName.c_str());
									m_pPickingInfo->Set_SearchTag(m_pPickingInfo->Get_ModelName());
								}
							}
							else
							{
								m_pPickingInfo->Set_Model((C2DModel*)pProtoModel);
								m_pPickingInfo->Set_ModelName(strFileName.c_str());
								m_pPickingInfo->Set_SearchTag(m_pPickingInfo->Get_ModelName());

							}

						}
					}
					ImGui::SameLine();
				}
				else
				{
					ImGui::SetNextItemWidth(200.0f);
					ImGui::Text("Model File Name : %s", strTextureName.c_str());

					Begin_Draw_ColorButton("Object_Create_Style", (ImVec4(0.32f, 0.56f, 0.f, 1.f)));
					if (StyleButton(MINI, "Object Create"))
					{
						C2DMapObject::MAPOBJ_2D_DESC NormalDesc = { };
						NormalDesc.strProtoTag = StringToWstring(m_pPickingInfo->Get_ModelName());
						NormalDesc.fDefaultPosition = { 0.f,0.f };
						NormalDesc.fRenderTargetSize = { (_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y };
						NormalDesc.iCurLevelID = LEVEL_TOOL_2D_MAP;
						NormalDesc.iRenderGroupID_2D = m_p2DRenderGroup->Get_RenderGroupID();
						NormalDesc.iPriorityID_2D = m_p2DRenderGroup->Get_PriorityID();
						NormalDesc.pInfo = m_pPickingInfo;


						CGameObject* pGameObject = nullptr;
						if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DMapObject"),
							LEVEL_TOOL_2D_MAP,
							L"Layer_2DMapObject",
							&pGameObject,
							(void*)&NormalDesc)))
						{
							// 생성실패
						}
						else
						{

							m_pPickingObject = static_cast<C2DMapObject*>(pGameObject);
							m_pPickingObject->Set_OffsetPos(m_fOffsetPos);
						}

					}
					End_Draw_ColorButton();


				}


			}
			ImGui::EndGroup();


			ImGui::SeparatorText("Model Option");
			if (ImGui::Checkbox("Sorting", &isSorting))
				m_pPickingInfo->Set_Sorting(isSorting);
			ImGui::SameLine();
			if (ImGui::Checkbox("Active", &isActive))
				m_pPickingInfo->Set_Active(isActive);
			ImGui::SameLine();
			if (ImGui::Checkbox("Collider", &isCollider))
				m_pPickingInfo->Set_Collider(isCollider);
			if (ImGui::Checkbox("BackGround", &isBackGround))
				m_pPickingInfo->Set_BackGround(isBackGround);


			if (isSorting)
			{
				ImGui::SeparatorText("Model Sorting");
				_float2 fSortingPos = m_pPickingInfo->Get_Sorting_Pos();
				ImGui::SetNextItemWidth(70.f);
				if (ImGui::InputFloat("##SortPosX", &fSortingPos.x, 0.f, 0.f, "x:%.1f"))
					m_pPickingInfo->Set_Sorting_Pos(fSortingPos);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f);
				if (ImGui::InputFloat("##SortPosY", &fSortingPos.y, 0.f, 0.f, "y:%.1f"))
					m_pPickingInfo->Set_Sorting_Pos(fSortingPos);

				Begin_Draw_ColorButton("##Edit Sorting Postion", (ImVec4)ImColor::HSV(0.3f, 0.3f, 0.3f));
				if (isToolRendering && StartPopupButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE::MINI, "Edit Sorting Postion", ImGuiWindowFlags_NoMove))
				{
					_float2 fDefaultSize = { 128.f, 128.f };
					_float2 fOffSize = { 128.f, 128.f };
					_float fRatio = 1.f;
					auto pSRV = m_pPickingInfo->Get_SRV(&fOffSize);
					if (nullptr != pSRV)
					{
						if (fOffSize.x != -1)
						{
							fRatio = fDefaultSize.x / fOffSize.x;
							fDefaultSize.x = fOffSize.x * fRatio;
							fDefaultSize.y = fOffSize.y * fRatio;
						}

						ImVec2 imagePos = ImGui::GetCursorScreenPos();
						ImGui::Image((ImTextureID)pSRV,
							ImVec2(fDefaultSize.x, fDefaultSize.y)
						);
						ImDrawList* drawList = ImGui::GetWindowDrawList();

						ImVec2 mousePos = ImGui::GetMousePos();
						bool isMouseOverImage = (mousePos.x >= imagePos.x && mousePos.x <= imagePos.x + fDefaultSize.x &&
							mousePos.y >= imagePos.y && mousePos.y <= imagePos.y + fDefaultSize.y);

						if (isMouseOverImage && ImGui::IsMouseDown(0))
							m_pPickingInfo->Set_Sorting_Pos({ (mousePos.x - imagePos.x) / fRatio - (fOffSize.x * 0.5f), (mousePos.y - imagePos.y) / fRatio - (fOffSize.y * 0.5f) });

						ImVec2 DrawPos = { fSortingPos.x * fRatio + imagePos.x + (fDefaultSize.x * 0.5f),fSortingPos.y * fRatio + imagePos.y + (fDefaultSize.y * 0.5f) };
						drawList->AddCircleFilled(DrawPos, 5.f, IM_COL32(255, 0, 0, 255));
					}
					Begin_Draw_ColorButton("NO_Style", (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
					if (ImGui::Button("Close"))
					{
						ImGui::CloseCurrentPopup();
					}
					End_Draw_ColorButton();

					ImGui::EndPopup();

				}
				End_Draw_ColorButton();
			}
			if (isActive)
			{
				_uint eActiveType = m_pPickingInfo->Get_ActiveType();
				ImGui::SeparatorText("Active Type");
				if (ImGui::BeginListBox("##ActiveType", ImVec2(-FLT_MIN, (_uint)m_ActiveTypeTexts.size() * ImGui::GetTextLineHeightWithSpacing())))
				{
					_uint iSelectIndex = 0;
					for (auto& strActiveTag : m_ActiveTypeTexts)
					{
						if (ImGui::Selectable(strActiveTag.c_str(), eActiveType == iSelectIndex))
							if (eActiveType != iSelectIndex)
								m_pPickingInfo->Set_ActiveType(iSelectIndex);
						iSelectIndex++;
					}
					ImGui::EndListBox();
				}
			}

			if (isCollider)
			{
				CCollider::TYPE eColliderType = (CCollider::TYPE)m_pPickingInfo->Get_ColliderType();
				_bool isNone = eColliderType == CCollider::TYPE::TYPE_LAST;
				_bool isAABB = eColliderType == CCollider::TYPE::AABB_2D;
				_bool isSquare = eColliderType == CCollider::TYPE::CIRCLE_2D;
				ImGui::SeparatorText("Collider Setting");
				if (ImGui::RadioButton("None", isNone))
					m_pPickingInfo->Set_ColliderType(CCollider::TYPE::TYPE_LAST);
				ImGui::SameLine();
				if (ImGui::RadioButton("AABB", isAABB))
					m_pPickingInfo->Set_ColliderType(CCollider::TYPE::AABB_2D);
				ImGui::SameLine();
				if (ImGui::RadioButton("Square", isSquare))
					m_pPickingInfo->Set_ColliderType(CCollider::TYPE::CIRCLE_2D);

				//PosOffset
				if (!isNone)
				{
					_float2 m_fOffsetPos = m_pPickingInfo->Get_Collider_Offset_Pos();
					ImGui::SetNextItemWidth(50.f);
					if (ImGui::InputFloat("##PosX", &m_fOffsetPos.x, 0.f, 0.f, "x:%.1f"))
						m_pPickingInfo->Set_Collider_Offset_Pos(m_fOffsetPos);
					ImGui::SameLine();
					ImGui::SetNextItemWidth(50.f);
					if (ImGui::InputFloat("##PosY", &m_fOffsetPos.y, 0.f, 0.f, "y:%.1f"))
						m_pPickingInfo->Set_Collider_Offset_Pos(m_fOffsetPos);
					ImGui::SameLine();
					ImGui::Text("Offset Pos");
					_float fRadius = m_pPickingInfo->Get_Collider_Radius();
					_float2 fExtent = m_pPickingInfo->Get_Collider_Extent();

					if (isAABB)
					{
						//Extent,
						ImGui::SetNextItemWidth(50.f);
						if (ImGui::InputFloat("##ExtentX", &fExtent.x, 0.f, 0.f, "x:%.1f"))
							m_pPickingInfo->Set_Collider_Extent(fExtent);
						ImGui::SameLine();
						ImGui::SetNextItemWidth(50.f);
						if (ImGui::InputFloat("##ExtentY", &fExtent.y, 0.f, 0.f, "y:%.1f"))
							m_pPickingInfo->Set_Collider_Extent(fExtent);
						ImGui::SameLine();
						ImGui::Text("Extent");
					}

					if (isSquare)
					{
						//Radius
						ImGui::SetNextItemWidth(110.f);
						if (ImGui::InputFloat("Radius", &fRadius, 0.f, 0.f, "%.1f"))
							m_pPickingInfo->Set_Collider_Radius(fRadius);

					}
					Begin_Draw_ColorButton("##Edit Collider Setting", (ImVec4)ImColor::HSV(0.3f, 0.3f, 0.3f));
					if (isToolRendering && StartPopupButton(IMGUI_MAPTOOL_BUTTON_STYLE_TYPE::MINI, "Edit Collider Setting", ImGuiWindowFlags_NoMove))
					{
						_float2 fDefaultSize = { 128.f, 128.f };
						_float2 fOffSize = { 128.f, 128.f };
						_float fRatio = 1.f;
						auto pSRV = m_pPickingInfo->Get_SRV(&fOffSize);
						if (nullptr != pSRV)
						{
							if (fOffSize.x != -1)
							{
								fRatio = fDefaultSize.x / fOffSize.x;
								fDefaultSize.x = fOffSize.x * fRatio;
								fDefaultSize.y = fOffSize.y * fRatio;
							}
							ImVec2 padding(100.f, 100.f);  // 좌측(20px), 위쪽(30px) 여백
							ImGui::SetCursorPos(ImVec2(padding.x * 0.5f, padding.y * 0.5f));  // 좌측 20px, 위쪽 30px
							ImVec2 imagePos = ImGui::GetCursorScreenPos();
							ImGui::Image((ImTextureID)pSRV,
								ImVec2(fDefaultSize.x, fDefaultSize.y)
							);

							ImGui::Dummy({ padding.x * 2.f, padding.y * 0.5f });

							ImDrawList* drawList = ImGui::GetWindowDrawList();
							ImVec2 DefaultDrawPosMin = { imagePos.x, imagePos.y };
							ImVec2 DefaultDrawPosMax = { imagePos.x + fDefaultSize.x, imagePos.y + fDefaultSize.y };
							drawList->AddRect(DefaultDrawPosMin, DefaultDrawPosMax, IM_COL32(222, 222, 222, 255), 0.0f, ImDrawFlags_None, 3.0f);
							ImGui::SetNextItemWidth(50.f);
							if (ImGui::DragFloat("##PosX", &m_fOffsetPos.x, 1.f, -FLT_MAX, FLT_MAX, "x:%.1f"))
								m_pPickingInfo->Set_Collider_Offset_Pos(m_fOffsetPos);
							ImGui::SameLine();
							ImGui::SetNextItemWidth(50.f);
							if (ImGui::DragFloat("##PosY", &m_fOffsetPos.y, 1.f, -FLT_MAX, FLT_MAX, "y:%.1f"))
								m_pPickingInfo->Set_Collider_Offset_Pos(m_fOffsetPos);
							ImGui::SameLine();
							ImGui::Text("Offset Pos");
							if (isAABB)
							{
								_float2 fDrawExtent = fExtent;
								fDrawExtent.x *= fRatio;
								fDrawExtent.y *= fRatio;
								ImVec2 DrawPosMin = { imagePos.x + (m_fOffsetPos.x * fRatio) + (fDefaultSize.x * 0.5f) - (fDrawExtent.x * 0.5f), imagePos.y + (m_fOffsetPos.y * fRatio) + (fDefaultSize.y * 0.5f) - (fDrawExtent.y * 0.5f) };
								ImVec2 DrawPosMax = { imagePos.x + (m_fOffsetPos.x * fRatio) + (fDefaultSize.x * 0.5f) + (fDrawExtent.x * 0.5f), imagePos.y + (m_fOffsetPos.y * fRatio) + (fDefaultSize.y * 0.5f) + (fDrawExtent.y * 0.5f) };
								drawList->AddRect(DrawPosMin, DrawPosMax, IM_COL32(255, 0, 0, 255), 0.0f, ImDrawFlags_None, 2.0f);
								ImGui::SetNextItemWidth(50.f);
								if (ImGui::DragFloat("##ExtentX", &fExtent.x, 1.f, -FLT_MAX, FLT_MAX, "x:%.1f"))
									m_pPickingInfo->Set_Collider_Extent(fExtent);
								ImGui::SameLine();
								ImGui::SetNextItemWidth(50.f);
								if (ImGui::DragFloat("##ExtentY", &fExtent.y, 1.f, -FLT_MAX, FLT_MAX, "y:%.1f"))
									m_pPickingInfo->Set_Collider_Extent(fExtent);
								ImGui::SameLine();
								ImGui::Text("Extent");
							}

							if (isSquare)
							{
								ImVec2 DrawPos = { imagePos.x + (m_fOffsetPos.x * fRatio) + (fDefaultSize.x * 0.5f), imagePos.y + (m_fOffsetPos.y * fRatio) + (fDefaultSize.y * 0.5f) };
								drawList->AddCircle(DrawPos, fRadius * fRatio, IM_COL32(255, 0, 0, 255));
								ImGui::SetNextItemWidth(110.f);
								if (ImGui::DragFloat("Radius", &fRadius, 1.f, -FLT_MAX, FLT_MAX, "%.1f"))
									m_pPickingInfo->Set_Collider_Radius(fRadius);

							}

						}
						Begin_Draw_ColorButton("NO_Style", (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
						if (ImGui::Button("Close"))
						{
							ImGui::CloseCurrentPopup();
						}
						End_Draw_ColorButton();

						ImGui::EndPopup();

					}
					End_Draw_ColorButton();

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
void C2DMap_Tool_Manager::TriggerSetting_Imgui(_bool bLock)
{
	ImGui::Begin("Trigger");
	{
		Begin_Draw_ColorButton("Save_FileStyle", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
		if (ImGui::Button("Trigger Save"))
		{
			Save_Trigger();
		}
		ImGui::SameLine();
		if (ImGui::BeginPopup("Save_Popup"))
		{
			ImGui::InputText("##Save File Name", m_szSaveFileName, MAX_PATH);
			ImGui::SameLine();
			Begin_Draw_ColorButton("Save_", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("Save"))
			{
			}
			End_Draw_ColorButton();

			ImGui::EndPopup();
		}
		End_Draw_ColorButton();
		Begin_Draw_ColorButton("Load_FileStyle", (ImVec4)ImColor::HSV(0.7f, 0.6f, 0.6f));
		if (ImGui::Button("Trigger Load"))
		{
			ImGui::OpenPopup("Load_Popup");
		}
		if (ImGui::BeginPopup("Load_Popup"))
		{
			ImGui::Text("All those Mesh Object will be deleted.");
			ImGui::Text("Do you want to proceed?");
			Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("OK"))
			{
				//Load_Trigger(L"../../Client/Bin/SettingFile/", L"Trigger");
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




		ImGui::SeparatorText("Trigger List");
		if (ImGui::BeginListBox("##Trigger List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			auto pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_Trigger");
			if (pLayer)
			{
				auto& TriggerList = pLayer->Get_GameObjects();
				_uint iLoop = 0;

				for_each(TriggerList.begin(), TriggerList.end(), [this, &iLoop](CGameObject* pGameObject) {
					_string strName = _string("Trigger_" + to_string(iLoop));
					_wstring wstrName = StringToWstring(strName);
					C2DTrigger_Sample* pTriggerObject = static_cast<C2DTrigger_Sample*>(pGameObject);
					if (pTriggerObject)
					{
						if (ImGui::Selectable(strName.c_str(), wstrName == m_arrSelectName[TRIGGER_LIST])) {
							if (wstrName != m_arrSelectName[TRIGGER_LIST])
							{
								m_arrSelectName[TRIGGER_LIST] = wstrName;
								m_pPickingTrigger = pTriggerObject;
							}
						}
					}
					iLoop++;
					});
			}

			ImGui::EndListBox();
		}



		Begin_Draw_ColorButton("Clear_All_ObjectStyle", (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
		if (!bLock && ImGui::Button("Clear All Trigger", ImVec2(-FLT_MIN, 1.7f * ImGui::GetTextLineHeightWithSpacing())))
		{
			ImGui::OpenPopup("Clear_Popup");
		}

		if (ImGui::BeginPopup("Clear_Popup"))
		{
			ImGui::Text("All those Trigger will be deleted.");
			ImGui::Text("Do you want to proceed?");
			Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("OK"))
			{
				//m_TriggerEvents.clear();

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

		if (nullptr != m_pPickingTrigger)
		{
			string strTriggerKey = m_pPickingTrigger->Get_TriggerKey();
			_int iTriggerIdx = 0;
			auto iter = find_if(m_TriggerEvents.begin(), m_TriggerEvents.end(), [&iTriggerIdx, &strTriggerKey](TRIGGER_EVENT& tEvent)->_bool {
				_bool bReturn = tEvent.strEventName == strTriggerKey;
				if (!bReturn)
					iTriggerIdx++;
				return bReturn;
				});
			if (iter == m_TriggerEvents.end())
			{
				iTriggerIdx = -1;
			}
			_vector vPos = m_pPickingTrigger->Get_FinalPosition();
			_float2 fPos = { XMVectorGetX(vPos),XMVectorGetY(vPos) };
			_float3 fScale = m_pPickingTrigger->Get_FinalScale();
			ImGui::SetNextItemWidth(100.f);
			if (ImGui::DragFloat("##TriggerPosX", &fPos.x, 1.f, -FLT_MAX, FLT_MAX, "x:%.1f"))
			{
				vPos = XMVectorSetX(vPos, fPos.x);
				m_pPickingTrigger->Set_Position(vPos);

			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.f);
			if (ImGui::DragFloat("##TriggerPosY", &fPos.y, 1.f, -FLT_MAX, FLT_MAX, "y:%.1f"))
			{
				vPos = XMVectorSetY(vPos, fPos.y);
				m_pPickingTrigger->Set_Position(vPos);
			}
			ImGui::SameLine();
			ImGui::Text("Trigger Position");

			ImGui::SetNextItemWidth(100.f);
			if (ImGui::DragFloat("##TriggerScaleX", &fScale.x, 1.f, 0.f, FLT_MAX, "x:%.1f"))
				m_pPickingTrigger->Set_Scale(fScale);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100.f);
			if (ImGui::DragFloat("##TriggerScaleY", &fScale.y, 1.f, 0.f, FLT_MAX, "y:%.1f"))
				m_pPickingTrigger->Set_Scale(fScale);
			ImGui::SameLine();
			ImGui::Text("Trigger Scale");

			ImGui::SeparatorText("Trigger Event List");
			if (ImGui::BeginListBox("##Trigger Selected Event List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				_uint iLoop = 0;
				for (auto tTriggerEvent : m_TriggerEvents)
				{
					if (ImGui::Selectable(tTriggerEvent.strEventName.c_str(), iLoop == iTriggerIdx))
					{
						if (iLoop != iTriggerIdx)
						{
							m_pPickingTrigger->Set_TriggerKey(tTriggerEvent.strEventName);
						}
					}
					iLoop++;
				}

				ImGui::EndListBox();
			}

		}
	}

	ImGui::End();
}

void C2DMap_Tool_Manager::TriggerEvent_Imgui(_bool bLock)
{
	ImGui::Begin("Trigger Event");
	{
		string m_strConditionNames[] = { "Collision Enter",
											"Collision",
											"Collision Exit", };
		string m_strEventNames[] = { "Object Spawn",
										"CutScene Event",
										"Map_Change",
										"Popup_UI",
		};


		ImGui::SeparatorText("Trigger Event List");
		if (ImGui::BeginListBox("##Trigger Event List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_uint iLoop = 0;
			for (auto tTriggerEvent : m_TriggerEvents)
			{
				if (ImGui::Selectable(tTriggerEvent.strEventName == "" ? "None" : tTriggerEvent.strEventName.c_str(), iLoop == m_SelectTriggerEventIdx))
				{
					if (iLoop != m_SelectTriggerEventIdx)
					{
						m_arrSelectName[TRIGGER_EVENT_LIST] = StringToWstring(tTriggerEvent.strEventName);
						m_SelectTriggerEventIdx = iLoop;
					}
				}
				iLoop++;
			}

			ImGui::EndListBox();
		}



		Begin_Draw_ColorButton("Create_TriggerEventStyle", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
		if (ImGui::Button("Trigger Event Create"))
		{
			TRIGGER_EVENT tEvent = { "TriggerEvent_" + std::to_string(m_TriggerEvents.size()) , 0, 0 };
			m_TriggerEvents.push_back(tEvent);

			m_arrSelectName[TRIGGER_EVENT_LIST] = StringToWstring(tEvent.strEventName);
			m_SelectTriggerEventIdx = (_int)m_TriggerEvents.size() - 1;
		}

		End_Draw_ColorButton();
		ImGui::SameLine();
		Begin_Draw_ColorButton("Delete_TriggerEventStyle", (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
		if (ImGui::Button("Trigger Event Delete"))
		{
			if (-1 != m_SelectTriggerEventIdx)
				m_TriggerEvents.erase(m_TriggerEvents.begin() + m_SelectTriggerEventIdx);

			m_SelectTriggerEventIdx = m_TriggerEvents.size() - 1 < m_SelectTriggerEventIdx ? (_int)m_TriggerEvents.size() : m_SelectTriggerEventIdx;
		}
		End_Draw_ColorButton();

		if (m_SelectTriggerEventIdx != -1)
		{
			TRIGGER_EVENT& tEvent = m_TriggerEvents[m_SelectTriggerEventIdx];


			ImGui::SeparatorText("Event Name");
			_char szName[MAX_PATH] = {};

			strcpy_s(szName, tEvent.strEventName.c_str());

			if (ImGui::InputText("##NewName", szName, MAX_PATH))
			{
				tEvent.strEventName = szName;
			}
			ImGui::SeparatorText("Condition Type");

			if (ImGui::BeginListBox("##Trigger Condition List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				_uint iLoop = 0;
				for (auto strConditionName : m_strConditionNames)
				{
					if (ImGui::Selectable(strConditionName.c_str(), iLoop == tEvent.uTriggerCondition))
					{
						if (iLoop != tEvent.uTriggerCondition)
						{
							tEvent.uTriggerCondition = iLoop;
						}
					}
					iLoop++;
				}

				ImGui::EndListBox();
			}
			ImGui::SeparatorText("Event Type");

			if (ImGui::BeginListBox("##Trigger EventType List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				_uint iLoop = 0;
				for (auto strEventName : m_strEventNames)
				{
					if (ImGui::Selectable(strEventName.c_str(), iLoop == tEvent.uTriggerEvent))
					{
						if (iLoop != tEvent.uTriggerEvent)
						{
							tEvent.uTriggerEvent = iLoop;
						}
					}
					iLoop++;
				}

				ImGui::EndListBox();
			}

		}

	}


	ImGui::End();
}


void C2DMap_Tool_Manager::Save(_bool _bSelected)
{
	string filename = m_szSaveFileName;
	string log = "";


	// 1. 레이어 검사

	auto pLayerMaps = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_2DMapObject");
	if (nullptr == pLayerMaps)
	{
		log = "Save Failed... Save Layer Empty! ";
		LOG_TYPE(log, LOG_ERROR);
		return;
	}
	auto Objects = pLayerMaps->Get_GameObjects();

	if (Objects.empty())
	{
		log = "Save Failed... Save Layer Empty! ";
		LOG_TYPE(log, LOG_ERROR);
		return;
	}
	// 1. 레이어 검사 END


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
	strFullFilePath = (m_strMapBinaryPath + m_pGameInstance->StringToWString(filename) + L".m2chc");
	log = "Save Start... File Name : ";
	log += filename;
	LOG_TYPE(log, LOG_SAVE);

	HANDLE	hFile = nullptr;
	hFile = CreateFile(strFullFilePath.c_str(),
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

	// 저장 데이터 - 1. 맵을 일단 이미지로 뽑는다. -> (fileName)_BackGround.dds
	// 저장 데이터 - 2. (fileName)_BackGround.dds 저장. MapName : (fileName)_BackGround.dds
	//			---	MapObj Start
	// 저장 데이터 - 3. 오브젝트 갯수
	// 저장 데이터 - 4. 모델인덱스
	// 저장 데이터 - 5. 모델 위치
	// 저장 데이터 - 6. 오버라이드 여부 
	// 저장 데이터 - 7. 오버라이드 요소 (이건 나중에? 필요해지면?? 일단 여부는 모두 false로)
	//			---	MapObj End
	// 저장 데이터 - 7. 트리거 (이건, 2D 콜리전 작업이 끝난 뒤)

	// 3. 세이브

	DWORD	dwByte(0);


	_char		szSaveMapName[MAX_PATH];

	_string strBackGroundName = filename + "_BackGround.dds";
	_wstring strModelPath = L"../../Client/Bin/Resources/Textures/Map/" + StringToWstring(strBackGroundName);
	// 1. 맵뽑기
	if (nullptr == m_pTileRenderObject)
		m_DefaultRenderObject->Texture_Output(strModelPath);
	else
		m_pTileRenderObject->Set_OutputPath(strModelPath);

	strcpy_s(szSaveMapName, strBackGroundName.c_str());

	//2. 맵 이름
	WriteFile(hFile, &szSaveMapName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);

	_uint iObjectCnt = (_uint)Objects.size();
	// 3. 오브젝트 갯수
	WriteFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

	for (auto& pObject : Objects)
	{
		// 4. 루프돌면서 직접 저장하세용.
		if (FAILED(static_cast<C2DMapObject*>(pObject)->Export(hFile)))
			CloseHandle(hFile);
	}

	CloseHandle(hFile);
	// 3. 세이브 END


	Load_SaveFileList();
}




C2DMapObject* C2DMap_Tool_Manager::Picking_2DMap()
{
	_float2 fCursorPos = m_pGameInstance->Get_CursorPos();
	if (m_DefaultRenderObject->IsCursor_In(fCursorPos))
	{
		auto pLayerMaps = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_2DMapObject");
		if (nullptr != pLayerMaps)
		{
			auto Objects = pLayerMaps->Get_GameObjects();

			for (auto& pObject : Objects)
			{
				C2DMapObject* pMapObject = static_cast<C2DMapObject*>(pObject);
				if (pMapObject->IsCursor_In(fCursorPos) && pMapObject != m_pPickingObject)
					return pMapObject;
			}
		}

	}

	return nullptr;
}

HRESULT C2DMap_Tool_Manager::Setting_TileMap(const _string _strFileMapJsonName)
{
	_uint iTileXSize = 0;
	_uint iTileYSize = 0;
	_uint iTileWidthInTiles = 0;
	_uint iTileHeightInTiles = 0;
	string arrAxisKey[2] = { "X","Y" };

	std::string filePathDialog = "../Bin/json/2DMapJson/Tile/";
	filePathDialog += _strFileMapJsonName + ".json";
	std::ifstream inputFile(filePathDialog);
	if (!inputFile.is_open()) {
		LOG_TYPE("TileData Load Error -> " + filePathDialog, LOG_ERROR);
		return E_FAIL;
	}

	vector<pair<_string, _int>> IndexsInfos;
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
									IndexsInfos.push_back(make_pair("", -1));
								}
								else
								{
									_string txt = IndexData["TileSet"]["ObjectName"];
									_uint iIndex = IndexData["PackedTileIndex"];
									IndexsInfos.push_back(make_pair(txt, iIndex));
								}
							}
						}
					}
				}
			}
		}
	}

	inputFile.close();

	if (!IndexsInfos.empty())
	{
		if (nullptr != m_pTileRenderObject)
			Event_DeleteObject(m_pTileRenderObject);



		C2DTile_RenderObject::TILE_RENDEROBJECT_DESC Desc = { };

		Desc.iMapSizeWidth = RTSIZE_BOOK2D_X;
		Desc.iMapSizeHeight = RTSIZE_BOOK2D_Y;


		Desc.iIndexCountX = iTileWidthInTiles;
		Desc.iIndexCountY = iTileHeightInTiles;
		Desc.iIndexSIzeX = RTSIZE_BOOK2D_X / iTileWidthInTiles;
		Desc.iIndexSIzeY = RTSIZE_BOOK2D_Y / iTileHeightInTiles;
		CGameObject* pGameObject = nullptr;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DTile_RenderObject"),
			LEVEL_TOOL_2D_MAP, L"Layer_Default", &pGameObject, &Desc)))
			return E_FAIL;

		if (pGameObject)
		{
			m_pTileRenderObject = static_cast<C2DTile_RenderObject*>(pGameObject);

			vector<_string> TexturesTags;
			// TextureIndex, UVIndex
			vector<XMINT2> FinalIndexs;
			// 중복소팅


			for_each(IndexsInfos.begin(), IndexsInfos.end(), [&TexturesTags, &FinalIndexs](const pair<_string, _int>& Pair)
				{
					auto iter = find_if(TexturesTags.begin(), TexturesTags.end(), [&Pair](const _string& strTextureName)->_bool {
						return Pair.first == strTextureName;
						});

					if (iter == TexturesTags.end() && (-1 != Pair.second))
						TexturesTags.push_back(Pair.first);
				});
			// 인덱스로 변경 구성 
			for_each(IndexsInfos.begin(), IndexsInfos.end(), [&TexturesTags, &FinalIndexs](const pair<_string, _int>& Pair)
				{
					_int iTextureIndex = 0;
					auto iter = find_if(TexturesTags.begin(), TexturesTags.end(), [&iTextureIndex, &Pair](const _string& strTextureName)->_bool {
						if (Pair.first != strTextureName)
							iTextureIndex++;
						return Pair.first == strTextureName;
						});
					FinalIndexs.push_back({ -1 == Pair.second ? -1 : iTextureIndex, Pair.second });
				});


			for (auto& strTextureTag : TexturesTags)
			{
				HRESULT		hr = E_FAIL;

				C2DTile_RenderObject::TILE_TEXTURE_INFO tInfo = {};


				_string strTexturePath = "../Bin/Resources/TileMap/";
				//PaperTileSet'CO1_P0910_TD_Tilemap_TileSet'
				_string strTextureSetTag = OutName(strTextureTag);
				// -> CO1_P0910_TD_Tilemap_TileSet

				//image Info Load 1. ##ImageName##_TileSet.json Load -> TileTexture Count & Size

				const std::string filePathDialog = strTexturePath + strTextureSetTag + ".json";
				std::ifstream inputFile(filePathDialog);
				if (!inputFile.is_open()) {
					LOG_TYPE("TileSet json Read Error -> " + filePathDialog, LOG_ERROR);
					return E_FAIL;
				}

				json TileImageJson;
				inputFile >> TileImageJson;
				if (TileImageJson.is_array())
				{
					for (auto& ChildJson : TileImageJson)
					{
						if (
							ChildJson.is_object() &&
							ChildJson.contains("Type") &&
							ChildJson.contains("Properties") &&
							ChildJson["Type"] == "PaperTileSet")
						{
							auto PropertiesJson = ChildJson["Properties"];

							if (
								PropertiesJson.contains("TileSheet")
								&&
								PropertiesJson["TileSheet"].contains("ObjectName")
								)
							{
								//"Texture2D'CO1_P0910_TD_Tilemap'"-
								_string strObjectName = PropertiesJson["TileSheet"]["ObjectName"];
								strObjectName = OutName(strObjectName);

								// -> CO1_P0910_TD_Tilemap.dds
								strObjectName = strObjectName + ".dds";
								tInfo.strTextureTag = StringToWstring(strObjectName);
							}
							if (PropertiesJson.contains("TileSize"))
							{
								tInfo.iTileIndexSizeX = PropertiesJson["TileSize"]["X"];
								tInfo.iTileIndexSizeY = PropertiesJson["TileSize"]["Y"];
							}

							if (PropertiesJson.contains("WidthInTiles"))
								tInfo.iTileIndexCountX = PropertiesJson["WidthInTiles"];
							if (PropertiesJson.contains("HeightInTiles"))
								tInfo.iTileIndexCountY = PropertiesJson["HeightInTiles"];

						}
					}
				}
				// -> CO1_P0910_TD_Tilemap.dds

				ID3D11ShaderResourceView* pSRV = { nullptr };

				// image Load
				hr = CreateDDSTextureFromFile(m_pDevice, (StringToWstring(strTexturePath) + tInfo.strTextureTag).c_str(), nullptr, &pSRV);
				if (SUCCEEDED(hr))
				{

					//image Info Load 2. ShaderResourceView -> Texture2D -> Full Image SIze
					ID3D11Resource* pResource = nullptr;
					pSRV->GetResource(&pResource);
					if (pResource)
					{
						ID3D11Texture2D* pTexture2D = nullptr;
						HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture2D));
						pResource->Release();

						if (SUCCEEDED(hr) && pTexture2D)
						{
							D3D11_TEXTURE2D_DESC desc;
							pTexture2D->GetDesc(&desc);
							tInfo.iTileTextureSizeX = desc.Width;
							tInfo.iTileTextureSizeY = desc.Height;
						}
					}
					m_pTileRenderObject->Set_Texture(tInfo, pSRV);
				}
				else
				{
					LOG_TYPE("Tile dds Load Error -> " + strTexturePath, LOG_ERROR);
					return E_FAIL;
				}
			}


			for (_uint i = 0; i < (_uint)FinalIndexs.size(); ++i)
			{
				if (FAILED(m_pTileRenderObject->Set_Index(i, FinalIndexs[i].x, FinalIndexs[i].y)))
					return E_FAIL;
			}
			return S_OK;

		}

	}
	else
	{
		LOG_TYPE("TileSet json Read Error -> " + filePathDialog, LOG_ERROR);
		return E_FAIL;
	}

	return S_OK;
}

C2DMapObjectInfo* C2DMap_Tool_Manager::Find_Info(const _wstring _strTag)
{
	for (auto pInfo : m_ObjectInfoLists)
		if (ContainString(WstringToString(_strTag), pInfo->Get_SearchTag()))
			return pInfo;
	return nullptr;
}

HRESULT C2DMap_Tool_Manager::Update_Model_Index()
{
	auto pLayerMaps = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_2DMapObject");
	if (nullptr != pLayerMaps)
	{
		auto Objects = pLayerMaps->Get_GameObjects();

		for (auto& pObject : Objects)
		{
			C2DMapObject* pMapObject = static_cast<C2DMapObject*>(pObject);
			if (FAILED(pMapObject->Update_Model_Index()))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT C2DMap_Tool_Manager::Change_RenderGroup(_float2 _fRenderTargetSize)
{
	/* Target Book2D */
	if (FAILED(m_pGameInstance->Add_RenderTarget(m_strRTKey, (_uint)_fRenderTargetSize.x, (_uint)_fRenderTargetSize.y, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(m_strDSVKey, (_uint)_fRenderTargetSize.x, (_uint)_fRenderTargetSize.y)))
		return E_FAIL;

	/* MRT_Book_2D*/
	if (FAILED(m_pGameInstance->Add_MRT(m_strMRTKey, m_strRTKey)))
		return E_FAIL;


	CRenderGroup_2D::RG_MRT_DESC RG_Book2DDesc;
	RG_Book2DDesc.iRenderGroupID = RENDERGROUP::RG_2D;
	RG_Book2DDesc.iPriorityID = PRIORITY_2D::PR2D_BOOK_SECTION;
	RG_Book2DDesc.isViewportSizeChange = true;
	RG_Book2DDesc.strMRTTag = m_strMRTKey;
	RG_Book2DDesc.pDSV = m_pGameInstance->Find_DSV(m_strDSVKey);
	RG_Book2DDesc.vViewportSize = _fRenderTargetSize;
	RG_Book2DDesc.isClear = true;
	if (nullptr == RG_Book2DDesc.pDSV)
	{
		MSG_BOX("Book2D DSV가 없대.");
		return E_FAIL;
	}
	m_p2DRenderGroup = CRenderGroup_2D::Create(m_pDevice, m_pContext, &RG_Book2DDesc);
	if (nullptr == m_p2DRenderGroup)
	{
		MSG_BOX("Failed Create PR3D_BOOK2D");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(m_p2DRenderGroup->Get_RenderGroupID(),
		m_p2DRenderGroup->Get_PriorityID(), m_p2DRenderGroup)))
		return E_FAIL;


	if (FAILED(m_DefaultRenderObject->Change_RenderGroup(_fRenderTargetSize, m_p2DRenderGroup->Get_RenderGroupID(),
		m_p2DRenderGroup->Get_PriorityID())))
		return E_FAIL;


	return S_OK;
}

HRESULT C2DMap_Tool_Manager::Clear_RenderGroup()
{

	m_pGameInstance->Erase_RenderGroup_New(RENDERGROUP::RG_2D, PRIORITY_2D::PR2D_BOOK_SECTION);
	m_pGameInstance->Erase_DSV_ToRenderer(m_strDSVKey);
	m_pGameInstance->Erase_RenderTarget(m_strRTKey);
	m_pGameInstance->Erase_MRT(m_strMRTKey);

	Safe_Release(m_p2DRenderGroup);

	return S_OK;
}

_bool C2DMap_Tool_Manager::Check_Import_Egnore_2DObject(const _string& _strTag)
{
	auto iter = find_if(m_Egnore2DObjectrTags.begin(), m_Egnore2DObjectrTags.end(), [&_strTag]
	(const _string& strEgnoreObjectTag)->_bool {
			return ContainString(_strTag,strEgnoreObjectTag);
		});
	return iter != m_Egnore2DObjectrTags.end();
}



void C2DMap_Tool_Manager::Import(const _string& _strFileName, json& _MapFileJson, _float4* _fColor)
{

	string arrAxisKey[3] = { "X","Y","Z" };
	string arrColorKey[4] = { "R","G","B","A" };

	vector<pair<_string, _float3>> MapObjectInfos;
	vector<pair<_string, _float4>> MapColors;

	_string strTileMapName = "";

	_bool	isBook = true;
	_float2 fLevelSizePixels = {-1.f, -1.f};
	_float2 fRenderResolution = { -1.f, -1.f };



#pragma region Import - Json 읽어들이기

	if (_MapFileJson.is_array())
	{
		for (auto ChildJson : _MapFileJson)
		{
			if (
				ChildJson.is_object() &&
				ChildJson.contains("Type") &&
				ChildJson.contains("Properties") &&
				ChildJson.contains("Outer") &&
				ChildJson["Type"] == "MinigameController_BP_C")
			{
				auto TargetJson = ChildJson["Properties"];



				if (TargetJson.contains("CurrentAssignedBook"))
				{
					if (TargetJson["CurrentAssignedBook"].contains("SubPathString"))
					{
						_string strStageText = TargetJson["CurrentAssignedBook"]["SubPathString"];


						// 책 일때
						if (ContainString(strStageText, "Book_MAIN"))
						{
							isBook = true;
						}
						// 렌더타겟 일때
						else
						{
							isBook = false;
						}
					}
				}

				if (TargetJson.contains("LevelSizePixels"))
				{
					_float2 fScale = {};
					for (_uint i = 0; i < 2; i++)
					{
						_float fValue = TargetJson["fLevelSizePixels"].at(arrAxisKey[i]);
						reinterpret_cast<_float*>(&fScale.x)[i] = fValue;
					}
					int a = 1;
				}
				if (TargetJson.contains("RenderResolution"))
				{
					_float2 fScale = {};
					for (_uint i = 0; i < 2; i++)
					{
						_float fValue = TargetJson["RenderResolution"].at(arrAxisKey[i]);
						reinterpret_cast<_float*>(&fScale.x)[i] = fValue;
					}
					int a = 1;
				}


			}

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
						//if (i == 0)
						//{
						//	fValue = ((_int)fValue % RTSIZE_BOOK2D_X + RTSIZE_BOOK2D_X) % RTSIZE_BOOK2D_X;
						//}
						//else if (i == 1)
						//{
						//	fValue = ((_int)fValue % RTSIZE_BOOK2D_Y + RTSIZE_BOOK2D_Y) % RTSIZE_BOOK2D_Y;

						//}
						memcpy((&fValuePos.x) + i, &fValue, sizeof(_float));
					}

					if (!Check_Import_Egnore_2DObject(strText))
						MapObjectInfos.push_back(make_pair(strText, fValuePos));
				}

			}
#pragma region Find Map Color


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
								MapColors.push_back(make_pair(strText, fValueColor));
							}
						}
					}

				}

			}
#pragma endregion
			else if (
				ChildJson.is_object() &&
				ChildJson.contains("Type") &&
				ChildJson.contains("Properties") &&
				ChildJson.contains("Outer") &&
				ChildJson["Type"] ==
				"PaperTileMapComponent")
			{
				if (ChildJson["Properties"].contains("TileMap"))
				{
					auto TargetJson = ChildJson["Properties"];

					strTileMapName = TargetJson["TileMap"]["ObjectName"];
					strTileMapName = OutName(strTileMapName);
				}
				else
				{

				}

			}
		}
	}

#pragma endregion


#pragma region 렌더타겟 생성



	CRenderGroup_2D* pTargetRenderGroup = nullptr;

	if (m_p2DRenderGroup != nullptr)
	{
		_float2 fLgcRenderTargetSize = m_p2DRenderGroup->Get_RenderTarget_Size();
	
		if (fLevelSizePixels.x != fLgcRenderTargetSize.x
			||
			fLevelSizePixels.y != fLgcRenderTargetSize.y)
		{
			Clear_RenderGroup();
			Change_RenderGroup(fLevelSizePixels);
		}
	}
#pragma endregion


#pragma region 타일 맵 검사 및 생성

	if (strTileMapName != "")
	{
		if (FAILED(Setting_TileMap(strTileMapName)))
		{
			LOG_TYPE("Tile Map Load Error! -> " + strTileMapName, LOG_ERROR);
			return;
		}
	}
	else
	{
		LOG_TYPE("Tile Map Not Exist! -> " + strTileMapName, LOG_ERROR);

	}
#pragma endregion

#pragma region 맵 컬러 검사 및 반영(하나만 함)

	if (!MapColors.empty())
	{
		*_fColor = MapColors.front().second;
		m_DefaultRenderObject->Set_Default_Render_Mode();
		m_DefaultRenderObject->Set_Color(MapColors.front().second);
	}
	else
	{
		LOG_TYPE("Tile Color Not Exist! -> " + strTileMapName, LOG_ERROR);

		m_DefaultRenderObject->Set_Color(*_fColor);
	}

#pragma endregion

#pragma region 맵 오브젝트 소팅, 반영

	if (!MapObjectInfos.empty())
	{
		_float2 fMin = { MapObjectInfos.front().second.x, MapObjectInfos.front().second.y };
		_float2 fMax = fMin;
		for_each(MapObjectInfos.begin(), MapObjectInfos.end(), [&fMin, &fMax](pair<_string, _float3>& Pair) {
			fMin.x = min(fMin.x, Pair.second.x);
			fMin.y = min(fMin.y, Pair.second.y);
			fMax.x = max(fMax.x, Pair.second.x);
			fMax.y = max(fMax.y, Pair.second.y);
			});
		std::sort(MapObjectInfos.begin(), MapObjectInfos.end(), [](pair<_string, _float3>& FirstPair, pair<_string, _float3>& SecoundPair) {
			return FirstPair.second.x > SecoundPair.second.x;
			});
		_float2 fMapCenterOffset = { (fMin.x + fMax.x) * 0.5f,(fMin.y + fMax.y) * 0.5f };
		for_each(MapObjectInfos.begin(), MapObjectInfos.end(), [&fMapCenterOffset](pair<_string, _float3>& Pair) {
			Pair.second.x -= fMapCenterOffset.x;
			Pair.second.y -= fMapCenterOffset.y;
			});

		vector<_string> NotExistTextures;

		for (auto& Pair : MapObjectInfos)
		{
			C2DMapObject::MAPOBJ_2D_DESC NormalDesc = {};
			NormalDesc.strProtoTag = StringToWstring(Pair.first);
			NormalDesc.fDefaultPosition.x = Pair.second.x;
			NormalDesc.fDefaultPosition.y = Pair.second.y;
			NormalDesc.fRenderTargetSize = { (_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y };
			NormalDesc.iCurLevelID = LEVEL_TOOL_2D_MAP;
			NormalDesc.pInfo = Find_Info(NormalDesc.strProtoTag);
			NormalDesc.iRenderGroupID_2D = m_p2DRenderGroup->Get_RenderGroupID();
			NormalDesc.iPriorityID_2D = m_p2DRenderGroup->Get_PriorityID();

			CGameObject* pGameObject = nullptr;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_2D_MAP, TEXT("Prototype_GameObject_2DMapObject"),
				LEVEL_TOOL_2D_MAP,
				L"Layer_2DMapObject",
				&pGameObject,
				(void*)&NormalDesc)))
			{
				_string strNotExistTextureMessage = WstringToString(_wstring(L"2D Texture Info not Exist ->") + NormalDesc.strProtoTag);
				LOG_TYPE(strNotExistTextureMessage, LOG_ERROR);
				NotExistTextures.push_back(strNotExistTextureMessage);
			}

		}
		wstring strResultFileFath = L"../Bin/json/Result/";

		if (!NotExistTextures.empty())
			m_pTaskManager->Export_Result(strResultFileFath, StringToWstring(_strFileName), NotExistTextures);
		LOG_TYPE(_string("=====  2D Map Read End  =====") + _strFileName, LOG_LOAD);

	}
#pragma endregion


}

void C2DMap_Tool_Manager::Load_3D_Map(_bool _bSelected)
{
	//Object_Clear(false);

	string filename = m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]);
	string log = "";

	log = "Load Start... File Name : ";
	log += filename;
	_wstring strFullFilePath = MAP_3D_DEFAULT_PATH;
	strFullFilePath += L"/" + m_arrSelectName[SAVE_LIST] + L".mchc";



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
			NormalDesc.iCurLevelID = LEVEL_TOOL_2D_MAP;
			NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TOOL_2D_MAP;
			NormalDesc.tTransform3DDesc.isMatrix = true;
			NormalDesc.tTransform3DDesc.matWorld = vWorld;


			CGameObject* pGameObject = nullptr;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
				LEVEL_TOOL_2D_MAP,
				strLayerTag,
				&pGameObject,
				(void*)&NormalDesc);

			if (pGameObject)
			{
				static_cast<CMapObject*>(pGameObject)->Load_Override_Color(hFile);
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


void C2DMap_Tool_Manager::Load(_bool _bSelected)
{
	Object_Clear(false);

	string filename = m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]);
	string log = "";

	log = "Load Start... File Name : ";
	log += filename;
	_wstring strFullFilePath = MAP_2D_DEFAULT_PATH;
	strFullFilePath += m_arrSelectName[SAVE_LIST] + L".m2chc";



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
		LOG_TYPE(log, LOG_ERROR);
		return;
	}


	DWORD	dwByte(0);

	_char		szSaveMapName[MAX_PATH];

	_uint iObjectCnt = 0;
	ReadFile(hFile, &szSaveMapName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);

	// 텍스쳐로 렌더하는 모드로 바까주고
	m_DefaultRenderObject->Set_Texture_Mode(szSaveMapName);
	// 타일 렌더하는 객체 지우고 !
	Event_DeleteObject(m_pTileRenderObject);
	m_pTileRenderObject = nullptr;

	_uint iLayerCount = 0;

	ReadFile(hFile, &iLayerCount, sizeof(_uint), &dwByte, nullptr);

	WriteFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iLayerCount; ++i)
	{

		C2DMapObject* pObject = C2DMapObject::Create(m_pDevice, m_pContext, hFile, m_ObjectInfoLists);
		if (nullptr != pObject)
		{
			Event_CreateObject(LEVEL_TOOL_2D_MAP, L"Layer_2DMapObject", pObject);
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
	if (_bSelected)
		LOG_TYPE("Object Clear", LOG_DELETE);
	m_pPickingObject = nullptr;
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_2DMapObject");
	if (pLayer != nullptr)
	{
		auto ObjList = pLayer->Get_GameObjects();
		for_each(ObjList.begin(), ObjList.end(), [](CGameObject* pGameObject)
			{
				OBJECT_DESTROY(pGameObject);
			});
	}
}


void C2DMap_Tool_Manager::Save_Popup()
{
	ImGui::OpenPopup("Save_Popup");
	strcpy_s(m_szSaveFileName, m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]).c_str());
}

void C2DMap_Tool_Manager::Save_Trigger()
{
	_tchar originalDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, originalDir);
	SetCurrentDirectory(originalDir);
	_wstring strTriggerPath = L"../../Client/Bin/MapSaveFiles/2D/Trigger/";
	_wstring strTriggerName = m_arrSelectName[SAVE_LIST] + L".json";
	_tchar szName[MAX_PATH];
	lstrcpy(szName, strTriggerName.c_str());

	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szName;
	ofn.nMaxFile = sizeof(szName);
	ofn.lpstrFilter = L".json\0*.y\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	wstring strPath = strTriggerPath;
	if (GetFullPathName(strTriggerPath.c_str(), MAX_PATH, originalDir, NULL))
		ofn.lpstrInitialDir = originalDir;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn))
	{
		json Result = json::array();

		auto pLayerMaps = m_pGameInstance->Find_Layer(LEVEL_TOOL_2D_MAP, L"Layer_Trigger");

		auto& pTriggers = pLayerMaps->Get_GameObjects();

		for (auto& Trigger : pTriggers) {

			C2DTrigger_Sample* pTrigger = static_cast<C2DTrigger_Sample*>(Trigger);

			const _string& strTriggerKey = pTrigger->Get_TriggerKey();

			auto iter = find_if(m_TriggerEvents.begin(), m_TriggerEvents.end(), [&strTriggerKey](TRIGGER_EVENT& _tTriggerEvent) {
				return strTriggerKey == _tTriggerEvent.strEventName;
				});
			if (iter == m_TriggerEvents.end())
				continue;

			TRIGGER_EVENT& tCurEvent = *iter;

			_vector vPosition = Trigger->Get_FinalPosition();
			_float3 fScale = pTrigger->Get_FinalScale();
			json Trigger_json;
			auto& Collider_Json = Trigger_json["Collider_Info"];
			Trigger_json["Trigger_Coordinate"] = COORDINATE_2D;
			Trigger_json["Trigger_Type"] = tCurEvent.uTriggerEvent;
			Trigger_json["Trigger_EventTag"] = strTriggerKey;
			Trigger_json["Trigger_ConditionType"] = tCurEvent.uTriggerCondition;
			Trigger_json["Fillter_MyGroup"] = 2;
			Trigger_json["Fillter_OtherGroupMask"] = 128;
			Collider_Json["Position"] = { XMVectorGetX(vPosition), XMVectorGetY(vPosition)};
			Collider_Json["Scale"] = { fScale.x, fScale.y };

			//auto& Collider_Json = Trigger_json["Event_Param_Info"];

			Result.push_back(Trigger_json);
		}

		ofstream file(szName);

		if (!file.is_open())
		{
			MSG_BOX("파일을 저장할 수 없습니다.");
			file.close();
			return;
		}

		file << Result.dump(4);
		file.close();
	}

}


void C2DMap_Tool_Manager::Load_2DModelList()
{

	m_ObjectInfoLists.clear();
	_wstring wstrPath = MAP_2D_DEFAULT_PATH;
	wstrPath += L"ComponentTagMatching/Chapter1_TagMatchingData.json";


	const std::string strPath = WstringToString(wstrPath);
	const string strFileName = WstringToString(Get_FileName_From_Path(wstrPath).first);
	std::ifstream inputFile(strPath);
	if (!inputFile.is_open()) {
		throw std::runtime_error("json Error :  " + strPath);
		return;
	}

	json jsonDialogs;
	inputFile >> jsonDialogs;
	if (jsonDialogs.is_array())
	{
		_uint iIndex = 0;
		for (auto ChildJson : jsonDialogs)
		{
			if (ChildJson.is_object())
			{
				C2DMapObjectInfo* pInfo = C2DMapObjectInfo::Create(ChildJson);

				m_ObjectInfoLists.push_back(pInfo);
				pInfo->Set_ModelIndex(iIndex);
			}
			iIndex++;
		}
	}

}

void C2DMap_Tool_Manager::Save_2DModelList()
{
	json Outputjson = json::array();
	for (auto pModelInfo : m_ObjectInfoLists)
	{
		json ObjJson;
		if (SUCCEEDED(pModelInfo->Export(ObjJson)))
			Outputjson.push_back(ObjJson);
	}
	_wstring wstrPath = MAP_2D_DEFAULT_PATH;
	wstrPath += L"ComponentTagMatching/Chapter1_TagMatchingData.json";


	const std::string strPath = WstringToString(wstrPath);

	std::ofstream file(strPath);
	if (file.is_open())
	{
		file << Outputjson.dump(1);
		file.close();
	}
}

void C2DMap_Tool_Manager::Load_SaveFileList()
{
	m_SaveFileLists.clear();
	for (const auto& entry : ::recursive_directory_iterator(m_strMapBinaryPath))
	{
		if (entry.path().extension() == ".m2chc")
		{
			wstring strKey = entry.path().stem().wstring();
			m_SaveFileLists.push_back(strKey);
		}
	}
}

void C2DMap_Tool_Manager::Load_String()
{

#pragma region Active


	_wstring strModelPath = L"../../Client/Bin/MapSaveFiles/2D/Default2DOption/Active_Data.json";
	std::ifstream inputFile(strModelPath);

	if (!inputFile.is_open())
	{
		throw std::runtime_error("json Error : Active_Data.json");
		return;
	}
	json ActiveJson;
	inputFile >> ActiveJson;
	if (ActiveJson.is_array())
	{
		m_ActiveTypeTexts.reserve(ActiveJson.size());
		for (auto& ChildJson : ActiveJson)
			m_ActiveTypeTexts.push_back(ChildJson["ActiveName"]);
	}
#pragma endregion
	m_ModelTypeTexts.resize(CModel::ANIM_TYPE::LAST);
	m_ColliderTypeTexts.resize(CCollider::TYPE_LAST);

	m_ColliderTypeTexts[CCollider::AABB_2D] = "Collider_AABB";
	m_ColliderTypeTexts[CCollider::CIRCLE_2D] = "Collider_CIRCLE";

	m_ModelTypeTexts[(_uint)CModel::ANIM_TYPE::ANIM] = "Anim";
	m_ModelTypeTexts[(_uint)CModel::ANIM_TYPE::NONANIM] = "NonAnim";
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
	Safe_Release(m_pTaskManager);
	for (auto pInfo : m_ObjectInfoLists)
		Safe_Release(pInfo);
	__super::Free();
}
