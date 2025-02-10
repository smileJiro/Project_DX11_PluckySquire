#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Event_Manager.h"
#include "Pooling_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Section_Manager.h"
#include "Collision_Manager.h"
#include "Trigger_Manager.h"


#include "RenderGroup_MRT.h"
#include "RenderGroup_DirectLights.h"
#include "RenderGroup_Lighting.h"
#include "RenderGroup_AfterEffect.h"
#include "RenderGroup_Combine.h"

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

	int a = sizeof(CONST_LIGHT);

	return S_OK;
}

void CMainApp::Progress(_float _fTimeDelta)
{
	m_pGameInstance->Start_Imgui();

	m_pGameInstance->Priority_Update_Engine(_fTimeDelta);
	Imgui_FPS(_fTimeDelta);

	m_pGameInstance->Update_Engine(_fTimeDelta);
	CCamera_Manager::GetInstance()->Update(_fTimeDelta);
	CCollision_Manager::GetInstance()->Update();			// 충돌 검사 수행.

	m_pGameInstance->Late_Update_Engine(_fTimeDelta);

	// TODO :: 여기가 맞는지? >> 맞는 것 같삼.
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
	if (FAILED(CPooling_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CSection_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CTrigger_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
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

	/* RG_3D, PR3D_PRIORITY */
	CRenderGroup_MRT::RG_MRT_DESC RGPriorityDesc;
	RGPriorityDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RGPriorityDesc.iPriorityID = PRIORITY_3D::PR3D_PRIORITY;
	RGPriorityDesc.strMRTTag = TEXT("MRT_Lighting");
	RGPriorityDesc.pDSV = m_pGameInstance->Find_DSV(TEXT("Target_Lighting"));
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

	/* RG_3D, PR3D_GEOMETRY */
	CRenderGroup_MRT::RG_MRT_DESC RG_MRTDesc;
	RG_MRTDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_MRTDesc.iPriorityID = PRIORITY_3D::PR3D_GEOMETRY;
	RG_MRTDesc.strMRTTag = TEXT("MRT_Geometry");
	pRenderGroup_MRT = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_MRTDesc);
	if (nullptr == pRenderGroup_MRT)
	{
		MSG_BOX("Failed Create PR3D_GEOMETRY");
		return E_FAIL;
	}
	if(FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
		return E_FAIL;
	Safe_Release(pRenderGroup_MRT);
	pRenderGroup_MRT = nullptr;

	/* RG_3D, PR3D_DIRECTLIGHTS */
	//MRT_DirectLightAcc;
	CRenderGroup_DirectLights::RG_MRT_DESC RG_DirectLightsDesc;
	RG_DirectLightsDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_DirectLightsDesc.iPriorityID = PRIORITY_3D::PR3D_DIRECTLIGHTS;
	RG_DirectLightsDesc.strMRTTag = TEXT("MRT_DirectLightAcc");
	RG_DirectLightsDesc.isClear = true;
	CRenderGroup_DirectLights* pRenderGroup_DirectLights = CRenderGroup_DirectLights::Create(m_pDevice, m_pContext, &RG_DirectLightsDesc);
	if (nullptr == pRenderGroup_DirectLights)
	{
		MSG_BOX("Failed Create PR3D_DIRECTLIGHTS");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_DirectLights->Get_RenderGroupID(), pRenderGroup_DirectLights->Get_PriorityID(), pRenderGroup_DirectLights)))
		return E_FAIL;
	Safe_Release(pRenderGroup_DirectLights);
	pRenderGroup_DirectLights = nullptr;

	/* RG_3D, PR3D_LIGHTNG */
	CRenderGroup_Lighting::RG_MRT_DESC RG_LightingDesc;
	RG_LightingDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_LightingDesc.iPriorityID = PRIORITY_3D::PR3D_LIGHTNG;
	RG_LightingDesc.strMRTTag = TEXT("MRT_Lighting");
	RG_LightingDesc.isClear = false;
	RG_LightingDesc.pDSV = m_pGameInstance->Find_DSV(TEXT("Target_Lighting"));
	CRenderGroup_Lighting* pRenderGroup_Lighting = CRenderGroup_Lighting::Create(m_pDevice, m_pContext, &RG_LightingDesc);
	if (nullptr == pRenderGroup_Lighting)
	{
		MSG_BOX("Failed Create PR3D_LIGHTNG");
		return E_FAIL;
	}
	if(FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Lighting->Get_RenderGroupID(), pRenderGroup_Lighting->Get_PriorityID(), pRenderGroup_Lighting)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Lighting);
	pRenderGroup_Lighting = nullptr;

	/* RG_3D, PR3D_COMBINE */
	CRenderGroup_Combine::RG_MRT_DESC RG_CombineDesc;
	RG_CombineDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_CombineDesc.iPriorityID = PRIORITY_3D::PR3D_COMBINE;
	RG_CombineDesc.strMRTTag = TEXT("MRT_Combine");
	RG_CombineDesc.isClear = true;
	CRenderGroup_Combine* pRenderGroup_Combine = CRenderGroup_Combine::Create(m_pDevice, m_pContext, &RG_CombineDesc);
	if (nullptr == pRenderGroup_Combine)
	{
		MSG_BOX("Failed Create PR3D_COMBINE");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Combine->Get_RenderGroupID(), pRenderGroup_Combine->Get_PriorityID(), pRenderGroup_Combine)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Combine);
	pRenderGroup_Combine = nullptr;

	/* RG_3D, PR3D_BLEND */
	CRenderGroup_MRT::RG_MRT_DESC RG_BlendDesc;
	RG_BlendDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_BlendDesc.iPriorityID = PRIORITY_3D::PR3D_BLEND;
	RG_BlendDesc.strMRTTag = TEXT("MRT_Combine");
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
	/* Target_Albedo */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Albedo"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	/* Target_ORMH */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ORMH"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

#pragma region Light,Shadow(Old)
	///* Target_Shade */ /* HDR */
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
	//	return E_FAIL;
	//
	///* Target_Specular */ /* HDR */
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
	//	return E_FAIL;

	///* Target_LightDepth */
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iShadowWidth, g_iShadowHeight, DXGI_FORMAT_R32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
	//	return E_FAIL;

#pragma endregion
	/* Target_DirectLightAcc */ 
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DirectLightAcc"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Lighting */ /* HDR */
	if (FAILED(m_pGameInstance->Add_RenderTarget_MSAA(TEXT("Target_Lighting"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Combine */ 
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Combine"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_EffectAccumulate */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectAccumulate"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_EffectRevealage */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectRevealage"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16_FLOAT, _float4(1.0f, 0.f, 0.0f, 0.0f))))
		return E_FAIL;

	/* RTV를 모아두는 MRT를 세팅 */

	/* MRT_Geometry */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Geometry"), TEXT("Target_Albedo"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Geometry"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Geometry"), TEXT("Target_ORMH"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Geometry"), TEXT("Target_Depth"))))
		return E_FAIL;

#pragma region LightAcc(Old)
	///* MRT_LightAcc */
//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
//	return E_FAIL;
//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
//	return E_FAIL;
//
///* MRT_Shadow */
//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
//	return E_FAIL;
#pragma endregion
	/* MRT_DirectLightAcc*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DirectLightAcc"), TEXT("Target_DirectLightAcc"))))
		return E_FAIL;

	/* MRT_Lighting */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lighting"), TEXT("Target_Lighting"))))
		return E_FAIL;

	/* MRT_Combine */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combine"), TEXT("Target_Combine"))))
		return E_FAIL;

	/* MRT_Weighted_Blended*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Weighted_Blended"), TEXT("Target_EffectAccumulate"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Weighted_Blended"), TEXT("Target_EffectRevealage"))))
		return E_FAIL;

	/* Settiong DSV */

	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Shadow"), g_iShadowWidth, g_iShadowHeight)))
		return E_FAIL;


	/* 위치 설정. */
	_float fSizeX = (_float)g_iWinSizeX * 0.2f;
	_float fSizeY = (_float)g_iWinSizeY * 0.2f;
	_float fX = (_float)g_iWinSizeX * 0.2f * 0.5f;
	_float fY = (_float)g_iWinSizeY * 0.2f * 0.5f;

	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Albedo"), fX, fY, fSizeX, fSizeY);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), fX, fY + fSizeY * 1.0f, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);
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
	CCollision_Manager::DestroyInstance();
	CTrigger_Manager::DestroyInstance();

	/* GameInstance Release*/
	CGameInstance::Release_Engine();
}

