#include "stdafx.h"
#include "Level_2D_Map_Tool.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "2DMap_Tool_Manager.h"
#include "CubeMap.h"
CLevel_2D_Map_Tool::CLevel_2D_Map_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_2D_Map_Tool::Initialize(CImguiLogger* _pLogger)
{

	m_pLogger = _pLogger;
	Safe_AddRef(m_pLogger);

	Ready_Lights();
	Ready_CubeMap(TEXT("Layer_CubeMap"));
	Ready_Layer_Camera(TEXT("Layer_Camera"));

	m_pToolManager = C2DMap_Tool_Manager::Create(m_pDevice, m_pContext, m_pLogger);
	if (nullptr == m_pToolManager)
		return E_FAIL;
	m_pGameInstance->Set_DebugRender(false);

	return S_OK;
}

void CLevel_2D_Map_Tool::Update(_float _fTimeDelta)
{
	m_pToolManager->Update_Tool();
}

HRESULT CLevel_2D_Map_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Mojam 2D Map Tool"));
#endif

	m_pToolManager->Render();


	return S_OK;
}

HRESULT CLevel_2D_Map_Tool::Ready_Lights()
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

HRESULT CLevel_2D_Map_Tool::Ready_CubeMap(const _wstring& _strLayerTag)
{
	CGameObject* pCubeMap = nullptr;
	CCubeMap::CUBEMAP_DESC Desc;
	Desc.iCurLevelID = LEVEL_TOOL_2D_MAP;
	Desc.iRenderGroupID = RG_3D;
	Desc.iPriorityID = PR3D_PRIORITY;
	Desc.strBRDFPrototypeTag = TEXT("Prototype_Component_Texture_BRDF_Shilick");
	Desc.strCubeMapPrototypeTag = TEXT("Prototype_Component_Texture_TestEnv");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
		LEVEL_TOOL_2D_MAP, _strLayerTag, &pCubeMap, &Desc)))
		return E_FAIL;

	m_pGameInstance->Set_CubeMap(static_cast<CCubeMap*>(pCubeMap));
	return S_OK;
}


HRESULT CLevel_2D_Map_Tool::Ready_Layer_Camera(const _wstring& _strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		Desc{};
	Desc.fMouseSensor = 0.2f;
	Desc.eZoomLevel = Engine::CCamera::ZOOM_LEVEL::LEVEL_6;
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.2f;
	Desc.fFar = 10000.f;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.fFovy = XMConvertToRadians(60.f);
	Desc.eMode = CCamera_Free::INPUT_MODE_WASD;
	CGameObject* pGameObject = nullptr;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_TOOL_2D_MAP, _strLayerTag, &pGameObject, &Desc)))
		return E_FAIL;
	else
	{
		pGameObject->Get_ControllerTransform()->Set_SpeedPerSec(100.f);

		pGameObject->Set_Position(XMVectorSet(1.f, 20.f, -30.f, 1.f));
		pGameObject->Late_Update(1.f);
		pGameObject->Set_Active(false);

	}
	return S_OK;
}


CLevel_2D_Map_Tool* CLevel_2D_Map_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger)
{
	CLevel_2D_Map_Tool* pInstance = new CLevel_2D_Map_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pLogger)))
	{
		MSG_BOX("Failed to Created : CLevel_2D_Map_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_2D_Map_Tool::Free()
{
	Safe_Release(m_pToolManager);
	Safe_Release(m_pLogger);
	__super::Free();
}
