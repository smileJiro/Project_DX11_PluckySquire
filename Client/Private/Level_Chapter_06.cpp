#include "stdafx.h"
#include "Level_Chapter_06.h"

#include "GameInstance.h"
#include "Pooling_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_CutScene.h"
#include "Camera_2D.h"
#include "Section_Manager.h"
#include "Trigger_Manager.h"
#include "PlayerData_Manager.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"

#include "CubeMap.h"
#include "MainTable.h"
#include "Player.h"
#include "Beetle.h"
#include "BirdMonster.h"
#include "Projectile_BirdMonster.h"
#include "Spear_Soldier.h"
#include "CrossBow_Soldier.h"
#include "Bomb_Soldier.h"
#include "Soldier_Spear.h"
#include "Soldier_Shield.h"
#include "Soldier_CrossBow.h"
#include "CrossBow_Arrow.h"
#include "SketchSpace_Alien.h"
#include "SketchSpace_SpikeBall.h"
#include "SketchSpace_UFO.h"
#include "Projectile_SketchSpace_UFO.h"
#include "Bomb.h"
#include "SlipperyObject.h"
#include "LightningBolt.h"
#include "ButterGrump.h"
#include "Rubboink_Tiny.h"
#include "RabbitLunch.h"
#include "TiltSwapPusher.h"
#include "MudPit.h"
#include "Book.h"

//DEFENDER
#include "DefenderPlayer.h"
#include "Minigame_Defender.h"
#include "DefenderSpawner.h"
#include "DefenderSmShip.h"
#include "DefenderPlayerProjectile.h"

#include "RayShape.h"
#include "CarriableObject.h"
#include "DraggableObject.h"
#include "Bulb.h"


#include "2DMapObject.h"
#include "3DMapObject.h"
#include "FallingRock.h"
#include "Spawner.h"
#include "CollapseBlock.h"


//#include "UI.h"
#include "UI_Manager.h"
#include "Dialog_Manager.h"
#include "NPC_Manager.h"
#include "SettingPanelBG.h"
#include "SettingPanel.h"
#include "ESC_HeartPoint.h"
#include "ShopItemBG.h"
#include "MapObjectFactory.h"
#include "Shop_Manager.h"
#include "ShopPanel_New.h"
#include "NPC.h"
#include "Loader.h"

// FatherGame
#include "FatherGame.h"
#include "Candle.h"
#include "CandleGame.h"
#include "ZetPack_Child.h"


CLevel_Chapter_06::CLevel_Chapter_06(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:
	m_eLevelID(LEVEL_CHAPTER_6)
	, CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Chapter_06::Initialize(LEVEL_ID _eLevelID)
{
	m_eLevelID = _eLevelID;

	if (FAILED(CSection_Manager::GetInstance()->Level_Enter(_eLevelID)))
	{
		MSG_BOX(" Failed CSection_Manager Level_Enter(Level_Chapter_06::Initialize)");
		assert(nullptr);
	}


	if (FAILED(Ready_Lights()))
	{
		MSG_BOX(" Failed Ready_Lights (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	CGameObject* pCameraTarget = nullptr;
	if (FAILED(Ready_CubeMap(TEXT("Layer_CubeMap"))))
	{
		MSG_BOX(" Failed Ready_CubeMap (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_MainTable(TEXT("Layer_MainTable"))))
	{
		MSG_BOX(" Failed Ready_Layer_MainTable (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Book(TEXT("Layer_Terrain"))))
	{
		MSG_BOX(" Failed Ready_Layer_Book (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &pCameraTarget)))
	{
		MSG_BOX(" Failed Ready_Layer_Player (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"), pCameraTarget)))
	{
		MSG_BOX(" Failed Ready_Layer_Camera (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Monster()))
	{
		MSG_BOX(" Failed Ready_Layer_Monster (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Monster_Projectile(TEXT("Layer_Monster_Projectile"))))
	{
		MSG_BOX(" Failed Ready_Layer_Monster_Projectile (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	{
		MSG_BOX(" Failed Ready_Layer_UI (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Item(TEXT("Layer_Item"))))
	{
		MSG_BOX(" Failed Ready_Layer_Item (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
	{
		MSG_BOX(" Failed Ready_Layer_NPC (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}

	if (FAILED(Ready_Layer_Effects(TEXT("Layer_Effect"))))
	{
		MSG_BOX(" Failed Ready_Layer_Effects (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Effects2D(TEXT("Layer_Effect2D"))))
	{
		MSG_BOX(" Failed Ready_Layer_Effects2D (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Map()))
	{
		MSG_BOX(" Failed Ready_Layer_Map (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}
	if (FAILED(Ready_Layer_Slippery()))
	{
		MSG_BOX(" Failed Ready_Layer_Slippery (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}

	if (FAILED(Ready_Layer_Defender()))
	{
		MSG_BOX(" Failed Ready_Layer_Defender (Level_Chapter_06::Initialize)");
		assert(nullptr);
	}


	/* Collision Check Matrix */
	// 그룹필터 추가 >> 중복해서 넣어도 돼 내부적으로 걸러줌 알아서 
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::MONSTER);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::MONSTER_PROJECTILE);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::TRIGGER_OBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::MAPOBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER_PROJECTILE, OBJECT_GROUP::MAPOBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::INTERACTION_OBEJCT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::PLAYER_PROJECTILE);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::BLOCKER);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::BLOCKER_JUMPPASS); // 플레이어점프시 꺼지는
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::FALLINGROCK_BASIC);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::SLIPPERY);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::NPC_EVENT); // ZetPack
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::GIMMICK_OBJECT); // 
	//m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::PORTAL);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER_TRIGGER, OBJECT_GROUP::INTERACTION_OBEJCT); //3 8

	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MAPOBJECT, OBJECT_GROUP::PLAYER_PROJECTILE);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::TRIGGER_OBJECT, OBJECT_GROUP::GIMMICK_OBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::SLIPPERY, OBJECT_GROUP::MAPOBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::SLIPPERY, OBJECT_GROUP::BLOCKER);

	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::NPC_EVENT, OBJECT_GROUP::INTERACTION_OBEJCT); //3 8

	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::MAPOBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER_PROJECTILE);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::INTERACTION_OBEJCT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::BLOCKER);
	

	
	/* 돌덩이 */
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER_PROJECTILE, OBJECT_GROUP::BLOCKER);


	// 그룹필터 제거
	// 삭제도 중복해서 해도 돼 >> 내부적으로 걸러줌. >> 가독성이 및 사용감이 더 중요해서 이렇게 처리했음
	//m_pGameInstance->Erase_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER);
	//m_pGameInstance->Erase_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER);


	/* Blur RenderGroupOn */
	m_pGameInstance->Set_Active_RenderGroup_New(RENDERGROUP::RG_3D, PR3D_POSTPROCESSING, true);

	// Trigger
	CTrigger_Manager::GetInstance()->Load_Trigger(LEVEL_STATIC, (LEVEL_ID)m_eLevelID, TEXT("../Bin/DataFiles/Trigger/Chapter6_Trigger.json"));
	CTrigger_Manager::GetInstance()->Load_TriggerEvents(TEXT("../Bin/DataFiles/Trigger/Chapter6_Trigger_Events.json"));

	// BGM 시작
	m_pGameInstance->Start_BGM(TEXT("LCD_MUS_C02_C2FIELDMUSIC_LOOP_Stem_Base"), 20.f);



	//CTrigger_Manager::GetInstance()->Resister_TriggerEvent(TEXT("Chapter2_Intro"),
	//	50);

	/* Set Shader PlayerHideColor */
	m_pGameInstance->Set_PlayerHideColor(_float3(0.8f, 0.8f, 0.8f), true);



#pragma region TestCode
	
	

	


	/* Test FatherGame Progress */
	if (FAILED(CFatherGame::GetInstance()->Start_Game(m_pDevice, m_pContext)))
		return E_FAIL;
#pragma endregion // TestCode
	
	/* Chapter6 Intro Trigger 동적 생성 임시 코드*/
	Create_IntroTrigger();

	return S_OK;
}

void CLevel_Chapter_06::Update(_float _fTimeDelta)
{
	CFatherGame::GetInstance()->Update();
	Uimgr->UI_Update();

	// 피직스 업데이트 
	m_pGameInstance->Physx_Update(_fTimeDelta);


	/*ImGuiIO& IO = ImGui::GetIO(); (void)IO;*/

	if (KEY_DOWN(KEY::NUM6))
	{

		// DXGI 팩토리 생성
		IDXGIFactory4* pFactory;
		CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory);

		// 기본 GPU 어댑터 가져오기
		IDXGIAdapter3* pAdapter;
		pFactory->EnumAdapters1(0, (IDXGIAdapter1**)&pAdapter);

		// VRAM 사용c량 쿼리
		DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
		pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo);

		// 결과 출력 (MB 단위)
		SIZE_T currentUsageMB = memoryInfo.CurrentUsage / (1024 * 1024); // 현재 사용량
		SIZE_T availableMB = memoryInfo.AvailableForReservation / (1024 * 1024); // 예약 가능량

		int a = 0;
	}

	if (KEY_DOWN(KEY::I))
	{
		CTrigger_Manager::GetInstance()->Register_TriggerEvent(L"Next_Chapter_Event", 0);
	}
	// Change Camera Free  Or Target
	if (KEY_DOWN(KEY::C)) {
		_uint iCurCameraType = CCamera_Manager::GetInstance()->Get_CameraType();
		iCurCameraType ^= 1;
		CCamera_Manager::GetInstance()->Change_CameraType(iCurCameraType);
	}

	if (KEY_DOWN(KEY::NUM1))
	{
		CUI_Manager* pUIManager = CUI_Manager::GetInstance();

		if (nullptr == pUIManager)
			assert(nullptr);

		if (true == pUIManager->Get_StampHave(0) &&
			true == pUIManager->Get_StampHave(1))
		{
			CUI_Manager::STAMP eStamp;
			eStamp = pUIManager->Get_StampIndex();

			if (eStamp == CUI_Manager::STAMP_BOMB)
			{
				pUIManager->Set_StampIndex(CUI_Manager::STAMP_STOP);
			}
			else if (eStamp == CUI_Manager::STAMP_STOP)
			{
				pUIManager->Set_StampIndex(CUI_Manager::STAMP_BOMB);
			}
		}

	}

#ifdef _DEBUG
	if (KEY_DOWN(KEY::P))
		CCamera_Manager::GetInstance()->Start_ZoomIn();


	if (KEY_DOWN(KEY::O))
		CCamera_Manager::GetInstance()->Start_ZoomOut();
#endif // _DEBUG



	static _float3 vOutPos = {};
	if (IS_IMPORT_IMGUI)
	{
		ImGui::Begin("PickingPos");
		ImGui::InputFloat3("PickingPos##Pick", &vOutPos.x, "%.2f");
		ImGui::End();
	}
	if (MOUSE_DOWN(MOUSE_KEY::MB))
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		_vector vMousePos = XMVectorSet((_float)pt.x, (_float)pt.y, 0.f, 1.f);

		_uint		iNumViewports = { 1 };
		D3D11_VIEWPORT		ViewportDesc{};

		m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

		// 마우스 -> 투영
		vMousePos = XMVectorSet(pt.x / (ViewportDesc.Width * 0.5f) - 1.f,
			pt.y / -(ViewportDesc.Height * 0.5f) + 1.f,
			0.f,
			1.f);

		// 투영 -> 뷰 스페이스
		_matrix matProj = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_PROJ);
		vMousePos = XMVector3TransformCoord(vMousePos, matProj);

		_vector vRayPos, vRayDir;

		vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		vRayDir = vMousePos - vRayPos;

		_matrix matView = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW);
		vRayPos = XMVector3TransformCoord(vRayPos, matView);
		vRayDir = XMVector3TransformNormal(vRayDir, matView);
		vRayDir = XMVectorSetW(vRayDir, 0.f);
		vRayDir = XMVector3Normalize(vRayDir);

		_float3 vOrigin = {};
		_float3 vRayDirection = {};
		XMStoreFloat3(&vOrigin, vRayPos);
		XMStoreFloat3(&vRayDirection, vRayDir);


		CActorObject* pActorObject = {};
		_bool isResult = false;

		isResult = m_pGameInstance->RayCast_Nearest(vOrigin, vRayDirection, 1000.f, &vOutPos, &pActorObject);
		int a = 0;
	}

	if (KEY_DOWN(KEY::F5))
	{
		_float3 vPos = { 5.f, 0.35f, -3.f };
		CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Bomb"), COORDINATE_3D, &vPos);
	}

	if (KEY_DOWN(KEY::L)) {
		CCamera_Manager::GetInstance()->Set_NextCutSceneData(TEXT("Chapter6_Intro"));
		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE, true, 0.8f);
	}

	if (KEY_DOWN(KEY::NUMPAD5))
	{
		CGameObject* pObject;

		wstring strLayerTag = TEXT("Layer_Monster");

		CSpear_Soldier::SPEARSOLDIER_DESC Spear_Soldier2D_Desc;
		Spear_Soldier2D_Desc.iCurLevelID = m_eLevelID;
		Spear_Soldier2D_Desc.eStartCoord = COORDINATE_2D;
		Spear_Soldier2D_Desc.tTransform2DDesc.vInitialPosition = _float3(1000.0f, -64.f, 0.f);
		//Spear_Soldier2D_Desc.tTransform2DDesc.vInitialPosition = _float3(690.0f, -64.f, 0.f);
		Spear_Soldier2D_Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
		Spear_Soldier2D_Desc.isC6BossMode = true;
		Spear_Soldier2D_Desc.eDirection = F_DIRECTION::LEFT;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), m_eLevelID, strLayerTag, &pObject, &Spear_Soldier2D_Desc)))
			return;

		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pObject);

		//Spear_Soldier2D_Desc.tTransform2DDesc.vInitialPosition = _float3(1050.0f, -242.f, 0.f);
		//Spear_Soldier2D_Desc.isC6BossMode = true;
		//Spear_Soldier2D_Desc.eDirection = F_DIRECTION::LEFT;

		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), m_eLevelID, strLayerTag, &pObject, &Spear_Soldier2D_Desc)))
		//	return;

		//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pObject);


		//Spear_Soldier2D_Desc.tTransform2DDesc.vInitialPosition = _float3(-1000.0f, -87.f, 0.f);
		//Spear_Soldier2D_Desc.isC6BossMode = true;
		//Spear_Soldier2D_Desc.eDirection = F_DIRECTION::RIGHT;

		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), m_eLevelID, strLayerTag, &pObject, &Spear_Soldier2D_Desc)))
		//	return;

		//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pObject);

		Spear_Soldier2D_Desc.tTransform2DDesc.vInitialPosition = _float3(-1200.0f, -242.f, 0.f);
		Spear_Soldier2D_Desc.isC6BossMode = true;
		Spear_Soldier2D_Desc.eDirection = F_DIRECTION::RIGHT;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), m_eLevelID, strLayerTag, &pObject, &Spear_Soldier2D_Desc)))
			return;

		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pObject);
	}
}

HRESULT CLevel_Chapter_06::Render()
{
#ifdef _DEBUG
	m_pGameInstance->Render_FPS(TEXT("Timer_120"));
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Lights()
{
#ifdef _DEBUG
	m_pGameInstance->Load_Lights(TEXT("../Bin/DataFiles/DirectLights/DirectionalTest2.json"));
#elif NDEBUG
	m_pGameInstance->Load_Lights(TEXT("../Bin/DataFiles/DirectLights/Chapter6_2.json"));
#endif // _DEBUG

	m_pGameInstance->Load_IBL(TEXT("../Bin/DataFiles/IBL/Chapter6.json"));

	//CONST_LIGHT LightDesc{};

	///* 방향성광원*/
	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.vDirection = { 0.0f, -1.0f, -1.0f };
	//LightDesc.vRadiance = _float3(1.0f, 1.0f, 1.0f);
	//LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.0f);
	//LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
	//LightDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	////LightDesc.isShadow = true;
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::DIRECTOINAL)))
	//	return E_FAIL;

	///* 방향성광원*/
	//ZeroMemory(&LightDesc, sizeof LightDesc);

	//LightDesc.vDirection = { -1.0f, -1.0f, -1.0f };
	//LightDesc.vRadiance = _float3(1.0f, 1.0f, 1.0f);
	//LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.0f);
	//LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
	//LightDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	//LightDesc.isShadow = true;
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::DIRECTOINAL)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_CubeMap(const _wstring& _strLayerTag)
{
	CGameObject* pCubeMap = nullptr;
	CCubeMap::CUBEMAP_DESC Desc;
	Desc.iCurLevelID = m_eLevelID;
	Desc.iRenderGroupID = RG_3D;
	Desc.iPriorityID = PR3D_PRIORITY;
	Desc.strBRDFPrototypeTag = TEXT("Prototype_Component_Texture_BRDF_Shilick");
	Desc.strCubeMapPrototypeTag = TEXT("Prototype_Component_Texture_Chapter6Env");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
		m_eLevelID, _strLayerTag, &pCubeMap, &Desc)))
		return E_FAIL;

	m_pGameInstance->Set_CubeMap(static_cast<CCubeMap*>(pCubeMap));

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_MainTable(const _wstring& _strLayerTag)
{
	CMainTable::MAINTABLE_DESC Desc;
	Desc.iCurLevelID = m_eLevelID;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MainTable"),
		m_eLevelID, _strLayerTag, &Desc)))
		return E_FAIL;

	Desc = {};
	Desc.isOverride = true;
	Desc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 17.25f, 47.f);
	Desc.vHalfExtents = { 40.f, 1.f, 12.f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MainTable"),
		m_eLevelID, _strLayerTag, &Desc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Map()
{
	LEVEL_ID eLevelID = (LEVEL_ID)m_eLevelID;

	switch (eLevelID)
	{
	case Client::LEVEL_CHAPTER_2:
		if (FAILED(Map_Object_Create(L"Chapter_02_Play_Desk.mchc")))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_4:
		if (FAILED(Map_Object_Create(L"Chapter_04_Play_Desk.mchc")))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_6:
		if (FAILED(Map_Object_Create(L"Chapter_06_Play_Desk.mchc")))
			return E_FAIL;
		break;
	case Client::LEVEL_CHAPTER_TEST:
		if (FAILED(Map_Object_Create(L"Chapter_04_Default_Desk.mchc")))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(m_eLevelID, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
		return E_FAIL;

	CGameObject* pCamera = nullptr;

	// Free Camera
	CCamera_Free::CAMERA_FREE_DESC Desc{};
	Desc.iCurLevelID = m_eLevelID;

	Desc.fMouseSensor = 0.1f;
	Desc.fFovy = XMConvertToRadians(60.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.eZoomLevel = CCamera::LEVEL_6;
	Desc.iCameraType = CCamera_Manager::FREE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		m_eLevelID, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::FREE, static_cast<CCamera*>(pCamera));

	// Target Camera
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};
	TargetDesc.iCurLevelID = m_eLevelID;

	TargetDesc.fSmoothSpeed = 7.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;
	TargetDesc.vAtOffset = _float3(0.0f, 1.5f, 0.0f);
	TargetDesc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE::COORDINATE_3D);

	TargetDesc.fFovy = XMConvertToRadians(60.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);
	TargetDesc.eZoomLevel = CCamera::LEVEL_5;
	TargetDesc.iCameraType = CCamera_Manager::TARGET;

	XMStoreFloat3(&TargetDesc.vArm, XMVector3Normalize(XMVectorSet(0.f, 0.5776063799858093f, -0.87f, 0.f)));
	TargetDesc.fLength = { 12.5f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Target"),
		m_eLevelID, _strLayerTag, &pCamera, &TargetDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::TARGET, static_cast<CCamera*>(pCamera));

	// CutScene Camera
	CCamera_CutScene::CAMERA_DESC CutSceneDesc{};
	CutSceneDesc.iCurLevelID = m_eLevelID;

	CutSceneDesc.fFovy = XMConvertToRadians(60.f);
	CutSceneDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CutSceneDesc.vEye = _float3(0.f, 10.f, -7.f);
	CutSceneDesc.vAt = _float3(0.f, 0.f, 0.f);
	CutSceneDesc.eZoomLevel = CCamera::LEVEL_6;
	CutSceneDesc.iCameraType = CCamera_Manager::CUTSCENE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_CutScene"),
		m_eLevelID, _strLayerTag, &pCamera, &CutSceneDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::CUTSCENE, static_cast<CCamera*>(pCamera));

	// 2D Camera
	CCamera_2D::CAMERA_2D_DESC Target2DDesc{};
	Target2DDesc.iCurLevelID = m_eLevelID;

	Target2DDesc.fSmoothSpeed = 5.f;
	Target2DDesc.eCameraMode = CCamera_2D::DEFAULT;
	Target2DDesc.vAtOffset = _float3(0.0f, 0.f, 0.0f);
	Target2DDesc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE::COORDINATE_2D);

	Target2DDesc.fFovy = XMConvertToRadians(60.f);
	Target2DDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Target2DDesc.vEye = _float3(0.f, 10.f, -7.f);
	Target2DDesc.vAt = _float3(0.f, 0.f, 0.f);
	Target2DDesc.eZoomLevel = CCamera::LEVEL_6;
	Target2DDesc.iCameraType = CCamera_Manager::TARGET_2D;

	XMStoreFloat3(&Target2DDesc.vArm, XMVector3Normalize(XMVectorSet(0.f, 0.981f, -0.191f, 0.f)));
	Target2DDesc.fLength = { 12.5f };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_2D"),
		m_eLevelID, _strLayerTag, &pCamera, &Target2DDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::TARGET_2D, static_cast<CCamera*>(pCamera));

	// Load CutSceneData, ArmData
	CCamera_Manager::GetInstance()->Load_CutSceneData(m_eLevelID);
	CCamera_Manager::GetInstance()->Load_ArmData(m_eLevelID);

	COORDINATE eCoord = pPlayer->Get_CurCoord();

	if (COORDINATE_2D == eCoord)
	{
		auto pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D);
	/*	pCamera->Enter_Section(pPlayer->Get_Include_Section_Name());
		pCamera->Switch_CameraView(nullptr);*/
	}
	CCamera_Manager::GetInstance()->Start_FadeIn(3.f);

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CPlayer::CHARACTER_DESC Desc;
	Desc.iCurLevelID = m_eLevelID;
	Desc.eStartCoord = COORDINATE_2D;
	Desc.tTransform2DDesc.vInitialPosition = { 0.f, 400.f, 0.f };   // TODO ::임시 위치

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TestPlayer"), m_eLevelID, _strLayerTag, _ppOut, &Desc)))
		return E_FAIL;


	CPlayer* pPlayer = { nullptr };
	pPlayer = dynamic_cast<CPlayer*>(*_ppOut);

	if (nullptr == Uimgr->Get_Player())
		CUI_Manager::GetInstance()->Set_Player(pPlayer);

	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(pPlayer, SECTION_2D_PLAYMAP_OBJECT);
	pPlayer->Set_Mode(CPlayer::PLAYER_MODE_SWORD);

	CPlayerData_Manager::GetInstance()->Set_CurrentPlayer(PLAYABLE_ID::NORMAL);



	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Defender()
{
	CSection_Manager* pSectionMgr = CSection_Manager::GetInstance();

	CMiniGame_Defender::DEFENDER_DESC tDesc = {};
	tDesc.iCurLevelID = m_eLevelID;
	tDesc.tTransform2DDesc.vInitialPosition = { -2020.f, -80.f, 0.f };   // TODO ::임시 위치
	CMiniGame_Defender* pTower = static_cast<CMiniGame_Defender*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_eLevelID, TEXT("Prototype_GameObject_Minigame_Defender"), &tDesc));;
	m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Layer_Defender"), pTower);
	pSectionMgr->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_04"), pTower, SECTION_2D_PLAYMAP_OBJECT);

	CDefenderPlayer::DEFENDERPLAYER_DESC tDeffenderPlayerDesc = {};
	tDeffenderPlayerDesc.iCurLevelID = m_eLevelID;
	tDeffenderPlayerDesc.pMinigame = pTower;
	CDefenderPlayer* pPlayer = static_cast<CDefenderPlayer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_eLevelID, TEXT("Prototype_GameObject_DefenderPlayer"), &tDeffenderPlayerDesc));
	m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Layer_Defender"), pPlayer);
	pSectionMgr->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_04"), pPlayer, SECTION_2D_PLAYMAP_OBJECT);
	pPlayer->Set_Active(false);

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Book(const _wstring& _strLayerTag)
{
	//TODO :: SAMPLE
	CBook::BOOK_DESC Desc = {};
	Desc.iCurLevelID = m_eLevelID;

	CGameObject* pBook = nullptr;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Book"),
		m_eLevelID, L"Layer_Book", &pBook, &Desc)))
		return E_FAIL;

	Uimgr->Set_Book(static_cast<CBook*>(pBook));


	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_UI(const _wstring& _strLayerTag)
{
	CUI::UIOBJDESC pDesc = {};
	CUI::UIOBJDESC pDescs[CUI::SETTINGPANEL::SETTING_END] = {};
	CUI::UIOBJDESC pShopDescs[CUI::SHOPPANEL::SHOP_END] = {};
	pDesc.iCurLevelID = m_eLevelID;
	pDesc.strLayerTag = _strLayerTag;

#pragma region PickBubble UI
	pDesc.fX = g_iWinSizeX - g_iWinSizeX / 12.f;
	pDesc.fY = g_iWinSizeY / 10.f;
	pDesc.fSizeX = 182.f;
	pDesc.fSizeY = 100.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_UIObejct_PickBubble"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;
#pragma endregion PickBubble UI

#pragma region STAMP UI
	pDesc.fX = g_iWinSizeX / 20.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 6.f;

	// 원래 크기
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 111.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_StopStamp"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;


	pDesc.fX = g_iWinSizeX / 7.5f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 6.f;
	pDesc.fSizeX = 99.f;
	pDesc.fSizeY = 153.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_BombStamp"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;


	pDesc.fX = g_iWinSizeX / 10.8f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 8.f;
	pDesc.fSizeX = 42.f;
	pDesc.fSizeY = 27.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_ArrowForStamp"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;



	// -------------------------------------- //
	pDesc.fX = g_iWinSizeX - g_iWinSizeX / 10.8f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 20.f;
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 72.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_StampKey_Q"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;


	pDesc.fX = g_iWinSizeX / 18.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 20.f;
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 72.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_StampKey_1"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;
#pragma endregion STAMP UI

#pragma region InterAction UI
	pDesc.fX = g_iWinSizeX / 2.f - g_iWinSizeX / 20.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 18.f;
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 72.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Interaction_Book"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;

	CGameObject* pInteractionE;

	pDesc.fSizeX = 360.f / 3.f;
	pDesc.fSizeY = 149.f / 3.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_Interaction_E"), pDesc.iCurLevelID, _strLayerTag, &pInteractionE, &pDesc)))
		return E_FAIL;

	Uimgr->Set_InterActionE(static_cast<CInteraction_E*>(pInteractionE));




#pragma endregion InterAction UI

	//_uint ShopPanelUICount = { CUI::SHOPPANEL::SHOP_END };
	//
	//if (ShopPanelUICount != CUI_Manager::GetInstance()->Get_ShopPanels().size())
	//{
	//	
	//}

	CGameObject* pShop;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopPannel"), pDesc.iCurLevelID, _strLayerTag, &pShop, &pDesc)))
		return E_FAIL;

	CShop_Manager::GetInstance()->Set_Shop(static_cast<CShopPanel_New*>(pShop));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pDesc.iCurLevelID, TEXT("Prototype_GameObject_ShopPannelRenderFont"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;


#pragma region SettingPanel UI

	_uint SettingPanelUICount = CUI::SETTINGPANEL::SETTING_END;

	if (SettingPanelUICount != CUI_Manager::GetInstance()->Get_SettingPanels().size())
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_ParentSettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BG].iCurLevelID, _strLayerTag, &pDesc)))
			return E_FAIL;
	}

	for (_uint i = 0; i < CUI::SETTINGPANEL::SETTING_END; ++i)
	{
		pDescs[i].iCurLevelID = m_eLevelID;
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
				//Safe_Release(pSettingPanel);

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
				//Safe_Release(pSettingPanel);
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
				//Safe_Release(pSettingPanel);

				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fX = g_iWinSizeX / 14.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fSizeX = 72;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].fSizeY = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].iTextureCount = CUI::SETTINGPANEL::SETTING_BACKESC;
				pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_BACKESC;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_BACKESC])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_BACKESC].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				//Safe_Release(pSettingPanel);
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
				//Safe_Release(pSettingPanel);
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
				//Safe_Release(pSettingPanel);


				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fX = g_iWinSizeX - g_iWinSizeX / 10.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fY = g_iWinSizeY - g_iWinSizeY / 18.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fSizeX = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].fSizeY = 72.f;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].iTextureCount = CUI::SETTINGPANEL::SETTING_ESCENTER;
				pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].eSettingPanelKind = CUI::SETTINGPANEL::SETTING_ESCENTER;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_SettingPanel"), pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].iCurLevelID, _strLayerTag, &pSettingPanel, &pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER])))
					return E_FAIL;

				CUI_Manager::GetInstance()->Emplace_SettingPanels((_uint)pDescs[CUI::SETTINGPANEL::SETTING_ESCENTER].iTextureCount, static_cast<CSettingPanelBG*>(pSettingPanel));
				//Safe_Release(pSettingPanel);
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
				//Safe_Release(pSettingPanel);

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

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_ESCHeartPoint"), pDesc.iCurLevelID, _strLayerTag, &pSettingPanel, &pDesc)))
					return E_FAIL;

			}
			break;

			case CUI::SETTINGPANEL::SETTING_ESCGOBLIN:
			{
				CGameObject* pSettingPanel = { nullptr };
				pDesc.fX = g_iWinSizeX - g_iWinSizeX / 2.8f;
				pDesc.fY = g_iWinSizeY / 4.2f;
				pDesc.fSizeX = 124.f;
				pDesc.fSizeY = 108.f;
				pDesc.eSettingPanelKind = CUI::SETTINGPANEL::SETTING_ESCGOBLIN;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_UIObejct_ESC_Goblin"), pDesc.iCurLevelID, _strLayerTag, &pSettingPanel, &pDesc)))
					return E_FAIL;

			}
			break;


			}

		}
	}

	pDesc.fX = 0.f; // 전체 사이즈 / RTSIZE 끝으로 변경
	pDesc.fY = 0.f;// 전체 사이즈 / RTSIZE 끝으로 변경
	pDesc.fSizeX = 2328.f * 0.8f;
	pDesc.fSizeY = 504.f * 0.8f;
	CGameObject* pDialogueObject;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Dialogue"), pDesc.iCurLevelID, _strLayerTag, &pDialogueObject, &pDesc)))
		return E_FAIL;

	CDialog_Manager::GetInstance()->Set_Dialog(static_cast<CDialog*>(pDialogueObject));



	pDesc.fX = DEFAULT_SIZE_BOOK2D_X / RATIO_BOOK2D_X;
	pDesc.fY = DEFAULT_SIZE_BOOK2D_Y / RATIO_BOOK2D_Y;
	pDesc.fSizeX = 512.f * 0.8f;
	pDesc.fSizeY = 512.f * 0.8f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Dialogue_Portrait"), pDesc.iCurLevelID, _strLayerTag, &pDialogueObject, &pDesc)))
		return E_FAIL;

	CDialog_Manager::GetInstance()->Get_Dialog()->Set_Portrait(static_cast<CPortrait*>(pDialogueObject));

	/* 테스트 용 */
	/* (0.0) ~ MAXSIZE 기준으로 fX, fY 를 설정하여야합니다. */
	/* 해당 부분은 객체가 투명하며 오직 글자 렌더만 사용합니다. */
	//CGameObject* pPrintFloorWord = { nullptr };
	//pDesc.fX = 996.f;
	//pDesc.fY = -30.f;
	//pDesc.fSizeX = 0.f;
	//pDesc.fSizeY = 0.f;
	//
	//
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_FloorWord"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;


	CGameObject* pHeartObject;

	pDesc.fSizeX = 256.f / 4.f;
	pDesc.fSizeY = 256.f / 4.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Interaction_Heart"), pDesc.iCurLevelID, _strLayerTag, &pHeartObject, &pDesc)))
		return E_FAIL;

	Uimgr->Set_InterActionHeart(static_cast<CInteraction_Heart*>(pHeartObject));


	CGameObject* pGameObject;
	
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Narration"), pDesc.iCurLevelID, _strLayerTag, &pGameObject, &pDesc)))
		return E_FAIL;
	
	Uimgr->Set_Narration(static_cast<CNarration*>(pGameObject));

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Item(const _wstring& _strLayerTag)
{
	// Test(PlayerItem: Glove, Stamp)
	//CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)m_eLevelID, TEXT("Flipping_Glove"), _float3(59.936f, 6.273f, -19.097f));
	//CPlayerData_Manager::GetInstance()->Spawn_Bulb(LEVEL_STATIC, (LEVEL_ID)m_eLevelID);



	//3D Bulb
	Pooling_DESC Pooling_Desc;
	Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
	Pooling_Desc.strLayerTag = _strLayerTag;
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Bulb");
	Pooling_Desc.eSection2DRenderGroup = SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_OBJECT;

	CBulb::BULB_DESC* pBulbDesc = new CBulb::BULB_DESC;
	pBulbDesc->eStartCoord = COORDINATE_3D;
	pBulbDesc->iCurLevelID = m_eLevelID;
	pBulbDesc->tTransform2DDesc.vInitialScaling = { 1.f,1.f,1.f };
	pBulbDesc->iObjectGroupID = OBJECT_GROUP::TRIGGER_OBJECT;

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Bulb"), Pooling_Desc, pBulbDesc);

	//2D Bulb
	CBulb::BULB_DESC* p2DBulbDesc = new CBulb::BULB_DESC;
	p2DBulbDesc->eStartCoord = COORDINATE_2D;
	p2DBulbDesc->iCurLevelID = m_eLevelID;
	p2DBulbDesc->tTransform2DDesc.vInitialScaling = { 256.f,256.f,1.f };
	p2DBulbDesc->iObjectGroupID = OBJECT_GROUP::TRIGGER_OBJECT;

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_2DBulb"), Pooling_Desc, p2DBulbDesc);

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_NPC(const _wstring& _strLayerTag)
{
	CNPC::NPC_DESC NPCDesc;
	CGameObject* pGameObject = { nullptr };
	NPCDesc.iCurLevelID = m_eLevelID;
	NPCDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	NPCDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	NPCDesc.iNumPartObjects = 3;
	NPCDesc.iMainIndex = 0;
	NPCDesc.iSubIndex = 0;
	NPCDesc.vPos = _float2{ 347.9f, 353.3f };
	//wsprintf(NPCDesc.strDialogueIndex, TEXT("Store_Dialog_01"));
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_NPC_OnlySocial"), NPCDesc.iCurLevelID, _strLayerTag, &pGameObject, &NPCDesc)))
		return E_FAIL;

	CNPC_Manager::GetInstance()->Set_OnlyNpc(static_cast<CNPC_OnlySocial*>(pGameObject));


	NPCDesc.iCurLevelID = m_eLevelID;
	NPCDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	NPCDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	NPCDesc.iNumPartObjects = 3;
	NPCDesc.iMainIndex = 0;
	NPCDesc.iSubIndex = 0;
	wsprintf(NPCDesc.strLocateSection, TEXT("Chapter5_P0102"));
	wsprintf(NPCDesc.strDialogueIndex, TEXT("Store_Dialog_01"));
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_StoreNPC"), NPCDesc.iCurLevelID, _strLayerTag, &NPCDesc)))
		return E_FAIL;

	return S_OK;

}

HRESULT CLevel_Chapter_06::Ready_Layer_Monster_2D()
{
	CGameObject* pObject = nullptr;

	const json* pJson = m_pGameInstance->Find_Json_InLevel(TEXT("Chapter6_Monsters_2D"), m_eLevelID);

	if (nullptr == pJson)
		return E_FAIL;

	if (pJson->contains("2D"))
	{
		_wstring strLayerTag = L"Layer_Monster";
		_wstring strSectionTag = L"";
		_wstring strMonsterTag = L"";

		for (auto Json : (*pJson)["2D"])
		{
			CMonster::MONSTER_DESC MonsterDesc2D = {};

			MonsterDesc2D.iCurLevelID = m_eLevelID;
			MonsterDesc2D.eStartCoord = COORDINATE_2D;

			if (Json.contains("Position"))
			{
				for (_int j = 0; j < 3; ++j)
				{
					*(((_float*)&MonsterDesc2D.tTransform2DDesc.vInitialPosition) + j) = Json["Position"][j];
				}
			}
			if (Json.contains("Scaling"))
			{
				for (_int j = 0; j < 3; ++j)
				{
					*(((_float*)&MonsterDesc2D.tTransform2DDesc.vInitialScaling) + j) = Json["Scaling"][j];
				}
			}
			if (Json.contains("LayerTag"))
			{
				strLayerTag = STRINGTOWSTRING(Json["LayerTag"]);
			}

			if (Json.contains("SectionTag"))
			{
				strSectionTag = STRINGTOWSTRING(Json["SectionTag"]);
			}
			else
				return E_FAIL;

			if (Json.contains("MonsterTag"))
			{
				strMonsterTag = STRINGTOWSTRING(Json["MonsterTag"]);
			}
			else
				return E_FAIL;

			if (Json.contains("IsStay"))
			{
				MonsterDesc2D.isStay = Json["IsStay"];
			}

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, strMonsterTag, m_eLevelID, strLayerTag, &pObject, &MonsterDesc2D)))
				return E_FAIL;
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strSectionTag, pObject);
		}
	}
	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Monster_3D()
{
	CGameObject* pObject = nullptr;

	const json* pJson = m_pGameInstance->Find_Json_InLevel(TEXT("Chapter6_Monsters_3D"), m_eLevelID);

	if (nullptr == pJson)
		return E_FAIL;
	if (pJson->contains("3D"))
	{
		_wstring strLayerTag = L"Layer_Monster";
		_wstring strMonsterTag = L"";

		for (auto Json : (*pJson)["3D"])
		{
			CMonster::MONSTER_DESC MonsterDesc3D = {};

			MonsterDesc3D.iCurLevelID = m_eLevelID;
			MonsterDesc3D.eStartCoord = COORDINATE_3D;

			if (Json.contains("Position"))
			{
				for (_int j = 0; j < 3; ++j)
				{
					*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialPosition) + j) = Json["Position"][j];
				}
			}
			if (Json.contains("Scaling"))
			{
				for (_int j = 0; j < 3; ++j)
				{
					*(((_float*)&MonsterDesc3D.tTransform3DDesc.vInitialScaling) + j) = Json["Scaling"][j];
				}
			}
			if (Json.contains("LayerTag"))
			{
				strLayerTag = STRINGTOWSTRING(Json["LayerTag"]);
			}

			if (Json.contains("MonsterTag"))
			{
				strMonsterTag = STRINGTOWSTRING(Json["MonsterTag"]);
			}
			else
				return E_FAIL;

			if (Json.contains("SneakMode"))
			{
				if (Json.contains("SneakWayPointIndex"))
				{
					MonsterDesc3D.eWayIndex = Json["SneakWayPointIndex"];
				}
				else
					return E_FAIL;
				MonsterDesc3D.isSneakMode = Json["SneakMode"];
			}

			if (Json.contains("IsStay"))
			{
				MonsterDesc3D.isStay = Json["IsStay"];
				if (Json.contains("vLook"))
				{
					for (_int j = 0; j < 3; ++j)
					{
						*(((_float*)&MonsterDesc3D.vLook) + j) = Json["vLook"][j];
					}
				}
			}

			if (Json.contains("IsIgnoreGround"))
			{
				MonsterDesc3D._isIgnoreGround = Json["IsIgnoreGround"];
			}

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, strMonsterTag, m_eLevelID, strLayerTag, &pObject, &MonsterDesc3D)))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Monster()
{
	CGameObject* pObject = nullptr;

	//if (FAILED(Ready_Layer_Monster_2D()))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Monster_3D()))
	//	return E_FAIL;

	//wstring strLayerTag = TEXT("Layer_Monster");

	//CSpear_Soldier::MONSTER_DESC Spear_Soldier2D_Desc;
	//Spear_Soldier2D_Desc.iCurLevelID = m_eLevelID;
	//Spear_Soldier2D_Desc.eStartCoord = COORDINATE_2D;
	////Spear_Soldier2D_Desc.tTransform2DDesc.vInitialPosition = _float3(1000.0f, -64.f, 0.f);
	//Spear_Soldier2D_Desc.tTransform2DDesc.vInitialPosition = _float3(690.0f, -64.f, 0.f);
	//Spear_Soldier2D_Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), m_eLevelID, strLayerTag, &pObject, &Spear_Soldier2D_Desc)))
	//	return E_FAIL;

	//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_P0708"), pObject);

	//CBirdMonster::MONSTER_DESC BirdMonster_Desc;
	//BirdMonster_Desc.iCurLevelID = m_eLevelID;
	//BirdMonster_Desc.eStartCoord = COORDINATE_3D;
	//BirdMonster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	//BirdMonster_Desc.tTransform3DDesc.vInitialPosition = _float3(-18.5f, 6.55f, 21.0f);
	//
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_BirdMonster"), m_eLevelID, _strLayerTag, &BirdMonster_Desc)))
	//	return E_FAIL;


	//CSpear_Soldier::MONSTER_DESC Spear_Soldier_Desc;
	//Spear_Soldier_Desc.iCurLevelID = m_eLevelID;
	//Spear_Soldier_Desc.eStartCoord = COORDINATE_3D;
	//Spear_Soldier_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	//Spear_Soldier_Desc.tTransform3DDesc.vInitialPosition = _float3(-5.5f, 0.35f, -23.0f);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Spear_Soldier"), m_eLevelID, _strLayerTag, &Spear_Soldier_Desc)))
	//	return E_FAIL;

	//CCrossBow_Soldier::MONSTER_DESC CrossBow_Soldier_Desc;
	//CrossBow_Soldier_Desc.iCurLevelID = m_eLevelID;
	//CrossBow_Soldier_Desc.eStartCoord = COORDINATE_3D;
	//CrossBow_Soldier_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	//CrossBow_Soldier_Desc.tTransform3DDesc.vInitialPosition = _float3(4.5f, 0.35f, -31.0f);
	//CrossBow_Soldier_Desc.isStay = true;
	//CrossBow_Soldier_Desc.vLook = { -1.f,0.f,1.f };

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CrossBow_Soldier"), m_eLevelID, _strLayerTag, &CrossBow_Soldier_Desc)))
	//	return E_FAIL;

	//CBomb_Soldier::MONSTER_DESC Bomb_Soldier_Desc;
	//Bomb_Soldier_Desc.iCurLevelID = m_eLevelID;
	//Bomb_Soldier_Desc.eStartCoord = COORDINATE_3D;
	//Bomb_Soldier_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	//Bomb_Soldier_Desc.tTransform3DDesc.vInitialPosition = _float3(-5.5f, 0.35f, -13.0f);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Bomb_Soldier"), m_eLevelID, _strLayerTag, &Bomb_Soldier_Desc)))
	//	return E_FAIL;

	const _wstring _strLayerTag = L"Layer_Monster";

	CSketchSpace_Alien::SIDESCROLLDESC SketchSpace_Alien_Desc;
	SketchSpace_Alien_Desc.iCurLevelID = m_eLevelID;
	SketchSpace_Alien_Desc.eStartCoord = COORDINATE_2D;
	SketchSpace_Alien_Desc.tTransform2DDesc.vInitialPosition = _float3(-580.f, -44.8f, 0.0f);
	SketchSpace_Alien_Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	SketchSpace_Alien_Desc.eSideScroll_Bound = SIDESCROLL_PATROLBOUND::CHAPTER6_1_1;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_Alien"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_Alien_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);


	SketchSpace_Alien_Desc.tTransform2DDesc.vInitialPosition = _float3(-510.f, 605.1f, 0.0f);
	SketchSpace_Alien_Desc.eSideScroll_Bound = SIDESCROLL_PATROLBOUND::CHAPTER6_1_2;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_Alien"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_Alien_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);

	SketchSpace_Alien_Desc.tTransform2DDesc.vInitialPosition = _float3(-120.f, 605.1f, 0.0f);
	SketchSpace_Alien_Desc.eSideScroll_Bound = SIDESCROLL_PATROLBOUND::CHAPTER6_1_2;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_Alien"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_Alien_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);

	SketchSpace_Alien_Desc.tTransform2DDesc.vInitialPosition = _float3(130.f, -812.f, 0.0f);
	SketchSpace_Alien_Desc.eSideScroll_Bound = SIDESCROLL_PATROLBOUND::CHAPTER6_1_3;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_Alien"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_Alien_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);

	SketchSpace_Alien_Desc.tTransform2DDesc.vInitialPosition = _float3(860.f, -812.f, 0.0f);
	SketchSpace_Alien_Desc.eSideScroll_Bound = SIDESCROLL_PATROLBOUND::CHAPTER6_1_3;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_Alien"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_Alien_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);

	CSketchSpace_SpikeBall::SIDESCROLLDESC SketchSpace_SpikeBall_Desc;
	SketchSpace_SpikeBall_Desc.iCurLevelID = m_eLevelID;
	SketchSpace_SpikeBall_Desc.eStartCoord = COORDINATE_2D;
	SketchSpace_SpikeBall_Desc.tTransform2DDesc.vInitialPosition = _float3(-50.f, -180.f, 0.0f);
	SketchSpace_SpikeBall_Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	SketchSpace_SpikeBall_Desc.isStay = true;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_SpikeBall"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_SpikeBall_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);

	SketchSpace_SpikeBall_Desc.tTransform2DDesc.vInitialPosition = _float3(50.f, -180.f, 0.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_SpikeBall"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_SpikeBall_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);


	SketchSpace_SpikeBall_Desc.tTransform2DDesc.vInitialPosition = _float3(500.f, 303.f, 0.0f);
	SketchSpace_SpikeBall_Desc.eSideScroll_Bound = SIDESCROLL_PATROLBOUND::CHAPTER6_1_4;
	SketchSpace_SpikeBall_Desc.isStay = false;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_SpikeBall"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_SpikeBall_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);


	CSketchSpace_UFO::SIDESCROLLDESC SketchSpace_UFO_Desc;
	SketchSpace_UFO_Desc.iCurLevelID = m_eLevelID;
	SketchSpace_UFO_Desc.eStartCoord = COORDINATE_2D;
	SketchSpace_UFO_Desc.tTransform2DDesc.vInitialPosition = _float3(-750.f, -300.f, 0.0f);
	SketchSpace_UFO_Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_UFO"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_UFO_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);

	SketchSpace_UFO_Desc.tTransform2DDesc.vInitialPosition = _float3(350.f, -370.f, 0.0f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_UFO"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_UFO_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);


	SketchSpace_UFO_Desc.tTransform2DDesc.vInitialPosition = _float3(1074.f, -99.f, 0.0f);
	SketchSpace_UFO_Desc.isStay = true;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SketchSpace_UFO"), m_eLevelID, _strLayerTag, &pObject, &SketchSpace_UFO_Desc)))
		return E_FAIL;

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_02"), pObject);


	//alien
	//.tTransform2DDesc.vInitialPosition = _float3(-570.f, -44.8f, 0.0f); //-977.f, -44.8f
	// 
	//.tTransform2DDesc.vInitialPosition = _float3(-522.f, 605.1f, 0.0f);
	//.tTransform2DDesc.vInitialPosition = _float3(-115.f, 605.1f, 0.0f);

	//.tTransform2DDesc.vInitialPosition = _float3(121.f, -812.f, 0.0f);
	//.tTransform2DDesc.vInitialPosition = _float3(868.f, -812.f, 0.0f);

	//spikeball
	//.tTransform2DDesc.vInitialPosition = _float3(-50.f, -180.f, 0.0f);
	//.tTransform2DDesc.vInitialPosition = _float3(50.f, -180.f, 0.0f);

	//.tTransform2DDesc.vInitialPosition = _float3(484.f, 303.f, 0.0f); // ~841.f

	//ufo
	//.tTransform2DDesc.vInitialPosition = _float3(-750.f, -300.f, 0.0f);
	// 
	//.tTransform2DDesc.vInitialPosition = _float3(350.f, -370.f, 0.0f);
	//.tTransform2DDesc.vInitialPosition = _float3(1074.f, -99.f, 0.0f);


	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Monster_Projectile(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	Pooling_DESC Pooling_Desc;
	Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
	Pooling_Desc.strLayerTag = TEXT("Layer_Monster_Projectile");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Projectile_BirdMonster");

	CProjectile_BirdMonster::PROJECTILE_BIRDMONSTER_DESC* pBirdProjDesc = new CProjectile_BirdMonster::PROJECTILE_BIRDMONSTER_DESC;
	pBirdProjDesc->iCurLevelID = m_eLevelID;
	pBirdProjDesc->eStartCoord = COORDINATE_3D;

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Projectile_BirdMonster"), Pooling_Desc, pBirdProjDesc);

	Pooling_Desc;
	Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
	Pooling_Desc.strLayerTag = TEXT("Layer_Monster_Projectile");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_CrossBow_Arrow");

	CCrossBow_Arrow::CROSSBOW_ARROW_DESC* ArrowDesc = new CCrossBow_Arrow::CROSSBOW_ARROW_DESC;
	ArrowDesc->iCurLevelID = m_eLevelID;
	ArrowDesc->eStartCoord = COORDINATE_3D;

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_CrossBow_Arrow"), Pooling_Desc, ArrowDesc);


	Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
	Pooling_Desc.strLayerTag = TEXT("Layer_Monster_Projectile");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Bomb");
	Pooling_Desc.eSection2DRenderGroup = SECTION_2D_PLAYMAP_OBJECT;

	CBomb::CARRIABLE_DESC* BombDesc = new CBomb::CARRIABLE_DESC;
	BombDesc->iCurLevelID = m_eLevelID;

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Bomb"), Pooling_Desc, BombDesc);

	Pooling_Desc.iPrototypeLevelID = m_eLevelID;
	Pooling_Desc.strLayerTag = TEXT("Layer_Monster_Projectile");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Projectile_SketchSpace_UFO");

	CProjectile_SketchSpace_UFO::PROJECTILE_MONSTER_DESC* pUFOProjDesc = new CProjectile_SketchSpace_UFO::PROJECTILE_MONSTER_DESC;
	pUFOProjDesc->iCurLevelID = m_eLevelID;
	pUFOProjDesc->eStartCoord = COORDINATE_2D;

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Projectile_SketchSpace_UFO"), Pooling_Desc, pUFOProjDesc);

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Effects(const _wstring& _strLayerTag)
{
	CEffect_System::EFFECT_SYSTEM_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = m_eLevelID;
	Desc.isCoordChangeEnable = false;
	Desc.iSpriteShaderLevel = LEVEL_STATIC;
	Desc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";

	Desc.iModelShaderLevel = LEVEL_STATIC;
	Desc.szModelShaderTags = L"Prototype_Component_Shader_VtxMeshInstance";

	Desc.iEffectShaderLevel = LEVEL_STATIC;
	Desc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";

	Desc.iSingleSpriteShaderLevel = LEVEL_STATIC;
	Desc.szSingleSpriteShaderTags = L"Prototype_Component_Shader_VtxPoint";
	Desc.iSingleSpriteBufferLevel = LEVEL_STATIC;
	Desc.szSingleSpriteBufferTags = L"Prototype_Component_VIBuffer_Point";

	Desc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";
	Desc.szMeshComputeShaderTag = L"Prototype_Component_Compute_Shader_MeshInstance";

	const json* pJson = m_pGameInstance->Find_Json_InLevel(TEXT("FX_Static"), LEVEL_STATIC);
	if (nullptr == pJson)
		return E_FAIL;


	CGameObject* pOut = nullptr;
	for (_int i = 0; i < (*pJson).size(); ++i)
	{
		_wstring strName = STRINGTOWSTRING((*pJson)[i]["Name"]);
		_int iCount = (*pJson)[i]["Count"];

		for (_int j = 0; j < iCount; ++j)
		{
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, strName, m_eLevelID, _strLayerTag, &pOut, &Desc)))
				return E_FAIL;
			CEffect_Manager::GetInstance()->Add_Effect(static_cast<CEffect_System*>(pOut));
		}
	}

	pJson = m_pGameInstance->Find_Json_InLevel(TEXT("FX_Level6"), m_eLevelID);
	if (nullptr == pJson)
		return E_FAIL;

	for (_int i = 0; i < (*pJson).size(); ++i)
	{
		_wstring strName = STRINGTOWSTRING((*pJson)[i]["Name"]);
		_int iCount = (*pJson)[i]["Count"];

		for (_int j = 0; j < iCount; ++j)
		{
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, strName, m_eLevelID, _strLayerTag, &pOut, &Desc)))
				return E_FAIL;
			CEffect_Manager::GetInstance()->Add_Effect(static_cast<CEffect_System*>(pOut));
		}
	}

	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Effects2D(const _wstring& _strLayerTag)
{
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Jump_Dust"), LEVEL_STATIC, 1);

	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("bushburst_leaves1"), m_eLevelID, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("bushburst_leaves2"), m_eLevelID, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("bushburst_dust1"), m_eLevelID, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("bushburst_dust2"), m_eLevelID, 3);

	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Death_Burst"), LEVEL_STATIC, 3);

	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_FX1"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_FX2"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_FX3"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_FX4"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_FX5"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_Words1"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_Words2"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_Words4"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Hit_Words5"), LEVEL_STATIC, 3);

	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("health_pickup_small"), LEVEL_STATIC, 3);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("health_pickup_large"), LEVEL_STATIC, 3);

	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("Player2dJumpAttackFX"), LEVEL_STATIC, 1);

	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefDebrisFSmall"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefDebrisMed"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefDebrisTurret"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefExplosionMed"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefExplosionSmall"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefExplosionTurret"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefRedBullet"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefTeleport"), LEVEL_CHAPTER_6, 10);
	CEffect2D_Manager::GetInstance()->Register_EffectPool(TEXT("DefPlayerHit"), LEVEL_CHAPTER_6, 1);


	return S_OK;
}

HRESULT CLevel_Chapter_06::Ready_Layer_Slippery()
{
	CSection_Manager* pSectionMgr = CSection_Manager::GetInstance();

	CModelObject::MODELOBJECT_DESC tModelDesc = {};
	tModelDesc.iCurLevelID = m_eLevelID;
	CMudPit* pMudpit = static_cast<CMudPit*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_eLevelID, TEXT("Prototype_GameObject_MudPit"), &tModelDesc));
	m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Layer_Slippery"), pMudpit);
	pSectionMgr->Add_GameObject_ToSectionLayer(TEXT("Chapter5_P0102"), pMudpit, SECTION_2D_PLAYMAP_OBJECT);


	CRubboink_Tiny::TINY_DESC  tSlipperyDesc{};
	tSlipperyDesc.pMudPit = pMudpit;
	tSlipperyDesc.iCurLevelID = m_eLevelID;
	tSlipperyDesc.iImpactCollisionFilter = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER;

	CSlipperyObject* pSlippery = static_cast<CSlipperyObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_eLevelID, TEXT("Prototype_GameObject_Rubboink_Tiny"), &tSlipperyDesc));
	m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Layer_Slippery"), pSlippery);
	pSectionMgr->Add_GameObject_ToSectionLayer(TEXT("Chapter5_P0102"), pSlippery, SECTION_2D_PLAYMAP_OBJECT);


	//TMP
	CTiltSwapPusher::TILTSWAPPUSHER_DESC tTiltSwapPusherDesc{};
	tTiltSwapPusherDesc.eLookDirection = F_DIRECTION::LEFT;
	tTiltSwapPusherDesc.iCurLevelID = m_eLevelID;
	tTiltSwapPusherDesc.tTransform2DDesc.vInitialPosition = _float3(1530.f, 515.f, 0.f);
	CTiltSwapPusher* pTSP = static_cast<CTiltSwapPusher*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_eLevelID, TEXT("Prototype_GameObject_TiltSwapPusher"), &tTiltSwapPusherDesc));
	m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Layer_Slippery"), pTSP);
	pSectionMgr->Add_GameObject_ToSectionLayer(TEXT("Chapter5_P0102"), pTSP, SECTION_2D_PLAYMAP_OBJECT);

	return S_OK;
}

void CLevel_Chapter_06::Create_IntroTrigger()
{
	CTriggerObject::TRIGGEROBJECT_DESC Desc = {};
	Desc.vHalfExtents = { 35.f, 35.f, 0.f };
	Desc.iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
	Desc.szEventTag = TEXT("Chapter6_Intro");
	Desc.eConditionType = CTriggerObject::TRIGGER_ENTER;
	Desc.isReusable = false; // 한 번 하고 삭제할 때
	Desc.eStartCoord = COORDINATE_2D;
	Desc.tTransform2DDesc.vInitialPosition = { 1170.09f, -156.f, 0.f };

	CSection* pSection = CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_P0102"));
	CTrigger_Manager::GetInstance()->Create_TriggerObject(LEVEL_STATIC, LEVEL_CHAPTER_6, &Desc, pSection);
}

HRESULT CLevel_Chapter_06::Map_Object_Create(_wstring _strFileName)
{
	wstring strFileName = _strFileName;

	_wstring strFullFilePath = MAP_3D_DEFAULT_PATH + strFileName;

	HANDLE	hFile = CreateFile(strFullFilePath.c_str(),
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}
	_uint iCount = 0;

	DWORD	dwByte(0);
	_uint iLayerCount = 0;
	_int isTempReturn = 0;
	isTempReturn = ReadFile(hFile, &iLayerCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iLayerCount; i++)
	{
		_uint		iObjectCnt = 0;
		_char		szLayerTag[MAX_PATH];
		_string		strLayerTag;
		_wstring		wstrLayerTag;



		isTempReturn = ReadFile(hFile, &szLayerTag, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
		isTempReturn = ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);
		strLayerTag = szLayerTag;
		wstrLayerTag = m_pGameInstance->StringToWString(strLayerTag);

		for (size_t i = 0; i < iObjectCnt; i++)
		{
			if (i == 694)
			{
				int a = 1;

			}
			_wstring strIncludeLayerTag = wstrLayerTag;
			C3DMapObject* pGameObject =
				CMapObjectFactory::Bulid_3DObject<C3DMapObject>(
					(LEVEL_ID)m_eLevelID,
					m_pGameInstance,
					hFile);
			if (nullptr != pGameObject)
			{
				if (ContainWstring(pGameObject->Get_MapObjectModelName(), L"SM_sticky_notes"))
					strIncludeLayerTag = L"Layer_Postit";
				m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, strIncludeLayerTag.c_str(), pGameObject);
			}
		}
	}
	CloseHandle(hFile);
	return S_OK;
}

CLevel_Chapter_06* CLevel_Chapter_06::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eLevelID)
{
	CLevel_Chapter_06* pInstance = new CLevel_Chapter_06(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Chapter_06");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CLevel_Chapter_06::Free()
{
	//Safe_Release(m_pCandleGame);
	CFatherGame::GetInstance()->DestroyInstance();

	m_pGameInstance->End_BGM();

	__super::Free();
}
