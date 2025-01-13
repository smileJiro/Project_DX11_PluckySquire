#include "Imgui_Manager.h"


CImgui_Manager::CImgui_Manager()
{
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

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	__super::Free();

}
