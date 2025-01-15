#include "stdafx.h"
#include "Map_Tool_Manager.h"
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
#include "CellContainor.h"
#include "Event_Manager.h"
#include "MapParsing_Manager.h"

using namespace std::filesystem;




CMap_Tool_Manager::CMap_Tool_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext) :
	m_pDevice(_pDevice),
	m_pContext(_pContext), m_pGameInstance(Engine::CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMap_Tool_Manager::Initialize(CImguiLogger* _pLogger)
{
	// Logger 등록
	m_pLogger = _pLogger;
	Safe_AddRef(m_pLogger);

	// 임구이 크기 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(600, 1200), ImVec2(FLT_MAX, FLT_MAX));

	// 모델 리스트 불러오기
	Load_ModelList();
	// 캐시파일 불러오기
	Load(false);

	// 셀 관리 컨테이너 생성
	CGameObject* pGameObject = nullptr;
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_MAP, TEXT("Prototype_GameObject_CellContainor"),
		LEVEL_TOOL_MAP, L"Layer_Cell", &pGameObject, nullptr);
	if (pGameObject)
	{
		m_pCellContainor = static_cast<CCellContainor*>(pGameObject);
		Safe_AddRef(m_pCellContainor);
	}
	
	// SM_desk_split_topboard_02
	m_pLogger = CImguiLogger::Create();
	if (nullptr == m_pLogger)
		return E_FAIL;

	CMapObject::MAPOBJ_DESC NormalDesc = {};
	lstrcpy(NormalDesc.szModelName, L"SM_desk_split_topboard_02");
	//lstrcpy(NormalDesc.szModelName, L"WoodenPlatform_01");
	NormalDesc.eCreateType = CMapObject::OBJ_CREATE;
	//NormalDesc.matWorld = vWorld;



	pGameObject = nullptr;
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL_MAP, TEXT("Prototype_GameObject_MapObject"),
		LEVEL_TOOL_MAP,
		L"Layer_Environment",
		&pGameObject,
		(void*)&NormalDesc);
	if (pGameObject)
	{
		m_pCellContainor = static_cast<CCellContainor*>(pGameObject);
		Safe_AddRef(m_pCellContainor);
	}


	CMapParsing_Manager* pParsingManager = CMapParsing_Manager::Create(m_pDevice, m_pContext, m_pLogger);
	if (nullptr == pParsingManager)
		return E_FAIL;

	pParsingManager->Parsing(string("..\\Bin\\json\\Desk_C04.json"));


	return S_OK;
}

void CMap_Tool_Manager::Update_Tool()
{
	// 입력 설정 Navigation Tool Mode - Obejct Tool Mode 전환하여. 나중에 씬 분리할것
	if (!m_bNaviMode)
		Input_Object_Tool_Mode();
	else
		Input_Navigation_Tool_Mode();

	// 임구이 화면 구성
	Update_Imgui_Logic();

}


void CMap_Tool_Manager::Update_Imgui_Logic()
{
	Navigation_Imgui(m_arrObjects[OBJECT_PREVIEW]);
	Object_Create_Imgui(m_bNaviMode);
	Object_Info_Imgui();
	SaveLoad_Imgui();
}



void CMap_Tool_Manager::Input_Object_Tool_Mode()
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
			if (MOUSE_DOWN(MOUSE_KEY::LB) && !ImGuizmo::IsOver())
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

void CMap_Tool_Manager::Input_Navigation_Tool_Mode()
{
	HWND hWnd = GetFocus();
	auto& io = ImGui::GetIO();
	if (m_bNaviMode)
	{
		if (nullptr != hWnd && !io.WantCaptureMouse)
		{
			switch (m_eNaviMode)
			{
			case Map_Tool::CMap_Tool_Manager::NAV_CREATE:
			case Map_Tool::CMap_Tool_Manager::NAV_EDIT:
				if (((ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt)) && ImGui::IsKeyPressed(ImGuiKey_MouseLeft)))
				{
					_float3 fPos = {};
					bool bDuple = false;

					CNavigationVertex* pVertex = Picking_On_Vertex();
					if (pVertex)
					{
						for (auto iter = m_vecVertexStack.begin(); iter != m_vecVertexStack.end();)
						{
							if (bDuple = pVertex == (*iter))
							{
								pVertex->Set_Mode(CNavigationVertex::NORMAL);
								m_vecVertexStack.erase(iter);

								break;
							}
							else
								iter++;
						}
						if (!bDuple)
							pVertex->Set_Mode(CNavigationVertex::SET);
					}
					else if (SUCCEEDED(Picking_On_Terrain(&fPos)))
					{
						pVertex = CNavigationVertex::Create(m_pDevice, m_pContext, fPos);
						m_pCellContainor->Add_Vertex(pVertex);
						pVertex->Set_Mode(CNavigationVertex::SET);
					}

					if (pVertex && !bDuple)
					{
						m_vecVertexStack.push_back(pVertex);
						if (m_vecVertexStack.size() == 3)
						{
							Create_Cell();
						}
					}
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft))
				{
					if (!Check_VertexSelect())
					{
						int a = 1;
					}
				}
				if (ImGui::IsKeyPressed(ImGuiKey_Delete))
				{
					if (m_pPickingVertex)
					{
						m_pPickingVertex->Delete_Vertex();
						m_pPickingVertex = nullptr;
					}
				}
				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Z))
				{
					if (!m_vecVertexStack.empty())
					{
						if (0 >= m_vecVertexStack.back()->Get_IncludeCellCount())
						{
							m_vecVertexStack.back()->Delete_Vertex();
						}
						else
						{
							m_vecVertexStack.back()->Set_Mode(CNavigationVertex::NORMAL);
						}
						m_vecVertexStack.pop_back();
					}
				}

				break;
			default:
				break;
			}
		}
	}
}

void CMap_Tool_Manager::Object_Info_Imgui(_bool _bLock)
{
	ImGui::Begin("Setting");
	ImGui::SeparatorText("Map Cursor Pos");
	ImGui::BulletText("X : %.2f", m_fPickingPos.x);
	ImGui::SameLine();
	ImGui::Text("Y : %.2f", m_fPickingPos.y);
	ImGui::SameLine();
	ImGui::Text("Z : %.2f", m_fPickingPos.z);

	_float3 vPos = {};

	CMapObject::OPERATION eOper = CMapObject::TRANSLATE;

	CMapObject* pPickingObj = m_arrObjects[OBJECT_PICKING] ? m_arrObjects[OBJECT_PICKING] : nullptr;




	// 1. 기본 위치 세팅, 기즈모 세팅.
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
	else if (m_arrObjects[OBJECT_PICKING])
	{

		ImGui::Separator();
		Begin_Draw_ColorButton("Delete_Obj", (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
		if (ImGui::Button("Delete Picking Obj", ImVec2(-FLT_MIN, 1.5f * ImGui::GetTextLineHeightWithSpacing())))
		{
			if (m_arrObjects[OBJECT_PICKING])
			{
				OBJECT_DESTROY(m_arrObjects[OBJECT_PICKING]);
				m_arrObjects[OBJECT_PICKING] = nullptr;
				Object_Clear_PickingMode();
			}
		}
		End_Draw_ColorButton();

	}
	ImGui::End();
}

void CMap_Tool_Manager::Object_Create_Imgui(_bool _bLock)
{
	ImGui::Begin("Object");
	{
		ImGui::SeparatorText("Object List");
		if (ImGui::BeginListBox("##Object List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const auto& fileName : m_ObjectFileLists) {
				_char szName[MAX_PATH] = {};

				WideCharToMultiByte(CP_ACP, 0, fileName.c_str(), -1, szName, (_int)(lstrlen(fileName.c_str())), NULL, NULL);

				if (ImGui::Selectable(szName, fileName == m_arrSelectName[CREATE_OBJECT])) {
					if (m_arrSelectName[CREATE_OBJECT] == fileName)
						m_arrSelectName[CREATE_OBJECT] = L"";
					else
					{
						wstring filename;
						filename.assign(fileName.begin(), fileName.end());
						m_arrSelectName[CREATE_OBJECT] = filename;
					}
				}
			}
			ImGui::EndListBox();
		}


		Begin_Draw_ColorButton("Create_Obj_Mode", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
		if (!_bLock && ImGui::Button(!m_arrObjects[OBJECT_PREVIEW] ? "Create Object Mode On" : "Create Object Mode Off", ImVec2(-FLT_MIN, 2.f * ImGui::GetTextLineHeightWithSpacing())))
		{
			if (!m_arrObjects[OBJECT_PREVIEW])
				Object_Open_PreviewMode();
			else
				Object_Close_PreviewMode();
		}
		End_Draw_ColorButton();


		Begin_Draw_ColorButton("Clear_All_ObjectStyle", (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
		if (!_bLock && ImGui::Button(!m_arrObjects[OBJECT_PREVIEW] ? "Clear All Object" : "Create Object Mode Off", ImVec2(-FLT_MIN, 1.7f * ImGui::GetTextLineHeightWithSpacing())))
		{
			ImGui::OpenPopup("Clear_Popup");
		}

		if (ImGui::BeginPopup("Clear_Popup"))
		{
			ImGui::Text("All those Object will be deleted.");
			ImGui::Text("Do you want to proceed?");
			Begin_Draw_ColorButton("OK_Style", (ImVec4)ImColor::HSV(0.5f, 0.6f, 0.6f));
			if (ImGui::Button("OK"))
			{
				Object_Clear();
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



		ImGui::BulletText("Preview Mode : %s", m_arrObjects[OBJECT_PREVIEW] ? "On" : "Off");
		if (m_arrObjects[OBJECT_PREVIEW])
		{
			ImGui::Text("Object Pos :");
			ImGui::Text("X : %.2f", m_fPreviewPos.x);
			ImGui::Text("Y : %.2f", m_fPreviewPos.y);
			ImGui::Text("Z : %.2f", m_fPreviewPos.z);
		}

	}


	ImGui::End();
}

void CMap_Tool_Manager::Navigation_Imgui(_bool _bLock)
{
	ImGui::Begin("Navigation");
	ImGui::BulletText("Navigation Mode");
	ImGui::SameLine();
	if (ImGui::Checkbox("##NaviMode", &m_bNaviMode))
	{
		if (_bLock)
			m_bNaviMode = false;
	}

	if (m_bNaviMode)
	{
		NAVIGATION_MODE eMode = m_eNaviMode;

		ImGui::SeparatorText("Mode");
		if (ImGui::RadioButton("Vertect - Cell Create", eMode == NAV_CREATE))
			m_eNaviMode = NAV_CREATE;
		if (ImGui::RadioButton("Vertex - Cell Edit", eMode == NAV_EDIT))
			m_eNaviMode = NAV_EDIT;

		ImGui::SeparatorText("Cell List");
		if (ImGui::BeginListBox("##Cell_List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			auto CellList = m_pCellContainor->Get_CellList();
			_uint iLoop = 0;
			for (const auto& pCell : CellList)
			{
				string strCellName = "Cell_";
				strCellName += to_string(iLoop);
				if (ImGui::Selectable(strCellName.c_str(), iLoop == m_iSelectCellIndex)) {
					if (iLoop == m_iSelectCellIndex)
					{
						for (_uint i = 0; i < CEditableCell::POINT_END; i++)
						{
							pCell->Get_Point(CEditableCell::POINT(i))->Set_Mode(CNavigationVertex::NORMAL);
						}
						pCell->Set_Picking(false);
						m_iSelectCellIndex = -1;
					}
					else if (iLoop != m_iSelectCellIndex)
					{
						Clear_SelectCell();

						m_iSelectCellIndex = iLoop;
						m_eNaviMode = NAV_EDIT;
						pCell->Get_Point(CEditableCell::POINT_A)->Set_Mode(CNavigationVertex::PICKING_FIRST);
						pCell->Get_Point(CEditableCell::POINT_B)->Set_Mode(CNavigationVertex::PICKING_SECOND);
						pCell->Get_Point(CEditableCell::POINT_C)->Set_Mode(CNavigationVertex::PICKING_THIRD);
						pCell->Set_Picking(true);
					}
				}
				iLoop++;
			}
			ImGui::EndListBox();
		}
		_bool bRender = m_pCellContainor->Is_WireRender();
		ImGui::BulletText("Wire Render");
		ImGui::SameLine();
		if (ImGui::Checkbox("##WireRender", &bRender))
		{
			m_pCellContainor->Set_WireRender(bRender);
		}

		if (!m_vecVertexStack.empty())
		{
			ImGui::SeparatorText("Create Cell");
			string strTextArr[CEditableCell::POINT_END] = { "First  : %.2f %.2f %.2f","Second : %.2f %.2f %.2f","Third  : %.2f %.2f %.2f" };
			_float4 vColors[CEditableCell::POINT_END] = {};
			XMStoreFloat4(&vColors[CEditableCell::POINT_A], DirectX::Colors::Red);
			XMStoreFloat4(&vColors[CEditableCell::POINT_B], DirectX::Colors::Green);
			XMStoreFloat4(&vColors[CEditableCell::POINT_C], DirectX::Colors::Blue);

			for (_uint i = 0; i < m_vecVertexStack.size(); i++)
			{
				ImGui::TextColored(ImVec4(vColors[i].x, vColors[i].y, vColors[i].z, vColors[i].w), strTextArr[i].c_str(), m_vecVertexStack[i]->Get_Pos().x,
					m_vecVertexStack[i]->Get_Pos().y,
					m_vecVertexStack[i]->Get_Pos().z);
			}
		}



		if (m_iSelectCellIndex != -1)
		{
			ImGui::SeparatorText("Picking Cell");
			string strTextArr[CEditableCell::POINT_END] = { "First  : %.2f %.2f %.2f","Second : %.2f %.2f %.2f","Third  : %.2f %.2f %.2f" };
			_float4 vColors[CEditableCell::POINT_END] = {};
			XMStoreFloat4(&vColors[CEditableCell::POINT_A], DirectX::Colors::Red);
			XMStoreFloat4(&vColors[CEditableCell::POINT_B], DirectX::Colors::Green);
			XMStoreFloat4(&vColors[CEditableCell::POINT_C], DirectX::Colors::Blue);

			auto pCell = m_pCellContainor->Get_CellList()[m_iSelectCellIndex];
			for (_uint i = 0; i < CEditableCell::POINT_END; i++)
			{
				ImGui::TextColored(ImVec4(vColors[i].x, vColors[i].y, vColors[i].z, vColors[i].w), strTextArr[i].c_str(), pCell->Get_Point(CEditableCell::POINT(i))->Get_Pos().x,
					pCell->Get_Point(CEditableCell::POINT(i))->Get_Pos().y,
					pCell->Get_Point(CEditableCell::POINT(i))->Get_Pos().z);
			}

			ImGui::SeparatorText("Cell State");
			if (ImGui::BeginListBox("##Cell_State_List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				_uint iLoop = 0;
				for (const auto& strName : m_arrCellStateName)
				{
					if (ImGui::Selectable(strName.c_str(), iLoop == pCell->Get_State())) {
						if (iLoop != pCell->Get_State())
						{
							pCell->Set_State(iLoop);
						}
					}
					iLoop++;
				}
				ImGui::EndListBox();
			}


		}


		if (m_pPickingVertex)
		{
			ImGui::SeparatorText("Picking Vertex");
			ImGui::Text("X : %.2f Y : %.2f Z : %.2f", m_pPickingVertex->Get_Pos().x,
				m_pPickingVertex->Get_Pos().y,
				m_pPickingVertex->Get_Pos().z);

		}



		Begin_Draw_ColorButton("##ExportNav", (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f));
		if (ImGui::Button("Export Navigation File"))
		{
			if (SUCCEEDED(m_pCellContainor->Export(L"../../Client/Bin/SettingFile/", L"Nav")))
				LOG_TYPE("Navigation file Export Success!!!", LOG_SAVE);
			else
				LOG_TYPE("Navigation file Export Error...", LOG_ERROR);
		}
		End_Draw_ColorButton();

		ImGui::SameLine();
		Begin_Draw_ColorButton("##ImportNav", (ImVec4)ImColor::HSV(0.7f, 0.6f, 0.6f));
		if (ImGui::Button("Import Navigation File"))
		{
			if (SUCCEEDED(m_pCellContainor->Import(L"../../Client/Bin/SettingFile/", L"Nav")))
				LOG_TYPE("Navigation file Import Success!!!", LOG_SAVE);
			else
				LOG_TYPE("Navigation file Import Error...", LOG_ERROR);

		}
		End_Draw_ColorButton();


	}

	ImGui::End();

}

void CMap_Tool_Manager::SaveLoad_Imgui(_bool _bLock)
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
		if (ImGui::Button("Setting Save"))
			Save_Popup();
		ImGui::SameLine();
		if (ImGui::BeginPopup("Save_Popup"))
		{
			ImGui::InputText("##Save File Name", m_szSaveFileName, MAX_PATH);
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
		if (ImGui::Button("Setting Load"))
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



void CMap_Tool_Manager::Save(bool bSelected)
{
	string filename = m_szSaveFileName;
	string log = "";

	if (bSelected)
	{
		if (filename.empty() ||
			filename.rfind(".") != string::npos ||
			filename.rfind(":") != string::npos ||
			filename.rfind("*") != string::npos ||
			filename.rfind("?") != string::npos ||
			filename.rfind("\"") != string::npos ||
			filename.rfind("<") != string::npos ||
			filename.rfind(">") != string::npos ||
			filename.rfind("|") != string::npos ||
			filename.rfind("/") != string::npos ||
			filename.rfind("\\") != string::npos
			)
		{
			log = "Save Failed... file Name Error - File Name : ";
			log += filename;
			if (bSelected)
				LOG_TYPE(log, LOG_ERROR);
			return;
		}
	}

	_wstring strFullFilePath = L"";

	if (!bSelected)
		strFullFilePath = m_strCacheFilePath;
	else
		strFullFilePath = (m_strObjectLoadFilePath + L"/" + m_pGameInstance->StringToWString(filename) + L".mchc");
	log = "Save Start... File Name : ";
	log += filename;

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
	_uint iCount = 0;
	log = "Save... Save Object Count :  ";
	log += std::to_string(iCount);
	if (bSelected)
		LOG_TYPE(log, LOG_SAVE);

	//m_pImguiManager->Set_Lock(true);

	CLayer* arrLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_MAP,L"Layer_MapObject");
	DWORD	dwByte(0);

	_char		szSaveMeshName[MAX_PATH];

	_uint iAllCount = 0;
	if (arrLayer != nullptr)
	{
		auto ObjList = arrLayer->Get_GameObjects();
		iAllCount += (_uint)ObjList.size();
	}
	WriteFile(hFile, &iAllCount, sizeof(_uint), &dwByte, nullptr);

	if (arrLayer != nullptr)
	{
		auto ObjList = arrLayer->Get_GameObjects();

		for_each(ObjList.begin(), ObjList.end(), [&bSelected, &log, &iCount, &hFile, &dwByte, &szSaveMeshName, this](CGameObject* pGameObject)
			{
				CMapObject* pObject = static_cast<CMapObject*>(pGameObject);
				strcpy_s(szSaveMeshName,
					m_pGameInstance->WStringToString(pObject->Get_ModelName()).c_str());
				WriteFile(hFile, &szSaveMeshName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
				WriteFile(hFile, &pObject->Get_WorldMatrix(), sizeof(_float4x4), &dwByte, nullptr);

				iCount++;
				log = "Save... Save Object Count :  ";
				log += std::to_string(iCount);
				if (bSelected)
					LOG_TYPE(log, LOG_SAVE);
				//m_pImguiManager->Get_BackMsg()->strLogMsg = log;
			});

	}
	CloseHandle(hFile);

	//m_pImguiManager->Set_Lock(false);
	log = "Save Complete! FileName : ";
	log += m_szSaveFileName;
	if (bSelected)
		LOG_TYPE(log, LOG_SAVE);

	Load_SaveFileList();
}

void CMap_Tool_Manager::Load(bool bSelected)
{
	Object_Clear(false);
	
	string filename = m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]);
	string log = "";

	log = "Load Start... File Name : ";
	log += filename;
	_wstring strFullFilePath = (m_strObjectLoadFilePath + L"/" + m_arrSelectName[SAVE_LIST] + L".mchc");

	if (!bSelected)
		strFullFilePath = m_strCacheFilePath;

	HANDLE	hFile = CreateFile(strFullFilePath.c_str(),
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		log = "Load Failed... file Open Error - File Name : ";
		log += filename;
		if (bSelected)
			LOG_TYPE(log, LOG_ERROR);
		return;
	}
	_uint iCount = 0;
	log = "Load... Load Object Count :  ";
	log += std::to_string(iCount);
	if (bSelected)
		LOG_TYPE(log, LOG_LOAD);

	//m_pImguiManager->Set_Lock(true);

	const CLayer* arrLayer[2]{ m_pGameInstance->Find_Layer(LEVEL_TOOL_MAP, L"Layer_Environment")
	,m_pGameInstance->Find_Layer(LEVEL_TOOL_MAP,L"Layer_MapObject")
	};
	DWORD	dwByte(0);
	_uint iAllCount = 0;

	ReadFile(hFile, &iAllCount, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < iAllCount; i++)
	{
		_char		szSaveMeshName[MAX_PATH];
		_float4x4 vWorld = {};


		ReadFile(hFile, &szSaveMeshName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
		ReadFile(hFile, &vWorld, sizeof(_float4x4), &dwByte, nullptr);

		CMapObject::MAPOBJ_DESC NormalDesc = {};
		lstrcpy(NormalDesc.szModelName, m_pGameInstance->StringToWString(szSaveMeshName).c_str());
		NormalDesc.eCreateType = CMapObject::OBJ_LOAD;
		NormalDesc.matWorld = vWorld;



		CGameObject* pGameObject = nullptr;
		m_pGameInstance->Add_GameObject_ToLayer( LEVEL_TOOL_MAP, TEXT("Prototype_GameObject_MapObject"),
			LEVEL_TOOL_MAP,
			L"Layer_MapObject",
			&pGameObject,
			(void*)&NormalDesc);

		if (pGameObject)
		{
			iCount++;
			log = "Load... Loading Object Count :  ";
			log += std::to_string(iCount);
			if (bSelected)
				LOG_TYPE(log, LOG_SAVE);

			//m_pImguiManager->Get_BackMsg()->strLogMsg = log;
		}
		else
		{
			log = "Load Fail ! Model Prototype does not exist... : ";
			log += std::to_string(iCount);
			LOG_TYPE(log, LOG_ERROR);

		}
	}


	CloseHandle(hFile);
	//m_pImguiManager->Set_Lock(false);
	log = "Load Complete! FileName : ";
	log += m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]);
	if (bSelected)
		LOG_TYPE(log, LOG_LOAD);
}

void CMap_Tool_Manager::Object_Clear(_bool _bSelected)
{
	Object_Clear_PickingMode();
	Object_Close_PreviewMode();

	if (_bSelected)
		LOG_TYPE("Object All Clear", LOG_DELETE);
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_MAP, L"Layer_MapObject");
	DWORD	dwByte(0);

	if (pLayer != nullptr)
	{
		auto ObjList = pLayer->Get_GameObjects();
		
		for_each(ObjList.begin(), ObjList.end(), [](CGameObject* pGameObject) {
			OBJECT_DESTROY(pGameObject);
			});
	}
}


void CMap_Tool_Manager::Save_Popup()
{
	ImGui::OpenPopup("Save_Popup");
	strcpy_s(m_szSaveFileName, m_pGameInstance->WStringToString(m_arrSelectName[SAVE_LIST]).c_str());
}


HRESULT CMap_Tool_Manager::Picking_On_Terrain(_float3* fPickingPos, CMapObject** ppMap)
{
	_float3			vRayPos, vRayDir;

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fCursorPos = { (_float)ptMouse.x,(_float)ptMouse.y };
	Compute_World_PickingLay(&vRayPos, &vRayDir);

	auto pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_MAP, L"Layer_Environment");
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

HRESULT CMap_Tool_Manager::Picking_On_Terrain(_float3* fPickingPos)
{
	HRESULT hr = {};
	CMapObject* pToolObj = nullptr;

	if (SUCCEEDED(Picking_On_Terrain(fPickingPos, &pToolObj)))
		return S_OK;
	else
		return E_FAIL;
}

CMapObject* CMap_Tool_Manager::Picking_On_Object()
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fCursorPos = { (_float)ptMouse.x,(_float)ptMouse.y };

	auto pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL_MAP, L"Layer_GameObject");
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

CNavigationVertex* CMap_Tool_Manager::Picking_On_Vertex()
{
	_float3			vRayPos, vRayDir;

	Compute_World_PickingLay(&vRayPos, &vRayDir);
	return m_pCellContainor->Picking_On_Vertex(vRayPos, vRayDir);

}

HRESULT CMap_Tool_Manager::Create_Cell()
{
	_float3 vFirst = m_vecVertexStack[0]->Get_Pos();
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

	return E_FAIL;
}

HRESULT CMap_Tool_Manager::Compute_World_PickingLay(_float3* pLayPos, _float3* pLayDir)
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

void CMap_Tool_Manager::Clear_StackVertex()
{
	if (!m_vecVertexStack.empty())
	{
		for (auto& pVertex : m_vecVertexStack)
			OBJECT_DESTROY(pVertex);
		m_vecVertexStack.clear();
	}
}

void CMap_Tool_Manager::Clear_SelectCell()
{
	if (m_iSelectCellIndex != -1)
	{
		for (_uint i = 0; i < CEditableCell::POINT_END; i++)
		{
			m_pCellContainor->Get_CellList()[m_iSelectCellIndex]
				->Get_Point(CEditableCell::POINT(i))->Set_Mode(CNavigationVertex::NORMAL);
		}
		m_pCellContainor->Get_CellList()[m_iSelectCellIndex]->Set_Picking(false);
		m_iSelectCellIndex = -1;
	}
}

bool CMap_Tool_Manager::Check_VertexSelect()
{
	if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft) && !ImGuizmo::IsUsing())
	{
		// 셀렉트된 셀 클리어
		Clear_SelectCell();

		if (m_pPickingVertex)
		{
			m_pPickingVertex->Set_Mode(CNavigationVertex::NORMAL);
			m_pPickingVertex = nullptr;
		}

		_float3 fPos = {};
		CNavigationVertex* pVertex = Picking_On_Vertex();
		if (pVertex)
		{
			for (auto pStack : m_vecVertexStack)
				if (pStack == pVertex)
					return false;
			m_eNaviMode = NAV_EDIT;
			m_pPickingVertex = pVertex;
			pVertex->Set_Mode(CNavigationVertex::PICKING_VERTEX);
			return true;
		}

	}
	return false;
}

void CMap_Tool_Manager::Load_ModelList()
{
	m_ObjectFileLists.clear();
	_wstring strPath 
		= TEXT("../../Client/Bin/Resources/TestModels/");
		//= TEXT("../../Client/Bin/resources/Models/");

	for (const auto& entry : ::recursive_directory_iterator(strPath))
	{
		if (is_directory(entry))
		{
			for (const auto& file : ::recursive_directory_iterator(entry))
			{
				if (file.path().extension() == ".model")
				{
					wstring strKey = file.path().stem().wstring();
					m_ObjectFileLists.push_back(strKey);
				}
			}
		}
	}
}

void CMap_Tool_Manager::Load_SaveFileList()
{
}

void CMap_Tool_Manager::Object_Open_PickingMode()
{
	CMapObject* pGameObj = Picking_On_Object();
	if (pGameObj)
	{
		Object_Clear_PickingMode();
		m_arrObjects[OBJECT_PICKING] = pGameObj;
		m_arrObjects[OBJECT_PICKING]->Set_Mode(CMapObject::PICKING);
	}
}

void CMap_Tool_Manager::Object_Clear_PickingMode()
{
	if (m_arrObjects[OBJECT_PICKING])
	{
		static_cast<CMapObject*>(m_arrObjects[OBJECT_PICKING])->Set_Mode(CMapObject::NORMAL);
		m_arrObjects[OBJECT_PICKING] = nullptr;
	}

}

void CMap_Tool_Manager::Object_Open_PreviewMode()
{
	Object_Clear_PickingMode();

	if (m_arrSelectName[CREATE_OBJECT] != L"")
	{
		CMapObject::MAPOBJ_DESC NormalDesc = {};
		NormalDesc.eCreateType = CMapObject::OBJ_CREATE;

		lstrcpy(NormalDesc.szModelName, m_arrSelectName[CREATE_OBJECT].c_str());

		CGameObject* pGameObject = nullptr;

		m_pGameInstance->Add_GameObject_ToLayer( LEVEL_TOOL_MAP, TEXT("Prototype_GameObject_MapObject"),
			LEVEL_TOOL_MAP, L"Layer_GameObject", &pGameObject, (void*)&NormalDesc);
		
		if (nullptr != pGameObject)
		{
			m_arrObjects[OBJECT_PREVIEW] = static_cast<CMapObject*>(pGameObject);
			//Safe_AddRef(m_arrObjects[OBJECT_PREVIEW]);
			m_arrObjects[OBJECT_PREVIEW]->Set_Mode(CMapObject::PREVIEW);
		}
	}
}

void CMap_Tool_Manager::Object_Close_PreviewMode()
{
	if (m_arrObjects[OBJECT_PREVIEW])
	{
		OBJECT_DESTROY(m_arrObjects[OBJECT_PREVIEW]);
		m_arrObjects[OBJECT_PREVIEW] = nullptr;
	}
}



CMap_Tool_Manager* CMap_Tool_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	CMap_Tool_Manager* pInstance = new CMap_Tool_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : CMap_Tool_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap_Tool_Manager::Free()
{
	Save(false);

	Safe_Release(m_pCellContainor);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pLogger);
	__super::Free();
}
