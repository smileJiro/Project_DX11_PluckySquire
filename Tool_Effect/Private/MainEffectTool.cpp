#include "pch.h"
#include "MainEffectTool.h"
#include "GameInstance.h"

#include "Event_Manager.h"
#include "Object_Background.h"

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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Object_Background"),
		CObject_Background::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
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
	
	// 카메라 출력
	const _float4* pCamPosition = m_pGameInstance->Get_CamPosition();
	ImGui::Text("Camera Position : %.4f, %.4f, %.4f, %.4f", (*pCamPosition).x, (*pCamPosition).y, (*pCamPosition).z, (*pCamPosition).w);

	//_float4x4 CamMatrix = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	//ImGui::Text("Camera Look : %.4f, %.4f, %.4f, %.4f", CamMatrix._31, CamMatrix._32, CamMatrix._33, CamMatrix._34);
	//ImGui::Text("Camera Position : %.4f, %.4f, %.4f, %.4f", CamMatrix._41, CamMatrix._42, CamMatrix._43, CamMatrix._44);


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
		
	ImGui::End();
}
#endif