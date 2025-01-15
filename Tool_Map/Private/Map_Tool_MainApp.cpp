#include "stdafx.h"
#include "Map_Tool_MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Event_Manager.h"
#include <gizmo/ImGuizmo.h>


CMap_Tool_MainApp::CMap_Tool_MainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMap_Tool_MainApp::Initialize()
{
	// MainApp Init
	// 1. 윈도우 해상도에 대한 정보를 생성하고, 이를 통해 장치를 초기화한다.
	// 2. Level을 생성하고 초기화한다.

	ENGINE_DESC EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iNumLevels = LEVEL_END;
	EngineDesc.iViewportWidth = g_iWinSizeX;
	EngineDesc.iViewportHeight = g_iWinSizeY;
	EngineDesc.iStaticLevelID = LEVEL_STATIC;


	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
	{
		return E_FAIL;
	}

	m_pLogger = CImguiLogger::Create();
	if (nullptr == m_pLogger)
		return E_FAIL;

	/* Event Manager */
	CEvent_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, m_pLogger);

	if (FAILED(SetUp_StartLevel(LEVEL_STATIC))) // Logo로 초기화 Setup 하더라도 Loading에 반드시 들어가게되어있음.SetUp_StartLevel 참고.
	{
		return E_FAIL;
	}


	return S_OK;
}

void CMap_Tool_MainApp::Progress(_float _fTimeDelta)
{
	m_pGameInstance->Start_Imgui();
	ImGuizmo::SetOrthographic(true);
	ImGuizmo::BeginFrame();
	m_pGameInstance->Priority_Update_Engine(_fTimeDelta);

	m_pGameInstance->Update_Engine(_fTimeDelta);

	m_pGameInstance->Late_Update_Engine(_fTimeDelta);

	m_pLogger->Draw_Log();

	m_pGameInstance->End_Imgui();

	if (FAILED(Render()))
	{
		MSG_BOX("Failed Render MainApp");
		return;
	}

	// 뷰포트 나가도 렌더되는 처리 // 
	ImGui::RenderPlatformWindowsDefault();

	CEvent_Manager::GetInstance()->Update(_fTimeDelta);
}

HRESULT CMap_Tool_MainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Begin()))
		return E_FAIL;

	m_pGameInstance->Draw();

	m_pGameInstance->Render_DrawData_Imgui();

	if (FAILED(m_pGameInstance->Render_End()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_Tool_MainApp::SetUp_StartLevel(LEVEL_ID _eLevelID)
{
	// OpenLevel을 통해, Loading Scene을 반드시 거치게하고, 그 후 실제로 전환시킬 NextLevelID를 넘겨, Loading에서의 작업이 완료 후, Level을 전환한다.
	Event_LevelChange(LEVEL_LOADING, _eLevelID);

	return S_OK;
}

CMap_Tool_MainApp* CMap_Tool_MainApp::Create()
{
	CMap_Tool_MainApp* pInstance = new CMap_Tool_MainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMap_Tool_MainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap_Tool_MainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	/* Client Singleton Delete */
	CEvent_Manager::DestroyInstance();

	/* GameInstance Release*/
	CGameInstance::Release_Engine();
}

