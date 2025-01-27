#include "stdafx.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Pooling_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_CutScene.h"

#include "Player.h"
#include "TestTerrain.h"
#include "Beetle.h"
#include "BarfBug.h"
#include "JumpBug.h"
#include "BirdMonster.h"
#include "Goblin.h"
#include "Popuff.h"
#include "Rat.h"
#include "Soldier.h"
#include "ButterGrump.h"


//#include "UI.h"
#include "UI_Manager.h"
#include "SettingPanelBG.h"
#include "SettingPanel.h"
#include "ESC_HeartPoint.h"
#include "ShopItemBG.h"

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
	Pooling_DESC Pooling_Desc;
	Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
	Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Beetle");
	CBeetle::MONSTER_DESC* pDesc = new CBeetle::MONSTER_DESC;
	pDesc->iCurLevelID = LEVEL_GAMEPLAY;
	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_TestBeetle"), Pooling_Desc, pDesc);

	//
	return S_OK;
}

void CLevel_GamePlay::Update(_float _fTimeDelta)
{
	ImGuiIO& IO = ImGui::GetIO(); (void)IO;

	if (KEY_DOWN(KEY::ENTER) && !IO.WantCaptureKeyboard)
	{
		Event_LevelChange(LEVEL_LOADING, LEVEL_LOGO);
	}

	if (KEY_DOWN(KEY::NUM6))
	{
		/* Pooling Test */
		_float3 vPosition = _float3(m_pGameInstance->Compute_Random(-5.f, 5.f), m_pGameInstance->Compute_Random(1.f, 1.f), m_pGameInstance->Compute_Random(-5.f, 5.f));
		//CPooling_Manager::GetInstance()->Create_Objects(TEXT("Pooling_TestBeetle"), 1); // 여러마리 동시 생성. 

		CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_TestBeetle"), &vPosition); // 한마리 생성.
	}

	// Change Camera Free  Or Target
	if (KEY_DOWN(KEY::C)) {
		_uint iCurCameraType = CCamera_Manager::GetInstance()->Get_CameraType();
		iCurCameraType ^= 1;
		CCamera_Manager::GetInstance()->Change_CameraType(iCurCameraType);
	}

	if (KEY_DOWN(KEY::Z))
	{
		CUI_Manager::STAMP eStamp;
		eStamp = CUI_Manager::GetInstance()->Get_StampIndex();

		if (eStamp == CUI_Manager::STAMP_BOMB)
		{
			CUI_Manager::GetInstance()->Set_StampIndex(CUI_Manager::STAMP_STOP);
		}
		else if (eStamp == CUI_Manager::STAMP_STOP)
		{
			CUI_Manager::GetInstance()->Set_StampIndex(CUI_Manager::STAMP_BOMB);
		}

	}

	if (KEY_DOWN(KEY::P)) 
		CCamera_Manager::GetInstance()->Start_ZoomIn();
	

	if (KEY_DOWN(KEY::O)) 
		CCamera_Manager::GetInstance()->Start_ZoomOut();

	if (KEY_DOWN(KEY::U)) {
		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
		CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("B_InitialData"));
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
	CGameObject* pCamera = nullptr;

	// Free Camera
	CCamera_Free::CAMERA_FREE_DESC Desc{};

	Desc.fMouseSensor = 0.1f;

	Desc.fFovy = XMConvertToRadians(60.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.eZoomLevel = CCamera::LEVEL_6;
	Desc.iCameraType = CCamera_Manager::FREE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_GAMEPLAY, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::FREE, dynamic_cast<CCamera*>(pCamera));

	// Target Camera
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	TargetDesc.fSmoothSpeed = 5.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;
	TargetDesc.vAtOffset = _float3(0.0f, 0.5f, 0.0f);

	TargetDesc.fFovy = XMConvertToRadians(60.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.fNear = 0.1f;
	TargetDesc.fFar = 1000.f;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);
	TargetDesc.eZoomLevel = CCamera::LEVEL_6;
	TargetDesc.iCameraType = CCamera_Manager::TARGET;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Target"),
		LEVEL_GAMEPLAY, _strLayerTag, &pCamera, &TargetDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::TARGET, dynamic_cast<CCamera*>(pCamera));

	Create_Arm();

	// CutScene Camera
	CCamera_CutScene::CAMERA_DESC CutSceneDesc{};

	CutSceneDesc.fFovy = XMConvertToRadians(60.f);
	CutSceneDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CutSceneDesc.fNear = 0.1f;
	CutSceneDesc.fFar = 1000.f;
	CutSceneDesc.vEye = _float3(0.f, 10.f, -7.f);
	CutSceneDesc.vAt = _float3(0.f, 0.f, 0.f);
	CutSceneDesc.eZoomLevel = CCamera::LEVEL_6;
	CutSceneDesc.iCameraType = CCamera_Manager::CUTSCENE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_CutScene"),
		LEVEL_GAMEPLAY, _strLayerTag, &pCamera, &CutSceneDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::CUTSCENE, dynamic_cast<CCamera*>(pCamera));
	
	CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::FREE);

	// Load CutSceneData
	CCamera_Manager::GetInstance()->Load_CutSceneData();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CPlayer::CONTAINEROBJ_DESC Desc;
	Desc.iCurLevelID = LEVEL_GAMEPLAY;
	Desc.tTransform3DDesc.vInitialPosition = { -3.f, 0.35f, -19.3f };   // TODO ::임시 위치

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestPlayer"), LEVEL_GAMEPLAY, _strLayerTag, _ppOut, &Desc)))
		return E_FAIL;


	//CPlayer* pPlayer = { nullptr };
	//pPlayer = dynamic_cast<CPlayer*>(*_ppOut);
	//
	//if (nullptr == Uimgr->Get_Player())
	//{
	//	CUI_Manager::GetInstance()->Set_Player(pPlayer);
	//}
	
	//Safe_Release(pPlayer);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	/* Test Terrain */
	//CTestTerrain::MODELOBJECT_DESC TerrainDesc{};

	//TerrainDesc.eStartCoord = COORDINATE_3D;
	//TerrainDesc.iCurLevelID = LEVEL_GAMEPLAY;
	//TerrainDesc.isCoordChangeEnable = false;
	//TerrainDesc.iModelPrototypeLevelID_3D = LEVEL_GAMEPLAY;
	//TerrainDesc.strModelPrototypeTag_3D = TEXT("WoodenPlatform_01");
	//TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");

	//TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	//TerrainDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	//TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	//TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	//TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_TestTerrain"), LEVEL_GAMEPLAY, _strLayerTag, &TerrainDesc)))
	//	return E_FAIL;



	//TODO :: SAMPLE

	CModelObject::MODELOBJECT_DESC Desc = {};

	Desc.iCurLevelID = LEVEL_GAMEPLAY;
	Desc.iModelPrototypeLevelID_3D = LEVEL_GAMEPLAY;


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_2DDefaultRenderObject"),
		LEVEL_GAMEPLAY, L"Layer_Default", &Desc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_SampleBook"),
		LEVEL_GAMEPLAY, L"Layer_Default", &Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& _strLayerTag)
{
	CUI::UIOBJDESC pDesc = {};
	CUI::UIOBJDESC pDescs[CUI::SETTINGPANEL::SETTING_END] = {};
	CUI::UIOBJDESC pShopDescs[CUI::SHOPPANEL::SHOP_END] = {};
	pDesc.iCurLevelID = LEVEL_GAMEPLAY;

#pragma region PickBubble UI
	pDesc.fX = g_iWinSizeX - g_iWinSizeX / 12.f;
	pDesc.fY = g_iWinSizeY / 10.f;
	pDesc.fSizeX = 182.f;
	pDesc.fSizeY = 100.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_UIObejct_PickBubble"), LEVEL_GAMEPLAY, _strLayerTag, &pDesc)))
		return E_FAIL;
#pragma endregion PickBubble UI

#pragma region STAMP UI
	pDesc.fX = g_iWinSizeX / 20.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 10.f;

	// 원래 크기
	pDesc.fSizeX = 96.f;
	pDesc.fSizeY = 148.f;

	//작게  크기
	//pDesc.fSizeX = 48.f;
	//pDesc.fSizeY = 74.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_StopStamp"), LEVEL_GAMEPLAY, _strLayerTag, &pDesc)))
		return E_FAIL;

	pDesc.fX = g_iWinSizeX / 7.5f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 10.f;
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 111.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BombStamp"), LEVEL_GAMEPLAY, _strLayerTag, &pDesc)))
		return E_FAIL;

	pDesc.fX = g_iWinSizeX / 10.8f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 20.f;
	pDesc.fSizeX = 42.f;
	pDesc.fSizeY = 27.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ArrowForStamp"), LEVEL_GAMEPLAY, _strLayerTag, &pDesc)))
		return E_FAIL;
#pragma endregion STAMP UI

#pragma region InterAction UI
	pDesc.fX = g_iWinSizeX / 2.f - g_iWinSizeX / 20.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 18.f;
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 72.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Interaction_Book"), LEVEL_GAMEPLAY, _strLayerTag, &pDesc)))
		return E_FAIL;


#pragma endregion InterAction UI

#pragma region ShopPanel UI

	_uint ShopPanelUICount = { CUI::SHOPPANEL::SHOP_END };

	if (ShopPanelUICount != CUI_Manager::GetInstance()->Get_ShopPanels().size())
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pDesc.iCurLevelID, TEXT("Prototype_GameObject_ParentShopPannel"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
			return E_FAIL;
	}





	for (_uint i = 0; i < CUI::SHOPPANEL::SHOP_END; ++i)
	{
		pShopDescs[i].iCurLevelID = LEVEL_GAMEPLAY;
	}

	if (ShopPanelUICount != CUI_Manager::GetInstance()->Get_ShopPanels().size())
	{
		for (_uint i = 0; i < CUI::SHOPPANEL::SHOP_END; ++i)
		{
			CGameObject* pShopPanel = { nullptr };
			CUI::UIOBJDESC pShopPanelDesc{};


			switch (i)
			{
			case CUI::SHOPPANEL::SHOP_BG:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fX = g_iWinSizeX / 2.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fY = g_iWinSizeY / 2.5f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeX = 937.6f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].fSizeY = 617.6f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BG;
				pShopDescs[CUI::SHOPPANEL::SHOP_BG].eShopPanelKind = CUI::SHOPPANEL::SHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;

			case CUI::SHOPPANEL::SHOP_DIALOGUEBG:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fX = g_iWinSizeX / 2.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fY = g_iWinSizeY - g_iWinSizeY / 5.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeX = 928.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].fSizeY = 169.6f;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_DIALOGUEBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_DIALOGUEBG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_DIALOGUEBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;

			case CUI::SHOPPANEL::SHOP_CHOOSEBG:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fX = g_iWinSizeX - g_iWinSizeX / 5.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fY = g_iWinSizeY - g_iWinSizeY / 3.7f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeX = 400.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].fSizeY = 213.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_CHOOSEBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_CHOOSEBG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_CHOOSEBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;

			case CUI::SHOPPANEL::SHOP_BULB:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fX = g_iWinSizeX - g_iWinSizeX / 4.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fY = g_iWinSizeY / 8.7f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeX = 320.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].fSizeY = 136.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BULB;
				pShopDescs[CUI::SHOPPANEL::SHOP_BULB].eShopPanelKind = CUI::SHOPPANEL::SHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BULB])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BULB].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;

			case CUI::SHOPPANEL::SHOP_BACKESC:
			{

				/*
								pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fX = g_iWinSizeX / 30.f;
					pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
					pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fSizeX = 72.f;
					pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fSizeY = 72.f;

				*/

				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fX = g_iWinSizeX / 2.75f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fY = g_iWinSizeY - g_iWinSizeY / 13.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeX = 40.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].fSizeY = 40.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ESCBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ESCBG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ESCBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);


				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fX = g_iWinSizeX / 2.75f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fY = g_iWinSizeY - g_iWinSizeY / 13.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeX = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].fSizeY = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKESC;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKESC;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKESC].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;

			case CUI::SHOPPANEL::SHOP_BACKARROW:
			{


				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fX = g_iWinSizeX / 3.5f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fY = g_iWinSizeY - g_iWinSizeY / 13.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeX = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].fSizeY = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_BACKARROW;
				pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].eShopPanelKind = CUI::SHOPPANEL::SHOP_BACKARROW;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_BACKARROW].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;


			case CUI::SHOPPANEL::SHOP_ENTER:
			{
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fX = g_iWinSizeX - g_iWinSizeX / 3.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fY = g_iWinSizeY - g_iWinSizeY / 13.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeX = 40.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].fSizeY = 40.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTERBG;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTERBG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTERBG].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);


				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fX = g_iWinSizeX - g_iWinSizeX / 3.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fY = g_iWinSizeY - g_iWinSizeY / 13.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeX = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].fSizeY = 72.f;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount = (_uint)CUI::SHOPPANEL::SHOP_ENTER;
				pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].eShopPanelKind = CUI::SHOPPANEL::SHOP_ENTER;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pShopDescs[i].iCurLevelID, TEXT("Prototype_GameObject_ShopPannelBG"), pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iCurLevelID, _strLayerTag, &pShopPanel, &pShopDescs[CUI::SHOPPANEL::SHOP_ENTER])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_ShopPanels((_uint)pShopDescs[CUI::SHOPPANEL::SHOP_ENTER].iTextureCount, static_cast<CShopPanel_BG*>(pShopPanel));
				Safe_Release(pShopPanel);
			}
			break;

			default:
				break;
			}

		}
	}



	if (5 != CUI_Manager::GetInstance()->Get_ShopItems().size())
	{
		_uint iCurLevel = LEVEL_GAMEPLAY;


		for (_uint i = 0; i < 5; ++i)
		{
			CGameObject* pShopItem = { nullptr };
			CUI::UIOBJDESC eShopDesc;


			switch (i)
			{
			case 0:
			{
				vector<CShopItemBG*> _vItemBG;
				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 2.f;
				eShopDesc.fY = g_iWinSizeY / 4.f;
				eShopDesc.fSizeX = 860.f;
				eShopDesc.fSizeY = 110.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);


				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 3.5f;
				eShopDesc.fY = g_iWinSizeY / 4.f;
				eShopDesc.fSizeX = 128.f;
				eShopDesc.fSizeY = 128.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_SCROLLITEM;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);


				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX - g_iWinSizeX / 3.f;
				eShopDesc.fY = g_iWinSizeY / 4.f;
				eShopDesc.fSizeX = 60.f;
				eShopDesc.fSizeY = 72.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.isChooseItem = true;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);

				_vItemBG.push_back(pItemBG);
				CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

			}
			break;

			case 1:
			{
				vector<CShopItemBG*> _vItemBG;
				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 2.f;
				eShopDesc.fY = g_iWinSizeY / 2.5f;
				eShopDesc.fSizeX = 860.f;
				eShopDesc.fSizeY = 110.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);



				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 3.5f;
				eShopDesc.fY = g_iWinSizeY / 2.5f;
				eShopDesc.fSizeX = 128.f;
				eShopDesc.fSizeY = 128.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 2;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_ATTACKPLUSBADGE;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);




				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX - g_iWinSizeX / 3.f;
				eShopDesc.fY = g_iWinSizeY / 2.5f;
				eShopDesc.fSizeX = 60.f;
				eShopDesc.fSizeY = 72.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);


				CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

			}
			break;

			case 2:
			{
				vector<CShopItemBG*> _vItemBG;
				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 2.f;
				eShopDesc.fY = g_iWinSizeY - g_iWinSizeY / 2.2f;
				eShopDesc.fSizeX = 860.f;
				eShopDesc.fSizeY = 110.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				CShopItemBG* pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);



				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX / 3.5f;
				eShopDesc.fY = g_iWinSizeY - g_iWinSizeY / 2.2f;
				eShopDesc.fSizeX = 128.f;
				eShopDesc.fSizeY = 128.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_JUMPATTACKBADGE;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);




				eShopDesc.iCurLevelID = iCurLevel;
				eShopDesc.fX = g_iWinSizeX - g_iWinSizeX / 3.f;
				eShopDesc.fY = g_iWinSizeY - g_iWinSizeY / 2.2f;
				eShopDesc.fSizeX = 60.f;
				eShopDesc.fSizeY = 72.f;
				eShopDesc.iShopItemCount = i;
				eShopDesc.iSkillLevel = 0;
				eShopDesc.eShopSkillKind = CUI::SKILLSHOP_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(eShopDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopItem"), eShopDesc.iCurLevelID, _strLayerTag, &pShopItem, &eShopDesc)))
					return E_FAIL;

				pItemBG = static_cast<CShopItemBG*>(pShopItem);
				_vItemBG.push_back(pItemBG);


				CUI_Manager::GetInstance()->pushBack_ShopItem(_vItemBG);

			}
			break;

			case 3:
			{
				vector<CShopItemBG*> _vItemBG;

			}
			break;

			case 4:
			{
				vector<CShopItemBG*> _vItemBG;

			}
			break;

			}
		}
	}


#pragma endregion ShopPanel UI

#pragma region SettingPanel UI

	_uint SettingPanelUICount = CUI::SETTINGPANEL::SETTING_END;

	if (SettingPanelUICount != CUI_Manager::GetInstance()->Get_SettingPanels().size())
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_ParentSettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BG].iCurLevelID, _strLayerTag, &pDesc)))
			return E_FAIL;
	}

	for (_uint i = 0; i < CUI::SETTINGPANEL::SETTING_END; ++i)
	{
		pDescs[i].iCurLevelID = LEVEL_GAMEPLAY;
	}


	if (5 != CUI_Manager::GetInstance()->Get_SettingPanels().size())
	{
		for (_uint i = 0; i < CUI::SETTINGPANEL::SETTING_END; ++i)
		{
			CGameObject* pSettingPanel = { nullptr };
			CUI::UIOBJDESC pSettingPanelDesc{};




			switch (i)
			{
			case CUI::SETTINGPANEL::SETTING_BG:
			{
				pDescs[CUI::SETTINGPANEL::SETTING_BG].fX = g_iWinSizeX / 2.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BG].fY = g_iWinSizeY / 2.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BG].fSizeX = 879.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BG].fSizeY = 579.f;
				//pDescs[CUI::SETTINGPANEL::SETTING_BG].fSizeX = 1172.f;
				//pDescs[CUI::SETTINGPANEL::SETTING_BG].fSizeY = 772.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BG].iTextureCount = CUI::SETTINGPANEL::SETTING_BG;
				pDescs[CUI::SETTINGPANEL::SETTING_BG].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_BG;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BG].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_BG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_BG].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);

			}
			break;
			case CUI::SETTINGPANEL::SETTING_BULB:
			{
				pDescs[CUI::SETTINGPANEL::SETTING_BULB].fX = g_iWinSizeX - g_iWinSizeX / 9.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BULB].fY = g_iWinSizeY / 10.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BULB].fSizeX = 60.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BULB].fSizeY = 82.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BULB].iTextureCount = CUI::SETTINGPANEL::SETTING_BULB;
				pDescs[CUI::SETTINGPANEL::SETTING_BULB].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_BULB;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BULB].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_BULB])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_BULB].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);
			}
			break;
			case CUI::SETTINGPANEL::SETTING_BACKESC:
			{

				pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].fX = g_iWinSizeX / 14.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].fSizeX = 40;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].fSizeY = 40.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].iTextureCount = CUI::SETTINGPANEL::SETTING_ESCBG;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_ESCBG;
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_ESCBG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_ESCBG].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);

				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fX = g_iWinSizeX / 14.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fSizeX = 72;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fSizeY = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].iTextureCount = CUI::SETTINGPANEL::SETTING_BACKESC;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_BACKESC;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_BACKESC])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);
			}
			break;
			case CUI::SETTINGPANEL::SETTING_BACKARROW:
			{

				pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fX = g_iWinSizeX / 30.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fSizeX = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].fSizeY = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].iTextureCount = CUI::SETTINGPANEL::SETTING_BACKARROW;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_BACKARROW;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_BACKARROW].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);
			}
			break;
			case CUI::SETTINGPANEL::SETTING_ESCENTER:
			{
				pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].fX = g_iWinSizeX - g_iWinSizeX / 10.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].fSizeX = 40.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].fSizeY = 40.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].iTextureCount = CUI::SETTINGPANEL::SETTING_ENTERBG;
				pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_ENTERBG;
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_ENTERBG].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);


				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fX = g_iWinSizeX - g_iWinSizeX / 10.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fSizeX = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fSizeY = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].iTextureCount = CUI::SETTINGPANEL::SETTING_ESCENTER;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_ESCENTER;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);
			}
			break;

			case CUI::SETTINGPANEL::SETTING_BOOKMARK:
			{
				pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].fX = g_iWinSizeX / 3.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].fY = g_iWinSizeY - g_iWinSizeY / 4.3f;
				pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].fSizeX = 224.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].fSizeY = 268.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].iTextureCount = CUI::SETTINGPANEL::SETTING_BOOKMARK;
				pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_BOOKMARK;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_BOOKMARK].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				Safe_Release(pSettingPanel);

			}
			break;

			case CUI::SETTINGPANEL::SETTING_HEART:
			{
				CGameObject* pSettingPanel = { nullptr };
				pDesc.fX = g_iWinSizeX - g_iWinSizeX / 4.f;
				pDesc.fY = g_iWinSizeY / 10.f;
				pDesc.fSizeX = 128.f;
				pDesc.fSizeY = 128.f;
				pDesc.eSettingPanelKind = CUI::SETTINGPANEL::SETTING_HEART;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ESCHeartPoint"), pDesc.iCurLevelID, _strLayerTag, &pSettingPanel, &pDesc)))
					return E_FAIL;

			}
			break;

			//case CUI::SETTINGPANEL::SETTING_ESCGOBLIN:
			//{
			//	CGameObject* pSettingPanel = { nullptr };
			//	pDesc.fX = g_iWinSizeX - g_iWinSizeX / 2.8f;
			//	pDesc.fY = g_iWinSizeY / 4.2f;
			//	pDesc.fSizeX = 124.f;
			//	pDesc.fSizeY = 108.f;
			//	pDesc.eSettingPanelKind = CUI::SETTINGPANEL::SETTING_ESCGOBLIN;
			//
			//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_ESC_Goblin"), pDesc.iCurLevelID, _strLayerTag, &pSettingPanel, &pDesc)))
			//		return E_FAIL;
			//
			//}
			//break;

			
			}

		}
	}


	//CUI::UIOBJDESC pHeartDesc = {};
	//pHeartDesc.iCurLevelID = LEVEL_GAMEPLAY;
	//pHeartDesc.fX = g_iWinSizeX /2.f;
	//pHeartDesc.fY = g_iWinSizeY /2.f + 10.f;
	//pHeartDesc.fSizeX = 128.f;
	//pHeartDesc.fSizeY = 128.f;
	//
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Interaction_Heart"), pDesc.iCurLevelID, _strLayerTag, &pHeartDesc)))
	//	return E_FAIL;
	

#pragma region 로고씬



#pragma endregion 로고씬


	pDesc.fX = g_iWinSizeX / 2.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 6.f;
	pDesc.fSizeX = 1208.f;
	pDesc.fSizeY = 268.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Dialogue"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& _strLayerTag, CGameObject** _ppout)
{
	//CBeetle::MONSTER_DESC Monster_Desc;
	//Monster_Desc.iCurLevelID = LEVEL_GAMEPLAY;

	//Monster_Desc.tTransform3DDesc.vPosition = _float3(10.0f, 1.0f, 10.0f);
	//Monster_Desc.tTransform3DDesc.vScaling = _float3(1.f, 1.f, 1.f);

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Beetle"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
		return E_FAIL;*/

	CBarfBug::MONSTER_DESC Monster_Desc;
	Monster_Desc.iCurLevelID = LEVEL_GAMEPLAY;

	Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(-10.0f, 0.35f, -19.0f);
	Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_BarfBug"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
		return E_FAIL;

	//Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(10.0f, 0.35f, -19.0f);
	//Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Beetle"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
	//	return E_FAIL;
	//
	//Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(8.0f, 0.35f, -19.0f);
	//Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Goblin"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
	//	return E_FAIL;

	Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(-8.0f, 0.35f, -19.0f);
	Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
		return E_FAIL;

	/*Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.35f, -15.0f);
	Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Popuff"), LEVEL_GAMEPLAY, _strLayerTag, &Monster_Desc)))
		return E_FAIL;*/

	//CButterGrump::MONSTER_DESC Boss_Desc;
	//Boss_Desc.iCurLevelID = LEVEL_GAMEPLAY;

	//Boss_Desc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 20.35f, 40.0f);
	//Boss_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ButterGrump"), LEVEL_GAMEPLAY, _strLayerTag, &Boss_Desc)))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Create_Arm()
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
		return;
	_vector vPlayerLook = pPlayer->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);

	CCameraArm::CAMERA_ARM_DESC Desc{};

	XMStoreFloat3(&Desc.vArm, -vPlayerLook);
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	Desc.vRotation = { XMConvertToRadians(-30.f), XMConvertToRadians(0.f), 0.f };
	Desc.fLength = 7.f;
	Desc.wszArmTag = TEXT("Player_Arm");
	Desc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
	
	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);

	CCamera_Target* pTarget = dynamic_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));

	pTarget->Add_CurArm(pArm);
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
