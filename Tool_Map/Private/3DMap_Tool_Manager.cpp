#include "stdafx.h"
#include "3DMap_Tool_Manager.h"
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
#include <commdlg.h>
using namespace std::filesystem;




C3DMap_Tool_Manager::C3DMap_Tool_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext) :
	m_pDevice(_pDevice),
	m_pContext(_pContext), m_pGameInstance(Engine::CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT C3DMap_Tool_Manager::Initialize(CImguiLogger* _pLogger)
{
	// Logger 등록
	m_pLogger = _pLogger;
	Safe_AddRef(m_pLogger);

	ZeroMemory(m_szSaveFileName,sizeof(m_szSaveFileName));
	ZeroMemory(m_szImportLayerTag,sizeof(m_szSaveFileName));


	m_pGameInstance->Set_DebugRender(true);

	// 임구이 크기 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(600, 1200), ImVec2(FLT_MAX, FLT_MAX));

	// 모델 리스트 불러오기
	Load_ModelList();
	Load_SaveFileList();

	m_DefaultEgnoreLayerTags.push_back(L"Layer_Cell");
	m_DefaultEgnoreLayerTags.push_back(L"Layer_Camera");

	
	m_pMapParsingManager = CTask_Manager::Create(m_pDevice, m_pContext, m_pLogger);
	if (nullptr == m_pMapParsingManager)
		return E_FAIL;

	return S_OK;
}

void C3DMap_Tool_Manager::Update_Tool()
{
	// 입력 설정 Navigation Tool Mode - Obejct Tool Mode 전환하여. 나중에 씬 분리할것
	Input_Object_Tool_Mode();

	// 임구이 화면 구성
	Update_Imgui_Logic();

	m_pMapParsingManager->Update();

}


void C3DMap_Tool_Manager::Update_Imgui_Logic()
{
	// 참 조 정 리 
	for (_uint i = 0 ; i < (_uint)OBJECT_END; ++i)
	{
		if (m_arrObjects[i] && m_arrObjects[i]->Is_Dead())
			m_arrObjects[i] = nullptr;
	}
	
	
	Navigation_Imgui(m_arrObjects[OBJECT_PREVIEW]);
	Object_Create_Imgui();
	SaveLoad_Imgui();
	Model_Imgui();
}



void C3DMap_Tool_Manager::Input_Object_Tool_Mode()
{
	HWND hWnd = GetFocus();
	auto& io = ImGui::GetIO();

	if (nullptr != hWnd)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			if (m_arrObjects[OBJECT_PICKING])
			{
				OBJECT_DESTROY(m_arrObjects[OBJECT_PICKING]);
				m_arrObjects[OBJECT_PICKING] = nullptr;
			}
		}

		if (ImGui::IsKeyPressed(ImGuiKey_C))
		{
			if (!m_arrObjects[OBJECT_PREVIEW])
				Object_Open_PreviewMode();
		}


	}


	if (nullptr != hWnd && !io.WantCaptureMouse)
	{
		// ESC -> 모든 모드 해제.
		if (KEY_DOWN(KEY::ESC))
		{
			Object_Clear_PickingMode();
			Object_Close_PreviewMode();
		}


		//if (FAILED(hrTerrainPicking))
		//	return;
		if (!m_arrObjects[OBJECT_PREVIEW])
		{
			if (MOUSE_DOWN(MOUSE_KEY::LB) && !ImGuizmo::IsUsing())
				Object_Open_PickingMode();
		}
		else
		{
			HRESULT hrTerrainPicking = Picking_On_Terrain(&m_fPickingPos);

			// 프리뷰 모드 코드.
			_float3 fPos = m_fPickingPos;
			fPos.y += 0.5f;
			m_arrObjects[OBJECT_PREVIEW]->Set_Position(XMVectorSetW(XMLoadFloat3(&fPos), 1.f));
			m_fPreviewPos = fPos;
			if (MOUSE_DOWN(MOUSE_KEY::LB))
			{
				// 실제 설치.
				m_arrObjects[OBJECT_PREVIEW]->Create_Complete();
				m_arrObjects[OBJECT_PREVIEW] = nullptr;
			}

		}

	}

}

void C3DMap_Tool_Manager::Object_Create_Imgui(_bool _bLock)
{
	ImGui::Begin("Object");
	{
		ImGui::SeparatorText("Object List");

		static char searchBuffer[128] = ""; // 검색어 입력을 위한 버퍼

		// 검색어에 따라 필터링된 항목을 저장할 임시 벡터
		static std::vector<pair<_wstring, _wstring>> filteredItems;

		// 검색 입력 필드
		ImGui::InputText("##Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));
		filteredItems.clear();
		_wstring searchTerm  = StringToWstring(searchBuffer);
		std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
		for (const auto& item : m_ObjectFileLists)
		{
			_wstring lowerItem = item.first;
			std::transform(lowerItem.begin(), lowerItem.end(), lowerItem.begin(), ::tolower);
			if (lowerItem.find(searchTerm) != _wstring::npos)
				filteredItems.push_back(item);
		}

		if (ImGui::BeginListBox("##Object List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const auto& PairFileInfo : filteredItems)
			{
				_char szName[MAX_PATH] = {};

				WideCharToMultiByte(CP_ACP, 0, PairFileInfo.first.c_str(), -1, szName, (_int)(lstrlen(PairFileInfo.first.c_str())), NULL, NULL);

				if (ImGui::Selectable(szName, PairFileInfo.first == m_arrSelectName[CREATE_OBJECT])) {
					if (m_arrSelectName[CREATE_OBJECT] == PairFileInfo.first)
						m_arrSelectName[CREATE_OBJECT] = L"";
					else
					{
						m_arrSelectName[CREATE_OBJECT] = PairFileInfo.first;
					}
				}
				_bool isClicked = false;
				if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("##ModelPreviewPopup");

					isClicked = true;
				}

				if (isClicked)
				{
					_float2 fCursorPos = m_pGameInstance->Get_CursorPos();
					ImGui::SetNextWindowPos({ fCursorPos.x + 150.f, fCursorPos.y + 100.f });
				}
				

			}
			if (ImGui::BeginPopup("##ModelPreviewPopup"))
			{

				ImGui::Text("All those Object will be deleted.");
				ImGui::Text("Do you want to proceed?");

				Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
				if (ImGui::Button("OK"))
				{
					ImGui::CloseCurrentPopup();
				}
				End_Draw_ColorButton();

				Begin_Draw_ColorButton("NO_Style", (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
				if (ImGui::Button("NO"))
				{
					ImGui::CloseCurrentPopup();
				}
				End_Draw_ColorButton();

				ImGui::EndPopup();
			}
			ImGui::EndListBox();
		}


		Begin_Draw_ColorButton("Create_Obj_Mode", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
		if (!_bLock &&
			ActiveButton(ALIGN_SQUARE, m_arrObjects[OBJECT_PREVIEW], "Create Object Mode Off", "Create Object Mode On"))
		{
			if (!m_arrObjects[OBJECT_PREVIEW])
				Object_Open_PreviewMode();
			else
				Object_Close_PreviewMode();
		}
		End_Draw_ColorButton();


		Begin_Draw_ColorButton("Clear_All_ObjectStyle", (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));

		if (!_bLock
			&&
			StyleButton(ALIGN_SQUARE, "Clear Layer Object")
			)
		{
			ImGui::OpenPopup("Clear_Popup");
			Init_Egnore_Layer();

		}
		if(ImGui::BeginPopup("Clear_Popup"))
		{
			auto pLayerMaps = m_pGameInstance->Get_Layers_Ptr();


			vector<wstring> strSaveLayerTags;

			ImGui::SeparatorText("Clear Layers");
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

			ImGui::Text("All Selected Layer will be deleted.");
			ImGui::Text("Do you want to proceed?");

			Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("Clear"))
			{
				Object_Clear();
				ImGui::CloseCurrentPopup();
			}
			End_Draw_ColorButton();

			Begin_Draw_ColorButton("NO_Style", (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			End_Draw_ColorButton();

			ImGui::EndPopup();
		}
		End_Draw_ColorButton();



		Begin_Draw_ColorButton("##File Parsing", (ImVec4)ImColor::HSV(0.8f, 0.6f, 0.6f));
		if (!_bLock
			&&
			StartPopupButton(ALIGN_SQUARE, ".umap File Parsing"))
		{
			_int iCurrent = 1;
			string strLayerName[] = { "Layer_MapObject", "Layer_Room_Environment", "Layer_Outside_Environment", };
			if (ImGui::BeginListBox("##LayerNameList", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (const auto& fileName : strLayerName)
				{
					if (ImGui::Selectable(fileName.c_str(), false))
						strcpy_s(m_szSaveFileName, fileName.c_str());
				}
				ImGui::EndListBox();
			}

			ImGui::InputText("Layer Tag", m_szSaveFileName, MAX_PATH);
			ImGui::SameLine();
			Begin_Draw_ColorButton("##Import", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("Import"))
			{
				const wstring strLayerTag = m_pGameInstance->StringToWString(m_szSaveFileName);
				m_pMapParsingManager->Open_ParsingDialog(strLayerTag);
			}
			End_Draw_ColorButton();

			ImGui::EndPopup();
		}
		End_Draw_ColorButton();


		CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(LEVEL_TOOL_3D_MAP, L"Layer_Camera", 0);
		if (pGameObject)
		{
			ImGui::SeparatorText("Camera Setting");

			CCamera_Free* pCamera = static_cast<CCamera_Free*>(pGameObject);
			_float fCameraSpeed = pCamera->Get_CameraMoveSpeed();
			_float fCameraSensor = pCamera->Get_MouseSensor();
			ImGui::BulletText("Camera Sensor : ");
			ImGui::SameLine();
			pCamera->Get_MouseSensor();
			if (ImGui::InputFloat("##CameraSensor", (_float*)&fCameraSensor, 0.f, 0.f, " %.1f", ImGuiInputTextFlags_EnterReturnsTrue))
				pCamera->Set_MouseSensor(fCameraSensor);
			ImGui::BulletText("Camera Speed  : ");
			ImGui::SameLine();
			if (ImGui::InputFloat("##CameraSpeed", (_float*)&fCameraSpeed, 0.f, 0.f, " %.1f", ImGuiInputTextFlags_EnterReturnsTrue))
				pCamera->Set_CameraMoveSpeed(fCameraSpeed);
		}


		ImGui::SeparatorText("Object Mode");
		ImGui::BulletText("Picking Layer");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##combo 1", WstringToString(m_strPickingLayerTag).c_str(), 0))
		{
			_uint iLoop = 0;
			auto pLayerMaps = m_pGameInstance->Get_Layers_Ptr();

			vector<wstring> strSaveLayerTags;
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

			for (const auto& pLayerName : strSaveLayerTags) 
			{
				string strLayerName = WstringToString(pLayerName);
				if (ImGui::Selectable(strLayerName.c_str(), pLayerName == m_strPickingLayerTag)) 
				{
					m_strPickingLayerTag = pLayerName;
				}

				if (m_strPickingLayerTag == pLayerName)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();

		}


		ImGui::BulletText("Preview Mode  :  %s", m_arrObjects[OBJECT_PREVIEW] ? "On" : "Off");
		if (m_arrObjects[OBJECT_PREVIEW])
		{
			ImGui::Text("Object Pos  :");
			ImGui::Text("X : %.2f", m_fPreviewPos.x);
			ImGui::Text("Y : %.2f", m_fPreviewPos.y);
			ImGui::Text("Z : %.2f", m_fPreviewPos.z);
		}


		_float3 vPos = {};
		CMapObject::OPERATION eOper = CMapObject::TRANSLATE;
		CMapObject* pPickingObj = m_arrObjects[OBJECT_PICKING] ? m_arrObjects[OBJECT_PICKING] : nullptr;
		ImGui::BulletText("Picking Mode  :  %s", m_arrObjects[OBJECT_PICKING] ? "On" : "Off");
		if (pPickingObj)
		{
			XMStoreFloat3(&vPos, pPickingObj->Get_Position());
			eOper = pPickingObj->Get_Operation();
			ImGui::SeparatorText("Gizmo Mode");
			if (ImGui::RadioButton("Translate", eOper == ImGuizmo::TRANSLATE))
				pPickingObj->Set_Operation(CMapObject::TRANSLATE);
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", eOper == ImGuizmo::ROTATE))
				pPickingObj->Set_Operation(CMapObject::ROTATE);
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", eOper == ImGuizmo::SCALE))
				pPickingObj->Set_Operation(CMapObject::SCALE);
			if (ImGui::IsKeyPressed(ImGuiKey_F1))
				pPickingObj->Set_Operation(CMapObject::TRANSLATE);
			if (ImGui::IsKeyPressed(ImGuiKey_F2))
				pPickingObj->Set_Operation(CMapObject::ROTATE);
			if (ImGui::IsKeyPressed(ImGuiKey_F3))
				pPickingObj->Set_Operation(CMapObject::SCALE);
		}

	}


	ImGui::End();
}

void C3DMap_Tool_Manager::Navigation_Imgui(_bool _bLock)
{
	//ImGui::Begin("Navigation");
	//ImGui::BulletText("Navigation Mode");
	//ImGui::SameLine();
	//if (ImGui::Checkbox("##NaviMode", &m_bNaviMode))
	//{
	//	if (_bLock)
	//		m_bNaviMode = false;
	//}

	//if (m_bNaviMode)
	//{
	//	NAVIGATION_MODE eMode = m_eNaviMode;

	//	ImGui::SeparatorText("Mode");
	//	if (ImGui::RadioButton("Vertect - Cell Create", eMode == NAV_CREATE))
	//		m_eNaviMode = NAV_CREATE;
	//	if (ImGui::RadioButton("Vertex - Cell Edit", eMode == NAV_EDIT))
	//		m_eNaviMode = NAV_EDIT;

	//	ImGui::SeparatorText("Cell List");
	//	if (ImGui::BeginListBox("##Cell_List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	//	{
	//		auto CellList = m_pCellContainor->Get_CellList();
	//		_uint iLoop = 0;
	//		for (const auto& pCell : CellList)
	//		{
	//			string strCellName = "Cell_";
	//			strCellName += to_string(iLoop);
	//			if (ImGui::Selectable(strCellName.c_str(), iLoop == m_iSelectCellIndex)) {
	//				if (iLoop == m_iSelectCellIndex)
	//				{
	//					for (_uint i = 0; i < CEditableCell::POINT_END; i++)
	//					{
	//						pCell->Get_Point(CEditableCell::POINT(i))->Set_Mode(CNavigationVertex::NORMAL);
	//					}
	//					pCell->Set_Picking(false);
	//					m_iSelectCellIndex = -1;
	//				}
	//				else if (iLoop != m_iSelectCellIndex)
	//				{
	//					Clear_SelectCell();

	//					m_iSelectCellIndex = iLoop;
	//					m_eNaviMode = NAV_EDIT;
	//					pCell->Get_Point(CEditableCell::POINT_A)->Set_Mode(CNavigationVertex::PICKING_FIRST);
	//					pCell->Get_Point(CEditableCell::POINT_B)->Set_Mode(CNavigationVertex::PICKING_SECOND);
	//					pCell->Get_Point(CEditableCell::POINT_C)->Set_Mode(CNavigationVertex::PICKING_THIRD);
	//					pCell->Set_Picking(true);
	//				}
	//			}
	//			iLoop++;
	//		}
	//		ImGui::EndListBox();
	//	}
	//	_bool bRender = m_pCellContainor->Is_WireRender();
	//	ImGui::BulletText("Wire Render");
	//	ImGui::SameLine();
	//	if (ImGui::Checkbox("##WireRender", &bRender))
	//	{
	//		m_pCellContainor->Set_WireRender(bRender);
	//	}

	//	if (!m_vecVertexStack.empty())
	//	{
	//		ImGui::SeparatorText("Create Cell");
	//		string strTextArr[CEditableCell::POINT_END] = { "First  : %.2f %.2f %.2f","Second : %.2f %.2f %.2f","Third  : %.2f %.2f %.2f" };
	//		_float4 vColors[CEditableCell::POINT_END] = {};
	//		XMStoreFloat4(&vColors[CEditableCell::POINT_A], DirectX::Colors::Red);
	//		XMStoreFloat4(&vColors[CEditableCell::POINT_B], DirectX::Colors::Green);
	//		XMStoreFloat4(&vColors[CEditableCell::POINT_C], DirectX::Colors::Blue);

	//		for (_uint i = 0; i < m_vecVertexStack.size(); i++)
	//		{
	//			ImGui::TextColored(ImVec4(vColors[i].x, vColors[i].y, vColors[i].z, vColors[i].w), strTextArr[i].c_str(), m_vecVertexStack[i]->Get_Pos().x,
	//				m_vecVertexStack[i]->Get_Pos().y,
	//				m_vecVertexStack[i]->Get_Pos().z);
	//		}
	//	}



	//	if (m_iSelectCellIndex != -1)
	//	{
	//		ImGui::SeparatorText("Picking Cell");
	//		string strTextArr[CEditableCell::POINT_END] = { "First  : %.2f %.2f %.2f","Second : %.2f %.2f %.2f","Third  : %.2f %.2f %.2f" };
	//		_float4 vColors[CEditableCell::POINT_END] = {};
	//		XMStoreFloat4(&vColors[CEditableCell::POINT_A], DirectX::Colors::Red);
	//		XMStoreFloat4(&vColors[CEditableCell::POINT_B], DirectX::Colors::Green);
	//		XMStoreFloat4(&vColors[CEditableCell::POINT_C], DirectX::Colors::Blue);

	//		auto pCell = m_pCellContainor->Get_CellList()[m_iSelectCellIndex];
	//		for (_uint i = 0; i < CEditableCell::POINT_END; i++)
	//		{
	//			ImGui::TextColored(ImVec4(vColors[i].x, vColors[i].y, vColors[i].z, vColors[i].w), strTextArr[i].c_str(), pCell->Get_Point(CEditableCell::POINT(i))->Get_Pos().x,
	//				pCell->Get_Point(CEditableCell::POINT(i))->Get_Pos().y,
	//				pCell->Get_Point(CEditableCell::POINT(i))->Get_Pos().z);
	//		}

	//		ImGui::SeparatorText("Cell State");
	//		if (ImGui::BeginListBox("##Cell_State_List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	//		{
	//			_uint iLoop = 0;
	//			for (const auto& strName : m_arrCellStateName)
	//			{
	//				if (ImGui::Selectable(strName.c_str(), iLoop == pCell->Get_State())) {
	//					if (iLoop != pCell->Get_State())
	//					{
	//						pCell->Set_State(iLoop);
	//					}
	//				}
	//				iLoop++;
	//			}
	//			ImGui::EndListBox();
	//		}


	//	}


	//	if (m_pPickingVertex)
	//	{
	//		ImGui::SeparatorText("Picking Vertex");
	//		ImGui::Text("X : %.2f Y : %.2f Z : %.2f", m_pPickingVertex->Get_Pos().x,
	//			m_pPickingVertex->Get_Pos().y,
	//			m_pPickingVertex->Get_Pos().z);

	//	}



	//	Begin_Draw_ColorButton("##ExportNav", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
	//	if (ImGui::Button("Export Navigation File"))
	//	{
	//		if (SUCCEEDED(m_pCellContainor->Export(L"../../Client/Bin/SettingFile/", L"Nav")))
	//			LOG_TYPE("Navigation file Export Success!!!", LOG_SAVE);
	//		else
	//			LOG_TYPE("Navigation file Export Error...", LOG_ERROR);
	//	}
	//	End_Draw_ColorButton();

	//	ImGui::SameLine();
	//	Begin_Draw_ColorButton("##ImportNav", (ImVec4)ImColor::HSV(0.7f, 0.6f, 0.6f));
	//	if (ImGui::Button("Import Navigation File"))
	//	{
	//		if (SUCCEEDED(m_pCellContainor->Import(L"../../Client/Bin/SettingFile/", L"Nav")))
	//			LOG_TYPE("Navigation file Import Success!!!", LOG_SAVE);
	//		else
	//			LOG_TYPE("Navigation file Import Error...", LOG_ERROR);

	//	}
	//	End_Draw_ColorButton();


	//}

	//ImGui::End();

}

void C3DMap_Tool_Manager::SaveLoad_Imgui(_bool _bLock)
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

			ImGui::Text("Static?");
			ImGui::SameLine();
			ImGui::Checkbox("##isStatic", &m_isStaticProto);
			ImGui::SameLine();
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


void C3DMap_Tool_Manager::Model_Imgui(_bool _bLock) 
{
#ifdef _DEBUG


	ImGui::Begin("Model Info");
	{

		_string arrEnumText[] =
		{
			"NONE",
			"DIFFUSE",
			"SPECULAR",
			"AMBIENT",
			"EMISSIVE",
			"HEIGHT",
			"NORMALS",
			"SHININESS",
			"OPACITY",
			"DISPLACEMENT",
			"LIGHTMAP",
			"REFLECTION",
			"BASE_COLOR",
			"NORMAL_CAMERA",
			"EMISSION_COLOR",
			"METALNESS",
			"DIFFUSE_ROUGHNESS",
			"AMBIENT_OCCLUSION",
			"UNKNOWN",
			"SHEEN",
			"CLEARCOAT",
			"TRANSMISSION",
			"MAYA_BASE",
			"MAYA_SPECULAR",
			"MAYA_SPECULAR_COLOR",
			"MAYA_SPECULAR_ROUGHNESS",
		};


		if (m_arrObjects[OBJECT_PICKING])
		{
			CMapObject* pTargetObj = m_arrObjects[OBJECT_PICKING];

			_wstring strModelName = pTargetObj->Get_ModelName();
			_wstring strModelPath = L"-";
			ImGui::BulletText("Model Name : %s", WstringToString(strModelName).c_str());

			auto iter = find_if(m_ObjectFileLists.begin(), m_ObjectFileLists.end(), [&strModelName](const pair<_wstring, _wstring>& PairFileInfo)
				->_bool {
				
				return PairFileInfo.first == strModelName;
				});

			if (iter != m_ObjectFileLists.end())
				strModelPath = (*iter).second;
			ImGui::BulletText("Model Path : %s", WstringToString(strModelPath).c_str());

			if (StyleButton(MINI, "Model RePackaging"))
			{
				m_pMapParsingManager->Register_RePackaging((*iter).second, pTargetObj);
			}

			vector<CMapObject::DIFFUSE_INFO> Textures;

			for (_uint iTextureType = 0; iTextureType < AI_TEXTURE_TYPE_MAX; iTextureType++)
			{
				if(0 < iTextureType)
				ImGui::NewLine();

				Textures.clear();

				if (SUCCEEDED(pTargetObj->Get_Textures(Textures, iTextureType))
					&&
					0 < Textures.size()
					)
				{
					ImGui::SeparatorText(arrEnumText[iTextureType].c_str());

					_uint iMaterialIdx = 0;
					for (auto& tDiffuseInfo : Textures)
					{
						_uint iMaxDiffuseIdx = 0;
						_uint iCurMaterial = iMaterialIdx;
						auto iter = find_if(Textures.begin(), Textures.end(), [&iCurMaterial, &iMaxDiffuseIdx](CMapObject::DIFFUSE_INFO _tDiffuseInfo) {
							if (iCurMaterial == _tDiffuseInfo.iMaterialIndex
								&&
								iMaxDiffuseIdx < _tDiffuseInfo.iDiffuseIIndex
								)
								iMaxDiffuseIdx = _tDiffuseInfo.iDiffuseIIndex;
							return _tDiffuseInfo.iMaterialIndex > iCurMaterial;
							});



						if (tDiffuseInfo.iMaterialIndex == iMaterialIdx)
						{
							_string strButtonText = "ADD ";
							_string strMaterialText = "Material ";
							strButtonText += arrEnumText[iTextureType];
							strMaterialText += std::to_string(iMaterialIdx);

							ImGui::BulletText(strMaterialText.c_str());
							//ImGui::SameLine();
							Begin_Draw_ColorButton("#AddButton_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));

							if (StyleButton(MINI, strButtonText.c_str()))
#pragma region ADD BUTTON
							{
								_tchar originalDir[MAX_PATH];
								GetCurrentDirectory(MAX_PATH, originalDir);

								_wstring strModelPath = L"..\\..\\Client\\Bin\\resources\\Models\\NonAnim\\";

								OPENFILENAME ofn = {};
								_tchar szName[MAX_PATH] = {};
								ofn.lStructSize = sizeof(OPENFILENAME);
								ofn.hwndOwner = g_hWnd;
								ofn.lpstrFile = szName;
								ofn.nMaxFile = sizeof(szName);
								ofn.lpstrFilter = L".dds\0*.dds\0";
								ofn.nFilterIndex = 0;
								ofn.lpstrFileTitle = nullptr;
								ofn.nMaxFileTitle = 0;
								wstring strPath = strModelPath + pTargetObj->Get_ModelName();
								ofn.lpstrInitialDir = strPath.c_str();
								ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

								if (GetOpenFileName(&ofn))
								{
									SetCurrentDirectory(originalDir);
									const _wstring strFilePath = szName;

									ID3D11ShaderResourceView* pSRV = { nullptr };
									HRESULT		hr = E_FAIL;
									if (_string::npos != strFilePath.rfind(L".dds") || _string::npos != strFilePath.rfind(L".DDS"))
									{
										hr = CreateDDSTextureFromFile(m_pDevice, szName, nullptr, &pSRV);

										if (SUCCEEDED(hr))
										{
											auto PairResult = Get_FileName_From_Path(strFilePath);


											CMapObject::DIFFUSE_INFO tAddInfo;
											tAddInfo.iDiffuseIIndex = iMaxDiffuseIdx;
											tAddInfo.iMaterialIndex = iMaterialIdx;
											tAddInfo.pSRV = pSRV;

											_wstring strNameAndExt = PairResult.first + L"."+PairResult.second;

											lstrcpy(tAddInfo.szTextureName, strNameAndExt.c_str());
											if (FAILED(pTargetObj->Add_Textures(tAddInfo, iTextureType)))
											{
												LOG_TYPE("Add Texture Failed... -> "+ WstringToString(strModelName), LOG_ERROR);
												Safe_Release(pSRV);
												return;
											}
										}

									}
								}
							}
#pragma endregion
							End_Draw_ColorButton();

							iMaterialIdx++;
						}

						string strButtonTag = std::to_string(tDiffuseInfo.iMaterialIndex) + "_" + std::to_string(tDiffuseInfo.iDiffuseIIndex) + "_" + arrEnumText[iTextureType];
						ImVec2 size = ImVec2(48.0f, 48.0f);
						ImVec2 uv0 = ImVec2(0.0f, 0.0f);
						ImVec2 uv1 = ImVec2(1.0f, 1.0f);
						ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
						ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
						_uint iCurrenrTextureIndex = pTargetObj->Get_TextureIdx(iTextureType, tDiffuseInfo.iMaterialIndex);

						if (tDiffuseInfo.iDiffuseIIndex == iCurrenrTextureIndex)
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.9f, 1.0f)); // 선택된 버튼의 배경색

						if (ImGui::ImageButton(strButtonTag.c_str(), (ImTextureID)tDiffuseInfo.pSRV, size, uv0, uv1, bg_col, tint_col))
							pTargetObj->Change_TextureIdx(tDiffuseInfo.iDiffuseIIndex, iTextureType, tDiffuseInfo.iMaterialIndex);

						if (tDiffuseInfo.iDiffuseIIndex == iCurrenrTextureIndex)
							ImGui::PopStyleColor(1);

						if (ImGui::IsItemHovered())
						{
							if (ImGui::BeginItemTooltip())
							{
								ImGui::SeparatorText(WstringToString(tDiffuseInfo.szTextureName).c_str());
								ImGui::Image((ImTextureID)tDiffuseInfo.pSRV,
									ImVec2(256.f, 256.f)
								);
								ImGui::EndTooltip();
							}
						}
						ImGui::SameLine();
					}
				}

			}

		}
	}

	ImGui::End();
#endif // _DEBUG
}


void C3DMap_Tool_Manager::Save(_bool _bSelected)
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

	if (!_bSelected)
		strFullFilePath = m_strCacheFilePath;
	else
		strFullFilePath = (STATIC_3D_MAP_SAVE_FILE_PATH + m_pGameInstance->StringToWString(filename) + L".mchc");
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
					//	세이브 파라미터 5. 월드 매트릭스
					pObject->Save_Override_Color(hFile);
					// 세이브 파라미터 6. 마테리얼 오버라이드
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
		Export_SaveResult_ToJson(strFullFilePath, vecSaveModelProtos, m_isStaticProto)
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

void C3DMap_Tool_Manager::Init_Egnore_Layer()
{

	m_EgnoreLayerTags.clear();
	m_EgnoreLayerTags.insert(m_EgnoreLayerTags.end(), m_DefaultEgnoreLayerTags.begin(), m_DefaultEgnoreLayerTags.end());
}


HRESULT C3DMap_Tool_Manager::Setting_Action_Layer(vector<pair<wstring, CLayer*>>& _TargetLayerPairs)
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

void C3DMap_Tool_Manager::Load(_bool _bSelected)
{
	//Object_Clear(false);
	
	string filename = m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]);
	string log = "";

	log = "Load Start... File Name : ";
	log += filename;
	_wstring strFullFilePath = STATIC_3D_MAP_SAVE_FILE_PATH; 
	strFullFilePath += L"/" + m_arrSelectName[SAVE_LIST] + L".mchc";

	if (!_bSelected)
		strFullFilePath = m_strCacheFilePath;



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
			NormalDesc.iCurLevelID = LEVEL_TOOL_3D_MAP;
			NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TOOL_3D_MAP;



			CGameObject* pGameObject = nullptr;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
				LEVEL_TOOL_3D_MAP,
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


void C3DMap_Tool_Manager::Object_Clear(_bool _bSelected)
{
	Object_Clear_PickingMode();
	Object_Close_PreviewMode();

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


void C3DMap_Tool_Manager::Save_Popup()
{
	Init_Egnore_Layer();
	// default EgnoreLayerTag 구성 !

	ImGui::OpenPopup("Save_Popup");
	strcpy_s(m_szSaveFileName, m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]).c_str());
}


HRESULT C3DMap_Tool_Manager::Picking_On_Terrain(_float3* fPickingPos, CMapObject** ppMap)
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
		//vLocalRayPos = XMVector3TransformCoord(vLocalRayPos, m_pTransformCom->Get_WorldMatrix());
		XMStoreFloat3(fPickingPos, vWorldRayPos);

		//*fPickingPos = vRayPos * fDist;
		return S_OK;
	}
	else
		return E_FAIL;

}

HRESULT C3DMap_Tool_Manager::Picking_On_Terrain(_float3* fPickingPos)
{
	HRESULT hr = {};
	CMapObject* pToolObj = nullptr;

	if (SUCCEEDED(Picking_On_Terrain(fPickingPos, &pToolObj)))
		return S_OK;
	else
		return E_FAIL;
}

CMapObject* C3DMap_Tool_Manager::Picking_On_Object()
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
				//bool bRange = pMapObject->Check_Pick785/ing(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir), &vReturnPos, &fNewDist);
				if (bRange && (fNewDist < fDist || fDist == 0.f))
				{
					pReturnObject = pMapObject;
					fDist = fNewDist;
				}
			}
		});

	return pReturnObject;
}

//CNavigationVertex* C3DMap_Tool_Manager::Picking_On_Vertex()
//{
//	//_float3			vRayPos, vRayDir;
//
//	//Compute_World_PickingLay(&vRayPos, &vRayDir);
//	//return m_pCellContainor->Picking_On_Vertex(vRayPos, vRayDir);
//
//}

//HRESULT C3DMap_Tool_Manager::Create_Cell()
//{
	/*_float3 vFirst = m_vecVertexStack[0]->Get_Pos();
	_float3 vSecond = m_vecVertexStack[1]->Get_Pos();
	_float3 vThird = m_vecVertexStack[2]->Get_Pos();

	_float2 vCross1 = { vSecond.x - vFirst.x, vSecond.z - vFirst.z };
	_float2 vCross2 = { vThird.x - vSecond.x, vThird.z - vSecond.z };


	_float fCross = {};

	XMStoreFloat(&fCross, XMVector2Cross(XMLoadFloat2(&vCross1), XMLoadFloat2(&vCross2)));

	if (fCross > 0.f)
	{
		swap(m_vecVertexStack[2], m_vecVertexStack[1]);
	}

	if (m_pCellContainor->Is_ContainCell(m_vecVertexStack.data()))
	{
		LOG_TYPE("Cell Create Fail ! already contain cell...", LOG_ERROR);
		m_vecVertexStack.clear();
		return E_FAIL;
	}
	CEditableCell* pCell = CEditableCell::Create(m_pDevice, m_pContext, m_vecVertexStack.data(),
		m_pCellContainor->Get_CellSize());
	if (pCell)
	{
		for (auto& pVertex : m_vecVertexStack)
		{
			pVertex->Add_Cell(pCell);
			pVertex->Set_Mode(CNavigationVertex::NORMAL);
		}
		m_vecVertexStack.clear();
		m_pCellContainor->Add_Cell(pCell);
		return S_OK;
	}

	return E_FAIL;*/
//}

HRESULT C3DMap_Tool_Manager::Compute_World_PickingLay(_float3* pLayPos, _float3* pLayDir)
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

//void C3DMap_Tool_Manager::Clear_StackVertex()
//{
//	if (!m_vecVertexStack.empty())
//	{
//		for (auto& pVertex : m_vecVertexStack)
//			OBJECT_DESTROY(pVertex);
//		m_vecVertexStack.clear();
//	}
//}

//void C3DMap_Tool_Manager::Clear_SelectCell()
//{
//	//if (m_iSelectCellIndex != -1)
//	//{
//	//	for (_uint i = 0; i < CEditableCell::POINT_END; i++)
//	//	{
//	//		m_pCellContainor->Get_CellList()[m_iSelectCellIndex]
//	//			->Get_Point(CEditableCell::POINT(i))->Set_Mode(CNavigationVertex::NORMAL);
//	//	}
//	//	m_pCellContainor->Get_CellList()[m_iSelectCellIndex]->Set_Picking(false);
//	//	m_iSelectCellIndex = -1;
//	//}
//}
//
//bool C3DMap_Tool_Manager::Check_VertexSelect()
//{
//	if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft) && !ImGuizmo::IsUsing())
//	{
//		// 셀렉트된 셀 클리어
//		Clear_SelectCell();
//
//		if (m_pPickingVertex)
//		{
//			m_pPickingVertex->Set_Mode(CNavigationVertex::NORMAL);
//			m_pPickingVertex = nullptr;
//		}
//
//		_float3 fPos = {};
//		CNavigationVertex* pVertex = Picking_On_Vertex();
//		if (pVertex)
//		{
//			for (auto pStack : m_vecVertexStack)
//				if (pStack == pVertex)
//					return false;
//			m_eNaviMode = NAV_EDIT;
//			m_pPickingVertex = pVertex;
//			pVertex->Set_Mode(CNavigationVertex::PICKING_VERTEX);
//			return true;
//		}
//
//	}
//	return false;
//}

void C3DMap_Tool_Manager::Load_ModelList()
{
	m_ObjectFileLists.clear();
	_wstring strPath 
		= STATIC_3D_MODEL_FILE_PATH;

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

void C3DMap_Tool_Manager::Load_SaveFileList()
{
	m_SaveFileLists.clear();
	for (const auto& entry : ::recursive_directory_iterator(STATIC_3D_MAP_SAVE_FILE_PATH))
	{
			if (entry.path().extension() == ".mchc")
			{
				wstring strKey = entry.path().stem().wstring();
				m_SaveFileLists.push_back(strKey);
			}
	}
}

void C3DMap_Tool_Manager::Object_Open_PickingMode()
{
	CMapObject* pGameObj = Picking_On_Object();
	if (pGameObj)
	{
		
		Object_Clear_PickingMode();
		m_arrObjects[OBJECT_PICKING] = pGameObj;
		m_arrObjects[OBJECT_PICKING]->Set_Mode(CMapObject::PICKING);
#ifdef _DEBUG
		m_pGameInstance->Imgui_Select_Debug_ObjectInfo(m_strPickingLayerTag, pGameObj->Get_GameObjectInstanceID());
#endif // _DEBUG

	}
}

void C3DMap_Tool_Manager::Object_Clear_PickingMode()
{
	if (m_arrObjects[OBJECT_PICKING])
	{
		static_cast<CMapObject*>(m_arrObjects[OBJECT_PICKING])->Set_Mode(CMapObject::NORMAL);
		m_arrObjects[OBJECT_PICKING] = nullptr;
	}

}

void C3DMap_Tool_Manager::Object_Open_PreviewMode()
{
	Object_Clear_PickingMode();

	if (m_arrSelectName[CREATE_OBJECT] != L"")
	{
		CMapObject::MAPOBJ_DESC NormalDesc = {};
		NormalDesc.eCreateType = CMapObject::OBJ_CREATE;
		NormalDesc.iCurLevelID = LEVEL_TOOL_3D_MAP;
		NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TOOL_3D_MAP;

		lstrcpy(NormalDesc.szModelName, m_arrSelectName[CREATE_OBJECT].c_str());

		CGameObject* pGameObject = nullptr;

		m_pGameInstance->Add_GameObject_ToLayer( LEVEL_TOOL_3D_MAP, TEXT("Prototype_GameObject_MapObject"),
			LEVEL_TOOL_3D_MAP, L"Layer_MapObject", &pGameObject, (void*)&NormalDesc);
		
		if (nullptr != pGameObject)
		{
			m_arrObjects[OBJECT_PREVIEW] = static_cast<CMapObject*>(pGameObject);
			//Safe_AddRef(m_arrObjects[OBJECT_PREVIEW]);
			m_arrObjects[OBJECT_PREVIEW]->Set_Mode(CMapObject::PREVIEW);
		}
	}
}

void C3DMap_Tool_Manager::Object_Close_PreviewMode()
{
	if (m_arrObjects[OBJECT_PREVIEW])
	{
		OBJECT_DESTROY(m_arrObjects[OBJECT_PREVIEW]);
		m_arrObjects[OBJECT_PREVIEW] = nullptr;
	}
}



C3DMap_Tool_Manager* C3DMap_Tool_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	C3DMap_Tool_Manager* pInstance = new C3DMap_Tool_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : C3DMap_Tool_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C3DMap_Tool_Manager::Free()
{
	//Save(false);

	//Safe_Release(m_pCellContainor);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pLogger);
	Safe_Release(m_pMapParsingManager);

	__super::Free();
}
