#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Event_Manager.h"
#include "Pooling_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Section_Manager.h"
#include "RenderGroup_MRT.h"
#include "RenderGroup_Lights.h"
#include "RenderGroup_Final.h"
#include "RenderGroup_AfterEffect.h"

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
	EngineDesc.isNewRenderer = true;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_RenderGroup()))
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

	// TODO :: 여기가 맞는지? 
	CSection_Manager::GetInstance()->Section_AddRenderGroup_Process();
	
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
	if (FAILED(CSection_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

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

HRESULT CMainApp::Ready_RenderGroup()
{
	CRenderGroup* pRenderGroup = nullptr;
	CRenderGroup_MRT* pRenderGroup_MRT = nullptr;


	/* RG_3D, PR3D_BOOK2D */
	CRenderGroup_MRT::RG_MRT_DESC RG_Book2DDesc;
	RG_Book2DDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_Book2DDesc.iPriorityID = PRIORITY_3D::PR3D_BOOK2D;
	RG_Book2DDesc.isViewportSizeChange = true;
	RG_Book2DDesc.strMRTTag = TEXT("MRT_Book_2D");
	RG_Book2DDesc.pDSV = m_pGameInstance->Find_DSV(TEXT("DSV_Book2D"));
	RG_Book2DDesc.vViewportSize = _float2((_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y);
	RG_Book2DDesc.isClear = true;
	if (nullptr == RG_Book2DDesc.pDSV)
	{
		MSG_BOX("Book2D DSV가 없대.");
		return E_FAIL;
	}
	pRenderGroup_MRT = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_Book2DDesc);
	if (nullptr == pRenderGroup_MRT)
	{
		MSG_BOX("Failed Create PR3D_BOOK2D");
		return E_FAIL;
	}
	if(FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
		return E_FAIL;

	Safe_Release(pRenderGroup_MRT);
	pRenderGroup_MRT = nullptr;

	/* RG_3D, PR3D_PRIORITY */
	CRenderGroup_MRT::RG_MRT_DESC RGPriorityDesc;
	RGPriorityDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RGPriorityDesc.iPriorityID = PRIORITY_3D::PR3D_PRIORITY;
	RGPriorityDesc.strMRTTag = TEXT("MRT_Final");
	pRenderGroup_MRT = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RGPriorityDesc);
	if (nullptr == pRenderGroup_MRT)
	{
		MSG_BOX("Failed Create PR3D_PRIORITY");
		return E_FAIL;
	}
	if(FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
		return E_FAIL;
	Safe_Release(pRenderGroup_MRT);
	pRenderGroup_MRT = nullptr;

	/* RG_3D, PR3D_NONBLEND */
	CRenderGroup_MRT::RG_MRT_DESC RG_MRTDesc;
	RG_MRTDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_MRTDesc.iPriorityID = PRIORITY_3D::PR3D_NONBLEND;
	RG_MRTDesc.strMRTTag = TEXT("MRT_GameObjects");
	pRenderGroup_MRT = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_MRTDesc);
	if (nullptr == pRenderGroup_MRT)
	{
		MSG_BOX("Failed Create PR3D_NONBLEND");
		return E_FAIL;
	}
	if(FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
		return E_FAIL;
	Safe_Release(pRenderGroup_MRT);
	pRenderGroup_MRT = nullptr;

	/* RG_3D, PR3D_LIGHTS */
	CRenderGroup_Lights::RG_MRT_DESC RG_LightDesc;
	RG_LightDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_LightDesc.iPriorityID = PRIORITY_3D::PR3D_LIGHTS;
	RG_LightDesc.strMRTTag = TEXT("MRT_LightAcc");
	CRenderGroup_Lights* pRenderGroup_Lights = CRenderGroup_Lights::Create(m_pDevice, m_pContext, &RG_LightDesc);
	if (nullptr == pRenderGroup_Lights)
	{
		MSG_BOX("Failed Create PR3D_LIGHTS");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Lights->Get_RenderGroupID(), pRenderGroup_Lights->Get_PriorityID(), pRenderGroup_Lights)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Lights);
	pRenderGroup_Lights = nullptr;

	/* RG_3D, PR3D_FINAL */
	CRenderGroup_Final::RG_MRT_DESC RG_FinalDesc;
	RG_FinalDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_FinalDesc.iPriorityID = PRIORITY_3D::PR3D_FINAL;
	RG_FinalDesc.strMRTTag = TEXT("MRT_Final");
	RG_FinalDesc.isClear = false;
	CRenderGroup_Final* pRenderGroup_Final = CRenderGroup_Final::Create(m_pDevice, m_pContext, &RG_FinalDesc);
	if (nullptr == pRenderGroup_Final)
	{
		MSG_BOX("Failed Create PR3D_FINAL");
		return E_FAIL;
	}
	if(FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Final->Get_RenderGroupID(), pRenderGroup_Final->Get_PriorityID(), pRenderGroup_Final)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Final);
	pRenderGroup_Final = nullptr;

	/* RG_3D, PR3D_BLEND */
	CRenderGroup_MRT::RG_MRT_DESC RG_BlendDesc;
	RG_BlendDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_BlendDesc.iPriorityID = PRIORITY_3D::PR3D_BLEND;
	RG_BlendDesc.strMRTTag = TEXT("MRT_Final");
	RG_BlendDesc.isClear = false;
	CRenderGroup_MRT* pRenderGroup_Blend = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_BlendDesc);
	if (nullptr == pRenderGroup_Blend)
	{
		MSG_BOX("Failed Create PR3D_BLEND");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Blend->Get_RenderGroupID(), pRenderGroup_Blend->Get_PriorityID(), pRenderGroup_Blend)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Blend);
	pRenderGroup_Blend = nullptr;

	/* RG_3D, PR3D_EFFECT */
	CRenderGroup_MRT::RG_MRT_DESC RG_EffectDesc;
	RG_EffectDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_EffectDesc.iPriorityID = PRIORITY_3D::PR3D_EFFECT;
	RG_EffectDesc.strMRTTag = TEXT("MRT_Weighted_Blended");

	CRenderGroup_MRT* pRenderGroup_Effect = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_EffectDesc);
	if (nullptr == pRenderGroup_Effect)
	{
		MSG_BOX("Failed Create PR3D_EFFECT");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Effect->Get_RenderGroupID(), pRenderGroup_Effect->Get_PriorityID(), pRenderGroup_Effect)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Effect);
	pRenderGroup_Effect = nullptr;

	/* RG_3D, PR3D_AFTEREFFECT */
	CRenderGroup_AfterEffect::RG_DESC RG_AfterEffectDesc;
	RG_AfterEffectDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_AfterEffectDesc.iPriorityID = PRIORITY_3D::PR3D_AFTEREFFECT;
	CRenderGroup_AfterEffect* pRenderGroup_AfterEffect = CRenderGroup_AfterEffect::Create(m_pDevice, m_pContext, &RG_AfterEffectDesc);
	if (nullptr == pRenderGroup_AfterEffect)
	{
		MSG_BOX("Failed Create PR3D_AFTEREFFECT");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_AfterEffect->Get_RenderGroupID(), pRenderGroup_AfterEffect->Get_PriorityID(), pRenderGroup_AfterEffect)))
		return E_FAIL;
	Safe_Release(pRenderGroup_AfterEffect);
	pRenderGroup_AfterEffect = nullptr;

	/* RG_3D, PR3D_UI */
	CRenderGroup::RG_DESC RG_UIDesc;
	RG_UIDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_UIDesc.iPriorityID = PRIORITY_3D::PR3D_UI;
	pRenderGroup = CRenderGroup::Create(m_pDevice, m_pContext, &RG_UIDesc);
	if (nullptr == pRenderGroup)
	{
		MSG_BOX("Failed Create PR3D_UI");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup->Get_RenderGroupID(), pRenderGroup->Get_PriorityID(), pRenderGroup)))
		return E_FAIL;
	Safe_Release(pRenderGroup);
	pRenderGroup = nullptr;

	return S_OK;
}

HRESULT CMainApp::Ready_RenderTargets()
{
	/* Target Book2D */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Book_2D"), (_uint)RTSIZE_BOOK2D_X, (_uint)RTSIZE_BOOK2D_Y, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	/* Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	/* Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	/* Target_LightDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iShadowWidth, g_iShadowHeight, DXGI_FORMAT_R32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
		return E_FAIL;

	/* Target_Final */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_EffectAccumulate */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectAccumulate"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_EffectRevealage */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectRevealage"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16_FLOAT, _float4(1.0f, 0.f, 0.0f, 0.0f))))
		return E_FAIL;

	/* RTV를 모아두는 MRT를 세팅 */

	/* MRT_Book_2D*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Book_2D"), TEXT("Target_Book_2D"))))
		return E_FAIL;

	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* MRT_Shadow */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/* MRT_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
		return E_FAIL;

	/* MRT_Weighted_Blended*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Weighted_Blended"), TEXT("Target_EffectAccumulate"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Weighted_Blended"), TEXT("Target_EffectRevealage"))))
		return E_FAIL;

	/* Settiong DSV */

	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Shadow"), g_iShadowWidth, g_iShadowHeight)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Book2D"), RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y)))
		return E_FAIL;

	/* 위치 설정. */
	_float fSizeX = (_float)g_iWinSizeX * 0.2f;
	_float fSizeY = (_float)g_iWinSizeY * 0.2f;
	_float fX = (_float)g_iWinSizeX * 0.2f * 0.5f;
	_float fY = (_float)g_iWinSizeY * 0.2f * 0.5f;

	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), fX, fY, fSizeX, fSizeY);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), fX, fY + fSizeY * 1.0f, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), fX, fY + fSizeY * 2.0f, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Book_2D"), fX, fY + fSizeY * 3.0f, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), fX, fY + fSizeY * 4.0f, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_EffectAccumulate"), fX, fY, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);


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
	CSection_Manager::DestroyInstance();

	/* GameInstance Release*/
	CGameInstance::Release_Engine();
}

