#include "stdafx.h"
#include "Level_Camera_Tool_Client.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_CutScene_Save.h"
#include "Camera_Manager.h"

#include "Layer.h"
#include "Collider.h"
#include "CubeMap.h"
#include "MainTable.h"
#include "Player.h"
#include "ModelObject.h"
#include "3DMapObject.h"
#include "MapObjectFactory.h"

#include "UI_Manager.h"

CLevel_Camera_Tool_Client::CLevel_Camera_Tool_Client(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
	, m_eLevelID(LEVEL_CAMERA_TOOL)
{
}

HRESULT CLevel_Camera_Tool_Client::Initialize(LEVEL_ID _eLevelID)
{
	m_eLevelID = _eLevelID;

	if (FAILED(CSection_Manager::GetInstance()->Level_Enter(LEVEL_CHAPTER_2)))
	{
		MSG_BOX(" Failed CSection_Manager Level_Enter(Level_Chapter_02::Initialize)");
		assert(nullptr);
	}

	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Map()))
	{
		MSG_BOX(" Failed Ready_Layer_Map (Level_Camera_Tool::Initialize)");
		assert(nullptr);
	}

	Ready_CubeMap(TEXT("Layer_CubeMap"));
	CGameObject* pCameraTarget = nullptr;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &pCameraTarget)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"), pCameraTarget)))
		return E_FAIL;

	if (FAILED(Ready_Layer_MainTable(TEXT("Layer_MainTable"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_TestTerrain(TEXT("Layer_Terrain"), pCameraTarget)))
		return E_FAIL;

	Ready_DataFiles();

	m_fFovys[0] = 8.f;
	m_fFovys[1] = 17.f;
	m_fFovys[2] = 35.f;
	m_fFovys[3] = 40.f;
	m_fFovys[4] = 47.f;
	m_fFovys[5] = 62.f;
	m_fFovys[6] = 74.f;
	m_fFovys[7] = 84.f;
	m_fFovys[8] = 100.f;
	m_fFovys[9] = 120.f;

	m_tKeyFrameInfo.iZoomLevel = 6;

	/* Blur RenderGroupOn */
	m_pGameInstance->Set_Active_RenderGroup_New(RENDERGROUP::RG_3D, PR3D_POSTPROCESSING, true);
	return S_OK;
}

void CLevel_Camera_Tool_Client::Update(_float _fTimeDelta)
{
	// 피직스 업데이트 
	m_pGameInstance->Physx_Update(_fTimeDelta);

	// Change Camera Free  Or Target Or CutScene
	if (KEY_DOWN(KEY::TAB)) {
		_uint iCurCameraType = CCamera_Manager::GetInstance()->Get_CameraType();
		iCurCameraType ^= 1;
		CCamera_Manager::GetInstance()->Change_CameraType(iCurCameraType);
	}

	if (KEY_DOWN(KEY::NUM0)) {
		_uint iCurCameraType = CCamera_Manager::GetInstance()->Get_CameraType();

		iCurCameraType = (iCurCameraType == CCamera_Manager::CUTSCENE) ? CCamera_Manager::FREE : CCamera_Manager::CUTSCENE;

		CCamera_Manager::GetInstance()->Change_CameraType(iCurCameraType);
	}

	//Show_CameraTool();
	Show_CutSceneTool(_fTimeDelta);
	//Show_ArmInfo();
	Show_CutSceneInfo();
	Show_SaveLoadFileWindow();

	Show_AnimModel(_fTimeDelta);
}

HRESULT CLevel_Camera_Tool_Client::Render()
{
#ifdef _DEBUG
	//SetWindowText(g_hWnd, TEXT("Camera Tool Level"));
	m_pGameInstance->Render_FPS(TEXT("Timer_Default"));
#endif

	return S_OK;
}

HRESULT CLevel_Camera_Tool_Client::Ready_Lights()
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

HRESULT CLevel_Camera_Tool_Client::Ready_Layer_Map()
{
	if (FAILED(Map_Object_Create(L"Chapter_02_Play_Desk.mchc")))
		return E_FAIL;
	//if (FAILED(Map_Object_Create(L"Chapter_04_Play_Desk.mchc")))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Camera_Tool_Client::Ready_CubeMap(const _wstring& _strLayerTag)
{
	CGameObject* pCubeMap = nullptr;
	CCubeMap::CUBEMAP_DESC Desc;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	Desc.iRenderGroupID = RG_3D;
	Desc.iPriorityID = PR3D_PRIORITY;
	Desc.strBRDFPrototypeTag = TEXT("Prototype_Component_Texture_BRDF_Shilick");
	Desc.strCubeMapPrototypeTag = TEXT("Prototype_Component_Texture_TestEnv");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_CubeMap"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCubeMap, &Desc)))
		return E_FAIL;

	m_pGameInstance->Set_CubeMap(static_cast<CCubeMap*>(pCubeMap));
	return S_OK;
}


HRESULT CLevel_Camera_Tool_Client::Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{
	//CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(LEVEL_CAMERA_TOOL, TEXT("Layer_Player"), 0);

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
	Desc.eZoomLevel = CCamera::LEVEL_6;
	Desc.iCameraType = CCamera_Manager::FREE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::FREE, dynamic_cast<CCamera*>(pCamera));

	//// Target Camera
	//CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	//TargetDesc.fSmoothSpeed = 5.f;
	//TargetDesc.eCameraMode = CCamera_Target::DEFAULT;
	//TargetDesc.vAtOffset = { 0.f, 3.f, 0.f };
	//TargetDesc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE::COORDINATE_3D);

	//TargetDesc.fFovy = XMConvertToRadians(60.f);
	//TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	//TargetDesc.fNear = 0.1f;
	//TargetDesc.fFar = 1000.f;
	//TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	//TargetDesc.vAt = _float3(0.f, 0.f, 0.f);
	//TargetDesc.eZoomLevel = CCamera::LEVEL_6;
	//TargetDesc.iCameraType = CCamera_Manager::TARGET;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Target"),
	//	LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &TargetDesc)))
	//	return E_FAIL;

	//CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::TARGET, dynamic_cast<CCamera*>(pCamera));

	//Create_Arms();

	// CutScene Camera
	CCamera_CutScene_Save::CAMERA_DESC CutSceneDesc{};

	CutSceneDesc.fFovy = XMConvertToRadians(60.f);
	CutSceneDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CutSceneDesc.fNear = 0.1f;
	CutSceneDesc.fFar = 1000.f;
	CutSceneDesc.vEye = _float3(0.f, 10.f, -7.f);
	CutSceneDesc.vAt = _float3(0.f, 0.f, 0.f);
	CutSceneDesc.eZoomLevel = CCamera::LEVEL_6;
	CutSceneDesc.iCameraType = CCamera_Manager::CUTSCENE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_CutScene_Save"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &CutSceneDesc)))
		return E_FAIL;

	CCamera_Manager::GetInstance()->Add_Camera(CCamera_Manager::CUTSCENE, dynamic_cast<CCamera*>(pCamera));

	CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::FREE);

	return S_OK;
}

HRESULT CLevel_Camera_Tool_Client::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CPlayer::CONTAINEROBJ_DESC PlayerDesc = {};
	PlayerDesc.iCurLevelID = m_eLevelID;
	PlayerDesc.tTransform3DDesc.vInitialPosition = { -3.f, 0.35f, -19.3f };   // TODO ::임시 위치
	PlayerDesc.tTransform3DDesc.vInitialRotation = { 0.f, XMConvertToRadians(180.f), 0.f};   // TODO ::임시 위치
	_int a = sizeof(CPlayer::CONTAINEROBJ_DESC);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TestPlayer"), m_eLevelID, _strLayerTag, _ppOut, &PlayerDesc)))
		return E_FAIL;

	//CPlayer* pPlayer = { nullptr };
	//pPlayer = dynamic_cast<CPlayer*>(*_ppOut);

	//if (nullptr == Uimgr->Get_Player())
	//{
	//	CUI_Manager::GetInstance()->Set_Player(pPlayer);
	//}
	//_int iCurCoord = (COORDINATE_2D);
	//_float3 vNewPos = _float3(0.0f, 0.0f, 0.0f);
	//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(pPlayer, SECTION_2D_PLAYMAP_OBJECT);

	//Event_Change_Coordinate(pPlayer, (COORDINATE)iCurCoord, &vNewPos);

	return S_OK;
}

HRESULT CLevel_Camera_Tool_Client::Ready_Layer_TestTerrain(const _wstring& _strLayerTag, CGameObject* _pPlayer)
{
	// Cube
	CModelObject* pOut = { nullptr };
	CModelObject::MODELOBJECT_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;

	Desc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	Desc.strModelPrototypeTag_3D = TEXT("alphabet_blocks_a_mesh");

	Desc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	Desc.tTransform3DDesc.vInitialPosition = m_tKeyFrameInfo.vPosition;
	Desc.tTransform3DDesc.vInitialScaling = _float3(0.5f, 0.5f, 0.5f);
	Desc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	Desc.tTransform3DDesc.fSpeedPerSec = 0.f;

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), LEVEL_CAMERA_TOOL, TEXT("Layer_Simulation"), &m_pSimulationCube, &Desc);
	Safe_AddRef(m_pSimulationCube);
	m_pSimulationCube->Set_Active(false);

	// Book
	//CModelObject::MODELOBJECT_DESC BookDesc = {};
	//BookDesc.iCurLevelID = m_eLevelID;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SampleBook"),
	//	m_eLevelID, L"Layer_Book", &m_pSimulationCube, &BookDesc)))
	//	return E_FAIL;
	//Desc.eStartCoord = COORDINATE_3D;
	//Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	//Desc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;
	//Desc.strModelPrototypeTag_3D = L"Prototype_Model_Book";
	//Desc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxAnimMesh";
	//Desc.iShaderPass_3D = 0;
	//Desc.iPriorityID_3D = PR3D_GEOMETRY;
	//Desc.iRenderGroupID_3D = RG_3D;
	//Desc.tTransform3DDesc.fSpeedPerSec = 1.f;
	//Desc.tTransform3DDesc.vInitialPosition = { 2.f, 0.4f, -17.3f };
	//Desc.tTransform3DDesc.vInitialScaling = { 1.f, 1.f, 1.f };

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
	//	LEVEL_CAMERA_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
	//	return E_FAIL;

	CModelObject::MODELOBJECT_DESC BookDesc = {};
	BookDesc.iCurLevelID = m_eLevelID;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eLevelID, TEXT("Prototype_GameObject_SampleBook"),
		m_eLevelID, L"_strLayerTag", reinterpret_cast<CGameObject**>(&pOut), &BookDesc)))
		return E_FAIL;

	//pOut->Set_Active(false);
	m_ModelObjects.push_back(pOut);

	// Hand
	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	Desc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;
	Desc.strModelPrototypeTag_3D = L"Prototype_Model_MagicHand";
	Desc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxAnimMesh";
	Desc.iShaderPass_3D = 0;
	Desc.iPriorityID_3D = PR3D_BLEND;
	Desc.iRenderGroupID_3D = RG_3D;
	Desc.tTransform3DDesc.fSpeedPerSec = 1.f;
	Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	Desc.tTransform3DDesc.vInitialPosition = { 2.92f, 1.17f, -21.02f };


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
		LEVEL_CAMERA_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
		return E_FAIL;

	//pOut->Set_Active(false);
	m_ModelObjects.push_back(pOut);

	// Player
	//Desc.eStartCoord = COORDINATE_3D;
	//Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	//Desc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;
	//Desc.isCoordChangeEnable = false;
	//Desc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;
	//Desc.strModelPrototypeTag_3D = L"Latch_SkelMesh_NewRig";
	//Desc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxAnimMesh";
	//Desc.iShaderPass_3D = 0;
	//Desc.iPriorityID_3D = PR3D_BLEND;
	//Desc.iRenderGroupID_3D = RG_3D;
	//Desc.tTransform3DDesc.fSpeedPerSec = 1.f;
	////Desc.tTransform3DDesc.vInitialPosition = { 2.92f, 0.352f, -21.02f };
	//Desc.tTransform3DDesc.vInitialPosition = { 2.94290805f, 0.351999998f, -21.1068630f };
	//Desc.tTransform3DDesc.vInitialRotation = { 0.f, XMConvertToRadians(180.f), 0.f };

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
	//	LEVEL_CAMERA_TOOL, _strLayerTag, reinterpret_cast<CGameObject**>(&pOut), &Desc)))
	//	return E_FAIL;

	//pOut->Set_Active(false);
	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CAMERA_TOOL, TEXT("Layer_Player"), 0));
	CModelObject* pPlayerBody = static_cast<CModelObject*>(pPlayer->Get_PlayerPartObject(CPlayer::PLAYER_PART_BODY));
	m_ModelObjects.push_back(pPlayerBody);

	return S_OK;
}

HRESULT CLevel_Camera_Tool_Client::Ready_Layer_MainTable(const _wstring& _strLayerTag)
{
	//CModelObject::MODELOBJECT_DESC NormalDesc = {};
	//NormalDesc.strModelPrototypeTag_3D = L"SM_desk_split_topboard_02";
	//NormalDesc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxMesh";
	//NormalDesc.isCoordChangeEnable = false;
	//NormalDesc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;
	//NormalDesc.eStartCoord = COORDINATE_3D;
	//CGameObject* pGameObject = nullptr;
	//m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MapObject"),
	//	LEVEL_CAMERA_TOOL,
	//	L"Layer_MapObject",
	//	&pGameObject,
	//	(void*)&NormalDesc);


	// MainTable Actor
	CMainTable::ACTOROBJECT_DESC Desc;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_MainTable"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Camera_Tool_Client::Ready_DataFiles()
{
	std::filesystem::path path;

	path = "../Bin/DataFiles/Camera/ArmData/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".json") {
			m_JsonFilePaths.push_back(entry.path().string());
		}
	}

	path = "../Bin/DataFiles/Camera/CutSceneData/Tool/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".json") {
			m_JsonFilePaths.push_back(entry.path().string());
		}
	}

	return S_OK;
}

void CLevel_Camera_Tool_Client::Show_CameraTool()
{
	ImGui::Begin("Camera Basic Tool");

	// Play Move
	Set_MovementInfo();

	// Length
	Change_ArmLength();

	// CheckBox
	ImGui::NewLine();

	ImGui::Checkbox("Use SubData", &m_isUseSubData);

	// Edit CopyArm
	ImGui::SameLine();
	if (ImGui::Button("Edit Arm")) {
		Edit_Arm();
	}

	ImGui::SameLine();
	if (ImGui::Button("Set Cur Arm Info")) {
		Set_CurInfo();
	}

	// Zoom
	Set_Zoom();

	// At Offset
	Set_AtOffsetInfo();

	// Shake
	Set_ShakeInfo();

	ImGui::End();
}

void CLevel_Camera_Tool_Client::Show_CutSceneTool(_float fTimeDelta)
{
	ImGui::Begin("CutScene Tool");

	Picking();

	Set_KeyFrameInfo();
	Set_CameraInfo();

	ImGui::Checkbox("Create Point", &m_isCreatePoint);

	ImGui::SameLine(150);

	ImGui::Checkbox("Edit Cell", &m_isEditPoint);

	Set_CurrentKeyFrame();

	if (true == m_isCreatePoint && false == m_isEditPoint && false == m_isEditSector && false == m_isCreateSector) {
		Create_KeyFrame();
		Delete_KeyFrame();
	}
	if (false == m_isCreatePoint && true == m_isEditPoint && false == m_isEditSector && false == m_isCreateSector) {
		Edit_KeyFrame();
	}


	// CutScene
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             CutScene");
	ImGui::Separator();

	ImGui::Checkbox("Add Sector", &m_isCreateSector);
	ImGui::SameLine();
	ImGui::Checkbox("Edit Sector Frame", &m_isEditSector);

	if (true == m_isCreateSector && false == m_isCreatePoint && false == m_isEditPoint)
		Create_Sector();

	if (true == m_isEditSector && false == m_isCreatePoint && false == m_isEditPoint)
		Edit_Sector();

	Play_CutScene(fTimeDelta);

	ImGui::End();
}

void CLevel_Camera_Tool_Client::Show_ArmInfo()
{
	ImGui::Begin("Arm Info");

	Show_SelectedArmData();
	Show_CameraZoomInfo();

	ImGui::End();
}

void CLevel_Camera_Tool_Client::Show_CutSceneInfo()
{
	ImGui::Begin("CutScene Info");

	Show_KeyFrameInfo();

	ImGui::End();
}

void CLevel_Camera_Tool_Client::Create_Arms()
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(LEVEL_CAMERA_TOOL, TEXT("Layer_Player"), 0);
	_vector vPlayerLook = pPlayer->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);

	CCameraArm::CAMERA_ARM_DESC Desc{};

	XMStoreFloat3(&Desc.vArm, -vPlayerLook);
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	//Desc.vRotation = { XMConvertToRadians(-30.f), XMConvertToRadians(0.f), 0.f };
	Desc.fLength = 14.6f;
	Desc.wszArmTag = TEXT("Player_Arm");

	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);

	CCamera_Target* pTarget = dynamic_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));

	pTarget->Add_CurArm(pArm);
	pTarget->Get_Arm()->Set_ArmVector(XMVectorSet(0.0f, 0.67f, -0.74f, 0.f));
	XMStoreFloat3(&m_vFirstResetArm, pTarget->Get_Arm()->Get_ArmVector());
}

void CLevel_Camera_Tool_Client::Show_ComboBox()
{
	if (m_ArmNames.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ArmNames[m_iSelectedArmNum]);

	if (ImGui::BeginCombo("Label", Name.c_str())) {
		for (_int i = 0; i < m_ArmNames.size(); ++i) {
			_bool bSelected = (m_iSelectedArmNum == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_ArmNames[i]).c_str(), bSelected))
				m_iSelectedArmNum = i;

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Camera_Tool_Client::Show_SelectedArmData()
{
	if (m_ArmNames.size() <= 0)
		return;

	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Selected Arm Data");
	ImGui::Separator();

	CCamera_Target* pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
	pair<ARM_DATA*, SUB_DATA*>* pData = pTargetCamera->Get_ArmData(m_ArmNames[m_iSelectedArmNum]);

	if (nullptr == pData)
		return;

	//m_szSelectedArmName = m_pGameInstance->WStringToString(m_ArmNames[m_iSelectedArmNum]).c_str();

	//ImGui::Text("Copy Arm Tag Input:      ");
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(-1);
	//ImGui::InputText("##SelectedArm Tag",  m_ArmNames[m_iSelectedArmNum], MAX_PATH);

	ImGui::Text("SelectedArm Length Time: %.2f        ", pData->first->fLengthTime.x);

	ImGui::Text("Rotation Time: %.2f    ", pData->first->fMoveTimeAxisY.x);


	if (nullptr == pData->second)
		return;

	ImGui::Text("Zoom Time : % .2f        ", pData->second->fZoomTime);

	ImGui::Text("Zoom Level : %d    ", pData->second->iZoomLevel + 1);

	switch (pData->second->iZoomRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;
	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT          ");
		break;
	}

	ImGui::Text("Offset Time : % .2f        ", pData->second->fAtOffsetTime);

	ImGui::Text("AtOffset:  %.2f, %.2f, %.2f ", pData->second->vAtOffset.x, pData->second->vAtOffset.y, pData->second->vAtOffset.z);

	switch (pData->second->iAtRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;
	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT          ");
		break;
	}

	ImGui::NewLine();
}

void CLevel_Camera_Tool_Client::Show_CameraZoomInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Zoom Data");
	ImGui::Separator();

	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET);
	_float fFovy = XMConvertToDegrees(pCamera->Get_Fovy());
	_uint iPreLevel = pCamera->Get_PreZoomLevel() + 1;
	_float fPreFovy = XMConvertToDegrees(pCamera->Get_Fovy(iPreLevel - 1));
	_uint iCurLevel = pCamera->Get_CurrentZoomLevel() + 1;
	_float fCurFovy = XMConvertToDegrees(pCamera->Get_Fovy(iCurLevel - 1));

	ImGui::Text("Pre Zoom Level: %d", iPreLevel);
	ImGui::SameLine();
	ImGui::Text("Pre Zoom Fovy: %.2f", fPreFovy);

	ImGui::Text("Cur Zoom Level: %d", iCurLevel);
	ImGui::SameLine();
	ImGui::Text("Cur Zoom Fovy: %.2f", fCurFovy);

	ImGui::Text("Cur Fovy: %.2f", fFovy);

}

void CLevel_Camera_Tool_Client::Show_AnimModel(_float _fTimeDelta)
{
	ImGui::Begin("Control Model");

	//static _int iModelIndex = {};
	ImGui::InputInt("Model Index: %d", &m_iModelIndex);

	if (m_iModelIndex >= m_ModelObjects.size() || m_iModelIndex < -1)
		return;

	//C3DModel* 
	_float fTime = static_cast<C3DModel*>(m_ModelObjects[m_iModelIndex]->Get_Model(COORDINATE_3D))->Get_AnimTime();
	ImGui::Text("Current Anim Time : %.4f", fTime);

	//static _bool isLoop = { true };
	if (ImGui::RadioButton("Loop", m_isLoop))
	{
		m_isLoop = !m_isLoop;
	}

	//static _int iAnim[3] = {};

	ImGui::Text("Model 0(Book) Index: ", m_iAnim[0]);
	ImGui::InputInt("Anim Index 0", &m_iAnim[0]);

	ImGui::Text("Model 1(Hand) Index: ", m_iAnim[1]);
	ImGui::InputInt("Anim Index 1", &m_iAnim[1]);

	ImGui::Text("Model 2(Player) Index: ", m_iAnim[2]);
	ImGui::InputInt("Anim Index 2", &m_iAnim[2]);

	//static _int iReset = { 0 };
	//ImGui::InputInt("Event ID", &iReset);

	//static _bool isSelectModel[3] = {true};

	if (ImGui::RadioButton("Book", m_isSelectModel[0]))
	{
		m_isSelectModel[0] = !m_isSelectModel[0];
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Hand", m_isSelectModel[1]))
	{
		m_isSelectModel[1] = !m_isSelectModel[1];
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Player", m_isSelectModel[2]))
	{
		m_isSelectModel[2] = !m_isSelectModel[2];
	}

	if (ImGui::Button("Reset All"))
	{
		for (_int i = 0; i < 3; ++i) {
			if (true == m_isSelectModel[i]) {
				m_ModelObjects[i]->Set_AnimationLoop(COORDINATE_3D, m_iAnim[i], m_isLoop);
				m_ModelObjects[i]->Set_Animation(m_iAnim[i]);
			}
		}

		m_fAnimTime = 0.f;
	}

	ImGui::SameLine();

	if (ImGui::Button("Stop")) {
		for (_int i = 0; i < 3; ++i) {
			if (true == m_isSelectModel[i]) {
				m_ModelObjects[i]->Set_PlayingAnim(false);
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("ReStart")) {
		for (_int i = 0; i < 3; ++i) {
			if (true == m_isSelectModel[i]) {
				m_ModelObjects[i]->Set_PlayingAnim(true);
			}
		}
	}

	_float fAnimTime[3] = {};
	pair<_float, _int> Greater = {};

	for (_int i = 0; i < 3; ++i) {
		if (true == m_isSelectModel[i]) {
			fAnimTime[i] = m_ModelObjects[i]->Get_AnimationTime();

			ImGui::Text("%d Time: %.2f", i, fAnimTime[i]);
		}

		if (Greater.first < fAnimTime[i]) {
			Greater.first = fAnimTime[i];
			Greater.second = i;
		}
	}

	if (true == m_isSelectModel[Greater.second]) {
		if (true == m_ModelObjects[Greater.second]->Is_PlayingAnim()) {
			m_fAnimTime += _fTimeDelta;
		}

		ImGui::Text("Max Anim %d, Cur Time: %.2f", Greater.second, m_fAnimTime);
	}


	ImGui::End();
}

void CLevel_Camera_Tool_Client::Set_KeyFrameInfo()
{


	ImGui::Text("Position: %.2f, %.2f, %.2f", m_tKeyFrameInfo.vPosition.x, m_tKeyFrameInfo.vPosition.y, m_tKeyFrameInfo.vPosition.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFramePosX", &m_tKeyFrameInfo.vPosition.x, 0.03f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFramePosY", &m_tKeyFrameInfo.vPosition.y, 0.03f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFramePosZ", &m_tKeyFrameInfo.vPosition.z, 0.03f);

	ImGui::SameLine();
	ImGui::Checkbox("Maintain Pos", &m_isMaintainOriginPos);

	ImGui::NewLine();

	ImGui::Text("TimeStamp: %.2f\t\t   ", m_tKeyFrameInfo.fTimeStamp);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	ImGui::DragFloat("##TimeStamp", &m_tKeyFrameInfo.fTimeStamp, 0.05f, 0.f, 30.f);

	ImGui::SameLine();
	if (ImGui::Button("Set Cur KeyFrameInfo")) {
		if (nullptr != m_pCurKeyFrame)
			m_tKeyFrameInfo = m_pCurKeyFrame->first;

	}

	ImGui::NewLine();

	ImGui::Text("Frame Zoom Level: %d\t   ", m_tKeyFrameInfo.iZoomLevel);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	_int iZoomLevel = m_tKeyFrameInfo.iZoomLevel;
	ImGui::InputInt("##Frame Zoom Level   ", &iZoomLevel);
	m_tKeyFrameInfo.iZoomLevel = iZoomLevel;

	if (m_tKeyFrameInfo.iZoomLevel <= 0)
		ImGui::Text("Frame Fovy: %.2f", 0.f);
	else
		ImGui::Text("Frame Fovy: %.2f", m_fFovys[m_tKeyFrameInfo.iZoomLevel - 1]);


	switch (m_tKeyFrameInfo.iZoomRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;
	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT          ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("FRAME EASE IN")) {
		m_tKeyFrameInfo.iZoomRatioType = RATIO_TYPE::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("FRAME EASE OUT")) {
		m_tKeyFrameInfo.iZoomRatioType = RATIO_TYPE::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("FRAME LERP")) {
		m_tKeyFrameInfo.iZoomRatioType = RATIO_TYPE::LERP;
	}
	ImGui::SameLine();
	if (ImGui::Button("FRAME EASE_IN_OUT")) {
		m_tKeyFrameInfo.iZoomRatioType = RATIO_TYPE::EASE_IN_OUT;
	}

	ImGui::NewLine();

	ImGui::Text("At: %.2f, %.2f, %.2f      ", m_tKeyFrameInfo.vAt.x, m_tKeyFrameInfo.vAt.y, m_tKeyFrameInfo.vAt.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFrameAtX", &m_tKeyFrameInfo.vAt.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFrameAtY", &m_tKeyFrameInfo.vAt.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFrameAtZ", &m_tKeyFrameInfo.vAt.z);

	if (true == m_tKeyFrameInfo.bLookTarget)
		ImGui::Text("Loot Target: TRUE             ");
	else
		ImGui::Text("Loot Target: FALSE            ");

	ImGui::SameLine();
	if (ImGui::Button("TRUE")) {
		m_tKeyFrameInfo.bLookTarget = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("FALSE")) {
		m_tKeyFrameInfo.bLookTarget = false;
	}

	switch (m_tKeyFrameInfo.iAtRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;
	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT          ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("AT EASE IN")) {
		m_tKeyFrameInfo.iAtRatioType = RATIO_TYPE::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("AT EASE OUT")) {
		m_tKeyFrameInfo.iAtRatioType = RATIO_TYPE::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("AT LERP")) {
		m_tKeyFrameInfo.iAtRatioType = RATIO_TYPE::LERP;
	}
	ImGui::SameLine();
	if (ImGui::Button("AT EASE_IN_OUT")) {
		m_tKeyFrameInfo.iAtRatioType = RATIO_TYPE::EASE_IN_OUT;
	}

	ImGui::NewLine();


	// List
	//list<pair<CUTSCENE_KEYFRAME, CGameObject*>>		m_KeyFrames;

	if (ImGui::BeginListBox("##KeyFrameList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (auto& KeyFramePair : m_KeyFrames) {
			_char szName[MAX_PATH] = {};
			_uint iInstanceID = KeyFramePair.second->Get_GameObjectInstanceID();
			_string strName = "Cube_";
			strName += std::to_string(iInstanceID);

			_int iSelectInstanceID = -1;
			if (nullptr != m_pCurKeyFrame)
			{
				iSelectInstanceID = (_int)m_pCurKeyFrame->second->Get_GameObjectInstanceID();
			}

			if (ImGui::Selectable(strName.c_str(), iSelectInstanceID == iInstanceID))
			{
				if (iSelectInstanceID != iInstanceID)
				{
					m_pCurKeyFrame = &KeyFramePair;
				}
				else 
				{
					m_pCurKeyFrame = nullptr;
				}
			}
		}
		ImGui::EndListBox();
	}




}

void CLevel_Camera_Tool_Client::Set_InitialData()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Initial Data");
	ImGui::Separator();

	ImGui::Text("Position: %.2f, %.2f, %.2f", m_tInitialData.vPosition.x, m_tInitialData.vPosition.y, m_tInitialData.vPosition.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InitialDataPosX", &m_tInitialData.vPosition.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InitialDataPosY", &m_tInitialData.vPosition.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InitialDataPosZ", &m_tInitialData.vPosition.z, 0.1f);

	ImGui::Text("LookAt Pos: %.2f, %.2f, %.2f", m_tInitialData.vAt.x, m_tInitialData.vAt.y, m_tInitialData.vAt.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InitialDataAtX", &m_tInitialData.vAt.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InitialDataAtY", &m_tInitialData.vAt.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InitialDataAtZ", &m_tInitialData.vAt.z, 0.1f);

	ImGui::Text("Frame Zoom Level: %d\t   ", m_tInitialData.iZoomLevel);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	_int iZoomLevel = m_tInitialData.iZoomLevel;
	ImGui::InputInt("##InitialData Zoom Level   ", &iZoomLevel);
	m_tInitialData.iZoomLevel = iZoomLevel;

	if (m_tInitialData.iZoomLevel <= 0)
		ImGui::Text("Frame Fovy: %.2f", 0.f);
	else
		ImGui::Text("Frame Fovy: %.2f", m_fFovys[m_tInitialData.iZoomLevel - 1]);

	ImGui::NewLine();
}

void CLevel_Camera_Tool_Client::Show_KeyFrameInfo()
{
	// Cur KeyFrame Info
	if (nullptr != m_pCurKeyFrame) {
		ImGui::Text("Cur KeyFrame Position: %.2f, %.2f, %.2f", m_pCurKeyFrame->first.vPosition.x, m_pCurKeyFrame->first.vPosition.y, m_pCurKeyFrame->first.vPosition.z);
		ImGui::Text("TimeStamp: %.2f", m_pCurKeyFrame->first.fTimeStamp);
		ImGui::Text("Frame Zoom Level: %d\t   ", m_pCurKeyFrame->first.iZoomLevel);

		if (m_pCurKeyFrame->first.iZoomLevel <= 0)
			ImGui::Text("Frame Fovy: %.2f", 0.f);
		else
			ImGui::Text("Frame Fovy: %.2f", m_fFovys[m_pCurKeyFrame->first.iZoomLevel - 1]);

		switch (m_pCurKeyFrame->first.iZoomRatioType) {
		case RATIO_TYPE::EASE_IN:
			ImGui::Text("Ratio Type: EASE_IN       ");
			break;

		case RATIO_TYPE::EASE_OUT:
			ImGui::Text("Ratio Type: EASE_OUT      ");
			break;

		case RATIO_TYPE::LERP:
			ImGui::Text("Ratio Type: LERP          ");
			break;
		case RATIO_TYPE::EASE_IN_OUT:
			ImGui::Text("Ratio Type: EASE_IN_OUT          ");
			break;
		}
		ImGui::Text("At: %.2f, %.2f, %.2f      ", m_pCurKeyFrame->first.vAt.x, m_pCurKeyFrame->first.vAt.y, m_pCurKeyFrame->first.vAt.z);

		if (true == m_pCurKeyFrame->first.bLookTarget)
			ImGui::Text("Loot At: TRUE             ");
		else
			ImGui::Text("Loot At: FALSE            ");

		switch (m_pCurKeyFrame->first.iAtRatioType) {
		case RATIO_TYPE::EASE_IN:
			ImGui::Text("Ratio Type: EASE_IN       ");
			break;

		case RATIO_TYPE::EASE_OUT:
			ImGui::Text("Ratio Type: EASE_OUT      ");
			break;

		case RATIO_TYPE::LERP:
			ImGui::Text("Ratio Type: LERP          ");
			break;
		case RATIO_TYPE::EASE_IN_OUT:
			ImGui::Text("Ratio Type: EASE_IN_OUT          ");
			break;
		}
		ImGui::NewLine();
	}

	// Selected Frame Info
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Selected Frame Info");
	ImGui::Separator();

	for (auto& Frame : m_SelectedKeyFrame) {
		ImGui::Text("Selected Position: %.2f, %.2f, %.2f", Frame.vPosition.x, Frame.vPosition.y, Frame.vPosition.z);
		ImGui::SameLine();
		ImGui::Text("   TimeStamp: %.2f", Frame.fTimeStamp);
	
	}

	// Cur Scene Info
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Current Scene Info");
	ImGui::Separator();


	_int iSectorNum = {};
	for (auto& CutScene : m_CutScenes) {
		if (CutScene.first == m_CutSceneTags[m_iSelectedCutSceneNum]) {

			_string Name = m_pGameInstance->WStringToString(m_CutSceneTags[m_iSelectedCutSceneNum]);
			ImGui::Text("Current CutScene Tag: %s  ", Name.c_str());
			//ImGui::NewLine();

			CUTSCENE_SUB_DATA tSubData;
			auto& Subiter = m_CutSceneSubDatas.find(CutScene.first);
			if (CCamera_Manager::TARGET == (*Subiter).second.iNextCameraType) {
				ImGui::Text("Current CutScene Next CameraType: TARGET");
			}
			else if (CCamera_Manager::TARGET_2D == (*Subiter).second.iNextCameraType) {
				ImGui::Text("Current CutScene Next CameraType: TARGET_2D");
			}

			ImGui::NewLine();

			for (auto& Sector : CutScene.second) {

				vector<CUTSCENE_KEYFRAME>* pKeyFrames = Sector->Get_KeyFrames();

				for (_uint i = 0; i < pKeyFrames->size(); ++i) {
					ImGui::Text("Sector Num: %d", iSectorNum);

					ImGui::Text("Position: %.2f, %.2f, %.2f", (*pKeyFrames)[i].vPosition.x, (*pKeyFrames)[i].vPosition.y, (*pKeyFrames)[i].vPosition.z);
					ImGui::Text("TimeStamp: %.2f", (*pKeyFrames)[i].fTimeStamp);
					ImGui::Text("Frame Zoom Level: %d\t   ", (*pKeyFrames)[i].iZoomLevel + 1);

					if ((*pKeyFrames)[i].iZoomLevel + 1 <= 0)
						ImGui::Text("Frame Fovy: %.2f", 0.f);
					else
						ImGui::Text("Frame Fovy: %.2f", m_fFovys[(*pKeyFrames)[i].iZoomLevel]);

					switch ((*pKeyFrames)[i].iZoomRatioType) {
					case RATIO_TYPE::EASE_IN:
						ImGui::Text("Ratio Type: EASE_IN       ");
						break;

					case RATIO_TYPE::EASE_OUT:
						ImGui::Text("Ratio Type: EASE_OUT      ");
						break;

					case RATIO_TYPE::LERP:
						ImGui::Text("Ratio Type: LERP          ");
						break;
					case RATIO_TYPE::EASE_IN_OUT:
						ImGui::Text("Ratio Type: EASE_IN_OUT          ");
						break;
					}
					ImGui::Text("At: %.2f, %.2f, %.2f      ", (*pKeyFrames)[i].vAt.x, (*pKeyFrames)[i].vAt.y, (*pKeyFrames)[i].vAt.z);

					if (true == (*pKeyFrames)[i].bLookTarget)
						ImGui::Text("Loot At: TRUE             ");
					else
						ImGui::Text("Loot At: FALSE            ");

					switch ((*pKeyFrames)[i].iAtRatioType) {
					case RATIO_TYPE::EASE_IN:
						ImGui::Text("Ratio Type: EASE_IN       ");
						break;

					case RATIO_TYPE::EASE_OUT:
						ImGui::Text("Ratio Type: EASE_OUT      ");
						break;

					case RATIO_TYPE::LERP:
						ImGui::Text("Ratio Type: LERP          ");
						break;
					case RATIO_TYPE::EASE_IN_OUT:
						ImGui::Text("Ratio Type: EASE_IN_OUT          ");
						break;
					}

					ImGui::NewLine();
				}
				ImGui::Text("======================================");
				iSectorNum++;
			}
		}
	}
}

void CLevel_Camera_Tool_Client::Show_CutSceneComboBox()
{
	if (m_CutScenes.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_CutSceneTags[m_iSelectedCutSceneNum]);

	if (ImGui::BeginCombo("Label", Name.c_str())) {
		for (_int i = 0; i < m_CutSceneTags.size(); ++i) {
			_bool bSelected = (m_iSelectedCutSceneNum == i);

			if (ImGui::Selectable(m_pGameInstance->WStringToString(m_CutSceneTags[i]).c_str(), bSelected))
				m_iSelectedCutSceneNum = i;

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Camera_Tool_Client::Show_SaveLoadFileWindow()
{
	ImGui::Begin("Save Load File");

	ImGui::Checkbox("For Client", &m_isForClient);
	ImGui::SameLine();
	ImGui::Checkbox("Arm Data", &m_isArmData);
	ImGui::SameLine();
	ImGui::Checkbox("CutScene Data", &m_isCutSceneData);

	ImGui::BeginChild("Files", ImVec2(400, 150), true);  // true는 border 표시
	for (int i = 0; i < m_JsonFilePaths.size(); ++i)
	{
		if (ImGui::Selectable(m_JsonFilePaths[i].c_str(), m_iCurrentJsonFileIndex == i))
		{
			m_iCurrentJsonFileIndex = i;
		}
	}
	ImGui::EndChild();

	if (ImGui::Button("Save")) {
		ImGui::OpenPopup("Save File");
		m_isShowPopUp = true;
	}

	if (ImGui::BeginPopupModal("Save File", &m_isShowPopUp)) {
		ImGui::Text("Enter File Name");

		if (ImGui::InputText("File Name", m_szSaveName, sizeof(_char) * MAX_PATH,
			ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::CloseCurrentPopup();
			m_isShowPopUp = false;
		}

		if (ImGui::Button("Save")) {
			ImGui::CloseCurrentPopup();
			m_isShowPopUp = false;

			if (true == m_isArmData && false == m_isCutSceneData)
				Save_Data_Arm();
			else if (true == m_isCutSceneData && false == m_isArmData)
				Save_Data_CutScene();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
			m_isShowPopUp = false;
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	if (ImGui::Button("Load")) {

		if (true == m_isArmData && false == m_isCutSceneData)
			Load_Data_Arm();
		else if (true == m_isCutSceneData && false == m_isArmData)
			Load_Data_CutScene();
	}

	ImGui::End();
}


void CLevel_Camera_Tool_Client::Change_ArmLength()
{
	CCamera_Target* pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
	CCameraArm* pArm = pTargetCamera->Get_Arm();

	ImGui::NewLine();
	ImGui::Text("Length Value: %.2f", m_fLengthValue);
	ImGui::SameLine();
	ImGui::DragFloat("##Length", &m_fLengthValue, 0.05f, 0.f, 10.f);

	_float fArmLength = pArm->Get_Length();
	_bool bActive = false;

	ImGui::Text("Arm Length: %.2f  ", fArmLength);
	ImGui::SameLine();
	if (ImGui::Button("- Length") || ImGui::IsItemActive()) {// 누르고 있는 동안 계속 동작
		fArmLength -= m_fLengthValue;
		pArm->Set_Length(fArmLength);
	}
	ImGui::SameLine();
	if (ImGui::Button("+ Length") || ImGui::IsItemActive()) {
		fArmLength += m_fLengthValue;
		pArm->Set_Length(fArmLength);
	}

	_int iZoomLevel = pTargetCamera->Get_CurrentZoomLevel();
	ImGui::Text("Cur Zoom Level : %d LEVEL   ", iZoomLevel + 1);
	ImGui::SameLine();
	ImGui::InputInt("##Zoom Level", &iZoomLevel);

	if (iZoomLevel < 0)
		iZoomLevel = 0;
	ImGui::Text("Fovy: %.2f", m_fFovys[iZoomLevel]);
	pTargetCamera->Set_ZoomLevel(iZoomLevel);
	m_iZoomLevel = iZoomLevel + 1;

	if (ImGui::Checkbox("Not LookAt", &m_isNotLookAt)) {
		CCamera* pTarget = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET);
		static_cast<CCamera_Target*>(pTarget)->Set_LookAt(!m_isNotLookAt);
	}

	// 여기서 동작
	// ============================Desire, Reset
	_float3 vCurArm = {};
	XMStoreFloat3(&vCurArm, pArm->Get_ArmVector());
	_float	fCurLength = pArm->Get_Length();
	_float3 vCurAtOffset;
	XMStoreFloat3(&vCurAtOffset, CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Get_AtOffset());

	ImGui::Text("Cur Arm: %.2f, %.2f, %.2f", vCurArm.x, vCurArm.y, vCurArm.z);
	ImGui::Text("Cur Length: %.2f", fCurLength);
	ImGui::Text("Cur AtOffset: %.2f, %.2f, %.2f", vCurAtOffset.x, vCurAtOffset.y, vCurAtOffset.z);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireAtOffsetX", &vCurAtOffset.x, 0.1f, 0.f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireAtOffsetY", &vCurAtOffset.y, 0.1f, 0.f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireAtOffsetZ", &vCurAtOffset.z, 0.1f, 0.f);

	CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET)->Set_AtOffset(XMLoadFloat3(&vCurAtOffset));
	ImGui::NewLine();

	// Desire
	ImGui::Text("Arm Tag Input:              ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##CopyArm Tag", m_szCopyArmName, MAX_PATH);

	ImGui::Text("Desire Arm: %.2f, %.2f, %.2f", m_tArmData.vDesireArm.x, m_tArmData.vDesireArm.y, m_tArmData.vDesireArm.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireX", &m_tArmData.vDesireArm.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireY", &m_tArmData.vDesireArm.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireZ", &m_tArmData.vDesireArm.z);

	ImGui::Text("Rotation Time: %.2f         ", m_tArmData.fMoveTimeAxisY.x);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);
	ImGui::DragFloat("##Rot Time D", &m_tArmData.fMoveTimeAxisY.x, 0.05f, 0.f, 10.f);

	switch (m_tArmData.iRotationRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN         ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT        ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP            ");
		break;
	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT     ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("Rot EASE IN")) {
		m_tArmData.iRotationRatioType = RATIO_TYPE::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rot EASE OUT")) {
		m_tArmData.iRotationRatioType = RATIO_TYPE::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rot LERP")) {
		m_tArmData.iRotationRatioType = RATIO_TYPE::LERP;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rot EASE_IN_OUT")) {
		m_tArmData.iRotationRatioType = RATIO_TYPE::EASE_IN_OUT;
	}

	ImGui::Text("Desire Length: %.2f         ", m_tArmData.fLength);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##DesireLength", &m_tArmData.fLength);

	ImGui::Text("Length Time: %.2f           ", m_tArmData.fLengthTime.x);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);
	ImGui::DragFloat("##DesireLength Time", &m_tArmData.fLengthTime.x, 0.05f, 0.f, 10.f);

	switch (m_tArmData.iLengthRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN     ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT    ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP        ");
		break;
	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("Length EASE IN")) {
		m_tArmData.iLengthRatioType = RATIO_TYPE::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("Length EASE OUT")) {
		m_tArmData.iLengthRatioType = RATIO_TYPE::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("Length LERP")) {
		m_tArmData.iLengthRatioType = RATIO_TYPE::LERP;
	}
	ImGui::SameLine();
	if (ImGui::Button("Length EASE_IN_OUT")) {
		m_tArmData.iLengthRatioType = RATIO_TYPE::EASE_IN_OUT;
	}

	if (ImGui::Button("Set_DesireInfo")) {
		m_tArmData.vDesireArm = vCurArm;
		m_tArmData.fLength = fCurLength;
		m_vNextAtOffset = vCurAtOffset;
	}

	// Reset Arm
	ImGui::NewLine();
	ImGui::Text("Reset Arm: %.2f, %.2f, %.2f    ", m_vResetArm.x, m_vResetArm.y, m_vResetArm.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetX", &m_vResetArm.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetY", &m_vResetArm.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResstZ", &m_vResetArm.z);


	ImGui::Text("Reset Length: %.2f             ", m_fResetLength);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetLength", &m_fResetLength);

	ImGui::Text("Reset ResetAtOffset: %.2f, %.2f, %.2f", m_vResetAtOffset.x, m_vResetAtOffset.y, m_vResetAtOffset.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetAtOffsetX", &m_vResetAtOffset.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetAtOffsetY", &m_vResetAtOffset.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##ResetAtOffsetZ", &m_vResetAtOffset.z);

	if (ImGui::Button("Set ResetInfo")) {
		m_vResetArm = vCurArm;
		m_fResetLength = fCurLength;
		m_vResetAtOffset = vCurAtOffset;
	}

	ImGui::NewLine();
	// Add Next ArmInfo
	if (ImGui::Button("Add Next Arm Info")) {

		ARM_DATA* pData = new ARM_DATA();
		*pData = m_tArmData;
		pData->fRotationPerSecAxisY = { XMConvertToRadians(m_tArmData.fRotationPerSecAxisY.x), XMConvertToRadians(m_tArmData.fRotationPerSecAxisY.y) };
		pData->fRotationPerSecAxisRight = { XMConvertToRadians(m_tArmData.fRotationPerSecAxisRight.x), XMConvertToRadians(m_tArmData.fRotationPerSecAxisRight.y) };

		SUB_DATA* pSubData = nullptr;

		if (true == m_isUseSubData) {
			pSubData = new SUB_DATA();

			pSubData->fZoomTime = m_fZoomTime;
			pSubData->iZoomLevel = m_iZoomLevel - 1;
			pSubData->iZoomRatioType = m_iRatioType;
			pSubData->fAtOffsetTime = m_fAtOffsetTime;
			pSubData->vAtOffset = { m_vNextAtOffset };
			pSubData->iAtRatioType = { m_iAtOffsetRatioType };
		}
		CCamera_Target* pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));

		pTargetCamera->Add_ArmData( m_pGameInstance->StringToWString(m_szCopyArmName), pData, pSubData);
		pTargetCamera->Get_ArmNames(&m_ArmNames);
	}
}

#pragma region Rotation_NotUse
void CLevel_Camera_Tool_Client::Input_NextArm_Info()
{
	//ImGui::NewLine();
	//ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	//ImGui::SameLine();
	//ImGui::Text("Add Arm");
	//ImGui::Separator();

	//ImGui::Text("Arm Tag Input:      ");
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(-1);
	//ImGui::InputText("##CopyArm Tag", m_szCopyArmName, MAX_PATH);

	//ImGui::Text("Length: %.2f             ", m_tArmData.fLength);
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(-1);
	//ImGui::DragFloat("##Length Info", &m_tArmData.fLength, 0.05f, 0.f, 30.f);

	//ImGui::Text("Length Time: %.2f        ", m_tArmData.fLengthTime.x);
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(-1);
	//ImGui::DragFloat("##Length Time", &m_tArmData.fLengthTime.x, 0.05f, 0.f, 10.f);

	////Type
	//switch (m_tArmData.iLengthRatioType) {
	//case RATIO_TYPE::EASE_IN:
	//	ImGui::Text("Ratio Type: EASE_IN   ");
	//	break;

	//case RATIO_TYPE::EASE_OUT:
	//	ImGui::Text("Ratio Type: EASE_OUT  ");
	//	break;

	//case RATIO_TYPE::LERP:
	//	ImGui::Text("Ratio Type: LERP      ");
	//	break;
	//case RATIO_TYPE::EASE_IN_OUT:
	//	ImGui::Text("Ratio Type: EASE_IN_OUT      ");
	//	break;
	//}

	//ImGui::SameLine();
	//if (ImGui::Button("Length EASE IN")) {
	//	m_tArmData.iLengthRatioType = RATIO_TYPE::EASE_IN;
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Length EASE OUT")) {
	//	m_tArmData.iLengthRatioType = RATIO_TYPE::EASE_OUT;
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Length LERP")) {
	//	m_tArmData.iLengthRatioType = RATIO_TYPE::LERP;
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Length EASE_IN_OUT")) {
	//	m_tArmData.iLengthRatioType = RATIO_TYPE::EASE_IN_OUT;
	//}

	//ImGui::Text("Move Time AxisY: %.2f    ", m_tArmData.fMoveTimeAxisY.x);
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(-1);
	//ImGui::DragFloat("##Move Time Y", &m_tArmData.fMoveTimeAxisY.x, 0.05f, 0.f, 10.f);

	//ImGui::Text("Move Time AxisRight: %.2f", m_tArmData.fMoveTimeAxisRight.x);
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(-1);
	//ImGui::DragFloat("##Move Time Right", &m_tArmData.fMoveTimeAxisRight.x, 0.05f, 0.f, 10.f);


	//ImGui::Text("Min RPS AxisY: %.2f      ", m_tArmData.fRotationPerSecAxisY.x);
	//ImGui::SameLine();
	//ImGui::DragFloat("##Min Rotation Per Sec AxisY", &m_tArmData.fRotationPerSecAxisY.x, 0.1f, -360.f, 360.f);
	//ImGui::Text("Max RPS AxisY: %.2f      ", m_tArmData.fRotationPerSecAxisY.y);
	//ImGui::SameLine();
	//ImGui::DragFloat("##Max Rotation Per Sec AxisY", &m_tArmData.fRotationPerSecAxisY.y, 0.1f, -360.f, 360.f);

	//ImGui::Text("Min RPS AxisRight: %.2f  ", m_tArmData.fRotationPerSecAxisRight.x);
	//ImGui::SameLine();
	//ImGui::DragFloat("##Min Rotation Per Sec AxisRight", &m_tArmData.fRotationPerSecAxisRight.x, 0.1f, -360.f, 360.f);
	//ImGui::Text("Max RPS AxisRight: %.2f  ", m_tArmData.fRotationPerSecAxisRight.y);
	//ImGui::SameLine();
	//ImGui::DragFloat("##Max Rotation Per Sec AxisRight", &m_tArmData.fRotationPerSecAxisRight.y, 0.1f, -360.f, 360.f);


	//ImGui::NewLine();

	// Sub Data
}
#pragma endregion

void CLevel_Camera_Tool_Client::Edit_Arm()
{
	if (m_ArmNames.size() <= 0)
		return;

	CCamera_Target* pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));

	pair<ARM_DATA*, SUB_DATA*>* pData = pTargetCamera->Get_ArmData(m_ArmNames[m_iSelectedArmNum]);

	if (nullptr == pData)
		return;

	*(*pData).first = m_tArmData;
	pData->first->fRotationPerSecAxisY = { XMConvertToRadians(m_tArmData.fRotationPerSecAxisY.x),   XMConvertToRadians(m_tArmData.fRotationPerSecAxisY.y) };
	pData->first->fRotationPerSecAxisRight = { XMConvertToRadians(m_tArmData.fRotationPerSecAxisRight.x),   XMConvertToRadians(m_tArmData.fRotationPerSecAxisRight.y) };

	// Sub Data
	if (true == m_isUseSubData) {
		if (nullptr != pData->second) {
			pData->second->fZoomTime = m_fZoomTime;
			pData->second->iZoomLevel = m_iZoomLevel - 1;
			pData->second->iZoomRatioType = m_iRatioType;

			pData->second->fAtOffsetTime = m_fAtOffsetTime;
			pData->second->vAtOffset = m_vNextAtOffset;
			pData->second->iAtRatioType = m_iAtOffsetRatioType;
		}
		else {
			SUB_DATA* pSubData = new SUB_DATA();

			pSubData->fZoomTime = m_fZoomTime;
			pSubData->iZoomLevel = m_iZoomLevel - 1;
			pSubData->iZoomRatioType = m_iRatioType;

			pSubData->fAtOffsetTime = m_fAtOffsetTime;
			pSubData->vAtOffset = m_vNextAtOffset;
			pSubData->iAtRatioType = m_iAtOffsetRatioType;

			pData->second = pSubData;
		}
	}
	else {
		// 아무 일도 하지 않음...아마도
	}
}


void CLevel_Camera_Tool_Client::Set_MovementInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Play Move");
	ImGui::Separator();

	if (m_ArmNames.size() > 0) {

		_string Name = m_pGameInstance->WStringToString(m_ArmNames[m_iSelectedArmNum]);

		ImGui::Text("Selected Arm Name: %s", Name.c_str());

		Show_ComboBox();

		if (ImGui::Button("Play Movement")) {
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_Target::MOVE_TO_NEXTARM);
			CCamera_Manager::GetInstance()->Set_NextArmData(m_ArmNames[m_iSelectedArmNum], 0);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset Arm, Length")) {
		CCamera_Target* pCameraTarget = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
		CCameraArm* pArm = pCameraTarget->Get_Arm();

		pArm->Set_Length(m_fResetLength);
		pArm->Set_ArmVector(XMLoadFloat3(&m_vResetArm));

		pCameraTarget->Set_AtOffset(XMLoadFloat3(&m_vResetAtOffset));
		pCameraTarget->Set_ZoomLevel(5);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Arm, Length_BYFIRST")) {
		CCamera_Target* pCameraTarget = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
		CCameraArm* pArm = pCameraTarget->Get_Arm();

		pArm->Set_Length(12.f);
		pArm->Set_ArmVector(XMLoadFloat3(&m_vFirstResetArm));

		pCameraTarget->Set_AtOffset(XMVectorSet(0.f, 3.f, 0.f, 0.f));
		pCameraTarget->Set_ZoomLevel(5);
	}
}

void CLevel_Camera_Tool_Client::Set_Zoom()
{
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             Zoom Info");
	ImGui::Separator();

	ImGui::Text("Next Zoom Level: %d    ", m_iZoomLevel);
	ImGui::SameLine();
	ImGui::InputInt("##Zoom Level", &m_iZoomLevel);

	if (m_iZoomLevel <= 0)
		ImGui::Text("Next Fovy: %.2f", 0.f);
	else
		ImGui::Text("Next Fovy: %.2f", m_fFovys[m_iZoomLevel - 1]);

	ImGui::Text("Zoom Time: %.2f       ", m_fZoomTime);
	ImGui::SameLine();
	ImGui::DragFloat("##Zoom Time", &m_fZoomTime, 0.05f, 0.f, 10.f);

	switch (m_iRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN   ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT  ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP      ");
		break;

	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT      ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("EASE IN")) {
		m_iRatioType = RATIO_TYPE::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("EASE OUT")) {
		m_iRatioType = RATIO_TYPE::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("LERP")) {
		m_iRatioType = RATIO_TYPE::LERP;
	}
	ImGui::SameLine();
	if (ImGui::Button("EASE_IN_OUT")) {
		m_iRatioType = RATIO_TYPE::EASE_IN_OUT;
	}

	ImGui::NewLine();
	if (ImGui::Button("Start Zoom")) {
		CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, m_fZoomTime, m_iZoomLevel - 1, m_iRatioType);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Fovy")) {
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET);
		pCamera->Set_ZoomLevel(CCamera::LEVEL_6);
		pCamera->Set_Fovy(XMConvertToRadians(m_fFovys[CCamera::LEVEL_6]));
	}


}

void CLevel_Camera_Tool_Client::Set_AtOffsetInfo()
{
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             AtOffset Info");
	ImGui::Separator();

	ImGui::Text("AtOffset: %.2f, %.2f, %.2f", m_vNextAtOffset.x, m_vNextAtOffset.y, m_vNextAtOffset.z);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##X", &m_vNextAtOffset.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##Y", &m_vNextAtOffset.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##Z", &m_vNextAtOffset.z);

	ImGui::Text("AtOffset Time: %.2f       ", m_fAtOffsetTime);
	ImGui::SameLine();
	ImGui::DragFloat("##AtOffset Time", &m_fAtOffsetTime, 0.05f, 0.f, 10.f);

	switch (m_iAtOffsetRatioType) {
	case RATIO_TYPE::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case RATIO_TYPE::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case RATIO_TYPE::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;

	case RATIO_TYPE::EASE_IN_OUT:
		ImGui::Text("Ratio Type: EASE_IN_OUT          ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("OFFSET EASE IN")) {
		m_iAtOffsetRatioType = RATIO_TYPE::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("OFFSET EASE OUT")) {
		m_iAtOffsetRatioType = RATIO_TYPE::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("OFFSET LERP")) {
		m_iAtOffsetRatioType = RATIO_TYPE::LERP;
	}
	ImGui::SameLine();
	if (ImGui::Button("OFFSET EASE_IN_OUT")) {
		m_iAtOffsetRatioType = RATIO_TYPE::EASE_IN_OUT;
	}

	ImGui::NewLine();
	if (ImGui::Button("Start Change AtOffset")) {
		CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET, m_fAtOffsetTime, XMLoadFloat3(&m_vNextAtOffset), m_iAtOffsetRatioType);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset AtOffset")) {
		CCamera_Manager::GetInstance()->Start_Changing_AtOffset(CCamera_Manager::TARGET, 1.f, XMVectorSet(0.f, 3.f, 0.f, 0.f), RATIO_TYPE::LERP);
	}


}

void CLevel_Camera_Tool_Client::Set_ShakeInfo()
{
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             Shake Info");
	ImGui::Separator();

	ImGui::Text("Shake Force: %.2f       ", m_fShakeForce);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);
	ImGui::DragFloat("##ShakeForce", &m_fShakeForce, 1.f, 0.f, 10.f);

	ImGui::SameLine();
	ImGui::Text("Shake Time: %.2f", m_fShakeTime);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);
	ImGui::DragFloat("##ShakeTime", &m_fShakeTime, 0.05f, 0.f, 20.f);

	ImGui::Text("Shake Cycle Time: %.2f  ", m_fShakeCycleTime);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);
	ImGui::DragFloat("##ShakeCycleTime", &m_fShakeCycleTime, 0.05f, 0.f, 5.f);

	ImGui::SameLine();
	ImGui::Text("Shake Count: %d  ", m_iShakeCount);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);
	ImGui::DragInt("##ShakeCount", &m_iShakeCount, 0.05f, 0, 200);

	ImGui::Text("Shake Delay Time: %.2f  ", m_fShakeDelayTime);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);
	ImGui::DragFloat("##ShakeDelayTime", &m_fShakeDelayTime, 0.05f, 0.f, 10.f);

	switch (m_iShakeType) {
	case CCamera::SHAKE_XY:
		ImGui::Text("Shake Type: SHAKE_XY    ");
		break;

	case CCamera::SHAKE_X:
		ImGui::Text("Shake Type: SHAKE_X     ");
		break;

	case CCamera::SHAKE_Y:
		ImGui::Text("Shake Type: SHAKE_Y     ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("SHAKE_XY")) {
		m_iShakeType = CCamera::SHAKE_XY;
	}
	ImGui::SameLine();
	if (ImGui::Button("SHAKE_X")) {
		m_iShakeType = CCamera::SHAKE_X;
	}
	ImGui::SameLine();
	if (ImGui::Button("SHAKE_Y")) {
		m_iShakeType = CCamera::SHAKE_Y;
	}

	switch (m_iCycleType) {
	case (_uint)CYCLE_TYPE::TIME:
		ImGui::Text("Cycle Type: Time        ");
		break;

	case (_uint)CYCLE_TYPE::COUNT:
		ImGui::Text("Cycle Type: Count       ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("TIME")) {
		m_iCycleType = (_uint)CYCLE_TYPE::TIME;
	}
	ImGui::SameLine();
	if (ImGui::Button("COUNT")) {
		m_iCycleType = (_uint)CYCLE_TYPE::COUNT;
	}

	ImGui::NewLine();
	if (ImGui::Button("Start Shake")) {

		switch ((_uint)m_iCycleType) {
		case (_uint)CYCLE_TYPE::TIME:
			CCamera_Manager::GetInstance()->Start_Shake_ByTime(CCamera_Manager::TARGET, m_fShakeTime, m_fShakeForce, m_fShakeCycleTime, m_iShakeType, m_fShakeDelayTime);
			break;
		case (_uint)CYCLE_TYPE::COUNT:
			CCamera_Manager::GetInstance()->Start_Shake_ByCount(CCamera_Manager::TARGET, m_fShakeTime, m_fShakeForce, m_iShakeCount, m_iShakeType, m_fShakeDelayTime);
			break;
		}
	}
}

void CLevel_Camera_Tool_Client::Set_CurInfo()
{
	CCamera_Target* pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));

	pair<ARM_DATA*, SUB_DATA*>* pData = pTargetCamera->Get_ArmData(m_ArmNames[m_iSelectedArmNum]);

	if (nullptr == pData)
		return;

	m_tArmData = *(pData->first);

	m_tArmData.fRotationPerSecAxisRight = { XMConvertToDegrees(m_tArmData.fRotationPerSecAxisRight.x), XMConvertToDegrees(m_tArmData.fRotationPerSecAxisRight.y) };
	m_tArmData.fRotationPerSecAxisY = { XMConvertToDegrees(m_tArmData.fRotationPerSecAxisY.x), XMConvertToDegrees(m_tArmData.fRotationPerSecAxisY.y) };

	if (nullptr == pData->second)
		return;

	m_fZoomTime = pData->second->fZoomTime;
	m_iZoomLevel = pData->second->iZoomLevel + 1;
	m_iRatioType = pData->second->iZoomRatioType;

	m_fAtOffsetTime = pData->second->fAtOffsetTime;
	m_vNextAtOffset = pData->second->vAtOffset;
	m_iAtOffsetRatioType = pData->second->iAtRatioType;
}

void CLevel_Camera_Tool_Client::Create_KeyFrame()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			if (false == m_isCreatePoint)
				return;

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();

			if (0 > fMousePos.x || g_iWinSizeX < fMousePos.x || 0 > fMousePos.y || g_iWinSizeY < fMousePos.y)
				return;

			_vector vRayPos, vRayDir;

			Get_RayInfo(&vRayPos, &vRayDir);

			_float fDist = {};

			for (auto& Point : m_KeyFrames) {
				CModelObject* pModelObject = dynamic_cast<CModelObject*>(Point.second);

				if (true == pModelObject->Is_PickingCursor_Model(fMousePos, fDist)) {
					return;
				}
			}

			CUTSCENE_KEYFRAME tFrame = m_tKeyFrameInfo;
			CGameObject* pCube = Create_Cube();

			if (nullptr == pCube)
				return;

			m_KeyFrames.push_back({ tFrame, pCube });
		}
	}
}

CGameObject* CLevel_Camera_Tool_Client::Create_Cube()
{
	/* Test Terrain */
	CModelObject::MODELOBJECT_DESC Desc{};

	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;

	Desc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	Desc.strModelPrototypeTag_3D = TEXT("alphabet_blocks_d_mesh");

	Desc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	Desc.iPriorityID_3D = PR3D_BLEND;
	Desc.iRenderGroupID_3D = RG_3D;

	Desc.tTransform3DDesc.vInitialPosition = m_tKeyFrameInfo.vPosition;
	Desc.tTransform3DDesc.vInitialScaling = _float3(0.1f, 0.1f, 0.1f);
	Desc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	Desc.tTransform3DDesc.fSpeedPerSec = 0.f;

	CGameObject* pCube;

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Cube"), LEVEL_CAMERA_TOOL, TEXT("Layer_Cube"), &pCube, &Desc);
	Safe_AddRef(pCube);

	return pCube;
}

void CLevel_Camera_Tool_Client::Edit_KeyFrame()
{
	if (KEY_PRESSING(KEY::SPACE)) {
		if (nullptr == m_pCurKeyFrame)
			return;

		if (true == m_isMaintainOriginPos)
			return;

		CController_Transform* pController = m_pCurKeyFrame->second->Get_ControllerTransform();

		pController->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tKeyFrameInfo.vPosition), 1.f));
		m_pCurKeyFrame->first.vPosition = m_tKeyFrameInfo.vPosition;
	}

	ImGui::SameLine();

	if (ImGui::Button("Edit Cur FrameInfo")) {

		if (nullptr == m_pCurKeyFrame)
			return;

		if (false == m_isMaintainOriginPos)
			m_pCurKeyFrame->first.vPosition = m_tKeyFrameInfo.vPosition;
		m_pCurKeyFrame->first.fTimeStamp = m_tKeyFrameInfo.fTimeStamp;
		m_pCurKeyFrame->first.iZoomLevel = m_tKeyFrameInfo.iZoomLevel;
		m_pCurKeyFrame->first.iZoomRatioType = m_tKeyFrameInfo.iZoomRatioType;
		m_pCurKeyFrame->first.vAt = m_tKeyFrameInfo.vAt;
		m_pCurKeyFrame->first.bLookTarget = m_tKeyFrameInfo.bLookTarget;
		m_pCurKeyFrame->first.iAtRatioType = m_tKeyFrameInfo.iAtRatioType;

		if (false == m_isMaintainOriginPos) {
			CController_Transform* pController = m_pCurKeyFrame->second->Get_ControllerTransform();
			pController->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tKeyFrameInfo.vPosition), 1.f));
		}
	}
}

void CLevel_Camera_Tool_Client::Delete_KeyFrame()
{
	if (KEY_PRESSING(KEY::LSHIFT)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();

			CGameObject* pCube = m_pGameInstance->Get_PickingModelObjectByCursor(LEVEL_CAMERA_TOOL, TEXT("Layer_Cube"), fMousePos);

			if (nullptr == pCube)
				return;

			for (auto iter = m_KeyFrames.begin(); iter != m_KeyFrames.end();) {

				if (pCube == (*iter).second) {

					// Cur 없애기
					if (nullptr != m_pCurKeyFrame) {
						if (pCube == m_pCurKeyFrame->second) {
							Safe_Release(m_pCurKeyFrame->second);
							m_pCurKeyFrame = nullptr;
						}
					}

					Safe_Release((*iter).second);
					Event_DeleteObject((*iter).second);
					iter = m_KeyFrames.erase(iter);
				}
				else
					++iter;
			}
		}
	}
}

void CLevel_Camera_Tool_Client::Set_CurrentKeyFrame()
{
	if (KEY_PRESSING(KEY::ALT)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();

			CGameObject* pCube = m_pGameInstance->Get_PickingModelObjectByCursor(LEVEL_CAMERA_TOOL, TEXT("Layer_Cube"), fMousePos);

			if (nullptr == pCube)
				return;

			for (auto& KeyFrame : m_KeyFrames) {
				if (KeyFrame.second == pCube) {
					if (m_pCurKeyFrame != nullptr)
						Safe_Release(m_pCurKeyFrame->second);
					m_pCurKeyFrame = &KeyFrame;
					Safe_AddRef(m_pCurKeyFrame->second);
				}
			}
		}
	}
}

void CLevel_Camera_Tool_Client::Set_CameraInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Camera Info");
	ImGui::Separator();

	// Camera 정보
	CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::FREE);
	_vector vPos = pCamera->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);

	ImGui::Text("Pos: % .2f, % .2f, % 2.f", XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
	ImGui::SameLine(0, 10.0f);
	if (ImGui::Button("Move To Clicked Pos")) {
		pCamera->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tKeyFrameInfo.vPosition), 1.f));
	}

	ImGui::Text("Input Position: %.2f, %.2f, %.2f", m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InputPosX", &m_vCameraPos.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InputPosY", &m_vCameraPos.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InputPosZ", &m_vCameraPos.z, 0.1f);

	ImGui::SameLine(0, 10.0f);
	if (ImGui::Button("Move To Input Pos")) {
		pCamera->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vCameraPos), 1.f));
	}


	ImGui::NewLine();

	ImGui::Text("Zoom Level(+1 X): %d", pCamera->Get_CurrentZoomLevel());
	ImGui::SameLine();
	ImGui::InputInt("##Zoom Level", &m_iZoomLevel);
	pCamera->Set_ZoomLevel(m_iZoomLevel);
	ImGui::Text("Fovy: %.2f", XMConvertToDegrees(pCamera->Get_Fovy()));

	ImGui::NewLine();

	ImGui::Text("Get Cur Camera LookAt");

	_vector vLook = pCamera->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);
	_vector vAt = vPos + vLook;

	ImGui::Text("vAt(vPos + vLook): %.2f, %.2f, %.2f", XMVectorGetX(vAt), XMVectorGetY(vAt), XMVectorGetZ(vAt));
	ImGui::SameLine();
	if (ImGui::Button("Set Cur At To Dummy")) {
		XMStoreFloat3(&m_vInitialLookAt, vAt);
	}


	ImGui::Text("Input At: %.2f, %.2f, %.2f", m_DummyAt.x, m_DummyAt.y, m_DummyAt.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InputAtX", &m_DummyAt.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InputAtsY", &m_DummyAt.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##InputAtZ", &m_DummyAt.z, 0.1f);

	ImGui::SameLine();
	if (ImGui::Button("Set At To Dummy")) {
		m_vInitialLookAt = m_DummyAt;
		pCamera->Get_ControllerTransform()->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_vInitialLookAt), 1.f));
	}

	ImGui::NewLine();
	ImGui::Text("Fixed LookAt: %.2f, %.2f, %.2f", m_vInitialLookAt.x, m_vInitialLookAt.y, m_vInitialLookAt.z);

	//_vector vNewLook = XMVector3Normalize(XMLoadFloat3(&m_vInitialLookAt) - vPos);
	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vInitialLookAt) - vPos));

	_vector vNewAt = vPos + (vLook * fDistance);

	//XMStoreFloat3(&m_vNextAtOffset, vNewAt - XMLoadFloat3(&m_vInitialLookAt));

	ImGui::Text("Cur At: %.2f, %.2f, %.2f", XMVectorGetX(vNewAt), XMVectorGetY(vNewAt), XMVectorGetZ(vNewAt));

	if (ImGui::Button("Create KeyFrame By Camera Pos")) {
		XMStoreFloat3(&m_tKeyFrameInfo.vPosition, pCamera->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION));
		CUTSCENE_KEYFRAME tFrame = m_tKeyFrameInfo;
		CGameObject* pCube = Create_Cube();

		XMStoreFloat3(&tFrame.vAt, vNewAt);
		
		if (nullptr == pCube)
			return;

		m_KeyFrames.push_back({ tFrame, pCube });
	}
}

void CLevel_Camera_Tool_Client::Create_Sector()
{
	ImGui::SameLine();

	if (ImGui::Button("Cur KeyFrame To Sector")) {

		if (nullptr == m_pCurKeyFrame)
			return;

		for (auto& SelectedFrame : m_SelectedKeyFrame) {

			_float fEpsilon = 0.01f;

			if (m_pCurKeyFrame->first.fTimeStamp < SelectedFrame.fTimeStamp + fEpsilon &&
				m_pCurKeyFrame->first.fTimeStamp > SelectedFrame.fTimeStamp - fEpsilon)
				return;
		}

		m_SelectedKeyFrame.push_back(m_pCurKeyFrame->first);
		
	}

	switch (m_iSectorType) {
	case CCutScene_Sector::SPLINE:
		ImGui::Text("Sector Type: SPLINE       ");
		break;

	case CCutScene_Sector::LINEAR:
		ImGui::Text("Sector Type: LINEAR      ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("SPLINE")) {
		m_iSectorType = CCutScene_Sector::SPLINE;
	}
	ImGui::SameLine();
	if (ImGui::Button("LINEAR")) {
		m_iSectorType = CCutScene_Sector::LINEAR;
	}

	ImGui::Text("Sector Duration:   %.2f    ", m_fSectorDuration);
	ImGui::SameLine();
	ImGui::DragFloat("##SectorDurationS   ", &m_fSectorDuration, 0.1f, 0.0f, 40.f);

	ImGui::Text("Delete Sector Num:   %d    ", m_iDeleteSectorNum);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	ImGui::InputInt("##Delete Sector Num   ", &m_iDeleteSectorNum);

	ImGui::Text("CutScene Tag Input:       ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputText("##CutScene Tag", m_szCutSceneTag, MAX_PATH);

	ImGui::Text("CutScene Next CameraType: ", m_tCutSceneSubData.iNextCameraType);
	ImGui::SameLine();
	if (ImGui::Button("2D")) {
		m_tCutSceneSubData.iNextCameraType = CCamera_Manager::TARGET_2D;
	}
	ImGui::SameLine();
	if (ImGui::Button("TARGET")) {
		m_tCutSceneSubData.iNextCameraType = CCamera_Manager::TARGET;
	}

	ImGui::NewLine();

	if (m_CutScenes.size() > 0) {
		_string Name = m_pGameInstance->WStringToString(m_CutSceneTags[m_iSelectedCutSceneNum]);
		ImGui::Text("CutScene Tag: %s  ", Name.c_str());
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	Show_CutSceneComboBox();

	ImGui::NewLine();

	if (ImGui::Button("Create Sector")) {

		if (m_SelectedKeyFrame.size() <= 0)
			return;

		CCutScene_Sector::CUTSCENE_SECTOR_DESC Desc{};

		Desc.iSectorType = m_iSectorType;
		Desc.fSectorDuration = m_fSectorDuration;

		CCutScene_Sector* pSector = CCutScene_Sector::Create(m_pDevice, m_pContext, &Desc);

		if (nullptr == pSector)
			return;

		for (auto& KeyFrame : m_SelectedKeyFrame) {
			KeyFrame.iZoomLevel -= 1;
			pSector->Add_KeyFrame(KeyFrame);
		}

		pSector->Sort_Sector();

		_bool isSameTag = false;

		for (auto& CutScene : m_CutScenes) {
			if (CutScene.first == m_pGameInstance->StringToWString(m_szCutSceneTag)) {
				isSameTag = true;
			}
		}

		if (true == isSameTag) {
			auto iter = m_CutScenes.find(m_pGameInstance->StringToWString(m_szCutSceneTag));
			iter->second.push_back(pSector);
	/*		auto SubDataiter = m_CutSceneSubDatas.find(m_pGameInstance->StringToWString(m_szCutSceneTag));
			SubDataiter->second = m_tCutSceneSubData;*/
		}
		else {
			vector<CCutScene_Sector*> vecSector;
			vecSector.push_back(pSector);

			m_CutScenes.emplace(m_pGameInstance->StringToWString(m_szCutSceneTag), vecSector);
			m_CutSceneSubDatas.emplace(m_pGameInstance->StringToWString(m_szCutSceneTag), m_tCutSceneSubData);
			m_CutSceneTags.push_back(m_pGameInstance->StringToWString(m_szCutSceneTag));
		}

		m_SelectedKeyFrame.clear();
	}
	ImGui::SameLine();

	if (ImGui::Button("Delete Sector")) {

		if (m_CutSceneTags.size() <= 0)
			return;

		for (auto& CutScene : m_CutScenes) {
			if (CutScene.first == m_CutSceneTags[m_iSelectedCutSceneNum]) {

				if (m_iDeleteSectorNum >= CutScene.second.size())
					return;

				Safe_Release(CutScene.second[m_iDeleteSectorNum]);
				CutScene.second.erase(CutScene.second.begin() + m_iDeleteSectorNum);

				return;
			}
		}

	}

	ImGui::SameLine();

	if (ImGui::Button("Delete Selected Frames")) {
		m_SelectedKeyFrame.clear();
	}
}

void CLevel_Camera_Tool_Client::Edit_Sector()
{
	ImGui::NewLine();

	ImGui::Text("Edit Sector Num:   %d      ", m_iEditSectorNum);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	ImGui::InputInt("##Edit Sector Num   ", &m_iEditSectorNum);

	ImGui::Text("Edit Frame Num:   %d       ", m_iEditFrameNum);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	ImGui::InputInt("##Edit Frame Num   ", &m_iEditFrameNum);

	if (ImGui::Button("Edit Sector Frames")) {
		for (auto& CutScene : m_CutScenes) {
			if (CutScene.first == m_CutSceneTags[m_iSelectedCutSceneNum]) {

				// (CutScene.second)[m_iEditSectorNum] -> 수정하기로 한 Sector
				vector<CUTSCENE_KEYFRAME>* pKeyFrames = (CutScene.second)[m_iEditSectorNum]->Get_KeyFrames();
				_float3 vOriginPos = (*pKeyFrames)[m_iEditFrameNum].vPosition;

				(*pKeyFrames)[m_iEditFrameNum] = m_tKeyFrameInfo;

				if (true == m_isMaintainOriginPos)
					(*pKeyFrames)[m_iEditFrameNum].vPosition = vOriginPos;
			}
		}
	}
}

void CLevel_Camera_Tool_Client::Picking()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {
			_vector vRayPos, vRayDir;

			Get_RayInfo(&vRayPos, &vRayDir);

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();
			_float fDist = { 99999999.f };
			_vector vClickedPos = {};

			if (0 > fMousePos.x || g_iWinSizeX < fMousePos.x || 0 > fMousePos.y || g_iWinSizeY < fMousePos.y)
				return;

			CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_CAMERA_TOOL, TEXT("Layer_MapObject"));
			list<CGameObject*> Objects = pLayer->Get_GameObjects();

			CGameObject* pGameObject = nullptr;

			for (auto& GameObject : Objects) {
				CModelObject* pModelObject = dynamic_cast<CModelObject*>(GameObject);
				_float fTemp = {};
				_bool isPicked = pModelObject->Is_PickingCursor_Model_Test(fMousePos, fTemp);

				if (true == isPicked) {
					if (fDist > fTemp) {
						fDist = fTemp;
						vClickedPos = vRayPos + (vRayDir * fDist);
					}
					//vClickedPos = vRayPos + (vRayDir * fDist);
				}
			}

			pLayer = m_pGameInstance->Find_Layer(LEVEL_CAMERA_TOOL, TEXT("Layer_Room_Environment"));

			if (nullptr == pLayer)
				return;

			Objects = pLayer->Get_GameObjects();

			pGameObject = nullptr;

			for (auto& GameObject : Objects) {
				CModelObject* pModelObject = dynamic_cast<CModelObject*>(GameObject);
				_float fTemp = {};
				_bool isPicked = pModelObject->Is_PickingCursor_Model_Test(fMousePos, fTemp);

				if (true == isPicked) {
					if (fDist > fTemp) {
						fDist = fTemp;
						vClickedPos = vRayPos + (vRayDir * fDist);
					}
				}
			}

			XMStoreFloat3(&m_tKeyFrameInfo.vPosition, vClickedPos);

			/*pLayer = m_pGameInstance->Find_Layer(LEVEL_CAMERA_TOOL, TEXT("Layer_Cube"));

			if (nullptr == pLayer)
				return;

			Objects = pLayer->Get_GameObjects();

			pGameObject = nullptr;

			for (auto& GameObject : Objects) {
				CModelObject* pModelObject = dynamic_cast<CModelObject*>(GameObject);
				_bool isPicked = pModelObject->Is_PickingCursor_Model_Test(fMousePos, fDist);

				if (true == isPicked) {
					vClickedPos = vRayPos + (vRayDir * fDist);

					XMStoreFloat3(&m_tKeyFrameInfo.vPosition, vClickedPos);
				}
			}*/
		}
	}
}

void CLevel_Camera_Tool_Client::Get_RayInfo(_vector* _pRayPos, _vector* _pRayDir)
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

	*_pRayPos = vRayPos;
	*_pRayDir = vRayDir;
}

void CLevel_Camera_Tool_Client::Play_CutScene(_float fTimeDelta)
{
	if (m_CutScenes.size() <= 0)
		return;

	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             Play CutScene");
	ImGui::Separator();

	if (ImGui::Button("Add CutScene Data To Camera")) {
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE);

		if (nullptr == pCamera)
			return;

		static_cast<CCamera_CutScene_Save*>(pCamera)->CutScene_Clear();

		for (auto& CutScene : m_CutScenes) {

			auto Subiter = m_CutSceneSubDatas.find(CutScene.first);

			static_cast<CCamera_CutScene_Save*>(pCamera)->Add_CutScene(CutScene.first, CutScene.second, (*Subiter).second);
		}
	}

	// Initial Data
	Set_InitialData();

	if (ImGui::Button("Simulation")) {
		m_isSimulation = true;
		m_pSimulationCube->Set_Active(true);
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE);

		if (nullptr == pCamera)
			return;

		static_cast<CCamera_CutScene_Save*>(pCamera)->Set_NextCutScene(m_CutSceneTags[m_iSelectedCutSceneNum]);
		static_cast<CCamera_CutScene_Save*>(pCamera)->Set_IsSimulation(true);
		m_pSimulationCube->Set_Active(true);
	}

	// 움직이기
	if (true == m_isSimulation) {
		CCamera_CutScene_Save* pCamera = static_cast<CCamera_CutScene_Save*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE));

		if (nullptr == pCamera)
			return;

		pCamera->Late_Update(fTimeDelta);
		_float3 vSimulationPos = pCamera->Get_SimulationPos();

		CController_Transform* pTransform = m_pSimulationCube->Get_ControllerTransform();
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vSimulationPos), 1.f));

		if (true == pCamera->Get_IsFinish()) {
			m_isSimulation = false;
			m_pSimulationCube->Set_Active(false);
			pCamera->Set_IsSimulation(false);
			pCamera->Set_IsFinish(false);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Play CutScene")) {

		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
		//CCamera_Manager::GetInstance()->Set_NextCutScene(m_CutSceneTags[m_iSelectedCutSceneNum]);
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE);

		if (nullptr == pCamera)
			return;

		static_cast<CCamera_CutScene_Save*>(pCamera)->Set_NextCutScene(m_CutSceneTags[m_iSelectedCutSceneNum]);

		for (_int i = 0; i < 3; ++i) {
			if (true == m_isSelectModel[i]) {
				m_ModelObjects[i]->Set_AnimationLoop(COORDINATE_3D, m_iAnim[i], m_isLoop);
				m_ModelObjects[i]->Set_Animation(m_iAnim[i]);

				if (2 == i) {
					CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CAMERA_TOOL, TEXT("Layer_Player"), 0));
					pPlayer->Get_ActorCom()->Set_GlobalPose({ 2.94290805f, 0.351999998f, -21.1068630f });
					pPlayer->Set_Kinematic(true);
					pPlayer->LookDirectionXZ_Kinematic(_vector{ 0,0,-1 });
				}
			}
		}

		m_fAnimTime = 0.f;
	}

	ImGui::SameLine();
	if (ImGui::Button("Play CutScene Initialize")) {
		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::CUTSCENE);
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE);

		if (nullptr == pCamera)
			return;

		static_cast<CCamera_CutScene_Save*>(pCamera)->Set_NextCutScene(m_CutSceneTags[m_iSelectedCutSceneNum], &m_tInitialData);
	}

}

void CLevel_Camera_Tool_Client::Save_Data_CutScene()
{
	if (true == m_isForClient) {
		CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CUTSCENE);

		if (nullptr == pCamera)
			return;

		map<_wstring, pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA>>>* pData = static_cast<CCamera_CutScene_Save*>(pCamera)->Get_CutSceneDatas();

		if (nullptr == pData)
			return;

		json Result = json::array();

		for (auto& CutSceneData : *pData) {

			json CutScene_json;

			// CutScene Tag 저장
			_string szCutSceneTag = m_pGameInstance->WStringToString(CutSceneData.first);
			CutScene_json["CutScene_Tag"] = szCutSceneTag;

			// Total Time 저장
			CutScene_json["CutScene_Time"] = { CutSceneData.second.first.fTotalTime.x, CutSceneData.second.first.fTotalTime.y };

			// NextCameraType 저장
			CutScene_json["CutScene_Next_CameraType"] = CutSceneData.second.first.iNextCameraType;

			// Data Struct 저장
			CutScene_json["Datas"] = json::array();
			for (auto& Data : CutSceneData.second.second) {
				json Data_json;

				Data_json["Position"] = { Data.vPosition.x, Data.vPosition.y, Data.vPosition.z };
				Data_json["Rotation"] = { Data.vRotation.x, Data.vRotation.y, Data.vRotation.z };
				Data_json["At"] = { Data.vAt.x, Data.vAt.y, Data.vAt.z };
				Data_json["Fovy"] = Data.fFovy;

				CutScene_json["Datas"].push_back(Data_json);
			}

			Result.push_back(CutScene_json);
		}

		_wstring wszSavePath = L"../../Client/Bin/DataFiles/Camera/CutSceneData/";
		_wstring wszSaveName = m_pGameInstance->StringToWString(m_szSaveName);

		ofstream file(wszSavePath + wszSaveName + TEXT(".json"));

		if (!file.is_open())
		{
			MSG_BOX("파일을 저장할 수 없습니다.");
			file.close();
			return;
		}

		file << Result.dump(4);
		file.close();


		_wstring FullPath = wszSavePath + wszSaveName + TEXT(".json");
		m_JsonFilePaths.push_back(m_pGameInstance->WStringToString(FullPath));
	}
	else {

		json Result;

		// KeyFrame
		Result["KeyFrame"] = json::array();

		for (auto& KeyFrame : m_KeyFrames) {

			json KeyFrame_json;

			KeyFrame_json["Position"] = { KeyFrame.first.vPosition.x, KeyFrame.first.vPosition.y, KeyFrame.first.vPosition.z };
			KeyFrame_json["TimeStamp"] = KeyFrame.first.fTimeStamp;
			KeyFrame_json["Zoom_Level"] = KeyFrame.first.iZoomLevel;
			KeyFrame_json["Zoom_RatioType"] = KeyFrame.first.iZoomRatioType;
			KeyFrame_json["At"] = { KeyFrame.first.vAt.x, KeyFrame.first.vAt.y, KeyFrame.first.vAt.z };
			KeyFrame_json["Is_LookTarget"] = KeyFrame.first.bLookTarget;
			KeyFrame_json["At_RatioType"] = KeyFrame.first.iAtRatioType;

			Result["KeyFrame"].push_back(KeyFrame_json);
		}

		// CutScene
		Result["CutScene"] = json::array();

		for (auto& CutScene : m_CutScenes) {

			json CutScene_json;

			// CutScene Tag
			CutScene_json["CutScene_Tag"] = m_pGameInstance->WStringToString(CutScene.first);

			auto& SubDataiter = m_CutSceneSubDatas.find(CutScene.first);

			// NextCameraType 저장
			CutScene_json["CutScene_Next_CameraType"] = (*SubDataiter).second.iNextCameraType;

			// Sector
			CutScene_json["Sector"] = json::array();
			for (auto& Sector : CutScene.second) {

				json Sector_json;

				vector<CUTSCENE_KEYFRAME>* pKeyFrames = Sector->Get_KeyFrames();

				// Sector Type 저장(Spline, Linear)
				Sector_json["Sector_Type"] = Sector->Get_SectorType();

				// Sector Duration 저장
				Sector_json["Sector_Duration"] = Sector->Get_SectorDuration();

				// KeyFrame
				Sector_json["KeyFrames"] = json::array();
				for (auto& KeyFrame : *pKeyFrames) {

					json KeyFrame_json;

					KeyFrame_json["Position"] = { KeyFrame.vPosition.x, KeyFrame.vPosition.y,KeyFrame.vPosition.z };
					KeyFrame_json["TimeStamp"] = KeyFrame.fTimeStamp;
					KeyFrame_json["Zoom_Level"] = KeyFrame.iZoomLevel;
					KeyFrame_json["Zoom_RatioType"] = KeyFrame.iZoomRatioType;
					KeyFrame_json["At"] = { KeyFrame.vAt.x, KeyFrame.vAt.y,KeyFrame.vAt.z };
					KeyFrame_json["Is_LookTarget"] = KeyFrame.bLookTarget;
					KeyFrame_json["At_RatioType"] = KeyFrame.iAtRatioType;

					Sector_json["KeyFrames"].push_back(KeyFrame_json);
				}

				CutScene_json["Sector"].push_back(Sector_json);
			}

			Result["CutScene"].push_back(CutScene_json);
		}

		_wstring wszSavePath = L"../../Client/Bin/DataFiles/Camera/CutSceneData/Tool/";
		_wstring wszSaveName = m_pGameInstance->StringToWString(m_szSaveName);

		ofstream file(wszSavePath + wszSaveName + TEXT(".json"));

		if (!file.is_open())
		{
			MSG_BOX("파일을 저장할 수 없습니다.");
			file.close();
			return;
		}

		file << Result.dump(4);
		file.close();

		_wstring FullPath = wszSavePath + wszSaveName + TEXT(".json");
		m_JsonFilePaths.push_back(m_pGameInstance->WStringToString(FullPath));
	}
}

void CLevel_Camera_Tool_Client::Save_Data_Arm()
{
	json Result = json::array();

	// Arm Data
	CCamera_Target* pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
	map<_wstring, pair<ARM_DATA*, SUB_DATA*>>* pDatas = pTargetCamera->Get_ArmDatas();

	for (auto& Data : *pDatas) {

		json Trigger_json;

		// Arm Data
		Trigger_json["Arm_Tag"] = m_pGameInstance->WStringToString(Data.first);
		Trigger_json["Arm_Data"]["Arm_Length"] = Data.second.first->fLength;
		Trigger_json["Arm_Data"]["Arm_Time"] = { Data.second.first->fLengthTime.x, Data.second.first->fLengthTime.y };
		Trigger_json["Arm_Data"]["Arm_Length_RatioType"] = Data.second.first->iLengthRatioType;

		Trigger_json["Arm_Data"]["Arm_MoveTime_AxisY"] = { Data.second.first->fMoveTimeAxisY.x, Data.second.first->fMoveTimeAxisY.y };
		Trigger_json["Arm_Data"]["Arm_MoveTime_AxisRight"] = { Data.second.first->fMoveTimeAxisRight.x, Data.second.first->fMoveTimeAxisRight.y };
		Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisY"] = { Data.second.first->fRotationPerSecAxisY.x, Data.second.first->fRotationPerSecAxisY.y };
		Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisRight"] = { Data.second.first->fRotationPerSecAxisRight.x, Data.second.first->fRotationPerSecAxisRight.y };

		Trigger_json["Arm_Data"]["Desire_Arm"] = { Data.second.first->vDesireArm.x, Data.second.first->vDesireArm.y, Data.second.first->vDesireArm.z };

		// Sub Data
		_bool IsUseSubData = { true };

		if (nullptr == Data.second.second)
			IsUseSubData = false;

		Trigger_json["Sub_Data"]["Use_SubData"] = IsUseSubData;

		if (true == IsUseSubData) {
			Trigger_json["Sub_Data"]["Zoom_Time"] = Data.second.second->fZoomTime;
			Trigger_json["Sub_Data"]["Zoom_Level"] = Data.second.second->iZoomLevel;
			Trigger_json["Sub_Data"]["Zoom_RatioType"] = Data.second.second->iZoomRatioType;

			Trigger_json["Sub_Data"]["AtOffset_Time"] = Data.second.second->fAtOffsetTime;
			Trigger_json["Sub_Data"]["AtOffset"] = { Data.second.second->vAtOffset.x, Data.second.second->vAtOffset.y, Data.second.second->vAtOffset.z };
			Trigger_json["Sub_Data"]["AtOffset_RatioType"] = Data.second.second->iAtRatioType;
		}

		Result.push_back(Trigger_json);
	}

	_wstring wszSavePath = L"../Bin/DataFiles/Camera/ArmData/";
	_wstring wszSaveName = m_pGameInstance->StringToWString(m_szSaveName);

	ofstream file(wszSavePath + wszSaveName + TEXT(".json"));

	if (!file.is_open())
	{
		MSG_BOX("파일을 저장할 수 없습니다.");
		file.close();
		return;
	}

	file << Result.dump(4);
	file.close();

	_wstring FullPath = wszSavePath + wszSaveName + TEXT(".json");
	m_JsonFilePaths.push_back(m_pGameInstance->WStringToString(FullPath));
}

void CLevel_Camera_Tool_Client::Load_Data_CutScene()
{
	if (true == m_isForClient) {

	}
	else {
		_wstring wszLoadPath = m_pGameInstance->StringToWString(m_JsonFilePaths[m_iCurrentJsonFileIndex]);

		ifstream file(wszLoadPath);

		if (!file.is_open())
		{
			MSG_BOX("파일을 열 수 없습니다.");
			file.close();
			return;
		}

		json Result;
		file >> Result;
		file.close();

		// KeyFrame
		if (Result.contains("KeyFrame") && Result["KeyFrame"].is_array()) {

			for (auto& KeyFrame_json : Result["KeyFrame"]) {
				CUTSCENE_KEYFRAME tKeyFrame;

				tKeyFrame.vPosition = { KeyFrame_json["Position"][0].get<_float>(), KeyFrame_json["Position"][1].get<_float>(), KeyFrame_json["Position"][2].get<_float>() };
				tKeyFrame.fTimeStamp = KeyFrame_json["TimeStamp"];
				tKeyFrame.iZoomLevel = KeyFrame_json["Zoom_Level"];
				tKeyFrame.iZoomRatioType = KeyFrame_json["Zoom_RatioType"];
				tKeyFrame.vAt = { KeyFrame_json["At"][0].get<_float>(), KeyFrame_json["At"][1].get<_float>(), KeyFrame_json["At"][2].get<_float>() };
				tKeyFrame.bLookTarget = KeyFrame_json["Is_LookTarget"];
				tKeyFrame.iAtRatioType = KeyFrame_json["At_RatioType"];

				CGameObject* pCube = Create_Cube();
				pCube->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tKeyFrame.vPosition), 1.f));
				m_KeyFrames.push_back({ tKeyFrame, pCube });
			}
		}

		// CutScene

		if (Result.contains("CutScene") && Result["CutScene"].is_array()) {

			for (auto& CutScene_json : Result["CutScene"]) {

				// CutScene Tag
				_string szCutSceneTag = CutScene_json["CutScene_Tag"];

				// SubData NextCameraType 저장
				CUTSCENE_SUB_DATA tSubData;
				tSubData.iNextCameraType = CutScene_json["CutScene_Next_CameraType"];

				// Sector
				if (CutScene_json.contains("Sector") && CutScene_json["Sector"].is_array()) {
					for (auto& Sector_json : CutScene_json["Sector"]) {

						CCutScene_Sector::CUTSCENE_SECTOR_DESC Desc{};

						Desc.iSectorType = Sector_json["Sector_Type"];
						Desc.fSectorDuration = Sector_json["Sector_Duration"];

						CCutScene_Sector* pSector = CCutScene_Sector::Create(m_pDevice, m_pContext, &Desc);

						// KeyFrame
						if (Sector_json.contains("KeyFrames") && Sector_json["KeyFrames"].is_array()) {
							for (auto& KeyFrame_json : Sector_json["KeyFrames"]) {

								CUTSCENE_KEYFRAME tKeyFrame = {};

								tKeyFrame.vPosition = { KeyFrame_json["Position"][0].get<_float>(), KeyFrame_json["Position"][1].get<_float>(), KeyFrame_json["Position"][2].get<_float>() };
								tKeyFrame.fTimeStamp = KeyFrame_json["TimeStamp"];
								tKeyFrame.iZoomLevel = KeyFrame_json["Zoom_Level"];
								tKeyFrame.iZoomRatioType = KeyFrame_json["Zoom_RatioType"];
								tKeyFrame.vAt = { KeyFrame_json["At"][0].get<_float>(), KeyFrame_json["At"][1].get<_float>(), KeyFrame_json["At"][2].get<_float>() };
								tKeyFrame.bLookTarget = KeyFrame_json["Is_LookTarget"];
								tKeyFrame.iAtRatioType = KeyFrame_json["At_RatioType"];

								pSector->Add_KeyFrame(tKeyFrame);
							}

							_bool isSameTag = false;
							// m_CutScenes에 추가
							for (auto& CutScene : m_CutScenes) {
								if (CutScene.first == m_pGameInstance->StringToWString(szCutSceneTag)) {
									isSameTag = true;
								}
							}

							if (true == isSameTag) {
								auto iter = m_CutScenes.find(m_pGameInstance->StringToWString(szCutSceneTag));
								iter->second.push_back(pSector);
							}
							else {
								vector<CCutScene_Sector*> vecSector;
								vecSector.push_back(pSector);

								m_CutScenes.emplace(m_pGameInstance->StringToWString(szCutSceneTag), vecSector);
								m_CutSceneSubDatas.emplace(m_pGameInstance->StringToWString(szCutSceneTag), tSubData);
								m_CutSceneTags.push_back(m_pGameInstance->StringToWString(szCutSceneTag));
							}
						}

					}
				}
			}
		}
	}
}

void CLevel_Camera_Tool_Client::Load_Data_Arm()
{
	_wstring wszLoadPath = m_pGameInstance->StringToWString(m_JsonFilePaths[m_iCurrentJsonFileIndex]);

	ifstream file(wszLoadPath);

	if (!file.is_open())
	{
		MSG_BOX("파일을 열 수 없습니다.");
		file.close();
		return;
	}

	json Result;
	file >> Result;
	file.close();

	json Trigger_json;

	CCamera_Target* pTargetCamera = static_cast<CCamera_Target*>(CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET));
	
	for (auto& Trigger_json : Result) {

		_string szArmTag = Trigger_json["Arm_Tag"];

		// Arm Data
		ARM_DATA* tArmData = new ARM_DATA();

		tArmData->fLength = Trigger_json["Arm_Data"]["Arm_Length"];
		tArmData->fLengthTime = { Trigger_json["Arm_Data"]["Arm_Time"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_Time"][1].get<_float>() };
		tArmData->iLengthRatioType = Trigger_json["Arm_Data"]["Arm_Length_RatioType"];

		tArmData->fMoveTimeAxisY = { Trigger_json["Arm_Data"]["Arm_MoveTime_AxisY"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_MoveTime_AxisY"][1].get<_float>() };
		tArmData->fMoveTimeAxisRight = { Trigger_json["Arm_Data"]["Arm_MoveTime_AxisRight"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_MoveTime_AxisRight"][1].get<_float>() };
		tArmData->fRotationPerSecAxisY = { Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisY"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisY"][1].get<_float>() };
		tArmData->fRotationPerSecAxisRight = { Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisRight"][0].get<_float>(), Trigger_json["Arm_Data"]["Arm_RotationPerSec_AxisRight"][1].get<_float>() };

		tArmData->vDesireArm = { Trigger_json["Arm_Data"]["Desire_Arm"][0].get<_float>(), Trigger_json["Arm_Data"]["Desire_Arm"][1].get<_float>(), Trigger_json["Arm_Data"]["Desire_Arm"][2].get<_float>() };

		// Sub Data
		_bool IsUseSubData = Trigger_json["Sub_Data"]["Use_SubData"];
		SUB_DATA* pSubData = nullptr;

		if (true == IsUseSubData) {
			pSubData = new SUB_DATA();

			pSubData->fZoomTime = Trigger_json["Sub_Data"]["Zoom_Time"];
			pSubData->iZoomLevel = Trigger_json["Sub_Data"]["Zoom_Level"];
			pSubData->iZoomRatioType = Trigger_json["Sub_Data"]["Zoom_RatioType"];

			pSubData->fAtOffsetTime = Trigger_json["Sub_Data"]["AtOffset_Time"];
			pSubData->vAtOffset = { Trigger_json["Sub_Data"]["AtOffset"][0].get<_float>(), Trigger_json["Sub_Data"]["AtOffset"][1].get<_float>(), Trigger_json["Sub_Data"]["AtOffset"][2].get<_float>() };
			pSubData->iAtRatioType = Trigger_json["Sub_Data"]["AtOffset_RatioType"];
		}

		pTargetCamera->Add_ArmData(m_pGameInstance->StringToWString(szArmTag), tArmData, pSubData);
	}

	pTargetCamera->Get_ArmNames(&m_ArmNames);
}

HRESULT CLevel_Camera_Tool_Client::Map_Object_Create(_wstring _strFileName)
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

CLevel_Camera_Tool_Client* CLevel_Camera_Tool_Client::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eLevelID)
{
	CLevel_Camera_Tool_Client* pInstance = new CLevel_Camera_Tool_Client(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Camera_Tool_Client");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Camera_Tool_Client::Free()
{
	for (auto& Point : m_KeyFrames)
		Safe_Release(Point.second);

	if (nullptr != m_pCurKeyFrame)
		Safe_Release(m_pCurKeyFrame->second);

	for (auto& CutScene : m_CutScenes) {
		for (auto& Sector : CutScene.second) {
			Safe_Release(Sector);
		}
	}

	m_KeyFrames.clear();

	Safe_Release(m_pSimulationCube);

	__super::Free();
}
