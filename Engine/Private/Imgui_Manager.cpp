#include "Imgui_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Light.h"
#include "Camera_Manager_Engine.h"

CImgui_Manager::CImgui_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _float2 _vViewportSize)
{
	m_vViewportSize = _vViewportSize;
	m_hWnd = _hWnd;

	m_pDevice = _pDevice;
	m_pContext = _pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	if (nullptr == m_pDevice || nullptr == m_pContext)
	{
		MSG_BOX("No Existance Device or Context From Imgui");
		return E_FAIL;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	ImGuiIO& IO = ImGui::GetIO(); (void)IO;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Control

	//IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Gamepad Controls
	IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// 본인의 창 사이즈에 맞게 설정
	IO.DisplaySize = ImVec2{ m_vViewportSize.x, m_vViewportSize.y };

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::GetStyle().WindowRounding = 0.f;
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = { 0.1f, 0.1f, 0.1f, 0.8f };
	}


	// Setup Platform/Renderer backends
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
	ImGui_ImplWin32_Init(m_hWnd);

	return S_OK;
}

HRESULT CImgui_Manager::Start_Imgui()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame(); // 임구이 데이터 조작 시작!

	//ImGui::DockSpace(ImGui::GetID("MyDockSpace")); // 도킹 스페이스 생성

	// demo window를 보고 싶다면 아래의 주석을 풀어보십시오..
	//ImGui::ShowDemoWindow(); // Show demo window! :)
	return S_OK;
}

HRESULT CImgui_Manager::End_Imgui()
{
	ImGui::Render(); // 임구이 데이터 조작 끝! 드로우데이터 만들게~  이때 dx11기준 버텍스를 생성함

	return S_OK;
}

void CImgui_Manager::Render_DrawData()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // 렌더에서 만든 드로우데이터로 실제 ui를 그림
	ImGui::UpdatePlatformWindows();
}

HRESULT CImgui_Manager::LevelChange_Imgui()
{
	ImGui::DestroyContext();

	/* 새로 만들기. */
	if (nullptr == m_pDevice || nullptr == m_pContext)
	{
		MSG_BOX("No Existance Device or Context From Imgui");
		return E_FAIL;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO(); (void)IO;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Control

	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// 본인의 창 사이즈에 맞게 설정
	IO.DisplaySize = ImVec2{ m_vViewportSize.x, m_vViewportSize.y };

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::GetStyle().WindowRounding = 0.f;
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = { 0.1f, 0.1f, 0.1f, 0.8f };
	}

	// Setup Platform/Renderer backends
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
	ImGui_ImplWin32_Init(m_hWnd);


	return S_OK;
}

#ifdef _DEBUG

bool CImgui_Manager::Is_ImguiFocused() const
{
	if (!ImGui::GetCurrentContext())
		return false;

	// 어떤 ImGui 윈도우든 포커스를 가졌는가 (클릭 등으로 포커스 획득)
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		return true;

	// 혹은 어떤 아이템(입력창/버튼/슬라이더 등)이 활성(클릭/드래그) 상태인가
	if (ImGui::IsAnyItemActive())
		return true;

	return false;

	//if (!ImGui::GetCurrentContext())
	//	return false;

	//ImGuiIO& io = ImGui::GetIO();
	//return io.WantCaptureMouse || io.WantCaptureKeyboard || io.WantTextInput;
}

HRESULT CImgui_Manager::Imgui_Debug_Render()
{
	if (true == m_isImguiRTRender)
	{
		if (FAILED(Imgui_Debug_Render_RT()))
		{
			MSG_BOX("Render Failed Imgui_Render_RT_Debug");
		}
		if (FAILED(Imgui_Debug_Render_RT_FullScreen()))
		{
			MSG_BOX("Render Failed Imgui_Render_RT_Debug_FullScreen");
		}

	}
	if (true == m_isImguiObjRender)
	{
		if (FAILED(Imgui_Debug_Render_ObjectInfo()))
		{
			MSG_BOX("Render Failed Imgui_Debug_Render_ObjectInfo");
		}
	}

	Imgui_Debug_IBLGlobalVariable();
	Imgui_Debug_Lights();

	Imgui_LevelLightingTool(); 

	
	HWND hWnd = GetFocus();
	auto& io = ImGui::GetIO();
	if (nullptr != hWnd && !io.WantCaptureKeyboard)
	{
		if (KEY_DOWN(KEY::F7))
		{
			m_isImguiObjRender ^= 1;
		}
		if (KEY_DOWN(KEY::F8))
		{
			m_isImguiRTRender ^= 1;
		}
	}


	return S_OK;
}

HRESULT CImgui_Manager::Imgui_Debug_Render_RT()
{
	ImGui::Begin("DebugRenderTarget");

	// 기존 스타일 백업
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 originalColor = style.Colors[ImGuiCol_WindowBg];

	// 알파 값 제거
	style.Colors[ImGuiCol_WindowBg].w = 1.0f; // 알파를 1.0으로 설정
	map<const _wstring, CRenderTarget*>& RenderTargets = m_pGameInstance->Get_RenderTargets();
	ID3D11ShaderResourceView* pSelectImage = nullptr;
	ImVec2 imageSize(160, 90); // 이미지 크기 설정
	for (auto& Pair : RenderTargets)
	{
		pSelectImage = Pair.second->Get_SRV();
		if (nullptr != pSelectImage)
		{
			_string strRTName = m_pGameInstance->WStringToString(Pair.first);

			ImGui::Text(strRTName.c_str());
			ImGui::Image((ImTextureID)(uintptr_t)pSelectImage, imageSize);
		}

	}

	// 스타일 복구
	style.Colors[ImGuiCol_WindowBg] = originalColor;


	ImGui::End();

	return S_OK;
}

HRESULT CImgui_Manager::Imgui_Debug_Render_RT_FullScreen()
{
	ImGui::Begin("Debug FullScreen");


	if (ImGui::TreeNode("MRTs"))
	{
		map<const _wstring, list<CRenderTarget*>> MRTs = m_pGameInstance->Get_MRTs();
		ImVec2 imageSize(1600, 900); // 이미지 크기 설정
		ID3D11ShaderResourceView* pSelectImage = nullptr;
		if (MRTs.empty())
		{
			// MRTs가 비어 있는 경우에도 처리
			ImGui::Text("No MRTs available");
		}
		else
		{
			for (auto& MRT : MRTs)
			{
				_string strMRTName = m_pGameInstance->WStringToString(MRT.first);
				if (ImGui::TreeNode(strMRTName.c_str()))
				{

					// TODO :: 렌더타겟 이름을 별도로 저장하시고~ CRenderTarget에서 하든 뭘 하든. MRT >>> RT Name >> 선택하면 >>> 큰화면 ㄱㄱ

					for (CRenderTarget* pRenderTarget : MRT.second)
					{
						_string strRTName = m_pGameInstance->WStringToString(pRenderTarget->Get_Name());
						if (ImGui::TreeNode(strRTName.c_str()))
						{

							pSelectImage = m_pGameInstance->Get_RT_SRV(pRenderTarget->Get_Name());
							_float2 fRenderTargetSize = pRenderTarget->Get_Size();
							_float fDefaultwidth = 800.f;
							_float fYRatio = fRenderTargetSize.y * (fDefaultwidth / (_float)fRenderTargetSize.x);

							imageSize = { fDefaultwidth, fYRatio };
							if (nullptr != pSelectImage)
								ImGui::Image((ImTextureID)(uintptr_t)pSelectImage, imageSize);
							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
				}

			}


		}

		ImGui::TreePop();


	}

	map<const _wstring, list<CRenderTarget*>> MRTs = m_pGameInstance->Get_MRTs();
	ImVec2 imageSize(800, 450); // 이미지 크기 설정
	ID3D11ShaderResourceView* pSelectImage = nullptr;
	pSelectImage = m_pGameInstance->Get_RT_SRV(TEXT("Target_Shadow_0"));
	if (nullptr != pSelectImage)
	{
		ImGui::Image((ImTextureID)(uintptr_t)pSelectImage, imageSize);
	}
	//TEXT("Target_Shadow_");

	ImGui::End();



	return S_OK;
}

HRESULT CImgui_Manager::Imgui_Debug_Render_ObjectInfo()
{
	_int iAddIndex = 0;
	/* 트리노드로 Layer 들을 렌더한다. */
	ImGui::Begin("ObjectsInfo");
	HWND hWnd = GetFocus();
	auto& io = ImGui::GetIO();
	if (nullptr != hWnd && io.WantCaptureKeyboard)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			iAddIndex += 1;
		if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			iAddIndex -= 1;
	}
	static CGameObject* pSelectObject;
	pSelectObject = nullptr;
	if (ImGui::TreeNode("Object Layers")) // Layer
	{
		map<const _wstring, CLayer*>* pLayers = m_pGameInstance->Get_Layers_Ptr();
		if (nullptr != pLayers)
		{
			ImGui::Text("Current Level Layers");
			_int iCurLevelID = m_pGameInstance->Get_CurLevelID();
			for (auto& Pair : pLayers[iCurLevelID])
			{
				_int iSelectObjectTag = -1;
				_string LayerTag = m_pGameInstance->WStringToString(Pair.first);
				auto iter = m_LayerToSelectObjects.find(Pair.first);
				if (iter == m_LayerToSelectObjects.end())
					m_LayerToSelectObjects.try_emplace(Pair.first, -1);
				else
					iSelectObjectTag = (*iter).second;
				_uint iSelectLoopIndix = 0;

				if (ImGui::TreeNode(LayerTag.c_str())) // LayerTag
				{
					const list<CGameObject*> pGameObjects = Pair.second->Get_GameObjects();
					_uint iMaxObjectSize = (_uint)pGameObjects.size();
					ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
					_string strObjectListTag = "##ObjectList_" + LayerTag;
					if (ImGui::BeginListBox(strObjectListTag.c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
					{
						_uint iLoopIndix = 0;
						for (auto& pGameObject : pGameObjects)
						{
							string strGameObjectName;
							strGameObjectName = typeid(*pGameObject).name();
							_int iInstanceID = (_int)(pGameObject->Get_GameObjectInstanceID());
							strGameObjectName += "_" + to_string(iInstanceID);

							if (ImGui::Selectable(strGameObjectName.c_str(), iSelectObjectTag == iInstanceID))
							{
								if (iSelectObjectTag == iInstanceID)
									(*iter).second = -1;
								else
									(*iter).second = iInstanceID;
							}
							if((*iter).second == iInstanceID)
								iSelectLoopIndix = iLoopIndix;
							iLoopIndix++;
						}

						ImGui::EndListBox();
					}
					ImGui::PopItemFlag();
					if ((*iter).second != -1)
					{
						if ((iAddIndex != 0 && iSelectLoopIndix + iAddIndex >= 0) && iSelectLoopIndix + iAddIndex < iMaxObjectSize - 1)
						{
							pSelectObject = m_pGameInstance->Get_GameObject_Ptr(iCurLevelID, Pair.first, iSelectLoopIndix + iAddIndex);
							(*iter).second = pSelectObject->Get_GameObjectInstanceID();
						}
						else
							pSelectObject = m_pGameInstance->Get_GameObject_Ptr(iCurLevelID, Pair.first, iSelectLoopIndix);
					}

					if(ImGui::Button("On Render"))
					{
						for (auto& pGameObject : pGameObjects)
						{
							pGameObject->Set_Render(true);
						}
					}
					ImGui::SameLine();
					if(ImGui::Button("Off Render"))
					{
						for (auto& pGameObject : pGameObjects)
						{
							pGameObject->Set_Render(false);
						}
					}

					if (ImGui::Button("On Active"))
					{
						for (auto& pGameObject : pGameObjects)
						{
							pGameObject->Set_Active(true);
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Off Active"))
					{
						for (auto& pGameObject : pGameObjects)
						{
							pGameObject->Set_Active(false);
						}
					}




					ImGui::TreePop();
				}
			}

			/* Static Level */
			ImGui::Text("Static Level Layers");
			for (auto& Pair : pLayers[m_pGameInstance->Get_StaticLevelID()])
			{
				_string LayerTag = m_pGameInstance->WStringToString(Pair.first);
				if (ImGui::TreeNode(LayerTag.c_str())) // LayerTag
				{
					const list<CGameObject*>& GameObjects = Pair.second->Get_GameObjects();

					for (auto& pGameObject : GameObjects)
					{
						//pGameObject.
					}
					ImGui::TreePop();
				}
			}

		}

		ImGui::TreePop();
	}
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Separator();
	ImGui::Separator();
	ImGui::Text("<Select Object Info>");

	/* Object 세부 정보 렌더링 */
	if (nullptr != pSelectObject)
	{
		pSelectObject->Imgui_Render_ObjectInfos();
		if (ImGui::Button("Change Target"))
		{
			CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Camera"), 1));
			if (nullptr != pCamera)
				pCamera->Change_Target(pSelectObject->Get_ControllerTransform()->Get_WorldMatrix_Ptr());
		}

	}


	ImGui::End();

	return S_OK;
}

HRESULT CImgui_Manager::Imgui_Select_Debug_ObjectInfo(const wstring _strLayerTag, _uint _iObjectId)
{
	auto iter = m_LayerToSelectObjects.find(_strLayerTag);

	if (iter == m_LayerToSelectObjects.end())
		return E_FAIL;
	else
	{
		(*iter).second = _iObjectId;
	}
	
	return S_OK;

}

HRESULT CImgui_Manager::Imgui_Debug_Render_ObjectInfo_Detail(CGameObject* _pGameObject)
{
	return S_OK;
}

HRESULT CImgui_Manager::Imgui_Debug_IBLGlobalVariable()
{
	//
	ImGui::Begin("IBL_GlobalVariable");
	CONST_IBL tConstIBLData = m_pGameInstance->Get_GlobalIBLData();

	ImGui::SliderFloat("StrengthIBL", &tConstIBLData.fStrengthIBL, 0.0f, 10.0f, "%.4f");
	ImGui::SliderInt("SpecularBaseMipLevel", &tConstIBLData.iSpecularBaseMipLevel, 0, 10);
	ImGui::SliderFloat("RoughnessToMipFactor", &tConstIBLData.fRoughnessToMipFactor, 0.0f, 20.0f, "%.4f");
	ImGui::SliderFloat("HDRMaxLuminance", &tConstIBLData.fHDRMaxLuminance, 0.5f, 50.0f, "%.4f");

	static int iSelectedCubeMap = 0;
	if (ImGui::RadioButton("IBL_TEST", iSelectedCubeMap == IBLTEXTURE::IBL_TEST))
	{
		iSelectedCubeMap = (_int)IBLTEXTURE::IBL_TEST; // Flag 1 선택
		m_pGameInstance->Change_CubeMap(TEXT("Prototype_Component_Texture_TestEnv"));
	}
	if (ImGui::RadioButton("IBL_CHAPTER2_BRIGHT", iSelectedCubeMap == IBLTEXTURE::IBL_CHAPTER2_BRIGHT))
	{
		iSelectedCubeMap = (_int)IBLTEXTURE::IBL_CHAPTER2_BRIGHT; // Flag 1 선택
		m_pGameInstance->Change_CubeMap(TEXT("Prototype_Component_Texture_Chapter2_BrightEnv"));
	}
	if (ImGui::RadioButton("IBL_CHAPTER2_NIGHT", iSelectedCubeMap == IBLTEXTURE::IBL_CHAPTER2_NIGHT))
	{
		iSelectedCubeMap = (_int)IBLTEXTURE::IBL_CHAPTER2_NIGHT; // Flag 1 선택
		m_pGameInstance->Change_CubeMap(TEXT("Prototype_Component_Texture_Chapter2_NightEnv"));
	}
	if (ImGui::RadioButton("IBL_CHAPTER4", iSelectedCubeMap == IBLTEXTURE::IBL_CHAPTER4))
	{
		iSelectedCubeMap = (_int)IBLTEXTURE::IBL_CHAPTER4; // Flag 1 선택
		m_pGameInstance->Change_CubeMap(TEXT("Prototype_Component_Texture_Chapter4Env"));
	}
	
	if (ImGui::RadioButton("IBL_CHAPTER6", iSelectedCubeMap == IBLTEXTURE::IBL_CHAPTER6))
	{
		iSelectedCubeMap = (_int)IBLTEXTURE::IBL_CHAPTER6; // Flag 1 선택
		m_pGameInstance->Change_CubeMap(TEXT("Prototype_Component_Texture_Chapter6Env"));
	}
	if (ImGui::RadioButton("IBL_CHAPTER6_2", iSelectedCubeMap == IBLTEXTURE::IBL_CHAPTER6_2))
	{
		iSelectedCubeMap = (_int)IBLTEXTURE::IBL_CHAPTER6_2; // Flag 1 선택
		m_pGameInstance->Change_CubeMap(TEXT("Prototype_Component_Texture_Chapter6_2Env"));
	}

	int iSelectedFlag = tConstIBLData.iToneMappingFlag;
	if (ImGui::RadioButton("TONE_LINEAR", iSelectedFlag == 0))
		iSelectedFlag = 0; // Flag 1 선택
	if (ImGui::RadioButton("TONE_FILMIC", iSelectedFlag == 1))
		iSelectedFlag = 1; // Flag 1 선택
	if (ImGui::RadioButton("TONE_UNCHARTED2", iSelectedFlag == 2))
		iSelectedFlag = 2; // Flag 1 선택
	if (ImGui::RadioButton("TONE_LUMAREINHARD", iSelectedFlag == 3))
		iSelectedFlag = 3; // Flag 1 선택
	tConstIBLData.iToneMappingFlag = iSelectedFlag;

	ImGui::SliderFloat("Exposure", &tConstIBLData.fExposure, 0.1f, 10.0f, "%.4f");
	ImGui::SliderFloat("Gamma", &tConstIBLData.fGamma, 0.1f, 5.0f, "%.4f");


	m_pGameInstance->Set_GlobalIBLData(tConstIBLData, true);

	// Save 
	static char IBLPathBuffer[MAX_PATH] = "../Bin/DataFiles/IBL/default.json";
	ImGui::InputText("Save Path", IBLPathBuffer, sizeof(IBLPathBuffer));
	if (ImGui::Button("Save IBL Data"))
	{
		json IBLJson;
		IBLJson["fStrengthIBL"] = tConstIBLData.fStrengthIBL;
		IBLJson["iSpecularBaseMipLevel"] = tConstIBLData.iSpecularBaseMipLevel;
		IBLJson["fRoughnessToMipFactor"] = tConstIBLData.fRoughnessToMipFactor;
		IBLJson["fHDRMaxLuminance"] = tConstIBLData.fHDRMaxLuminance;
		IBLJson["iToneMappingFlag"] = tConstIBLData.iToneMappingFlag;
		IBLJson["fExposure"] = tConstIBLData.fExposure;
		IBLJson["fGamma"] = tConstIBLData.fGamma;

		std::ofstream outFile(IBLPathBuffer);
		if (false == outFile.is_open()) 
		{
			MSG_BOX("Failed IBL Data Save Path Open");
		}
		outFile << IBLJson.dump(4);
		outFile.close();
	}
	// Load Files
	ImGui::Separator();
	ImGui::Separator();

	if (ImGui::Button("Load IBL Data##IBL"))
	{
		OPENFILENAME ofn = {};
		ZeroMemory(&ofn, sizeof(ofn));
		_tchar szName[MAX_PATH] = {};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = m_pGameInstance->Get_HWND();
		ofn.lpstrFile = szName;
		ofn.nMaxFile = sizeof(szName);
		ofn.lpstrFilter = L".json\0*.json\0";
		ofn.nFilterIndex = 0;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		wstring strPath = std::filesystem::absolute(L"../Bin/DataFiles/IBL/").wstring();
		ofn.lpstrInitialDir = strPath.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileName(&ofn))
		{
			//받아온 파일입니다
			const _wstring strFilePath = szName;
			m_pGameInstance->Load_IBL(strFilePath);
		}
	}

	ImGui::End();
	return S_OK;
}

HRESULT CImgui_Manager::Imgui_Debug_Lights()
{
	ImGui::Begin("Lights");

	{ /* Add Light */
		static int iSelectedType = 0;
		if (ImGui::RadioButton("POINT##AddLight", iSelectedType == (int)LIGHT_TYPE::POINT))
			iSelectedType = (int)LIGHT_TYPE::POINT; // Flag 1 선택
		if (ImGui::RadioButton("DIRECTIONAL##AddLight", iSelectedType == (int)LIGHT_TYPE::DIRECTOINAL))
			iSelectedType = (int)LIGHT_TYPE::DIRECTOINAL; // Flag 1 선택
		if (ImGui::RadioButton("SPOT##AddLight", iSelectedType == (int)LIGHT_TYPE::SPOT))
			iSelectedType = (int)LIGHT_TYPE::SPOT; // Flag 1 선택
		if (ImGui::Button("Add_Light"))
		{
			CONST_LIGHT AddLightDesc = {};
			m_pGameInstance->Add_Light(AddLightDesc, (LIGHT_TYPE)iSelectedType);
		}
	} /* Add Light */

	{ /* Light Info */
		const list<CLight*>& Lights = m_pGameInstance->Get_Lights();
		static int selected_index = -1; // 선택된 항목의 인덱스
		static CONST_LIGHT tConstLightData = {};
		auto iter = Lights.begin();
		static auto& Selectiter = Lights.begin();
		_float fLength = 0.0f;
		if (true == Lights.empty())
		{
			Selectiter = Lights.end();
			ImGui::End();
			return S_OK;
		}
		static LIGHT_TYPE eType = LIGHT_TYPE::LAST;
		static LIGHT_TYPE eListType = LIGHT_TYPE::LAST;
		if (ImGui::BeginListBox("##Lights"))
		{
			for (int i = 0; i < Lights.size(); ++i)
			{
				bool is_selected = (selected_index == i);
				string strGameObjectName;
				eListType = (*iter)->Get_Type();
				switch (eListType)
				{
				case Engine::LIGHT_TYPE::POINT:
					strGameObjectName += "Point";
					break;
				case Engine::LIGHT_TYPE::DIRECTOINAL:
					strGameObjectName += "Directional";
					break;
				case Engine::LIGHT_TYPE::SPOT:
					strGameObjectName += "Spot";
					break;
				default:
					break;
				}
				strGameObjectName += "Light_" + to_string(i);
				if (ImGui::Selectable(strGameObjectName.c_str(), is_selected))
				{
					selected_index = i; // 선택된 항목 업데이트
					if(Selectiter != Lights.end())
					{
						(*Selectiter)->Set_DrawLightColor(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
					}
					(*iter)->Set_DrawLightColor(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
					tConstLightData = (*iter)->Get_LightDesc();
					fLength = (*iter)->Get_DirectionalLightLength();
					eType = (*iter)->Get_Type();
					Selectiter = iter;
				}

				// 선택된 항목에 포커스 설정
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

				++iter;
			}
			ImGui::EndListBox();

		
		}
		ImGui::Separator();
		ImGui::Separator();
		switch (eType)
		{
		case Engine::LIGHT_TYPE::POINT:
			ImGui::Text("LightType : POINT");
			break;
		case Engine::LIGHT_TYPE::DIRECTOINAL:
			ImGui::Text("LightType : DIRECTOINAL");
			break;
		case Engine::LIGHT_TYPE::SPOT:
			ImGui::Text("LightType : SPOT");
			break;
		default:
			break;
		}

		if (ImGui::DragFloat3("Position##Light", &tConstLightData.vPosition.x, 0.1f, -400.f, 400.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
			(*Selectiter)->Compute_ViewProjMatrix();
		}
		if (ImGui::DragFloat3("Direction##Light", &tConstLightData.vDirection.x, 0.01f, -1.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
			(*Selectiter)->Compute_ViewProjMatrix();
		}

		 // 전역 광원의 위치에 기여하는거리값.
		if (ImGui::SliderFloat("DirectionalLength##Light", &fLength, 0.0f, 300.f, "%.2f"))
		{
			(*Selectiter)->Set_DirectionalLightLength(fLength);
			(*Selectiter)->Compute_ViewProjMatrix();
		}

		if (ImGui::DragFloat3("Radiance##Light", &tConstLightData.vRadiance.x, 0.1f, 0.f, 500.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		}
		if (ImGui::DragFloat3("Diffuse##Light", &tConstLightData.vDiffuse.x, 0.05f, 0.f, 100.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		}
		if (ImGui::DragFloat3("Ambient##Light", &tConstLightData.vAmbient.x, 0.05f, 0.f, 100.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		}
		if (ImGui::DragFloat3("Specular##Light", &tConstLightData.vSpecular.x, 0.05f, 0.f, 100.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		}

		if (ImGui::SliderFloat("FallOutStart##Light", &tConstLightData.fFallOutStart, 0.0f, tConstLightData.fFallOutEnd, "%.3f"))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		};
		if (ImGui::SliderFloat("FallOutEnd##Light", &tConstLightData.fFallOutEnd, tConstLightData.fFallOutStart, 300.f, "%.2f"))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		};
		if (ImGui::SliderFloat("SpotPower##Light", &tConstLightData.fSpotPower, 0.0f, 10.f, "%.3f"))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		};

		
		
		if (ImGui::Button("Copy_Light"))
		{
			if (Selectiter != Lights.end())
			{
				CONST_LIGHT AddLightDesc = tConstLightData;
				m_pGameInstance->Add_Light(AddLightDesc, (LIGHT_TYPE)(*Selectiter)->Get_Type());
			}

		}
		// Delete Light 
		if (ImGui::Button("Delete Light"))
		{
			// Shadow가 현재 비활성상태이고, 타입이 point가 아닌경우,
			m_pGameInstance->Delete_Light(selected_index);
			Selectiter = Lights.begin();

		}

		if (ImGui::Button("Shadow On"))
		{
			if(false == (*Selectiter)->Is_ShadowLight() && LIGHT_TYPE::POINT != (*Selectiter)->Get_Type())
			{
				// Shadow가 현재 비활성상태이고, 타입이 point가 아닌경우,
				(*Selectiter)->Set_Shadow(true);
			}
		}
		ImGui::SameLine();
		// Delete Light 
		if (ImGui::Button("Shadow Off"))
		{
			if (true == (*Selectiter)->Is_ShadowLight() && LIGHT_TYPE::POINT != (*Selectiter)->Get_Type())
			{
				// Shadow가 현재 비활성상태이고, 타입이 point가 아닌경우,
				(*Selectiter)->Set_Shadow(false);
			}

		}
		if (ImGui::SliderFloat("ShadowFactor##Light", &tConstLightData.fShadowFactor, -2.0f, 2.0f, "%.3f"))
		{
			(*Selectiter)->Set_LightConstData_AndUpdateBuffer(tConstLightData);
		};


		// Save Lights Data 
		ImGui::Separator();
		ImGui::Separator();
		static char LightsPathBuffer[MAX_PATH] = "../Bin/DataFiles/DirectLights/default.json";
		ImGui::InputText("Save Path##Lights", LightsPathBuffer, sizeof(LightsPathBuffer));
		if (ImGui::Button("Save Lights Data##Lights"))
		{
			json LightsJson;
			auto Saveiter = Lights.begin();
			for (; Saveiter != Lights.end(); ++Saveiter)
			{
				if (nullptr != dynamic_cast<CLight_Target*>(*Saveiter))
				{
					continue;
				}
				CONST_LIGHT tConstLightData = (*Saveiter)->Get_LightDesc();
				LIGHT_TYPE eType = (*Saveiter)->Get_Type();
				json LightJson;

				switch (eType)
				{
				case Engine::LIGHT_TYPE::POINT:
					LightJson["eType"] = "POINT";
					break;
				case Engine::LIGHT_TYPE::DIRECTOINAL:
					LightJson["eType"] = "DIRECTOINAL";
					break;
				case Engine::LIGHT_TYPE::SPOT:
					LightJson["eType"] = "SPOT";
					break;
				}
				LightJson["vRadiance"]["x"] = tConstLightData.vRadiance.x;
				LightJson["vRadiance"]["y"] = tConstLightData.vRadiance.y;
				LightJson["vRadiance"]["z"] = tConstLightData.vRadiance.z;

				LightJson["fFallOutStart"] = tConstLightData.fFallOutStart;

				LightJson["vDirection"]["x"] = tConstLightData.vDirection.x;
				LightJson["vDirection"]["y"] = tConstLightData.vDirection.y;
				LightJson["vDirection"]["z"] = tConstLightData.vDirection.z;

				LightJson["fFallOutEnd"] = tConstLightData.fFallOutEnd;

				LightJson["vPosition"]["x"] = tConstLightData.vPosition.x;
				LightJson["vPosition"]["y"] = tConstLightData.vPosition.y;
				LightJson["vPosition"]["z"] = tConstLightData.vPosition.z;

				LightJson["fSpotPower"] = tConstLightData.fSpotPower;

				LightJson["vDiffuse"]["r"] = tConstLightData.vDiffuse.x;
				LightJson["vDiffuse"]["g"] = tConstLightData.vDiffuse.y;
				LightJson["vDiffuse"]["b"] = tConstLightData.vDiffuse.z;
				LightJson["vDiffuse"]["a"] = tConstLightData.vDiffuse.w;

				LightJson["vAmbient"]["r"] = tConstLightData.vAmbient.x;
				LightJson["vAmbient"]["g"] = tConstLightData.vAmbient.y;
				LightJson["vAmbient"]["b"] = tConstLightData.vAmbient.z;
				LightJson["vAmbient"]["a"] = tConstLightData.vAmbient.w;

				LightJson["vSpecular"]["r"] = tConstLightData.vSpecular.x;
				LightJson["vSpecular"]["g"] = tConstLightData.vSpecular.y;
				LightJson["vSpecular"]["b"] = tConstLightData.vSpecular.z;
				LightJson["vSpecular"]["a"] = tConstLightData.vSpecular.w;

				LightJson["isShadow"] = tConstLightData.isShadow;

				if(eType != LIGHT_TYPE::POINT && true == (_bool)tConstLightData.isShadow)
					LightJson["fShadowFactor"] = tConstLightData.fShadowFactor;

				LightsJson.push_back(LightJson);
			}


			std::ofstream outFile(LightsPathBuffer);
			if (false == outFile.is_open())
			{
				MSG_BOX("Failed Lights Data Save Path Open");
			}
			outFile << LightsJson.dump(4);
			outFile.close();
		}

		// Load Files
		ImGui::Separator();
		ImGui::Separator();

		if (ImGui::Button("Load Lights Data##Lights"))
		{
			OPENFILENAME ofn = {};
			ZeroMemory(&ofn, sizeof(ofn));
			_tchar szName[MAX_PATH] = {};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = m_pGameInstance->Get_HWND();
			ofn.lpstrFile = szName;
			ofn.nMaxFile = sizeof(szName);
			ofn.lpstrFilter = L".json\0*.json\0";
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			wstring strPath = std::filesystem::absolute(L"../Bin/DataFiles/DirectLights/").wstring();
			ofn.lpstrInitialDir = strPath.c_str();
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileName(&ofn))
			{
				//받아온 파일입니다
				const _wstring strFilePath = szName;
				m_pGameInstance->Load_Lights(strFilePath);
				Selectiter = Lights.begin();
				m_pSelectedLight = nullptr; // 임시코드
				m_pRenamingLight = nullptr;
				m_tEditLightBuffer = CONST_LIGHT();
			}
		}
		


		
	} /* Light Info */

	ImGui::End();

	return S_OK;
	
}

HRESULT CImgui_Manager::Imgui_LevelLightingTool()
{
	ImGui::SetNextWindowPos(ImVec2(3.0f, 3.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(600.0f, 800.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Level Lighting Tool");	
	string strToolID = "Level Lighting Tool";
	ImGui::PushID(strToolID.c_str());					

	string strPresetName = "Level Preset";
	bool isDirty = false;
	DrawLevelPresetBar(strPresetName.c_str(), isDirty);
	ImGui::Separator(); // 한 줄 아래 얇은 라인(예쁨)

	if (ImGui::BeginTabBar("LightingTabs"))
	{
		if (ImGui::BeginTabItem("Direct Light(PBR)"))
		{
			// 2컬럼 레이아웃
			const ImGuiTableFlags flags =
				ImGuiTableFlags_Resizable |
				ImGuiTableFlags_BordersInnerV |
				ImGuiTableFlags_SizingStretchProp;

			// 셀 패딩
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 oldPad = style.CellPadding;
			style.CellPadding = ImVec2(4.0f, 4.0f);

			if (ImGui::BeginTable("DirectLayout", 2, flags))
			{
				constexpr float iColumnLeftDefaultSize = 320.0f;
				ImGui::TableSetupColumn("LightsList", ImGuiTableColumnFlags_WidthFixed, iColumnLeftDefaultSize);
				ImGui::TableSetupColumn("LightProps", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableNextRow();

				// Left: Lights List
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(16, 16, 16, 220));
				ImGui::BeginChild("LightsListChild", ImVec2(0, 0), true);
				DrawLightsList();        //  TODO:: 여기서 반환된 Select Light(ID)를 detail로 띄운다
				ImGui::EndChild();

				// Right: Light Properties
				ImGui::TableSetColumnIndex(1);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(28, 28, 28, 220));
				ImGui::BeginChild("LightDetailsChild", ImVec2(0, 0), true);
				DrawLightDetails(); // 선택된 라이트 디테일
				ImGui::EndChild();

				ImGui::EndTable();
			}


			ImGui::EndTabItem(); // BeginTabItem("Direct Light(PBR)")
		}

		if (ImGui::BeginTabItem("Ambient Light(IBL)"))
		{



			ImGui::EndTabItem(); // BeginTabItem("Ambient Light(IBL)")
		}

		ImGui::EndTabBar(); 
	}



	ImGui::PopID();										// ID: Level Lighting Tool
	ImGui::End();										// Level Lighting Tool
	return S_OK;
}

void CImgui_Manager::DrawLevelPresetBar(const char* _szCurrentPresetName, bool _isDirty)
{
	// TODO:: Client의 Level에 대한 데이터 어떻게 받아올지 구조 고민 
	
	// UI 튜닝 값
	const float fPresetComboWidth = 260.0f; // 프리셋 콤보 폭
	const float fActionButtonWidth = 72.0f;  // Save/Load 버튼 폭
	const float fSaveAsButtonWidth = 88.0f;  // Save As 폭

	// 상단 한 줄을 2컬럼으로 분리: (좌)프리셋 (우)액션버튼
	const ImGuiTableFlags eBarFlags =
		ImGuiTableFlags_SizingStretchProp |
		ImGuiTableFlags_NoPadOuterX;

	if (ImGui::BeginTable("LevelPresetBar", 2, eBarFlags))
	{
		ImGui::TableSetupColumn("PresetArea", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("ActionsArea", ImGuiTableColumnFlags_WidthFixed, 0.0f);

		ImGui::TableNextRow();

		// ----------------
		// Left: Preset Area
		// ----------------
		ImGui::TableSetColumnIndex(0);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Current Level:");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(fPresetComboWidth);


		const vector<string>& LevelNames = m_pGameInstance->Get_LevelNames();
		int iCurLevelID = m_pGameInstance->Get_CurLevelID();
		if (iCurLevelID == -1)
		{
			ImGui::EndTable();
			return;
		}

		const char* szCurrent = LevelNames[iCurLevelID].empty() ? "Level Name" : LevelNames[iCurLevelID].c_str();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.75f, 0.25f, 1.0f));
		ImGui::TextUnformatted(szCurrent);
		ImGui::PopStyleColor();

		ImGui::SameLine(0.0f, 20.0f);


		
		// ----------------
		// Right: Actions Area (오른쪽 정렬 느낌)
		// ----------------
		ImGui::TableSetColumnIndex(1);

		const ImGuiStyle& tStyle = ImGui::GetStyle();

		// 액션 그룹의 폭을 대략 계산 (버튼 3개 + badge)
		float fActionsWidth = 0.0f;
		fActionsWidth += fActionButtonWidth + tStyle.ItemSpacing.x; // Save
		fActionsWidth += fActionButtonWidth + tStyle.ItemSpacing.x; // Load
		fActionsWidth += fSaveAsButtonWidth;                        // Save As

		if (_isDirty)
		{
			fActionsWidth += tStyle.ItemSpacing.x;
			fActionsWidth += 14.0f + ImGui::CalcTextSize("Unsaved").x; // 점 + 텍스트(짧게)
		}

		// 현재 컬럼의 사용 가능 폭
		float fActionsAvail = ImGui::GetContentRegionAvail().x;
		if (fActionsAvail > fActionsWidth)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (fActionsAvail - fActionsWidth));

		// 실제 버튼들
		if (ImGui::Button("Save", ImVec2(fActionButtonWidth, 0))) { /* TODO */ }
		ImGui::SameLine();
		if (ImGui::Button("Load", ImVec2(fActionButtonWidth, 0))) { /* TODO */ }
		ImGui::SameLine();
		if (ImGui::Button("Save As", ImVec2(fSaveAsButtonWidth, 0))) { /* TODO */ }

		if (_isDirty)
		{
			ImGui::SameLine();

			// 노란 점 + "Unsaved"
			ImDrawList* pDrawList = ImGui::GetWindowDrawList();
			ImVec2 vScreenPos = ImGui::GetCursorScreenPos();

			const float fRadius = 5.0f;
			ImVec2 vCenter(vScreenPos.x + fRadius, vScreenPos.y + ImGui::GetFontSize() * 0.5f);

			pDrawList->AddCircleFilled(vCenter, fRadius, IM_COL32(240, 200, 40, 255));

			ImGui::Dummy(ImVec2(fRadius * 2 + 5.0f, 0));
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Unsaved");
		}

		ImGui::EndTable();
	}
}

void CImgui_Manager::DrawLightsList()
{

	const list<CLight*>& LightsList = m_pGameInstance->Get_Lights();

	const ImGuiTreeNodeFlags SectionHeaderFlags = ImGuiTreeNodeFlags_DefaultOpen |
												   ImGuiTreeNodeFlags_Framed |
												   ImGuiTreeNodeFlags_SpanAvailWidth |
												   ImGuiTreeNodeFlags_FramePadding;

	const ImGuiTableFlags TableFlags = ImGuiTableFlags_RowBg |
		ImGuiTableFlags_SizingStretchProp |
		ImGuiTableFlags_NoBordersInBody |
		ImGuiTableFlags_PadOuterX;
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted("Lights List");
	ImGui::SameLine();
	const float fAddBtnWidth = 48.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - fAddBtnWidth);

	if (ImGui::Button("Add", ImVec2(fAddBtnWidth, 0)))
	{
		ImGui::OpenPopup("AddLightPopup");
	}

	if (ImGui::BeginPopup("AddLightPopup"))
	{
		LIGHT_TYPE eAddLightType = LIGHT_TYPE::LAST;
		if (ImGui::MenuItem("Directional")) { eAddLightType = LIGHT_TYPE::DIRECTOINAL; }
		if (ImGui::MenuItem("Point")) { eAddLightType = LIGHT_TYPE::POINT; }
		if (ImGui::MenuItem("Spot")) { eAddLightType = LIGHT_TYPE::SPOT; }

		if (eAddLightType != LIGHT_TYPE::LAST)
		{
			m_pGameInstance->Add_Light(CONST_LIGHT(), (LIGHT_TYPE)eAddLightType);
		}

		ImGui::EndPopup();
	}

	ImGui::Separator();
	

	// -------------------------
	// Directional
	// -------------------------
	if (ImGui::CollapsingHeader("Directional", SectionHeaderFlags))
	{
		DrawLightsListTable(LightsList, "DirectionalTable", TableFlags, LIGHT_TYPE::DIRECTOINAL);
	}

	// -------------------------
	// Point
	// -------------------------
	if (ImGui::CollapsingHeader("Point", SectionHeaderFlags))
	{
		DrawLightsListTable(LightsList, "PointTable", TableFlags, LIGHT_TYPE::POINT);
	}

	// -------------------------
	// Spot
	// -------------------------
	if (ImGui::CollapsingHeader("Spot", SectionHeaderFlags))
	{
		DrawLightsListTable(LightsList, "SpotTable", TableFlags, LIGHT_TYPE::SPOT);
	}

	// (선택) 디버그용: 현재 선택된 라이트 표시
	if (m_pSelectedLight)
	{
		ImGui::Separator();
		//ImGui::Text("Selected: %s", m_pSelectedLight->().c_str());
	}
}

void CImgui_Manager::DrawLightsListTable(const list<CLight*>& LightsList, const char* _szTableName, const ImGuiTableFlags _flagTable, LIGHT_TYPE _eLightType)
{
	if (ImGui::BeginTable(_szTableName, 2, _flagTable))
	{
		ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_WidthFixed, 50.f);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);

		_int iPendingDeleteLightID = -1;
		for (CLight* Light : LightsList)
		{
			if (!Light) continue;
			if (Light->Get_Type() != _eLightType) // enum/함수명 맞추기				// Param 1 : Light Type enum
				continue;

			ImGui::PushID(Light);
			ImGui::TableNextRow();

			// Active
			ImGui::TableSetColumnIndex(0);
			bool isActive = Light->Is_Active();
			const bool isLocked = (dynamic_cast<CLight_Target*>(Light) != nullptr); // TODO:: 나중에 가상함수 기반으로 변경하자

			if (isLocked)
			{
				ImGui::BeginDisabled(true);
				ImGui::Checkbox("##active", &isActive);
				ImGui::EndDisabled();

				// (선택) 왜 잠겼는지 툴팁
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled))
					ImGui::SetTooltip("Target Lights are controlled automatically and cannot be toggled manually.");
			}
			else
			{
				if (ImGui::Checkbox("##active", &isActive))
					Light->Set_Active(isActive);
			}

			// Name (single selection)
			ImGui::TableSetColumnIndex(1);
			bool bSelected = (m_pSelectedLight == Light);
			
			if (m_pRenamingLight != Light) // (1) 평소 Selectable (리네임 중이 아님)
			{
				// 좌클릭으로 라이트 선택
				if (ImGui::Selectable(Light->Get_Name().c_str(), bSelected, ImGuiSelectableFlags_SpanAllColumns))
				{
					Set_SelectedLight(Light);
				}

				// 우클릭해도 팝업과 함께 선택
				if (ImGui::BeginPopupContextItem("LightItemContext"))
				{
					Set_SelectedLight(Light);
					if (ImGui::MenuItem("Focus Selected"))
					{
						// 카메라 세팅을 프리캠으로 변경하고
						// 프리캠을 라이트 기준 -z축으로 10만큼 떨어진 위치로 이동
						_float3 vLightPosition = m_pSelectedLight->Get_LightDesc_Ptr()->vPosition;
						CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), TEXT("Layer_Camera"), 0));
						pCamera->Set_Position(XMVectorSet(vLightPosition.x, vLightPosition.y + 2.5f, vLightPosition.z - 10.0f, 1.0f)); // TODO:: World Scale 이 있으면 그거 기준으로 하면 좋을듯함, 일단 매직넘버로 처리해둠
						pCamera->Get_ControllerTransform()->LookAt_3D(XMVectorSet(vLightPosition.x, vLightPosition.y, vLightPosition.z, 1.0f));
					}
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
						ImGui::SetTooltip("Moves the camera to focus on the selected light.");

					if (ImGui::MenuItem("Delete"))
					{
						iPendingDeleteLightID = Light->Get_LightID();
						ImGui::CloseCurrentPopup();
					}
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
						ImGui::SetTooltip("Delete selected light.");

					ImGui::EndPopup();
				}

				// F2로 rename 시작 (선택된 항목에 대해서만)
				// 리스트 창이 포커스일 때만 동작시키면 UX 안전함
				const bool bListFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
				if (bListFocused && bSelected && ImGui::IsKeyPressed(ImGuiKey_F2))
				{
					if (!Light)
						return;

					m_pRenamingLight = Light;
					strncpy_s(m_RenameBuffer, Light->Get_Name().c_str(), sizeof(m_RenameBuffer));

					m_RenameFocusRequested = true;
					m_RenameEverActive = false;
				}

				// (선택) 더블클릭도 지원하고 싶으면
				// if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) BeginRename(Light);
			}
			else // (2) Rename 중: InputText로 스왑
			{
				
				ImGui::SetNextItemWidth(-FLT_MIN);

				if (m_RenameFocusRequested)
				{
					ImGui::SetKeyboardFocusHere();       // "바로 아래 InputText"에 포커스 보장
					m_RenameFocusRequested = false;
				}

				const bool enterPressed = ImGui::InputText(
					"##Rename",
					m_RenameBuffer,
					sizeof(m_RenameBuffer),
					ImGuiInputTextFlags_EnterReturnsTrue |
					ImGuiInputTextFlags_AutoSelectAll
				);

				// Active였던 적이 있어야 blur-commit 허용 (첫 프레임 즉시 커밋 버그 방지)
				if (ImGui::IsItemActive())
					m_RenameEverActive = true;

				// Enter = 확정
				if (enterPressed)
				{
					Light->Set_Name(m_RenameBuffer);
					m_pRenamingLight = nullptr;
					m_RenameEverActive = false;
				}
				// Esc = 취소
				else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				{
					m_pRenamingLight = nullptr;
					m_RenameEverActive = false;
				}
				else
				{
					// Blur = 확정 (탐색기 느낌) : 단, Active 경험이 있을 때만
					if (m_RenameEverActive && !ImGui::IsItemActive() && !ImGui::IsItemFocused())
					{
						Light->Set_Name(m_RenameBuffer);
						m_pRenamingLight = nullptr;
						m_RenameEverActive = false;
					}
				}
			}


			ImGui::PopID();
		}


		// 지연 삭제 처리
		if (iPendingDeleteLightID != -1)
		{
			m_pGameInstance->Delete_Light(iPendingDeleteLightID);

			if (m_pSelectedLight && m_pSelectedLight->Get_LightID() == iPendingDeleteLightID)
				Set_SelectedLight(nullptr);
			if (m_pRenamingLight && m_pRenamingLight->Get_LightID() == iPendingDeleteLightID)
				m_pRenamingLight = nullptr;
		}


		ImGui::EndTable();
	}
}

void CImgui_Manager::DrawLightDetails()
{
	// 타이틀
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted("Details");
	ImGui::Separator();

	if (!m_pSelectedLight)
	{
		ImGui::TextDisabled("No light selected.");
		return;
	}

	// 섹션들
	DrawLightDetails_Transform();
	DrawLightDetails_Color();
	DrawLightDetails_Attenuation();
	DrawLightDetails_Shadow();
	DrawLightDetails_IO();

	if (m_isEditDirty)
	{
		m_isEditDirty = false;
		ApplyEdit_Light();
	}
}

void CImgui_Manager::DrawLightDetails_Transform()
{
	if (!ImGui::CollapsingHeader("Transform##LightDetails", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	m_isEditDirty |= ImGui::DragFloat3("Position##LightDetails", &m_tEditLightBuffer.vPosition.x, 0.05f);
	if (ImGui::DragFloat3("Direction", &m_tEditLightBuffer.vDirection.x, 0.01f)) 
	{ 
		m_isEditDirty = true; 
		XMStoreFloat3(&m_tEditLightBuffer.vDirection, XMVector3Normalize(XMLoadFloat3(&m_tEditLightBuffer.vDirection))); 
	}
	if (m_pSelectedLight->Get_Type() == LIGHT_TYPE::DIRECTOINAL)
	{
		m_isEditDirty |= ImGui::DragFloat("DirectionalLightDistance##LightDetails", &m_tEditLightBuffer.fDirectionalLightDistance, 0.1f);
	}
		
}

void CImgui_Manager::DrawLightDetails_Color()
{
	if (!ImGui::CollapsingHeader("Color##LightDetails", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	ImGuiColorEditFlags flags =
		//ImGuiColorEditFlags_HDR |        // 1.0 이상 허용
		ImGuiColorEditFlags_Float |		   // float 기반
		ImGuiColorEditFlags_DisplayRGB |   // RGB 표시
		ImGuiColorEditFlags_InputRGB;	   // RGB 입력

	m_isEditDirty |= ImGui::ColorEdit3("Diffuse##LightDetails",	&m_tEditLightBuffer.vDiffuse.x,	flags);
	m_isEditDirty |= ImGui::ColorEdit3("Ambient##LightDetails",	&m_tEditLightBuffer.vAmbient.x,	flags);
	m_isEditDirty |= ImGui::ColorEdit3("Specular##LightDetails",	&m_tEditLightBuffer.vSpecular.x,flags);
	ImGui::Separator();
	m_isEditDirty |= ImGui::DragFloat3("Radiance##LightDetails", &m_tEditLightBuffer.vRadiance.x, 0.05f, 0.0f, 100.0f);

}

void CImgui_Manager::DrawLightDetails_Attenuation()
{
	if (!ImGui::CollapsingHeader("Attenuation##LightDetails", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	if (m_pSelectedLight->Get_Type() == LIGHT_TYPE::DIRECTOINAL)
	{
		ImGui::TextDisabled("Attenuation does not apply to directional lights.");
		return;
	}

	m_isEditDirty |= ImGui::DragFloat("FalloutStart##LightDetails", &m_tEditLightBuffer.fFallOutStart, 0.05f, 0.0f, 500.0f);
	m_isEditDirty |= ImGui::DragFloat("FalloutEnd##LightDetails", &m_tEditLightBuffer.fFallOutEnd, 0.05f, 0.0f, 1000.0f);
}

void CImgui_Manager::DrawLightDetails_Shadow()
{
}

void CImgui_Manager::DrawLightDetails_IO()
{
}

void CImgui_Manager::Set_SelectedLight(CLight* _pNewLight)
{
	if (m_pSelectedLight == _pNewLight)
		return;

	if (m_pSelectedLight)
	{
		m_pSelectedLight->Set_DrawLightColor(XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
		if (m_isEditDirty)
		{
			m_isEditDirty = false;
			ApplyEdit_Light();
		}
	}
	if (_pNewLight)
	{
		_pNewLight->Set_DrawLightColor(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
		m_tEditLightBuffer = _pNewLight->Get_LightDesc();
	}

	m_pSelectedLight = _pNewLight;
	
}

void CImgui_Manager::ApplyEdit_Light()
{
	m_pSelectedLight->Set_LightConstData_AndUpdateBuffer(m_tEditLightBuffer);
	// (권장) Set 내부에서 clamp/fixup이 있을 수 있으니 최종 값으로 버퍼 동기화
	m_tEditLightBuffer = m_pSelectedLight->Get_LightDesc();
	m_pSelectedLight->Compute_ViewProjMatrix();
}


#endif // _DEBUG



CImgui_Manager* CImgui_Manager::Create(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _float2 _vViewportSize)
{
	CImgui_Manager* pInstance = new CImgui_Manager();

	if (FAILED(pInstance->Initialize(_hWnd, _pDevice, _pContext, _vViewportSize)))
	{
		MSG_BOX("Failed to Created : CImgui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImgui_Manager::Free()
{
	// Imgui Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	__super::Free();

}
