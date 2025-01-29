#include "pch.h"
#include "MainEffectTool.h"
#include "GameInstance.h"
#include "RenderGroup_MRT.h"
#include "ETool_RenderGroup_Lights.h"
#include "ETool_RenderGroup_AfterEffect.h"
#include "ETool_RenderGroup_Final.h"

#include "Event_Manager.h"
#include "Object_Background.h"
#include "SkyBox.h"



CMainEffectTool::CMainEffectTool()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainEffectTool::Initialize()
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
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype_Static()))
		return E_FAIL;


	if (FAILED(SetUp_StartLevel(LEVEL_TOOL))) // Logo로 초기화 Setup 하더라도 Loading에 반드시 들어가게되어있음.SetUp_StartLevel 참고.
	{
		return E_FAIL;
	}

	if (FAILED(Ready_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_RenderGroup()))
		return E_FAIL;

	Set_EffectRG();

	CEvent_Manager::GetInstance()->Initialize(m_pDevice, m_pContext);


	return S_OK;
}

void CMainEffectTool::Progress(_float _fTimeDelta)
{
	m_pGameInstance->Start_Imgui();

	m_pGameInstance->Priority_Update_Engine(_fTimeDelta);

	m_pGameInstance->Update_Engine(_fTimeDelta);

	m_pGameInstance->Late_Update_Engine(_fTimeDelta);

#ifdef _DEBUG
	Debug_Default(_fTimeDelta);
#endif
	m_pGameInstance->End_Imgui();

	if (FAILED(Render()))
	{
		MSG_BOX("Failed Render MainApp");
		return;
	}


	// 뷰포트 나가도 렌더되는 처리 // 
	ImGui::RenderPlatformWindowsDefault();

#ifdef _DEBUG
	m_pGameInstance->Render_FPS(TEXT("Timer_Default"));
#endif // _DEBUG

	CEvent_Manager::GetInstance()->Update(_fTimeDelta);

}

HRESULT CMainEffectTool::Render()
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



HRESULT CMainEffectTool::SetUp_StartLevel(LEVEL_ID _eLevelID)
{
	EVENT tEvent;
	tEvent.eType = EVENT_TYPE::LEVEL_CHANGE;
	tEvent.Parameters.resize(2); // NumParameters

	tEvent.Parameters[0] = (DWORD_PTR)LEVEL_LOADING;
	tEvent.Parameters[1] = (DWORD_PTR)_eLevelID;

	CEvent_Manager::GetInstance()->AddEvent(tEvent);

	return S_OK;
}

HRESULT CMainEffectTool::Ready_Prototype_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModelInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"), VTXMESHPARTICLE::Elements, VTXMESHPARTICLE::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Skybox/Dutch-Sky_0157_4k.dds")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Object_Background"),
		CObject_Background::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Object_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainEffectTool::Ready_RenderTargets()
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

	//if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Shadow"), g_iShadowWidth, g_iShadowHeight)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_Book2D"), RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y)))
	//	return E_FAIL;

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

HRESULT CMainEffectTool::Ready_RenderGroup()
{
	CRenderGroup* pRenderGroup = nullptr;
	CRenderGroup_MRT* pRenderGroup_MRT = nullptr;

	///* RG_3D, PR3D_BOOK2D */
	//CRenderGroup_MRT::RG_MRT_DESC RG_Book2DDesc;
	//RG_Book2DDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	//RG_Book2DDesc.iPriorityID = PRIORITY_3D::PR3D_BOOK2D;
	//RG_Book2DDesc.isViewportSizeChange = true;
	//RG_Book2DDesc.strMRTTag = TEXT("MRT_Book_2D");
	//RG_Book2DDesc.pDSV = m_pGameInstance->Find_DSV(TEXT("DSV_Book2D"));
	//RG_Book2DDesc.vViewportSize = _float2((_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y);
	//RG_Book2DDesc.isClear = true;
	//if (nullptr == RG_Book2DDesc.pDSV)
	//{
	//	MSG_BOX("Book2D DSV가 없대.");
	//	return E_FAIL;
	//}
	//pRenderGroup_MRT = CRenderGroup_MRT::Create(m_pDevice, m_pContext, &RG_Book2DDesc);
	//if (nullptr == pRenderGroup_MRT)
	//{
	//	MSG_BOX("Failed Create PR3D_BOOK2D");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
	//	return E_FAIL;

	//Safe_Release(pRenderGroup_MRT);
	//pRenderGroup_MRT = nullptr;

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
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
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
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
		return E_FAIL;
	Safe_Release(pRenderGroup_MRT);
	pRenderGroup_MRT = nullptr;

	/* RG_3D, PR3D_LIGHTS */
	CRenderGroup_MRT::RG_MRT_DESC RG_LightDesc;
	RG_LightDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_LightDesc.iPriorityID = PRIORITY_3D::PR3D_LIGHTS;
	RG_LightDesc.strMRTTag = TEXT("MRT_LightAcc");
	CETool_RenderGroup_AfterEffect* pRenderGroup_Lights = CETool_RenderGroup_AfterEffect::Create(m_pDevice, m_pContext, &RG_LightDesc);
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
	CRenderGroup_MRT::RG_MRT_DESC RG_FinalDesc;
	RG_FinalDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_FinalDesc.iPriorityID = PRIORITY_3D::PR3D_FINAL;
	RG_FinalDesc.strMRTTag = TEXT("MRT_Final");
	RG_FinalDesc.isClear = false;
	CETool_RenderGroup_Final* pRenderGroup_Final = CETool_RenderGroup_Final::Create(m_pDevice, m_pContext, &RG_FinalDesc);
	if (nullptr == pRenderGroup_Final)
	{
		MSG_BOX("Failed Create PR3D_FINAL");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_Final->Get_RenderGroupID(), pRenderGroup_Final->Get_PriorityID(), pRenderGroup_Final)))
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
	CRenderGroup::RG_DESC RG_AfterEffectDesc;
	RG_AfterEffectDesc.iRenderGroupID = RENDERGROUP::RG_3D;
	RG_AfterEffectDesc.iPriorityID = PRIORITY_3D::PR3D_AFTEREFFECT;
	CETool_RenderGroup_AfterEffect* pRenderGroup_AfterEffect = CETool_RenderGroup_AfterEffect::Create(m_pDevice, m_pContext, &RG_AfterEffectDesc);
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

void CMainEffectTool::Set_EffectRG()
{
	
	CParticle_Emitter::SetID_2D(RG_2D);
	CParticle_Emitter::SetID_3D(RG_3D);
	CParticle_Emitter::SetID_Effect(PR3D_EFFECT);

}

CMainEffectTool* CMainEffectTool::Create()
{
	CMainEffectTool* pInstance = new CMainEffectTool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainEffectTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainEffectTool::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	CEvent_Manager::GetInstance()->DestroyInstance();

	/* GameInstance Release*/
	CGameInstance::Release_Engine();
}

#ifdef _DEBUG
void CMainEffectTool::Debug_Default(_float _fTimeDelta)
{
	ImGui::Begin("Main");

	// 프레임 출력
	m_fAccTime += _fTimeDelta;
	++m_iFrame;

	if (1.f <= m_fAccTime)
	{
		m_fAccTime = 0.f;
		m_iRenderFrame = m_iFrame;
		m_iFrame = 0;
	}

	ImGui::Text("FPS : %i", m_iRenderFrame);


	// 카메라 출력
	const _float4* pCamPosition = m_pGameInstance->Get_CamPosition();
	ImGui::Text("Camera Position : %.4f, %.4f, %.4f, %.4f", (*pCamPosition).x, (*pCamPosition).y, (*pCamPosition).z, (*pCamPosition).w);

	//_float4x4 CamMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	//ImGui::Text("Camera Look : %.4f, %.4f, %.4f, %.4f", CamMatrix._31, CamMatrix._32, CamMatrix._33, CamMatrix._34);
	//ImGui::Text("Camera Position : %.4f, %.4f, %.4f, %.4f", CamMatrix._41, CamMatrix._42, CamMatrix._43, CamMatrix._44);

	// 마우스 위치
	

	// Matrix
	_float4x4 ViewMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	ImGui::Text("View");
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ViewMatrix._11, ViewMatrix._12, ViewMatrix._13, ViewMatrix._14);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ViewMatrix._21, ViewMatrix._22, ViewMatrix._23, ViewMatrix._24);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ViewMatrix._31, ViewMatrix._32, ViewMatrix._33, ViewMatrix._34);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ViewMatrix._41, ViewMatrix._42, ViewMatrix._43, ViewMatrix._44);

	ImGui::Text("Proj");
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ProjMatrix._11, ProjMatrix._12, ProjMatrix._13, ProjMatrix._14);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ProjMatrix._21, ProjMatrix._22, ProjMatrix._23, ProjMatrix._24);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ProjMatrix._31, ProjMatrix._32, ProjMatrix._33, ProjMatrix._34);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", ProjMatrix._41, ProjMatrix._42, ProjMatrix._43, ProjMatrix._44);
	
	_float4x4 mulMatrix;
	XMStoreFloat4x4(&mulMatrix, XMLoadFloat4x4(&ViewMatrix) * XMLoadFloat4x4(&ProjMatrix));

	ImGui::Text("Multiplied");
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", mulMatrix._11, mulMatrix._12, mulMatrix._13, mulMatrix._14);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", mulMatrix._21, mulMatrix._22, mulMatrix._23, mulMatrix._24);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", mulMatrix._31, mulMatrix._32, mulMatrix._33, mulMatrix._34);
	ImGui::Text("%.4f, %.4f, %.4f, %.4f", mulMatrix._41, mulMatrix._42, mulMatrix._43, mulMatrix._44);


	static _float4 vInputPos = { 0.f, 0.f, 0.f, 1.f };
	static _float4 vOutputPos = { 0.f, 0.f, 0.f, 0.f };

	ImGui::InputFloat4("Input Position", (_float*)&vInputPos, "%.4f");
	XMStoreFloat4(&vOutputPos, XMVector4Transform(XMLoadFloat4(&vInputPos), XMLoadFloat4x4(&mulMatrix)));

	ImGui::Text("%.4f, %.4f, %.4f, %.4f", vOutputPos.x, vOutputPos.y, vOutputPos.z, vOutputPos.w);


	ImGui::End();
}
#endif