#include "Imgui_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Layer.h"

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
		ImVec2 imageSize(800, 450); // 이미지 크기 설정
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
						if (iAddIndex != 0 && iSelectLoopIndix + iAddIndex >= 0 && iSelectLoopIndix + iAddIndex < iMaxObjectSize)
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
	ImGui::Begin("IBL_GlobalVarialbe");
	CONST_IBL tConstIBLData = m_pGameInstance->Get_GlobalIBLData();

	ImGui::SliderFloat("StrengthIBL", &tConstIBLData.fStrengthIBL, 0.0f, 10.0f);
	ImGui::SliderInt("SpecularBaseMipLevel", &tConstIBLData.iSpecularBaseMipLevel, 0, 10);
	ImGui::SliderFloat("RoughnessToMipFactor", &tConstIBLData.fRoughnessToMipFactor, 0.0f, 20.0f);
	ImGui::SliderFloat("HDRMaxLuminance", &tConstIBLData.fHDRMaxLuminance, 0.5f, 50.0f);

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

	ImGui::SliderFloat("Exposure", &tConstIBLData.fExposure, 0.1f, 10.0f);
	ImGui::SliderFloat("Gamma", &tConstIBLData.fGamma, 0.1f, 5.0f);


	m_pGameInstance->Set_GlobalIBLData(tConstIBLData, true);
	ImGui::End();
	return S_OK;
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
