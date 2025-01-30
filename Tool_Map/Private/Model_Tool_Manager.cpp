#include "stdafx.h"
#include "Model_Tool_Manager.h"
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




CModel_Tool_Manager::CModel_Tool_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext) :
	m_pDevice(_pDevice),
	m_pContext(_pContext), m_pGameInstance(Engine::CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CModel_Tool_Manager::Initialize(CImguiLogger* _pLogger)
{
	// Logger 등록
	m_pLogger = _pLogger;
	Safe_AddRef(m_pLogger);

	ZeroMemory(m_szSaveFileName, sizeof(m_szSaveFileName));

	m_pGameInstance->Set_DebugRender(true);

	// 임구이 크기 설정
	ImGui::SetNextWindowSizeConstraints(ImVec2(600, 1200), ImVec2(FLT_MAX, FLT_MAX));

	// 모델 리스트 불러오기
	Load_ModelList();

	m_pMapParsingManager = CTask_Manager::Create(m_pDevice, m_pContext, m_pLogger);
	if (nullptr == m_pMapParsingManager)
		return E_FAIL;


	m_matView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	m_matProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Model_Preview"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.22f, 0.22f, 0.22f, 1.0f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Model_Preview"), TEXT("Model_Preview"))))
		return E_FAIL;
	return S_OK;
}

void CModel_Tool_Manager::Update_Tool()
{
	// 입력 설정 Navigation Tool Mode - Obejct Tool Mode 전환하여. 나중에 씬 분리할것
	Input_Logic();

	// 임구이 화면 구성
	Update_Imgui_Logic();

	m_pMapParsingManager->Update();



}


void CModel_Tool_Manager::Update_Imgui_Logic()
{
	// 참 조 정 리 
	if (m_pCurObject && m_pCurObject->Is_Dead())
		m_pCurObject = nullptr;
	Model_Collider_Imgui();
}



void CModel_Tool_Manager::Input_Logic()
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

void CModel_Tool_Manager::Model_Collider_Imgui(_bool _bLock)
{

	#pragma region Tap 고정 설정
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	POINT clientPos = { clientRect.left, clientRect.top };
	ClientToScreen(g_hWnd, &clientPos);
	ImGui::SetNextWindowPos(ImVec2(
		(_float)clientPos.x + (_float)(clientRect.right - clientRect.left) - 400.f, (_float)clientPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(400.f, (_float)(clientRect.bottom - clientRect.top)), ImGuiCond_Always);
	ImVec4 customBackgroundColor = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, customBackgroundColor);
#pragma endregion

	ImGui::Begin("Model Select", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	{
		if (ImGui::BeginTabBar("ModelTap", ImGuiTabBarFlags_None))
		{
			#pragma region Select Model Tap
			if (ImGui::BeginTabItem("Select Model"))
			{
				ImGui::SeparatorText("Model List");
				#pragma region MODEL_LIST_FILTER
				static char searchBuffer[128] = ""; // 검색어 입력을 위한 버퍼

				// 검색어에 따라 필터링된 항목을 저장할 임시 벡터
				static std::vector<pair<_wstring, _wstring>> filteredItems;

				// 검색 입력 필드
				ImGui::SetNextItemWidth(-FLT_MIN - 50.f);
				ImGui::InputText("Filter", searchBuffer, IM_ARRAYSIZE(searchBuffer));
				filteredItems.clear();
				_wstring searchTerm = StringToWstring(searchBuffer);
				std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);
				for (const auto& item : m_ObjectFileLists)
				{
					_wstring lowerItem = item.first;
					std::transform(lowerItem.begin(), lowerItem.end(), lowerItem.begin(), ::tolower);
					if (lowerItem.find(searchTerm) != _wstring::npos)
						filteredItems.push_back(item);
				}
#pragma endregion
				#pragma region Model List
				if (ImGui::BeginListBox("##Model List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (const auto& PairFileInfo : filteredItems)
					{
						_char szName[MAX_PATH] = {};

						WideCharToMultiByte(CP_ACP, 0, PairFileInfo.first.c_str(), -1, szName, (_int)(lstrlen(PairFileInfo.first.c_str())), NULL, NULL);

						if (ImGui::Selectable(szName, PairFileInfo.first == m_arrSelectName[SELECT_MODEL])) {
							if (m_arrSelectName[SELECT_MODEL] == PairFileInfo.first)
							{
								//m_arrSelectName[SELECT_MODEL] = L"";
							}
							else
							{
								m_arrSelectName[SELECT_MODEL] = PairFileInfo.first;

								Safe_Release(m_pPreviewObject);

								CMapObject::MAPOBJ_DESC NormalDesc = {};
								lstrcpy(NormalDesc.szModelName, m_arrSelectName[SELECT_MODEL].c_str());
								NormalDesc.eCreateType = CMapObject::OBJ_CREATE;
								NormalDesc.iCurLevelID = LEVEL_TOOL_3D_MODEL;
								NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TOOL_3D_MODEL;

								CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, L"Prototype_GameObject_MapObject", &NormalDesc);

								if (nullptr != pObj)
								{

									m_pPreviewObject = static_cast<CMapObject*>(pObj);
									m_pPreviewObject->Set_Mode(CMapObject::PREVIEW);

									m_pGameInstance->Clear_MRT(L"MRT_Model_Preview");

									if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Model_Preview", false)))
										LOG_TYPE("PreView Error", LOG_ERROR);

									m_pPreviewObject->Render_Preview(&m_matView, &m_matProj);

									if (FAILED(m_pGameInstance->End_MRT()))
										LOG_TYPE("PreView Error", LOG_ERROR);

								}
							}
						}
					}
					ImGui::EndListBox();
				}
#pragma endregion
				#pragma region PreView Model Info
				if (nullptr != m_pPreviewObject)
				{
					ImGui::SeparatorText("Select Model Info");

					ID3D11ShaderResourceView* pSelectImage = nullptr;
					pSelectImage = m_pGameInstance->Get_RT_SRV(L"Model_Preview");
					ImVec2 imageSize(385, 216);
					if (nullptr != pSelectImage)
						ImGui::Image((ImTextureID)(uintptr_t)pSelectImage, imageSize);
					Begin_Draw_ColorButton("##EditModelButtonStyle", ImVec4(0.663f, 0.922f, 0.647f, 1.0f));
					if (StyleButton(ALIGN_SQUARE, "Edit Model", 2.f))
					{
						if (m_pCurObject)
							Event_DeleteObject(m_pCurObject);

						CMapObject::MAPOBJ_DESC NormalDesc = {};
						lstrcpy(NormalDesc.szModelName, m_pPreviewObject->Get_ModelName().c_str());
						NormalDesc.eCreateType = CMapObject::OBJ_CREATE;
						NormalDesc.iCurLevelID = LEVEL_TOOL_3D_MODEL;
						NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TOOL_3D_MODEL;
						CGameObject* pGameObject = nullptr;
						m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
							LEVEL_TOOL_3D_MAP,
							L"Layer_EditMapObject",
							&pGameObject,
							(void*)&NormalDesc);

						if (pGameObject)
							m_pCurObject = static_cast<CMapObject*>(pGameObject);
					}
					End_Draw_ColorButton();
				}
#pragma endregion
				ImGui::EndTabItem();
			}
#pragma endregion
			if (ImGui::BeginTabItem("Edit Model Material"))
			{
				Model_Material_Imgui();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Edit Model Collider"))
			{
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void CModel_Tool_Manager::Model_Material_Imgui(_bool _bLock)
{
#ifdef _DEBUG

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


	if (m_pCurObject)
	{
		ImGui::SeparatorText("Model Info");

		CMapObject* pTargetObj = m_pCurObject;

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
			if (0 < iTextureType)
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

										_wstring strNameAndExt = PairResult.first + L"." + PairResult.second;

										lstrcpy(tAddInfo.szTextureName, strNameAndExt.c_str());
										if (FAILED(pTargetObj->Add_Textures(tAddInfo, iTextureType)))
										{
											LOG_TYPE("Add Texture Failed... -> " + WstringToString(strModelName), LOG_ERROR);
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

					//if (tDiffuseInfo.iDiffuseIIndex == iCurrenrTextureIndex)
						//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.9f, 1.0f)); // 선택된 버튼의 배경색

					if (ImGui::ImageButton(strButtonTag.c_str(), (ImTextureID)tDiffuseInfo.pSRV, size, uv0, uv1, bg_col, tint_col))
						pTargetObj->Change_TextureIdx(tDiffuseInfo.iDiffuseIIndex, iTextureType, tDiffuseInfo.iMaterialIndex);

					//if (tDiffuseInfo.iDiffuseIIndex == iCurrenrTextureIndex)
						//ImGui::PopStyleColor(1);

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
#endif // _DEBUG
}

void CModel_Tool_Manager::Load(const _string& strFileName, _bool _bSelected)
{
	//Object_Clear(false);

	_wstring wstrFileName = StringToWstring(strFileName);
	string log = "";

	log = "Load Start... File Name : ";
	log += strFileName;
	_wstring strFullFilePath = MAP_3D_DEFAULT_PATH;
	strFullFilePath += L"/" + wstrFileName + L".mchc";



	if (!Path_String_Validation_Check(strFileName))
	{
		if (_bSelected)
		{
			log = "Load Failed... File Path Error - File Path : ";
			log += strFileName;
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
		log += strFileName;
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
			NormalDesc.iCurLevelID = LEVEL_TOOL_3D_MODEL;
			NormalDesc.iModelPrototypeLevelID_3D = LEVEL_TOOL_3D_MODEL;
			NormalDesc.tTransform3DDesc.isMatrix = true;
			NormalDesc.tTransform3DDesc.matWorld = vWorld;



			CGameObject* pGameObject = nullptr;
			m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
				LEVEL_TOOL_3D_MODEL,
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
	log += strFileName;
	if (_bSelected)
		LOG_TYPE(log, LOG_LOAD);
}

HRESULT CModel_Tool_Manager::Compute_World_PickingLay(_float3* pLayPos, _float3* pLayDir)
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

void CModel_Tool_Manager::Load_ModelList()
{
	m_ObjectFileLists.clear();
	_wstring strPath
		= STATIC_3D_MODEL_FILE_PATH;
	strPath += L"/NonAnim";

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

CModel_Tool_Manager* CModel_Tool_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	CModel_Tool_Manager* pInstance = new CModel_Tool_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : CModel_Tool_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Tool_Manager::Free()
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
