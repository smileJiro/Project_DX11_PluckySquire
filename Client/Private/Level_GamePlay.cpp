#include "stdafx.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Poolling_Manager.h"
#include "Camera_Manager.h"
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
	Ready_Layer_Player(TEXT("Layer_Player"), &pCameraTarget);
	Ready_Layer_Camera(TEXT("Layer_Camera"), pCameraTarget);
	Ready_Layer_Monster(TEXT("Layer_Monster"));
	Ready_Layer_UI(TEXT("Layer_UI"));


	/* Pooling Test */
	POOLLING_DESC Poolling_Desc;
	Poolling_Desc.iPrototypeLevelID = LEVEL_GAMEPLAY;
	Poolling_Desc.strLayerTag = TEXT("Layer_Monster");
	Poolling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Beetle");
	CBeetle::MONSTER_DESC* pDesc = new CBeetle::MONSTER_DESC;
	pDesc->iCurLevelID = LEVEL_GAMEPLAY;
	CPoolling_Manager::GetInstance()->Register_PoollingObject(TEXT("Poolling_TestBeetle"), Poolling_Desc, pDesc);

	
    return S_OK;
}

void CLevel_GamePlay::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::ENTER))
	{
		Event_LevelChange(LEVEL_LOADING, LEVEL_LOGO);
	}
	
	if (KEY_DOWN(KEY::NUM6))
	{
		/* Pooling Test */
		_float3 vPosition = _float3(m_pGameInstance->Compute_Random(-5.f, 5.f), m_pGameInstance->Compute_Random(1.f, 1.f), m_pGameInstance->Compute_Random(-5.f, 5.f));
		//CPoolling_Manager::GetInstance()->Create_Objects(TEXT("Poolling_TestBeetle"), 1); // 여러마리 동시 생성. 

		CPoolling_Manager::GetInstance()->Create_Object(TEXT("Poolling_TestBeetle"), &vPosition); // 한마리 생성.
	}

	// Change Camera Free  Or Target
	if (KEY_DOWN(KEY::B)) {
		m_pGameInstance->Change_CameraType(CCamera_Manager::FREE);
	}
	else if (KEY_DOWN(KEY::N)) {
		m_pGameInstance->Change_CameraType(CCamera_Manager::TARGET);
	}

}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	m_pGameInstance->Render_FPS(TEXT("Timer_Default"));
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
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
	CGameObject* pCamera = nullptr;

	// Free Camera
	CCamera_Free::CAMERA_FREE_DESC Desc{};

	Desc.fMouseSensor = 0.1f;

	Desc.fFovy = XMConvertToRadians(30.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_GAMEPLAY, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	m_pGameInstance->Add_Camera(CCamera_Manager::FREE, dynamic_cast<CCamera*>(pCamera));

	// Target Camera
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	TargetDesc.fSmoothSpeed = 5.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;

	TargetDesc.fFovy = XMConvertToRadians(30.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.fNear = 0.1f;
	TargetDesc.fFar = 1000.f;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Target"),
		LEVEL_GAMEPLAY, _strLayerTag, &pCamera, &TargetDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Camera(CCamera_Manager::TARGET, dynamic_cast<CCamera*>(pCamera));

	m_pGameInstance->Change_CameraType(CCamera_Manager::FREE);

	Create_Arm();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CTestPlayer::CONTAINEROBJ_DESC Desc;
	Desc.iCurLevelID = LEVEL_GAMEPLAY;

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
	TerrainDesc.iModelPrototypeLevelID_3D = LEVEL_GAMEPLAY;
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
	CBeetle::MONSTER_DESC Monster_Desc;
	Monster_Desc.iCurLevelID = LEVEL_GAMEPLAY;

	Monster_Desc.tTransform3DDesc.vPosition = _float3(10.0f, 1.0f, 10.0f);
	Monster_Desc.tTransform3DDesc.vScaling = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Beetle"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
		return E_FAIL;

	Monster_Desc.tTransform3DDesc.vPosition = _float3(-10.0f, 1.0f, 10.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BarfBug"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Create_Arm()
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	_vector vPlayerLook = pPlayer->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);

	CCameraArm::CAMERA_ARM_DESC Desc{};

	XMStoreFloat3(&Desc.vArm, -vPlayerLook);
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	Desc.vRotation = { XMConvertToRadians(60.f), XMConvertToRadians(0.f), 0.f };
	Desc.fLength = 20.f;
	Desc.wszArmTag = TEXT("Player_Arm");
	Desc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr();

	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);


	CCamera_Target* pTarget = dynamic_cast<CCamera_Target*>(m_pGameInstance->Get_Camera(CCamera_Manager::TARGET));

	pTarget->Add_Arm(pArm);
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
