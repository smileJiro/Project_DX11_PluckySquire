#include "stdafx.h"
#include "Level_Chapter_02.h"

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

#include "CubeMap.h"
#include "MainTable.h"
#include "Player.h"
#include "TestTerrain.h"
#include "Beetle.h"
#include "BarfBug.h"
#include "Projectile_BarfBug.h"
#include "JumpBug.h"
#include "BirdMonster.h"
#include "Goblin.h"
#include "Popuff.h"
#include "Rat.h"
#include "Soldier_Spear.h"
#include "Soldier_CrossBow.h"
#include "Soldier_Bomb.h"
#include "ButterGrump.h"


#include "RayShape.h"


#include "2DMapObject.h"
#include "3DMapObject.h"


//#include "UI.h"
#include "UI_Manager.h"
#include "SettingPanelBG.h"
#include "SettingPanel.h"
#include "ESC_HeartPoint.h"
#include "ShopItemBG.h"
#include "MapObjectFactory.h"

#include "NPC.h"


CLevel_Chapter_02::CLevel_Chapter_02(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: 
	m_eLevelID(LEVEL_CHAPTER_2)
	,CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Chapter_02::Initialize(LEVEL_ID _eLevelID)
{
	m_eLevelID = _eLevelID;
	Ready_Lights();
	CGameObject* pCameraTarget = nullptr;
	Ready_CubeMap(TEXT("Layer_CubeMap"));
	Ready_Layer_MainTable(TEXT("Layer_MainTable"));
	Ready_Layer_TestTerrain(TEXT("Layer_Terrain"));
	Ready_Layer_Player(TEXT("Layer_Player"), &pCameraTarget);
	Ready_Layer_Camera(TEXT("Layer_Camera"), pCameraTarget);
	Ready_Layer_Monster(TEXT("Layer_Monster"));
	Ready_Layer_UI(TEXT("Layer_UI"));
	//Ready_Layer_Effects(TEXT("Layer_Effect"));
	//Ready_Layer_NPC(TEXT("Layer_NPC"));

	//액터 들어가는넘.,
	Ready_Layer_Map();

	/* Pooling Test */
	Pooling_DESC Pooling_Desc;
	Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
	Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Beetle");
	CBeetle::MONSTER_DESC* pDesc = new CBeetle::MONSTER_DESC;
	pDesc->iCurLevelID = m_eLevelID;
	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_TestBeetle"), Pooling_Desc, pDesc);

	/* Collision Test */

	// 그룹필터 추가 >> 중복해서 넣어도 돼 내부적으로 걸러줌 알아서 
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::MONSTER);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::MONSTER_PROJECTILE);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::TRIGGER_OBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::MAPOBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::INTERACTION_OBEJCT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER, OBJECT_GROUP::PLAYER_PROJECTILE);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::PLAYER_TRIGGER, OBJECT_GROUP::INTERACTION_OBEJCT);

	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::MAPOBJECT);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER_PROJECTILE);
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::INTERACTION_OBEJCT);

	//실험용. -김지완-
	m_pGameInstance->Check_GroupFilter(OBJECT_GROUP::INTERACTION_OBEJCT, OBJECT_GROUP::PLAYER_PROJECTILE);


	//임시로 주사위 만들어 봄.
	CModelObject::MODELOBJECT_DESC tModelDesc{};
	tModelDesc.iCurLevelID = m_eLevelID;
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Dice"), m_eLevelID, TEXT("Layer_Test"), &tModelDesc)))
		return E_FAIL;
	 
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_Domino"), m_eLevelID, TEXT("Layer_Test"), &tModelDesc)))
		return E_FAIL;

	// 그룹필터 제거
	// 삭제도 중복해서 해도 돼 >> 내부적으로 걸러줌. >> 가독성이 및 사용감이 더 중요해서 이렇게 처리했음
	//m_pGameInstance->Erase_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER);
	//m_pGameInstance->Erase_GroupFilter(OBJECT_GROUP::MONSTER, OBJECT_GROUP::PLAYER);


	//RayShape Test
	CRayShape::RAYSHAPE_DESC Desc;
	Desc.iCurLevelID = m_eLevelID;

	//Desc.tTransform3DDesc.vInitialPosition = _float3(-28.9f, 0.32f, -16.9f);
	//Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	//Desc.eShapeType = SHAPE_TYPE::CAPSULE;
	//Desc.fRadius = 3.f;
	//Desc.fHalfHeight = 2.f;
	//Desc.vOffsetTrans = { 0.f,Desc.fHalfHeight * 0.5f,0.f };
	//Desc.fRotAngle = 90.f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_RayShape"), m_eLevelID, TEXT("Layer_Terrain"), &Desc)))
	//	return E_FAIL;

	//Desc.tTransform3DDesc.vInitialPosition = _float3(-23.9f, 0.32f, -16.9f);
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_RayShape"), m_eLevelID, TEXT("Layer_Terrain"), &Desc)))
	//	return E_FAIL;

	Desc.eShapeType = SHAPE_TYPE::CAPSULE;
	Desc.fRadius = 1.f;
	Desc.fHalfHeight = 2.f;
	Desc.vOffsetTrans = { 0.f,Desc.fRadius,0.f };
	Desc.fRotAngle = 0.f;

	Desc.tTransform3DDesc.vInitialPosition = _float3(-20.f, 6.36f, 20.19f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_RayShape"), m_eLevelID, TEXT("Layer_Terrain"), &Desc)))
		return E_FAIL;


	/* Blur RenderGroupOn */
	m_pGameInstance->Set_Active_RenderGroup_New(RENDERGROUP::RG_3D, PR3D_POSTPROCESSING, true);
	return S_OK;
}

void CLevel_Chapter_02::Update(_float _fTimeDelta)
{
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

#ifdef _DEBUG
	if (KEY_DOWN(KEY::P))
		CCamera_Manager::GetInstance()->Start_ZoomIn();


	if (KEY_DOWN(KEY::O))
		CCamera_Manager::GetInstance()->Start_ZoomOut();
#endif // _DEBUG



	static _float3 vOutPos = {};
	if (IS_IMPORT_IMGUI)
	{
		//ImGui::Begin("PickingPos");
		//ImGui::InputFloat3("PickingPos##Pick", &vOutPos.x, "%.2f");
		//ImGui::End();
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

	if (KEY_DOWN(KEY::T)) {
		CTrigger_Manager::GetInstance()->Load_Trigger(LEVEL_STATIC, (LEVEL_ID)m_eLevelID, TEXT("../Bin/DataFiles/Trigger/Chapter2_Trigger.json"));
		CTrigger_Manager::GetInstance()->Load_TriggerEvents(TEXT("../Bin/DataFiles/Trigger/Trigger_Events.json"));
	}

}

HRESULT CLevel_Chapter_02::Render()
{
#ifdef _DEBUG
	m_pGameInstance->Render_FPS(TEXT("Timer_120"));
	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Lights()
{
	m_pGameInstance->Load_Lights(TEXT("../Bin/DataFiles/DirectLights/DirectionalTest.json"));
	m_pGameInstance->Load_IBL(TEXT("../Bin/DataFiles/IBL/DirectionalTest.json"));

	//CONST_LIGHT LightDesc{};
	//
	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//
	//LightDesc.vPosition = _float3(0.0f, 20.0f, 0.0f);
	//LightDesc.fFallOutStart = 20.0f;
	//LightDesc.fFallOutEnd = 1000.0f;
	//LightDesc.vRadiance = _float3(1.0f, 1.0f, 1.0f);
	//LightDesc.vDiffuse = _float4(1.0f, 0.0f, 0.0f, 1.0f);
	//LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
	//LightDesc.vSpecular = _float4(1.0f, 0.0f, 0.0f, 1.0f);
	//
	//if (FAILED(m_pGameInstance->Add_Light(LightDesc, LIGHT_TYPE::POINT)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_CubeMap(const _wstring& _strLayerTag)
{
	CGameObject* pCubeMap = nullptr;
	CCubeMap::CUBEMAP_DESC Desc;
	Desc.iCurLevelID = m_eLevelID;
	Desc.iRenderGroupID = RG_3D;
	Desc.iPriorityID = PR3D_PRIORITY;
	Desc.strBRDFPrototypeTag = TEXT("Prototype_Component_Texture_BRDF_Shilick");
	Desc.strCubeMapPrototypeTag = TEXT("Prototype_Component_Texture_TestEnv");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
		m_eLevelID, _strLayerTag, &pCubeMap, &Desc)))
		return E_FAIL;

	m_pGameInstance->Set_CubeMap(static_cast<CCubeMap*>(pCubeMap));
	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_MainTable(const _wstring& _strLayerTag)
{
	CMainTable::ACTOROBJECT_DESC Desc;
	Desc.iCurLevelID = m_eLevelID;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MainTable"),
		m_eLevelID, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_Map()
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
	case Client::LEVEL_CHAPTER_TEST:
		if (FAILED(Map_Object_Create(L"Chapter_04_Default_Desk.mchc")))
			return E_FAIL;
		break;
	default:
		break;
	}

	//if (FAILED(Map_Object_Create(L"Chapter_02_Play_Desk.mchc")))

	return S_OK;
}


HRESULT CLevel_Chapter_02::Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(m_eLevelID, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
		return E_FAIL;

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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		m_eLevelID, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::FREE, dynamic_cast<CCamera*>(pCamera));

	// Target Camera
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	TargetDesc.fSmoothSpeed = 7.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;
	TargetDesc.vAtOffset = _float3(0.0f, 0.5f, 0.0f);
	TargetDesc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE::COORDINATE_3D);

	TargetDesc.fFovy = XMConvertToRadians(60.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.fNear = 0.1f;
	TargetDesc.fFar = 1000.f;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);
	TargetDesc.eZoomLevel = CCamera::LEVEL_6;
	TargetDesc.iCameraType = CCamera_Manager::TARGET;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Target"),
		m_eLevelID, _strLayerTag, &pCamera, &TargetDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::TARGET, dynamic_cast<CCamera*>(pCamera));

	_float3 vArm;
	XMStoreFloat3(&vArm, XMVector3Normalize(XMVectorSet(0.f, 0.67f, -0.74f, 0.f)));
	_float fLength = 14.6f;
	Create_Arm((_uint)COORDINATE_3D, pCamera, vArm, fLength);

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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_CutScene"),
		m_eLevelID, _strLayerTag, &pCamera, &CutSceneDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::CUTSCENE, dynamic_cast<CCamera*>(pCamera));

	// 2D Camera
	CCamera_2D::CAMERA_2D_DESC Target2DDesc{};

	Target2DDesc.fSmoothSpeed = 5.f;
	Target2DDesc.eCameraMode = CCamera_2D::DEFAULT;
	Target2DDesc.vAtOffset = _float3(0.0f, 0.f, 0.0f);
	Target2DDesc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE::COORDINATE_2D);

	Target2DDesc.fFovy = XMConvertToRadians(60.f);
	Target2DDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Target2DDesc.fNear = 0.1f;
	Target2DDesc.fFar = 1000.f;
	Target2DDesc.vEye = _float3(0.f, 10.f, -7.f);
	Target2DDesc.vAt = _float3(0.f, 0.f, 0.f);
	Target2DDesc.eZoomLevel = CCamera::LEVEL_6;
	Target2DDesc.iCameraType = CCamera_Manager::TARGET_2D;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_2D"),
		m_eLevelID, _strLayerTag, &pCamera, &Target2DDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::TARGET_2D, dynamic_cast<CCamera*>(pCamera));

	XMStoreFloat3(&vArm, XMVector3Normalize(XMVectorSet(0.f, 0.981f, -0.191f, 0.f)));
	//vRotation = { XMConvertToRadians(-79.2f), XMConvertToRadians(0.f), 0.f };
	fLength = 12.5f;
	Create_Arm((_uint)COORDINATE_2D, pCamera, vArm, fLength);

	// Set Cur Camera
	CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::FREE);

	// Load CutSceneData, ArmData
	CCamera_Manager::GetInstance()->Load_CutSceneData();
	CCamera_Manager::GetInstance()->Load_ArmData();

	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CPlayer::CONTAINEROBJ_DESC Desc;
	Desc.iCurLevelID = m_eLevelID;
	Desc.tTransform3DDesc.vInitialPosition = { -3.f, 0.35f, -19.3f };   // TODO ::임시 위치

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TestPlayer"), m_eLevelID, _strLayerTag, _ppOut, &Desc)))
		return E_FAIL;


	CPlayer* pPlayer = { nullptr };
	pPlayer = dynamic_cast<CPlayer*>(*_ppOut);

	if (nullptr == Uimgr->Get_Player())
	{
		CUI_Manager::GetInstance()->Set_Player(pPlayer);

	}



	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	/* Test Terrain */
	//CTestTerrain::MODELOBJECT_DESC TerrainDesc{};

	//TerrainDesc.eStartCoord = COORDINATE_3D;
	//TerrainDesc.iCurLevelID = m_eLevelID;
	//TerrainDesc.isCoordChangeEnable = false;
	//TerrainDesc.iModelPrototypeLevelID_3D = m_eLevelID;
	//TerrainDesc.strModelPrototypeTag_3D = TEXT("WoodenPlatform_01");
	//TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");

	//TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	//TerrainDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	//TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	//TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	//TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_TestTerrain"), m_eLevelID, _strLayerTag, &TerrainDesc)))
	//	return E_FAIL;



	//TODO :: SAMPLE

	CModelObject::MODELOBJECT_DESC Desc = {};
	Desc.iCurLevelID = m_eLevelID;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SampleBook"),
		m_eLevelID, L"Layer_Book", &Desc)))
		return E_FAIL;

	// Test(PlayerItem: Glove, Stamp)
	CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)m_eLevelID, TEXT("Flipping_Glove"), _float3(10.f, 10.f, -10.f));
	CPlayerData_Manager::GetInstance()->Spawn_Bulb(LEVEL_STATIC, (LEVEL_ID)m_eLevelID);

	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_UI(const _wstring& _strLayerTag)
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
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 10.f;

	// 원래 크기
	pDesc.fSizeX = 96.f;
	pDesc.fSizeY = 148.f;

	//작게  크기
	//pDesc.fSizeX = 48.f;
	//pDesc.fSizeY = 74.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_StopStamp"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;

	pDesc.fX = g_iWinSizeX / 7.5f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 10.f;
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 111.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_BombStamp"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;

	pDesc.fX = g_iWinSizeX / 10.8f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 20.f;
	pDesc.fSizeX = 42.f;
	pDesc.fSizeY = 27.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_ArrowForStamp"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;
#pragma endregion STAMP UI

#pragma region InterAction UI
	pDesc.fX = g_iWinSizeX / 2.f - g_iWinSizeX / 20.f;
	pDesc.fY = g_iWinSizeY - g_iWinSizeY / 18.f;
	pDesc.fSizeX = 72.f;
	pDesc.fSizeY = 72.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Interaction_Book"), m_eLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;


#pragma endregion InterAction UI

	_uint ShopPanelUICount = { CUI::SHOPPANEL::SHOP_END };

	if (ShopPanelUICount != CUI_Manager::GetInstance()->Get_ShopPanels().size())
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(pDesc.iCurLevelID, TEXT("Prototype_GameObject_ParentShopPannel"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
			return E_FAIL;
	}


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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Dialogue"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;

	pDesc.fX = DEFAULT_SIZE_BOOK2D_X / RATIO_BOOK2D_X;
	pDesc.fY = DEFAULT_SIZE_BOOK2D_Y / RATIO_BOOK2D_Y;
	pDesc.fSizeX = 512.f * 0.8f;
	pDesc.fSizeY = 512.f * 0.8f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Dialogue_Portrait"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;

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


	pDesc.fSizeX = 256.f / 4.f;
	pDesc.fSizeY = 256.f / 4.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Interaction_Heart"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;





	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_Narration"), pDesc.iCurLevelID, _strLayerTag, &pDesc)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_NPC(const _wstring& _strLayerTag)
{
	CNPC::NPC_DESC NPCDesc;

	NPCDesc.iCurLevelID = m_eLevelID;
	NPCDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	NPCDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	NPCDesc.iMainIndex = 0;
	NPCDesc.iSubIndex = 0;
	wsprintf(NPCDesc.strDialogueIndex, TEXT("dialog_01"));
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_StoreNPC"), NPCDesc.iCurLevelID, _strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.iCurLevelID = m_eLevelID;
	NPCDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	NPCDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	NPCDesc.iMainIndex = 0;
	NPCDesc.iSubIndex = 0;
	wsprintf(NPCDesc.strDialogueIndex, TEXT("DJ_Moobeard_Dialogue_01"));
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_NPC_OnlySocial"), NPCDesc.iCurLevelID, _strLayerTag, &NPCDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_Monster(const _wstring& _strLayerTag, CGameObject** _ppout)
{
	CBeetle::MONSTER_DESC Beetle_Desc;
	Beetle_Desc.iCurLevelID = m_eLevelID;
	Beetle_Desc.tTransform3DDesc.vInitialPosition = _float3(-17.f, 6.55f, 23.f);
	//Beetle_Desc.tTransform3DDesc.vInitialPosition = _float3(-9.f, 0.35f, -22.f);
	Beetle_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	Beetle_Desc.eWayIndex = WAYPOINTINDEX::CHAPTER2_1;
	Beetle_Desc.isSneakMode = true;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Beetle"), m_eLevelID, _strLayerTag, &Beetle_Desc)))
		return E_FAIL;

	//Monster_Desc.tTransform3DDesc.vPosition = _float3(10.0f, 1.0f, 10.0f);
	//Monster_Desc.tTransform3DDesc.vScaling = _float3(1.f, 1.f, 1.f);

	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Beetle"), m_eLevelID, _strLayerTag, &Monster_Desc)))
		return E_FAIL;*/

	CBarfBug::MONSTER_DESC Monster_Desc;
	Monster_Desc.iCurLevelID = m_eLevelID;

	Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(-10.0f, 0.35f, -23.0f);
	//Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(-20.0f, 0.35f, -17.0f);
	Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(0.75f, 0.75f, 0.75f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_BarfBug"), m_eLevelID, _strLayerTag, &Monster_Desc)))
		return E_FAIL;

	//Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(-18.0f, 0.35f, -17.0f);
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_BarfBug"), m_eLevelID, _strLayerTag, &Monster_Desc)))
	//	return E_FAIL;

	//Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(-9.0f, 0.35f, -19.0f);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_BarfBug"), m_eLevelID, _strLayerTag, &Monster_Desc)))
	//	return E_FAIL;
	
	//Monster_Desc.isSneakMode = false;
	
	CGoblin::MONSTER_DESC Goblin_Desc;
	Goblin_Desc.iCurLevelID = m_eLevelID;
	Goblin_Desc.tTransform3DDesc.vInitialPosition = _float3(-12.0f, 0.35f, -21.0f);
	Goblin_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Goblin"), m_eLevelID, _strLayerTag, &Goblin_Desc)))
		return E_FAIL;

	//Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(-8.0f, 0.35f, -19.0f);
	//Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	//CGameObject* pGameObject = nullptr;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Soldier_Spear"), m_eLevelID, _strLayerTag, &pGameObject, &Monster_Desc)))
	//	return E_FAIL;

	//CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Section_Test"), pGameObject);
	/*Monster_Desc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.35f, -15.0f);
	Monster_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Popuff"), m_eLevelID, _strLayerTag, &Monster_Desc)))
		return E_FAIL;*/

		//CButterGrump::MONSTER_DESC Boss_Desc;
		//Boss_Desc.iCurLevelID = m_eLevelID;

		//Boss_Desc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 20.35f, 40.0f);
		//Boss_Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

		//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_ButterGrump"), m_eLevelID, _strLayerTag, &Boss_Desc)))
		//	return E_FAIL;

			/*  Projectile  */
	Pooling_DESC Pooling_Desc;
	Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
	Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Projectile_BarfBug");

	CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC* pProjDesc = new CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC;
	pProjDesc->iCurLevelID = m_eLevelID;

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Projectile_BarfBug"), Pooling_Desc, pProjDesc);

	return S_OK;
}

HRESULT CLevel_Chapter_02::Ready_Layer_Effects(const _wstring& _strLayerTag)
{
	CEffect_System::PARTICLE_SYSTEM_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = m_eLevelID;
	Desc.isCoordChangeEnable = false;
	Desc.iSpriteShaderLevel = LEVEL_STATIC;
	Desc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
	Desc.iModelShaderLevel = LEVEL_STATIC;
	Desc.szModelShaderTags = L"Prototype_Component_Shader_VtxMeshInstance";

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Portal.json"), m_eLevelID, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Chapter_02::Create_Arm(_uint _iCoordinateType, CGameObject* _pCamera, _float3 _vArm, _float _fLength)
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(m_eLevelID, TEXT("Layer_Player"), 0);
	if (nullptr == pPlayer)
		return;
	_vector vPlayerLook = pPlayer->Get_ControllerTransform()->Get_Transform((COORDINATE)_iCoordinateType)->Get_State(CTransform::STATE_LOOK);

	CCameraArm::CAMERA_ARM_DESC Desc{};

	Desc.vArm = _vArm;
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	Desc.fLength = _fLength;
	Desc.wszArmTag = TEXT("Player_Arm");

	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);

	switch (_iCoordinateType) {
	case (_uint)COORDINATE_3D:
		dynamic_cast<CCamera_Target*>(_pCamera)->Add_CurArm(pArm);
		break;
	case (_uint)COORDINATE_2D:
		dynamic_cast<CCamera_2D*>(_pCamera)->Add_CurArm(pArm);
		break;
	}
}

HRESULT CLevel_Chapter_02::Map_Object_Create(_wstring _strFileName)
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

			C3DMapObject* pGameObject =
				CMapObjectFactory::Bulid_3DObject<C3DMapObject>(
					(LEVEL_ID)m_eLevelID,
					m_pGameInstance,
					hFile);
			if (nullptr != pGameObject)
				m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, wstrLayerTag.c_str(), pGameObject);
		}
	}
	CloseHandle(hFile);
	return S_OK;
}

CLevel_Chapter_02* CLevel_Chapter_02::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eLevelID)
{
	CLevel_Chapter_02* pInstance = new CLevel_Chapter_02(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Chapter_02");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CLevel_Chapter_02::Free()
{
	__super::Free();
}
