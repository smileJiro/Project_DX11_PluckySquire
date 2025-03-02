#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Event_Manager.h"
#include "Pooling_Manager.h"
#include "UI_Manager.h"
#include "Dialog_Manager.h"
#include "NPC_Manager.h"
#include "Camera_Manager.h"
#include "Section_Manager.h"

#include "Trigger_Manager.h"
#include "PlayerData_Manager.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"


#include "RenderGroup_MRT.h"
#include "RenderGroup_Lights.h"
#include "RenderGroup_Final.h"
#include "RenderGroup_AfterParticle.h"
#include "RenderGroup_DownSample.h"
#include "RenderGroup_Blur.h"
#include "RenderGroup_DirectLights.h"
#include "RenderGroup_Lighting.h"
#include "RenderGroup_AfterEffect.h"
#include "RenderGroup_Combine.h"
#include "RenderGroup_PostProcessing.h"
#include "RenderGroup_PlayerDepth.h"

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
#ifdef _DEBUG
	EngineDesc.eImportMode |= NONE_IMPORT | IMPORT_MESH_PICKING; // IMPORT_IMGUI | IMPORT_MESH_PICKING;NONE_IMPORT
#elif NDEBUG
	EngineDesc.eImportMode |= IMPORT_IMGUI;
#endif
	
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

	Set_EffectRG();

	return S_OK;
}

void CMainApp::Progress(_float _fTimeDelta)
{
	if (IS_IMPORT_IMGUI)
		m_pGameInstance->Start_Imgui();

	m_pGameInstance->Priority_Update_Engine(_fTimeDelta);
	if (IS_IMPORT_IMGUI)
		Imgui_FPS(_fTimeDelta);

	m_pGameInstance->Update_Engine(_fTimeDelta);
	CCamera_Manager::GetInstance()->Update(_fTimeDelta);
	CTrigger_Manager::GetInstance()->Update();

	m_pGameInstance->Late_Update_Engine(_fTimeDelta);

	// TODO :: 여기가 맞는지? >> 맞는 것 같삼.
	CSection_Manager::GetInstance()->Section_AddRenderGroup_Process();
	if (IS_IMPORT_IMGUI)
		m_pGameInstance->End_Imgui();

	if (FAILED(Render()))
	{
		MSG_BOX("Failed Render MainApp");
		return;
	}

	// 뷰포트 나가도 렌더되는 처리 // 
	if (IS_IMPORT_IMGUI)
		ImGui::RenderPlatformWindowsDefault(); 



	CEvent_Manager::GetInstance()->Update(_fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Begin()))
		return E_FAIL;

	m_pGameInstance->Draw();

	if (IS_IMPORT_IMGUI)
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

	static _int iSelectLevelID = LEVEL_LOGO;
	

	const char* pLevels[] = { "LEVEL_LOGO", "LEVEL_CHAPTER_2", "LEVEL_CHAPTER_4", "LEVEL_CHAPTER_TEST", "LEVEL_CAMERA_TOOL" };
	ImGui::Combo("Select Action", &iSelectLevelID, pLevels, IM_ARRAYSIZE(pLevels));

	if (ImGui::Button("Level_Change"))
	{
		Event_LevelChange(LEVEL_LOADING, iSelectLevelID + LEVEL_LOGO);
	}


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
	if (FAILED(CPlayerData_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CEffect2D_Manager::GetInstance()->Initialize(m_pDevice, m_pContext)))
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

	/* RG_3D, PR3D_PLAYERDEPTH */
	CRenderGroup_PlayerDepth::RG_MRT_DESC RG_PlayerDepthDesc;
	RG_PlayerDepthDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_PlayerDepthDesc.iPriorityID = PRIORITY_3D::PR3D_PLAYERDEPTH;
	RG_PlayerDepthDesc.strMRTTag = TEXT("MRT_PlayerDepth");
	CRenderGroup_PlayerDepth* pRenderGroup_PlayerDepth = CRenderGroup_PlayerDepth::Create(m_pDevice, m_pContext, &RG_PlayerDepthDesc);
	if (nullptr == pRenderGroup_PlayerDepth)
	{
		MSG_BOX("Failed Create PR3D_PLAYERDEPTH");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_PlayerDepth->Get_RenderGroupID(), pRenderGroup_PlayerDepth->Get_PriorityID(), pRenderGroup_PlayerDepth)))
		return E_FAIL;
	Safe_Release(pRenderGroup_PlayerDepth);
	pRenderGroup_PlayerDepth = nullptr;

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



	/* RG_3D, PR3D_POSTPROCESSING */
	CRenderGroup_PostProcessing::RG_POST_DESC RG_PostDesc;
	RG_PostDesc.iBlurLevel = 2;
	RG_PostDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_PostDesc.iPriorityID = PR3D_POSTPROCESSING;
	CRenderGroup_PostProcessing* pRenderGroup_Post = CRenderGroup_PostProcessing::Create(m_pDevice, m_pContext, &RG_PostDesc);
	if (nullptr == pRenderGroup_Post)
	{
		MSG_BOX("Failed Create PR3D_POSTPROCESSING");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Post->Get_RenderGroupID(), pRenderGroup_Post->Get_PriorityID(), pRenderGroup_Post)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Post);
	pRenderGroup_Post = nullptr;

	/* RG_3D, PR3D_AFTERPOSTPROCESSING */
	CRenderGroup_MRT::RG_MRT_DESC RG_AfterPostDesc;
	RG_AfterPostDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_AfterPostDesc.iPriorityID = PRIORITY_3D::PR3D_AFTERPOSTPROCESSING;
	RG_AfterPostDesc.strMRTTag = TEXT("MRT_PostProcessing");
	RG_AfterPostDesc.isClear = false;

	CRenderGroup_MRT* pRenderGroup_AfterPost = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_AfterPostDesc);
	if (nullptr == pRenderGroup_AfterPost)
	{
		MSG_BOX("Failed Create PR3D_AFTERPOSTPROCESSING");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_AfterPost->Get_RenderGroupID(), pRenderGroup_AfterPost->Get_PriorityID(), pRenderGroup_AfterPost)))
		return E_FAIL;
	Safe_Release(pRenderGroup_AfterPost);
	pRenderGroup_AfterPost = nullptr;

	/* RG_3D, PR3D_COMBINE */
	CRenderGroup_Combine::RG_MRT_DESC RG_CombineDesc;
	RG_CombineDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_CombineDesc.iPriorityID = PRIORITY_3D::PR3D_COMBINE;
	RG_CombineDesc.strMRTTag = TEXT("MRT_Combine");
	RG_CombineDesc.isClear = false;
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
	RG_EffectDesc.strMRTTag = TEXT("MRT_Effect");

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

	/* RG_3D, PR3D_PARTICLE */
	CRenderGroup_MRT::RG_MRT_DESC RG_ParticleDesc;
	RG_ParticleDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_ParticleDesc.iPriorityID = PRIORITY_3D::PR3D_PARTICLE;
	RG_ParticleDesc.strMRTTag = TEXT("MRT_Weighted_Blended");
	RG_ParticleDesc.isClear = true;

	CRenderGroup_MRT* pRenderGroup_Particle = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_ParticleDesc);
	if (nullptr == pRenderGroup_Particle)
	{
		MSG_BOX("Failed Create PR3D_PARTICLE");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Particle->Get_RenderGroupID(), pRenderGroup_Particle->Get_PriorityID(), pRenderGroup_Particle)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Particle);
	pRenderGroup_Particle = nullptr;

	/* RG_3D, PR3D_AFTERPARTICLE */
	CRenderGroup_MRT::RG_MRT_DESC RG_AfterParticleDesc;
	RG_AfterParticleDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_AfterParticleDesc.iPriorityID = PRIORITY_3D::PR3D_AFTERPARTICLE;
	RG_AfterParticleDesc.strMRTTag = TEXT("MRT_AfterParticle");
	RG_AfterParticleDesc.isClear = false;

	CRenderGroup_AfterParticle* pRenderGroup_AfterParticle = CRenderGroup_AfterParticle::Create(m_pDevice, m_pContext, &RG_AfterParticleDesc);
	if (nullptr == pRenderGroup_AfterParticle)
	{
		MSG_BOX("Failed Create PR3D_AFTERPARTICLE");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_AfterParticle->Get_RenderGroupID(), pRenderGroup_AfterParticle->Get_PriorityID(), pRenderGroup_AfterParticle)))
		return E_FAIL;
	Safe_Release(pRenderGroup_AfterParticle);
	pRenderGroup_AfterParticle = nullptr;

	/* RG_3D, PR3D_DOWNSAMPLE */
	CRenderGroup_DownSample::RG_MRT_DESC RG_DOWNBLUR_DESC;
	RG_DOWNBLUR_DESC.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_DOWNBLUR_DESC.iPriorityID = PRIORITY_3D::PR3D_BLURDOWN;
	RG_DOWNBLUR_DESC.pDSV = m_pGameInstance->Find_DSV(TEXT("DSV_Downsample1"));

	CRenderGroup_DownSample* pRenderGroup_BlurDown = CRenderGroup_DownSample::Create(m_pDevice, m_pContext, &RG_DOWNBLUR_DESC);
	if (nullptr == pRenderGroup_BlurDown)
	{
		MSG_BOX("Failed Create MRT_DOWNSample");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_BlurDown->Get_RenderGroupID(), pRenderGroup_BlurDown->Get_PriorityID(), pRenderGroup_BlurDown)))
		return E_FAIL;
	Safe_Release(pRenderGroup_BlurDown);
	pRenderGroup_BlurDown = nullptr;

	CRenderGroup_Blur::RG_MRT_DESC RG_BLUR_DESC;
	RG_BLUR_DESC.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_BLUR_DESC.iPriorityID = PRIORITY_3D::PR3D_BLUR;
	RG_BLUR_DESC.pDSV = m_pGameInstance->Find_DSV(TEXT("DSV_Downsample1"));

	CRenderGroup_Blur* pRenderGroup_Blur = CRenderGroup_Blur::Create(m_pDevice, m_pContext, &RG_BLUR_DESC);
	if (nullptr == pRenderGroup_Blur)
	{
		MSG_BOX("Failed Create MRT_BLUR");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Blur->Get_RenderGroupID(), pRenderGroup_Blur->Get_PriorityID(), pRenderGroup_Blur)))
		return E_FAIL;
	Safe_Release(pRenderGroup_Blur);
	pRenderGroup_Blur = nullptr;

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

void CMainApp::Set_EffectRG()
{
	CEmitter::SetID_2D(RG_2D);
	CEmitter::SetID_3D(RG_3D);
	CEmitter::SetID_Particle(PR3D_PARTICLE);
	CEmitter::SetID_Effect(PR3D_EFFECT);
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

	/* Target_Etc */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Etc"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
		return E_FAIL;
	
	/* Target_PlayerDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PlayerDepth"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	/* Target_DirectLightAcc */ 
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DirectLightAcc"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Lighting */ /* HDR */
	if (FAILED(m_pGameInstance->Add_RenderTarget_MSAA(TEXT("Target_Lighting"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_PostProcessing */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PostProcessing"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Combine */ 
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Combine"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_EffectColor*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectColor"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Bloom */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Distortion */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R8G8_SNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_DownSample */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DownSample1"), (_uint)(g_iWinSizeX / 6.f), (_uint)(g_iWinSizeY / 6.f), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DownSample2"), (_uint)(g_iWinSizeX / 24.f), (_uint)(g_iWinSizeY / 24.f), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_Blur */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DownBlur1"), (_uint)(g_iWinSizeX / 6.f), (_uint)(g_iWinSizeY / 6.f), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DownBlur2"), (_uint)(g_iWinSizeX / 24.f), (_uint)(g_iWinSizeY / 24.f), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_ParticleAccumulate */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ParticleAccumulate"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_ParticleRevealage */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ParticleRevelage"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16_FLOAT, _float4(1.0f, 0.f, 0.0f, 0.0f))))
		return E_FAIL;
	/* Target_ParticleBloom */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ParticleBloom"), (_uint)g_iWinSizeX, (_uint)g_iWinSizeY, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.0f, 0.0f))))
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
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Geometry"), TEXT("Target_Etc"))))
		return E_FAIL;
	
	/* MRT_PlayerDepth */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PlayerDepth"), TEXT("Target_PlayerDepth"))))
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

	/* MRT_PostProcessing */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_PostProcessing"))))
		return E_FAIL;

	/* MRT_Combine */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combine"), TEXT("Target_Combine"))))
		return E_FAIL;

	/* MRT_EFFECT */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_EffectColor"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Bloom"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Distortion"))))
		return E_FAIL;

	///* MRT_BLUR */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur1"), TEXT("Target_DownBlur1"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur2"), TEXT("Target_DownBlur2"))))
		return E_FAIL;

	/* MRT_DownSmample*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSample1"), TEXT("Target_DownSample1"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DownSample2"), TEXT("Target_DownSample2"))))
		return E_FAIL;


	/* MRT_Weighted_Blended*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Weighted_Blended"), TEXT("Target_ParticleAccumulate"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Weighted_Blended"), TEXT("Target_ParticleRevelage"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Weighted_Blended"), TEXT("Target_ParticleBloom"))))
		return E_FAIL;

	/* MRT_AfterParticle */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_AfterParticle"), TEXT("Target_Bloom"))))
		return E_FAIL;

	/* Settiong DSV */

	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Shadow"), (_uint)SHADOWMAP_X, (_uint)SHADOWMAP_Y)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Downsample1"), (_uint)(g_iWinSizeX / 6.f), (_uint)(g_iWinSizeY / 6.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Downsample2"), (_uint)(g_iWinSizeX / 24.f), (_uint)(g_iWinSizeY / 24.f))))
		return E_FAIL;


#ifdef _DEBUG

	/* 위치 설정. */
	_float fSizeX = (_float)g_iWinSizeX * 0.2f;
	_float fSizeY = (_float)g_iWinSizeY * 0.2f;
	_float fX = (_float)g_iWinSizeX * 0.2f * 0.5f;
	_float fY = (_float)g_iWinSizeY * 0.2f * 0.5f;

	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Albedo"), fX, fY, fSizeX, fSizeY);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), fX, fY + fSizeY * 1.0f, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);
	m_pGameInstance->Ready_RT_Debug(TEXT("Target_EffectAccumulate"), fX, fY, (_float)g_iWinSizeX * 0.2f, (_float)g_iWinSizeY * 0.2f);

#endif // _DEBUG


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
	/* Client Singleton Delete */ 
	CEffect2D_Manager::DestroyInstance();
	CEvent_Manager::DestroyInstance();
	CCamera_Manager::DestroyInstance();
	CPooling_Manager::DestroyInstance();
	CUI_Manager::DestroyInstance();
	CDialog_Manager::DestroyInstance();
	CNPC_Manager::DestroyInstance();
	CSection_Manager::DestroyInstance();
	CTrigger_Manager::DestroyInstance();
	CPlayerData_Manager::DestroyInstance();
	CEffect_Manager::DestroyInstance();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	/* GameInstance Release*/
	CGameInstance::Release_Engine();
	__super::Free();


}

