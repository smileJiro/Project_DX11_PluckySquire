#include "stdafx.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Cam_Manager.h"
#include "TestPlayer.h"
#include "TestTerrain.h"
#include "Beetle.h"

#include "UI.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	Ready_Lights();
	CGameObject* pCameraTarget = nullptr;
	Ready_Layer_TestTerrain(TEXT("Layer_Terrain"));
	//Ready_Layer_Player(TEXT("Layer_Player"), &pCameraTarget);
	Ready_Layer_Camera(TEXT("Layer_Camera"), pCameraTarget);
	Ready_Layer_Monster(TEXT("Layer_Monster"));
	Ready_Layer_UI(TEXT("Layer_UI"));
    return S_OK;
}

void CLevel_GamePlay::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::ENTER))
	{
		Event_LevelChange(LEVEL_LOADING, LEVEL_LOGO);
	}

}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	//m_pGameInstance->Render_FPS(TEXT("Timer_Default"));
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTOINAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{
	/* Camera_Target */
	CGameObject* pGameObject = nullptr;
	CCamera_Target::CAMERA_TARGET_DESC Desc{};
	Desc.tTransform3DDesc.fSpeedPerSec = 10.f;
	Desc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	Desc.eZoomLevel = CCamera::NORMAL;
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.0f, 10.0f, -7.0f);
	Desc.vAt = _float3(0.0f, 0.0f, 0.0f);
	Desc.iCurLevelID = (_uint)LEVEL_GAMEPLAY;
	Desc.fFovy = XMConvertToRadians(60.f);

	Desc.pTarget = _pTarget;
	Desc.vArmRotAxis = { 1.0f, 0.0f, 0.0f };
	Desc.fArmAngle = XMConvertToRadians(-157.f);
	Desc.fDistance = 4.5f;
	Desc.fMouseSensor = 1.2f;
	Desc.eCameraType = CCamera::TARGET;
	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Target"), LEVEL_GAMEPLAY, _strLayerTag, &pGameObject, &Desc);
	if (nullptr == pGameObject)
		return E_FAIL;



	CCam_Manager::GetInstance()->Set_TargetCamera(static_cast<CCamera_Target*>(pGameObject));

	CCam_Manager::GetInstance()->Change_Cam(CCam_Manager::CAM_TARGET);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CTestPlayer::CONTAINEROBJ_DESC Desc;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayer"), LEVEL_GAMEPLAY, _strLayerTag, _ppOut, &Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	/* Test Terrain */
	CTestTerrain::MODELOBJECT_DESC TerrainDesc{};

	TerrainDesc.eStartCoord = COORDINATE_3D;
	TerrainDesc.iCurLevelID = LEVEL_GAMEPLAY;
	TerrainDesc.isCoordChangeEnable = false;
	TerrainDesc.i3DModelPrototypeLevelID = LEVEL_GAMEPLAY;
	TerrainDesc.strModelPrototypeTag_3D = TEXT("WoodenPlatform_01");
	TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");

	TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	TerrainDesc.tTransform3DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
	TerrainDesc.tTransform3DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestTerrain"), LEVEL_GAMEPLAY, _strLayerTag, &TerrainDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& _strLayerTag)
{
	CUI::UIOBJDESC pDesc = {};

	pDesc.fX = g_iWinSizeX - g_iWinSizeX / 12;
	pDesc.fY = g_iWinSizeY / 10;
	pDesc.fSizeX = 182.f;
	pDesc.fSizeY = 100.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_UIObejct_PickBubble"), LEVEL_GAMEPLAY, _strLayerTag, &pDesc)))
		return E_FAIL;






	//pDesc.fX = g_iWinSizeX - g_iWinSizeX / 2;
	//pDesc.fY = g_iWinSizeY / 2;
	//
	//
	//pDesc.fSizeX = 1200.f;
	//pDesc.fSizeY = 600.f;
	////pDesc.fSizeX = g_iWinSizeX / 2;
	////pDesc.fSizeY = g_iWinSizeX / 2;
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), LEVEL_GAMEPLAY, _strLayerTag, &pDesc)))
	//	return E_FAIL;


	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& _strLayerTag, CGameObject** _ppout)
{
	CGameObject* pGameObject = nullptr;
	CBeetle::MONSTER_DESC Monster_Desc;
	Monster_Desc.iCurLevelID = LEVEL_GAMEPLAY;

	Monster_Desc.tTransform3DDesc.vPosition = _float3(10.0f, 1.0f, 10.0f);
	Monster_Desc.tTransform3DDesc.vScaling = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Beetle"), LEVEL_GAMEPLAY, _strLayerTag, &pGameObject, &Monster_Desc)))
		return E_FAIL;

	if(nullptr != _ppout)
		_ppout = &pGameObject;

	return S_OK;
}


CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
