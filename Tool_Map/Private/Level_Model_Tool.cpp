#include "stdafx.h"
#include "Level_Model_Tool.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Model_Tool_Manager.h"
#include "CubeMap.h"

CLevel_Model_Tool::CLevel_Model_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Model_Tool::Initialize(CImguiLogger* _pLogger)
{

	m_pLogger = _pLogger;
	Safe_AddRef(m_pLogger);
	Ready_Lights();
	Ready_CubeMap(TEXT("Layer_CubeMap"));
	Ready_Layer_Camera(L"Layer_Camera");
	m_pToolManager = CModel_Tool_Manager::Create(m_pDevice, m_pContext, m_pLogger);
	if (nullptr == m_pToolManager)
		return E_FAIL;
	m_pGameInstance->Set_DebugRender(true);


	/* Blur RenderGroupOn */
	m_pGameInstance->Set_Active_RenderGroup_New(RENDERGROUP::RG_3D, PR3D_POSTPROCESSING, true);
	return S_OK;
}

void CLevel_Model_Tool::Update(_float _fTimeDelta)
{
	m_pToolManager->Update_Tool();
}

HRESULT CLevel_Model_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Mojam 3D Model Tool."));
#endif

	return S_OK;
}

HRESULT CLevel_Model_Tool::Ready_Lights()
{
	CONST_LIGHT LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.vPosition = _float3(0.0f, 20.0f, 0.0f);
	LightDesc.fFallOutStart = 20.0f;
	LightDesc.fFallOutEnd = 1000.0f;
	LightDesc.vRadiance = _float3(1.0f, 1.0f, 1.0f);
	LightDesc.vDiffuse = _float4(1.0f, 0.0f, 0.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
	LightDesc.vSpecular = _float4(1.0f, 0.0f, 0.0f, 1.0f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::POINT)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Model_Tool::Ready_CubeMap(const _wstring& _strLayerTag)
{
	CGameObject* pCubeMap = nullptr;
	CCubeMap::CUBEMAP_DESC Desc;
	Desc.iCurLevelID = LEVEL_TOOL_3D_MODEL;
	Desc.iRenderGroupID = RG_3D;
	Desc.iPriorityID = PR3D_PRIORITY;
	Desc.strBRDFPrototypeTag = TEXT("Prototype_Component_Texture_BRDF_Shilick");
	Desc.strCubeMapPrototypeTag = TEXT("Prototype_Component_Texture_TestEnv");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
		LEVEL_TOOL_3D_MODEL, _strLayerTag, &pCubeMap, &Desc)))
		return E_FAIL;

	m_pGameInstance->Set_CubeMap(static_cast<CCubeMap*>(pCubeMap));
	return S_OK;
}


HRESULT CLevel_Model_Tool::Ready_Layer_Camera(const _wstring& _strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC Desc{};

	Desc.fMouseSensor = 0.1f;

	Desc.fFovy = XMConvertToRadians(60.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.eZoomLevel = CCamera::LEVEL_6;
	Desc.eMode = CCamera_Free::INPUT_MODE_WASD;
	//Desc.iCameraType = CCamera_Manager::FREE;
	CGameObject* pGameObject = nullptr;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_TOOL_3D_MAP, _strLayerTag, &pGameObject, &Desc)))
		return E_FAIL;
	else
	{
		pGameObject->Get_ControllerTransform()->Set_SpeedPerSec(100.f);
		//pGameObject->Late_Update(1.f);
		//pGameObject->Set_Active(false);
	}
	return S_OK;
}



CLevel_Model_Tool* CLevel_Model_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	CLevel_Model_Tool* pInstance = new CLevel_Model_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : CLevel_Model_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Model_Tool::Free()
{
	Safe_Release(m_pToolManager);
	Safe_Release(m_pLogger);
	__super::Free();
}
