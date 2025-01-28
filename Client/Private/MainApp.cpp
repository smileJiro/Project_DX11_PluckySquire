#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Event_Manager.h"
#include "Pooling_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Section_Manager.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
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
		return E_FAIL;

	/* Client Manager Initialize */
	Initialize_Client_Manager();


	if (FAILED(SetUp_StartLevel(LEVEL_STATIC))) // Logo로 초기화 Setup 하더라도 Loading에 반드시 들어가게되어있음.SetUp_StartLevel 참고.
		return E_FAIL;

	if (FAILED(Ready_Font()))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Progress(_float _fTimeDelta)
{
	m_pGameInstance->Start_Imgui();

	m_pGameInstance->Priority_Update_Engine(_fTimeDelta);
	Imgui_FPS(_fTimeDelta);

	m_pGameInstance->Update_Engine(_fTimeDelta);
	CCamera_Manager::GetInstance()->Update(_fTimeDelta);

	m_pGameInstance->Late_Update_Engine(_fTimeDelta);

	m_pGameInstance->End_Imgui();

	if (FAILED(Render()))
	{
		MSG_BOX("Failed Render MainApp");
		return;
	}

	// 뷰포트 나가도 렌더되는 처리 // 
	ImGui::RenderPlatformWindowsDefault(); 

#ifdef _DEBUG
	
#endif // _DEBUG

	CEvent_Manager::GetInstance()->Update(_fTimeDelta);
}

HRESULT CMainApp::Render()
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

void CMainApp::Imgui_FPS(_float _fTimeDelta)
{
	ImGui::Begin("FPS");
	static _int iMaxFPS = (_int)(1.0f / m_iOneFrameDeltaTime);
	_int iInGameFPS = m_pGameInstance->Get_FPS(TEXT("Timer_120"));
	m_vFPSRenderTime.y += _fTimeDelta;
	if (m_vFPSRenderTime.x <= m_vFPSRenderTime.y)
	{
		m_vFPSRenderTime.y = 0.0f;
		iMaxFPS = (_int)(1.0f / m_iOneFrameDeltaTime);		
	}
	ImGui::Text("MaxFPS : %d", iMaxFPS);
	ImGui::Text("InGameFPS : %d", iInGameFPS);
	ImGui::End();
}

HRESULT CMainApp::SetUp_StartLevel(LEVEL_ID _eLevelID)
{
	// OpenLevel을 통해, Loading Scene을 반드시 거치게하고, 그 후 실제로 전환시킬 NextLevelID를 넘겨, Loading에서의 작업이 완료 후, Level을 전환한다.
	Event_LevelChange(LEVEL_LOADING, _eLevelID);
	
	return S_OK;
}

HRESULT CMainApp::Initialize_Client_Manager()
{
	if (FAILED(CEvent_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CCamera_Manager::GetInstance()->Initialize()))
		return E_FAIL;
	if(FAILED(CPooling_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;
	//if (FAILED(CSection_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font18"), TEXT("../Bin/Resources/Fonts/YangRound18.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font20"), TEXT("../Bin/Resources/Fonts/YangRound20.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font24"), TEXT("../Bin/Resources/Fonts/YangRound24.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font28"), TEXT("../Bin/Resources/Fonts/YangRound28.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font30"), TEXT("../Bin/Resources/Fonts/YangRound30.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font35"), TEXT("../Bin/Resources/Fonts/YangRound35.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font38"), TEXT("../Bin/Resources/Fonts/YangRound38.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font40"), TEXT("../Bin/Resources/Fonts/YangRound40.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font54"), TEXT("../Bin/Resources/Fonts/YangRound54.spritefont"))))
		return E_FAIL;


	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	/* Client Singleton Delete */ 
	CEvent_Manager::DestroyInstance();
	CCamera_Manager::DestroyInstance();
	CPooling_Manager::DestroyInstance();
	CUI_Manager::DestroyInstance();
	//CSection_Manager::DestroyInstance();

	/* GameInstance Release*/
	CGameInstance::Release_Engine();
}

