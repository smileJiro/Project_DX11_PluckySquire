#include "stdafx.h"
#include "Level_Camera_Tool.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Test_Player.h"
#include "Test_Terrain.h"

#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_CutScene.h"
#include "Camera_Manager_Tool.h"

#include "ModelObject.h"

#include "Layer.h"
#include "Collider.h"

CLevel_Camera_Tool::CLevel_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Camera_Tool::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CGameObject* pCameraTarget = nullptr;
	
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &pCameraTarget)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"), pCameraTarget)))
		return E_FAIL;

	if (FAILED(Ready_Layer_TestTerrain(TEXT("Layer_Terrain"))))
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

	return S_OK;
}

void CLevel_Camera_Tool::Update(_float _fTimeDelta)
{
	//cout << "?" << endl;

	// Change Camera Free  Or Target
	if (KEY_DOWN(KEY::TAB)) {
		_uint iCurCameraType = CCamera_Manager_Tool::GetInstance()->Get_CameraType();
		iCurCameraType ^= 1;
		CCamera_Manager_Tool::GetInstance()->Change_CameraType(iCurCameraType);
	}

	if (KEY_DOWN(KEY::NUM0)) {
		_uint iCurCameraType = CCamera_Manager_Tool::GetInstance()->Get_CameraType();

		iCurCameraType = (iCurCameraType == CCamera_Manager_Tool::CUTSCENE) ? CCamera_Manager_Tool::FREE : CCamera_Manager_Tool::CUTSCENE;

		CCamera_Manager_Tool::GetInstance()->Change_CameraType(iCurCameraType);
	}

	Show_CameraTool();
	Show_CutSceneTool(_fTimeDelta);
	Show_ArmInfo();
	Show_CutSceneInfo();
	Show_SaveLoadFileWindow();
}

HRESULT CLevel_Camera_Tool::Render()
{
#ifdef _DEBUG
	//SetWindowText(g_hWnd, TEXT("Camera Tool Level"));
	m_pGameInstance->Render_FPS(TEXT("Timer_Default"));
#endif

	return S_OK;
}

HRESULT CLevel_Camera_Tool::Ready_Lights()
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

HRESULT CLevel_Camera_Tool::Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
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
	Desc.eZoomLevel = CCamera::LEVEL_6;
	Desc.iCameraType = CCamera_Manager_Tool::FREE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	CCamera_Manager_Tool::GetInstance()->Add_Camera(CCamera_Manager_Tool::FREE, dynamic_cast<CCamera*>(pCamera));

	// Target Camera
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	TargetDesc.fSmoothSpeed = 5.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;
	TargetDesc.vAtOffset = { 0.f, 3.f, 0.f };

	TargetDesc.fFovy = XMConvertToRadians(60.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.fNear = 0.1f;
	TargetDesc.fFar = 1000.f;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);
	TargetDesc.eZoomLevel = CCamera::LEVEL_6;
	TargetDesc.iCameraType = CCamera_Manager_Tool::TARGET;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_Target"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &TargetDesc)))
		return E_FAIL;

	CCamera_Manager_Tool::GetInstance()->Add_Camera(CCamera_Manager_Tool::TARGET, dynamic_cast<CCamera*>(pCamera));
	
	Create_Arms();
	
	// CutScene Camera
	CCamera_CutScene::CAMERA_DESC CutSceneDesc{};

	CutSceneDesc.fFovy = XMConvertToRadians(60.f);
	CutSceneDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CutSceneDesc.fNear = 0.1f;
	CutSceneDesc.fFar = 1000.f;
	CutSceneDesc.vEye = _float3(0.f, 10.f, -7.f);
	CutSceneDesc.vAt = _float3(0.f, 0.f, 0.f);
	CutSceneDesc.eZoomLevel = CCamera::LEVEL_6;
	CutSceneDesc.iCameraType = CCamera_Manager_Tool::CUTSCENE;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_CutScene"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &CutSceneDesc)))
		return E_FAIL;

	CCamera_Manager_Tool::GetInstance()->Add_Camera(CCamera_Manager_Tool::CUTSCENE, dynamic_cast<CCamera*>(pCamera));

	CCamera_Manager_Tool::GetInstance()->Change_CameraType(CCamera_Manager_Tool::FREE);

	return S_OK;
}

HRESULT CLevel_Camera_Tool::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CTest_Player::CONTAINEROBJ_DESC Desc;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Player"), LEVEL_CAMERA_TOOL, _strLayerTag, _ppOut, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Camera_Tool::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	/* Test Terrain */
	CTest_Terrain::MODELOBJECT_DESC TerrainDesc{};

	TerrainDesc.eStartCoord = COORDINATE_3D;
	TerrainDesc.iCurLevelID = LEVEL_CAMERA_TOOL;
	TerrainDesc.isCoordChangeEnable = false;
	TerrainDesc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;

	TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	TerrainDesc.strModelPrototypeTag_3D = TEXT("WoodenPlatform_01");

	TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	TerrainDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Terrain"), LEVEL_CAMERA_TOOL, _strLayerTag, &TerrainDesc)))
		return E_FAIL;


	/* Test Terrain */
	TerrainDesc.eStartCoord = COORDINATE_3D;
	TerrainDesc.iCurLevelID = LEVEL_CAMERA_TOOL;
	TerrainDesc.isCoordChangeEnable = false;
	TerrainDesc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;

	TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	TerrainDesc.strModelPrototypeTag_3D = TEXT("alphabet_blocks_a_mesh");

	TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	TerrainDesc.tTransform3DDesc.vInitialPosition = m_tKeyFrameInfo.vPosition;
	TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(0.5f, 0.5f, 0.5f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Terrain"), LEVEL_CAMERA_TOOL, TEXT("Layer_Simulation"), &m_pSimulationCube, &TerrainDesc);
	Safe_AddRef(m_pSimulationCube);
	m_pSimulationCube->Set_Active(false);

	return S_OK;
}

HRESULT CLevel_Camera_Tool::Ready_DataFiles()
{
	std::filesystem::path path;
	path = "../Bin/Resources/DataFiles/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".bin") {
			m_BinaryFilePaths.push_back(entry.path().string());
		}
	}

	return S_OK;
}

void CLevel_Camera_Tool::Show_CameraTool()
{
	ImGui::Begin("Camera Basic Tool");
	
	// Roteate
	Rotate_Arm();

	// Length
	Change_ArmLength();

	// CheckBox
	ImGui::NewLine();

	// Add CopyArm
	Input_NextArm_Info();
	if (ImGui::Button("Add Next Arm Info")) {
		
		ARM_DATA tData;
		tData.fLength = m_fLength;
		tData.fMoveTimeAxisY = { m_fMoveTimeAxisY, 0.f };
		tData.fMoveTimeAxisRight = { m_fMoveTimeAxisRight, 0.f };
		tData.fLengthTime = { m_fLengthTime, 0.f };
		tData.fRotationPerSecAxisY = { XMConvertToRadians(m_fRotationPerSecAxisY.x), XMConvertToRadians(m_fRotationPerSecAxisY.y) };
		tData.fRotationPerSecAxisRight = { XMConvertToRadians(m_fRotationPerSecAxisRight.x), XMConvertToRadians(m_fRotationPerSecAxisRight.y)};

		CCamera_Manager_Tool::GetInstance()->Add_ArmData(m_pGameInstance->StringToWString(m_szCopyArmName), tData);
		CCamera_Manager_Tool::GetInstance()->Get_ArmNames(&m_ArmNames);
	}

	// Edit CopyArm
	ImGui::SameLine();

	if (ImGui::Button("Edit CopyArm")) {
		Edit_CopyArm();
	}

	// Zoom
	Set_Zoom();

	// At Offset
	Set_AtOffsetInfo();

	// Shake
	Set_ShakeInfo();

	// Play Move
	Set_MovementInfo();
	
	// Reset Current Arm Pos
	Reset_CurrentArmPos();

	ImGui::End();
}

void CLevel_Camera_Tool::Show_CutSceneTool(_float fTimeDelta)
{
	ImGui::Begin("CutScene Tool");

	Picking();

	Set_KeyFrameInfo();

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

void CLevel_Camera_Tool::Show_ArmInfo()
{
	ImGui::Begin("Arm Info");

	Show_SelectedArmData();
	Show_CameraZoomInfo();

	ImGui::End();
}

void CLevel_Camera_Tool::Show_CutSceneInfo()
{
	ImGui::Begin("CutScene Info");

	Show_KeyFrameInfo();

	ImGui::End();
}

void CLevel_Camera_Tool::Create_Arms()
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(LEVEL_CAMERA_TOOL, TEXT("Layer_Player"), 0);
	_vector vPlayerLook = pPlayer->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);

	CCameraArm::CAMERA_ARM_DESC Desc{};

	XMStoreFloat3(&Desc.vArm, -vPlayerLook);
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	Desc.vRotation = { XMConvertToRadians(-30.f), XMConvertToRadians(0.f), 0.f };
	Desc.fLength = 12.f;
	Desc.wszArmTag = TEXT("Player_Arm");
	Desc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr();

	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);


	CCamera_Target* pTarget = dynamic_cast<CCamera_Target*>(CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::TARGET));

	pTarget->Add_Arm(pArm);
	CCamera_Manager_Tool::GetInstance()->Set_CurrentArm(pArm);

	m_vResetArmPos = CCamera_Manager_Tool::GetInstance()->Get_CurrentArmVector();
}

void CLevel_Camera_Tool::Show_ComboBox()
{
	if (m_ArmNames.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ArmNames[m_iSelectedArmNum]);

	if (ImGui::BeginCombo("Label", Name.c_str())) {
		for (_int i = 0; i < m_ArmNames.size(); ++i) {
			_bool bSelected = (m_iSelectedArmNum == i);

			if(ImGui::Selectable(m_pGameInstance->WStringToString(m_ArmNames[i]).c_str(), bSelected))
				m_iSelectedArmNum = i;

			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CLevel_Camera_Tool::Show_SelectedArmData()
{
	if (m_ArmNames.size() <= 0)
		return;

	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Selected Arm Data");
	ImGui::Separator();

	ARM_DATA* pData = CCamera_Manager_Tool::GetInstance()->Get_ArmData(m_ArmNames[m_iSelectedArmNum]);

	if (nullptr == pData)
		return;

	//m_szSelectedArmName = m_pGameInstance->WStringToString(m_ArmNames[m_iSelectedArmNum]).c_str();

	//ImGui::Text("Copy Arm Tag Input:      ");
	//ImGui::SameLine();
	//ImGui::SetNextItemWidth(-1);
	//ImGui::InputText("##SelectedArm Tag",  m_ArmNames[m_iSelectedArmNum], MAX_PATH);

	ImGui::Text("SelectedArm Length Time: %.2f        ", pData->fLengthTime.x);

	ImGui::Text("Move Time AxisY: %.2f    ", pData->fMoveTimeAxisY.x);

	ImGui::Text("Move Time AxisRight: %.2f", pData->fMoveTimeAxisRight.x);


	ImGui::Text("Min Rotation Per Sec AixsY: %.2f        ", XMConvertToDegrees(pData->fRotationPerSecAxisY.x));
	ImGui::Text("Max Rotation Per Sec AixsY: %.2f        ", XMConvertToDegrees(pData->fRotationPerSecAxisY.y));
	ImGui::NewLine();
	ImGui::Text("Min Rotation Per Sec AixsRight: %.2f        ", XMConvertToDegrees(pData->fRotationPerSecAxisRight.x));
	ImGui::Text("Max Rotation Per Sec AixsRight: %.2f        ", XMConvertToDegrees(pData->fRotationPerSecAxisRight.y));


	ImGui::NewLine();
}

void CLevel_Camera_Tool::Show_CameraZoomInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Zoom Data");
	ImGui::Separator();

	CCamera* pCamera = CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::TARGET);
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

void CLevel_Camera_Tool::Set_KeyFrameInfo()
{


	ImGui::Text("Position: %.2f, %.2f, %.2f", m_tKeyFrameInfo.vPosition.x, m_tKeyFrameInfo.vPosition.y, m_tKeyFrameInfo.vPosition.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFramePosX", &m_tKeyFrameInfo.vPosition.x, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFramePosY", &m_tKeyFrameInfo.vPosition.y, 0.1f);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFramePosZ", &m_tKeyFrameInfo.vPosition.z, 0.1f);

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
	case CCamera::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case CCamera::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case CCamera::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("FRAME EASE IN")) {
		m_tKeyFrameInfo.iZoomRatioType = CCamera::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("FRAME EASE OUT")) {
		m_tKeyFrameInfo.iZoomRatioType = CCamera::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("FRAME LERP")) {
		m_tKeyFrameInfo.iZoomRatioType = CCamera::LERP;
	}

	ImGui::NewLine();

	ImGui::Text("At: %.2f, %.2f, %.2f      ", m_tKeyFrameInfo.vAtOffset.x, m_tKeyFrameInfo.vAtOffset.y, m_tKeyFrameInfo.vAtOffset.z);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFrameAtX", &m_tKeyFrameInfo.vAtOffset.x);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFrameAtY", &m_tKeyFrameInfo.vAtOffset.y);
	ImGui::SameLine(0, 10.0f);

	ImGui::SetNextItemWidth(50.0f);    // 40으로 줄임
	ImGui::DragFloat("##KeyFrameAtZ", &m_tKeyFrameInfo.vAtOffset.z);

	if(true == m_tKeyFrameInfo.bLookTarget)
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
	case CCamera::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case CCamera::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case CCamera::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("AT EASE IN")) {
		m_tKeyFrameInfo.iAtRatioType = CCamera::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("AT EASE OUT")) {
		m_tKeyFrameInfo.iAtRatioType = CCamera::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("AT LERP")) {
		m_tKeyFrameInfo.iAtRatioType = CCamera::LERP;
	}

	ImGui::NewLine();

}

void CLevel_Camera_Tool::Show_KeyFrameInfo()
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
		case CCamera::EASE_IN:
			ImGui::Text("Ratio Type: EASE_IN       ");
			break;

		case CCamera::EASE_OUT:
			ImGui::Text("Ratio Type: EASE_OUT      ");
			break;

		case CCamera::LERP:
			ImGui::Text("Ratio Type: LERP          ");
			break;
		}
		ImGui::Text("At: %.2f, %.2f, %.2f      ", m_pCurKeyFrame->first.vAtOffset.x, m_pCurKeyFrame->first.vAtOffset.y, m_pCurKeyFrame->first.vAtOffset.z);

		if (true == m_pCurKeyFrame->first.bLookTarget)
			ImGui::Text("Loot At: TRUE             ");
		else
			ImGui::Text("Loot At: FALSE            ");

		switch (m_pCurKeyFrame->first.iAtRatioType) {
		case CCamera::EASE_IN:
			ImGui::Text("Ratio Type: EASE_IN       ");
			break;

		case CCamera::EASE_OUT:
			ImGui::Text("Ratio Type: EASE_OUT      ");
			break;

		case CCamera::LERP:
			ImGui::Text("Ratio Type: LERP          ");
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
					case CCamera::EASE_IN:
						ImGui::Text("Ratio Type: EASE_IN       ");
						break;

					case CCamera::EASE_OUT:
						ImGui::Text("Ratio Type: EASE_OUT      ");
						break;

					case CCamera::LERP:
						ImGui::Text("Ratio Type: LERP          ");
						break;
					}
					ImGui::Text("At: %.2f, %.2f, %.2f      ", (*pKeyFrames)[i].vAtOffset.x, (*pKeyFrames)[i].vAtOffset.y, (*pKeyFrames)[i].vAtOffset.z);
					
					if (true == (*pKeyFrames)[i].bLookTarget)
						ImGui::Text("Loot At: TRUE             ");
					else
						ImGui::Text("Loot At: FALSE            ");

					switch ((*pKeyFrames)[i].iAtRatioType) {
					case CCamera::EASE_IN:
						ImGui::Text("Ratio Type: EASE_IN       ");
						break;

					case CCamera::EASE_OUT:
						ImGui::Text("Ratio Type: EASE_OUT      ");
						break;

					case CCamera::LERP:
						ImGui::Text("Ratio Type: LERP          ");
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

void CLevel_Camera_Tool::Show_CutSceneComboBox()
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

void CLevel_Camera_Tool::Show_SaveLoadFileWindow()
{
	ImGui::Begin("Save Load File");

	ImGui::Checkbox("For Client", &m_isForClient);
	ImGui::SameLine();
	ImGui::Checkbox("Arm Data", &m_isArmData);
	ImGui::SameLine();
	ImGui::Checkbox("CutScene Data", &m_isCutSceneData);

	ImGui::BeginChild("Files", ImVec2(400, 150), true);  // true는 border 표시
	for (int i = 0; i < m_BinaryFilePaths.size(); ++i)
	{
		if (ImGui::Selectable(m_BinaryFilePaths[i].c_str(), m_iCurrentBinaryFileIndex == i))
		{
			m_iCurrentBinaryFileIndex = i;
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
				Save_Data_CutScene();
			else if (true == m_isCutSceneData && false == m_isArmData)
				Save_Data_Arm();
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
			Load_Data_CutScene();
		else if (true == m_isCutSceneData && false == m_isArmData)
			Load_Data_Arm();
	}

	ImGui::End();
}


void CLevel_Camera_Tool::Rotate_Arm()
{

}

void CLevel_Camera_Tool::Change_ArmLength()
{
	ImGui::NewLine();
	ImGui::Text("Length Value: %.2f", m_fLengthValue);
	ImGui::SameLine();
	ImGui::DragFloat("##Length", &m_fLengthValue, 0.05f, 0.f, 10.f);

	_float fArmLength = CCamera_Manager_Tool::GetInstance()->Get_ArmLength();
	_bool bActive = false;

	ImGui::Text("Arm Length: %.2f  ", m_fLengthValue);
	ImGui::SameLine();
	if (ImGui::Button("- Length") || ImGui::IsItemActive()) {// 누르고 있는 동안 계속 동작
		fArmLength -= m_fLengthValue;
		CCamera_Manager_Tool::GetInstance()->Set_ArmLength(fArmLength);
	}
	ImGui::SameLine();
	if (ImGui::Button("+ Length") || ImGui::IsItemActive()) {
		fArmLength += m_fLengthValue;
		CCamera_Manager_Tool::GetInstance()->Set_ArmLength(fArmLength);
	}
}

void CLevel_Camera_Tool::Input_NextArm_Info()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Add Copy Arm");
	ImGui::Separator();

	ImGui::Text("Copy Arm Tag Input:      ", m_fLengthTime);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputText("##CopyArm Tag", m_szCopyArmName, MAX_PATH);

	ImGui::Text("Length: %.2f             ", m_fLength);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::DragFloat("##Length Info", &m_fLength, 0.05f, 0.f, 30.f);

	ImGui::Text("Length Time: %.2f        ", m_fLengthTime);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::DragFloat("##Length Time", &m_fLengthTime, 0.05f, 0.f, 10.f);

	ImGui::Text("Move Time AxisY: %.2f    ", m_fMoveTimeAxisY);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::DragFloat("##Move Time Y", &m_fMoveTimeAxisY, 0.05f, 0.f, 10.f);

	ImGui::Text("Move Time AxisRight: %.2f", m_fMoveTimeAxisRight);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::DragFloat("##Move Time Right", &m_fMoveTimeAxisRight, 0.05f, 0.f, 10.f);

	
	ImGui::Text("Min RPS AxisY: %.2f      ", m_fRotationPerSecAxisY.x);
	ImGui::SameLine();
	ImGui::DragFloat("##Min Rotation Per Sec AxisY", &m_fRotationPerSecAxisY.x, 0.1f, -360.f, 360.f);
	ImGui::Text("Max RPS AxisY: %.2f      ", m_fRotationPerSecAxisY.y);
	ImGui::SameLine();
	ImGui::DragFloat("##Max Rotation Per Sec AxisY", &m_fRotationPerSecAxisY.y, 0.1f, -360.f, 360.f);

	ImGui::Text("Min RPS AxisRight: %.2f  ", m_fRotationPerSecAxisRight.x);
	ImGui::SameLine();
	ImGui::DragFloat("##Min Rotation Per Sec AxisRight", &m_fRotationPerSecAxisRight.x, 0.1f, -360.f, 360.f);
	ImGui::Text("Max RPS AxisRight: %.2f  ", m_fRotationPerSecAxisRight.y);
	ImGui::SameLine();
	ImGui::DragFloat("##Max Rotation Per Sec AxisRight", &m_fRotationPerSecAxisRight.y, 0.1f, -360.f, 360.f);

	ImGui::NewLine();
}

void CLevel_Camera_Tool::Edit_CopyArm()
{
	if (m_ArmNames.size() <= 0)
		return;

	ARM_DATA* pData = CCamera_Manager_Tool::GetInstance()->Get_ArmData(m_ArmNames[m_iSelectedArmNum]);

	if (nullptr == pData)
		return;

	pData->fMoveTimeAxisY = { m_fMoveTimeAxisY, 0.f };
	pData->fMoveTimeAxisRight = { m_fMoveTimeAxisRight, 0.f };
	pData->fLengthTime = { m_fLengthTime, 0.f };
	pData->fRotationPerSecAxisY = { XMConvertToRadians(m_fRotationPerSecAxisY.x),   XMConvertToRadians(m_fRotationPerSecAxisY.y) };
	pData->fRotationPerSecAxisRight = { XMConvertToRadians(m_fRotationPerSecAxisRight.x),   XMConvertToRadians(m_fRotationPerSecAxisRight.y) };
	// Show Data
}


void CLevel_Camera_Tool::Set_MovementInfo()
{
	ImGui::NewLine();
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.5f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Play Move");
	ImGui::Separator();

	if (m_ArmNames.size() <= 0)
		return;

	_string Name = m_pGameInstance->WStringToString(m_ArmNames[m_iSelectedArmNum]);

	ImGui::Text("Selected Arm Name: %s", Name.c_str());

	Show_ComboBox();

	if (ImGui::Button("Play Movement")) {
		CCamera_Manager_Tool::GetInstance()->Change_CameraMode(CCamera_Target::MOVE_TO_NEXTARM);
		CCamera_Manager_Tool::GetInstance()->Set_NextArmData(m_ArmNames[m_iSelectedArmNum]);
	}

	
	Reset_CurrentArmPos();
}

void CLevel_Camera_Tool::Play_Movement()
{
	
}

void CLevel_Camera_Tool::Reset_CurrentArmPos()
{
	ImGui::SameLine();

	if (ImGui::Button("Rest Pos")) {
		CCamera_Manager_Tool::GetInstance()->Reset_CurrentArmPos(XMLoadFloat3(&m_vResetArmPos), 12.f);
	}
}

void CLevel_Camera_Tool::Set_Zoom()
{
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             Zoom Info");
	ImGui::Separator();

	ImGui::Text("Next Zoom Level: %d    ", m_iZoomLevel);
	ImGui::SameLine();
	ImGui::InputInt("##Zoom Level", &m_iZoomLevel);

	if(m_iZoomLevel <= 0)
		ImGui::Text("Next Fovy: %.2f", 0.f);
	else
		ImGui::Text("Next Fovy: %.2f", m_fFovys[m_iZoomLevel - 1]);

	ImGui::Text("Zoom Time: %.2f       ", m_fZoomTime);
	ImGui::SameLine();
	ImGui::DragFloat("##Zoom Time", &m_fZoomTime, 0.05f, 0.f, 10.f);

	switch (m_iRatioType) {
	case CCamera::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN   ");
		break;

	case CCamera::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT  ");
		break;

	case CCamera::LERP:
		ImGui::Text("Ratio Type: LERP      ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("EASE IN")) {
		m_iRatioType = CCamera::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("EASE OUT")) {
		m_iRatioType = CCamera::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("LERP")) {
		m_iRatioType = CCamera::LERP;
	}

	ImGui::NewLine();
	if (ImGui::Button("Start Zoom")) {
		CCamera_Manager_Tool::GetInstance()->Start_Zoom(CCamera_Manager_Tool::TARGET, m_fZoomTime, m_iZoomLevel - 1, m_iRatioType);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Fovy")) {
		CCamera* pCamera = CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::TARGET);
		pCamera->Set_ZoomLevel(CCamera::LEVEL_6);
		pCamera->Set_Fovy(XMConvertToRadians(m_fFovys[CCamera::LEVEL_6]));
	}


}

void CLevel_Camera_Tool::Set_AtOffsetInfo()
{
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             AtOffset Info");
	ImGui::Separator();

	ImGui::Text("Position: %.2f, %.2f, %.2f", m_vNextAtOffset.x, m_vNextAtOffset.y, m_vNextAtOffset.z);
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
	case CCamera::EASE_IN:
		ImGui::Text("Ratio Type: EASE_IN       ");
		break;

	case CCamera::EASE_OUT:
		ImGui::Text("Ratio Type: EASE_OUT      ");
		break;

	case CCamera::LERP:
		ImGui::Text("Ratio Type: LERP          ");
		break;
	}

	ImGui::SameLine();
	if (ImGui::Button("OFFSET EASE IN")) {
		m_iAtOffsetRatioType = CCamera::EASE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("OFFSET EASE OUT")) {
		m_iAtOffsetRatioType = CCamera::EASE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("OFFSET LERP")) {
		m_iAtOffsetRatioType = CCamera::LERP;
	}

	ImGui::NewLine();
	if (ImGui::Button("Start Change AtOffset")) {
		CCamera_Manager_Tool::GetInstance()->Start_Changing_AtOffset(CCamera_Manager_Tool::TARGET, m_fAtOffsetTime, XMLoadFloat3(&m_vNextAtOffset), m_iAtOffsetRatioType);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset AtOffset")) {
		CCamera_Manager_Tool::GetInstance()->Start_Changing_AtOffset(CCamera_Manager_Tool::TARGET, 1.f, XMVectorSet(0.f, 3.f, 0.f, 0.f), CCamera::LERP);
	}


}

void CLevel_Camera_Tool::Set_ShakeInfo()
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
	ImGui::DragInt("##ShakeCount", &m_iShakeCount, 0.05f, 0.f, 200.f);

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
			CCamera_Manager_Tool::GetInstance()->Start_Shake_ByTime(CCamera_Manager_Tool::TARGET, m_fShakeTime, m_fShakeForce, m_fShakeCycleTime, m_iShakeType, m_fShakeDelayTime);
			break;
		case (_uint)CYCLE_TYPE::COUNT:
			CCamera_Manager_Tool::GetInstance()->Start_Shake_ByCount(CCamera_Manager_Tool::TARGET, m_fShakeTime, m_fShakeForce, m_iShakeCount, m_iShakeType, m_fShakeDelayTime);
			break;
		}
	}
}

void CLevel_Camera_Tool::Create_KeyFrame()
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

			CUTSCENE_KEYFRAME tFrame;
			CGameObject* pCube = Create_Cube(&tFrame);

			if (nullptr == pCube)
				return;

			m_KeyFrames.push_back({ tFrame, pCube });
		}
	}
}

CGameObject* CLevel_Camera_Tool::Create_Cube(CUTSCENE_KEYFRAME* _tKeyFrame)
{
	/* Test Terrain */
	CTest_Terrain::MODELOBJECT_DESC TerrainDesc{};

	TerrainDesc.eStartCoord = COORDINATE_3D;
	TerrainDesc.iCurLevelID = LEVEL_CAMERA_TOOL;
	TerrainDesc.isCoordChangeEnable = false;
	TerrainDesc.iModelPrototypeLevelID_3D = LEVEL_CAMERA_TOOL;

	TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	TerrainDesc.strModelPrototypeTag_3D = TEXT("alphabet_blocks_d_mesh");

	TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	TerrainDesc.tTransform3DDesc.vInitialPosition = m_tKeyFrameInfo.vPosition;
	TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(0.1f, 0.1f, 0.1f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	CGameObject* pCube;

	m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Terrain"), LEVEL_CAMERA_TOOL, TEXT("Layer_Cube"), &pCube, &TerrainDesc);
	Safe_AddRef(pCube);
	
	*_tKeyFrame = m_tKeyFrameInfo;

	return pCube;
}

void CLevel_Camera_Tool::Edit_KeyFrame()
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
		m_pCurKeyFrame->first.vAtOffset = m_tKeyFrameInfo.vAtOffset;
		m_pCurKeyFrame->first.bLookTarget = m_tKeyFrameInfo.bLookTarget;
		m_pCurKeyFrame->first.iAtRatioType = m_tKeyFrameInfo.iAtRatioType;

		if (false == m_isMaintainOriginPos) {
			CController_Transform* pController = m_pCurKeyFrame->second->Get_ControllerTransform();
			pController->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tKeyFrameInfo.vPosition), 1.f));
		}
	}
}

void CLevel_Camera_Tool::Delete_KeyFrame()
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

void CLevel_Camera_Tool::Set_CurrentKeyFrame()
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

void CLevel_Camera_Tool::Create_Sector()
{
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_DOWN(MOUSE_KEY::LB)) {

			_float2 fMousePos = m_pGameInstance->Get_CursorPos();

			CGameObject* pCube = m_pGameInstance->Get_PickingModelObjectByCursor(LEVEL_CAMERA_TOOL, TEXT("Layer_Cube"), fMousePos);

			if (nullptr == pCube)
				return;

			for (auto& KeyFrame : m_KeyFrames) {
				if (KeyFrame.second == pCube) {

					for (auto& SelectedFrame : m_SelectedKeyFrame) {
						if (true == XMVector3Equal(XMLoadFloat3(&SelectedFrame.vPosition), XMLoadFloat3(&KeyFrame.first.vPosition)))
							return;

						_float fEpsilon = 0.01;

						if (KeyFrame.first.fTimeStamp < SelectedFrame.fTimeStamp + fEpsilon &&
							KeyFrame.first.fTimeStamp > SelectedFrame.fTimeStamp - fEpsilon)
							return;
					}

					m_SelectedKeyFrame.push_back(KeyFrame.first);
				}
			}
		}
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
	
	ImGui::Text("Delete Sector Num:   %d    ", m_iDeleteSectorNum);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200.0f);
	ImGui::InputInt("##Delete Sector Num   ", &m_iDeleteSectorNum);

	ImGui::Text("CutScene Tag Input:       ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputText("##CutScene Tag", m_szCutSceneTag, MAX_PATH);

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
		}
		else {
			vector<CCutScene_Sector*> vecSector;
			vecSector.push_back(pSector);

			m_CutScenes.emplace(m_pGameInstance->StringToWString(m_szCutSceneTag), vecSector);
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

void CLevel_Camera_Tool::Edit_Sector()
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

				if(true == m_isMaintainOriginPos)
					(*pKeyFrames)[m_iEditFrameNum].vPosition = vOriginPos;
			}
		}
	}	
}

void CLevel_Camera_Tool::Picking()
{
	if (MOUSE_DOWN(MOUSE_KEY::LB)) {
		_vector vRayPos, vRayDir;

		Get_RayInfo(&vRayPos, &vRayDir);

		_float2 fMousePos = m_pGameInstance->Get_CursorPos();
		_float fDist = {};

		if (0 > fMousePos.x || g_iWinSizeX < fMousePos.x || 0 > fMousePos.y || g_iWinSizeY < fMousePos.y)
			return;

		CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_CAMERA_TOOL, TEXT("Layer_Terrain"));
		list<CGameObject*> Objects = pLayer->Get_GameObjects();

		CGameObject* pGameObject = nullptr;

		for (auto& GameObject : Objects) {
			CModelObject* pModelObject = dynamic_cast<CModelObject*>(GameObject);
			_bool isPicked = pModelObject->Is_PickingCursor_Model_Test(fMousePos, fDist);

			if (true == isPicked) {
				_vector vClickedPos = vRayPos + (vRayDir * fDist);

				XMStoreFloat3(&m_tKeyFrameInfo.vPosition, vClickedPos);
			}
		}

		pLayer = m_pGameInstance->Find_Layer(LEVEL_CAMERA_TOOL, TEXT("Layer_Cube"));

		if (nullptr == pLayer)
			return;

		Objects = pLayer->Get_GameObjects();

		pGameObject = nullptr;

		for (auto& GameObject : Objects) {
			CModelObject* pModelObject = dynamic_cast<CModelObject*>(GameObject);
			_bool isPicked = pModelObject->Is_PickingCursor_Model_Test(fMousePos, fDist);

			if (true == isPicked) {
				_vector vClickedPos = vRayPos + (vRayDir * fDist);

				XMStoreFloat3(&m_tKeyFrameInfo.vPosition, vClickedPos);
			}
		}
	}
}

void CLevel_Camera_Tool::Get_RayInfo(_vector* _pRayPos, _vector* _pRayDir)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_vector vMousePos = XMVectorSet(pt.x, pt.y, 0.f, 1.f);

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

void CLevel_Camera_Tool::Play_CutScene(_float fTimeDelta)
{
	if (m_CutScenes.size() <= 0)
		return;

	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("             Play CutScene");
	ImGui::Separator();

	if (ImGui::Button("Add CutScene Data To Camera")) {
		for (auto& CutScene : m_CutScenes) {
			CCamera* pCamera = CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::CUTSCENE);
		
			if (nullptr == pCamera)
				return;

			dynamic_cast<CCamera_CutScene*>(pCamera)->Add_CutScene(CutScene.first, CutScene.second);
		}
	}

	if (ImGui::Button("Simulation")) {
		m_isSimulation = true;

		CCamera* pCamera = CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::CUTSCENE);

		if (nullptr == pCamera)
			return;

		dynamic_cast<CCamera_CutScene*>(pCamera)->Set_NextCutScene(m_CutSceneTags[m_iSelectedCutSceneNum]);
		m_pSimulationCube->Set_Active(true);
	}

	// 움직이기
	if (true == m_isSimulation) {
		CCamera_CutScene* pCamera = dynamic_cast<CCamera_CutScene*>(CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::CUTSCENE));

		if (nullptr == pCamera)
			return;

		pCamera->Update(fTimeDelta);
		_float3 vSimulationPos = pCamera->Get_SimulationPos();

		CController_Transform* pTransform = m_pSimulationCube->Get_ControllerTransform();
		pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vSimulationPos), 1.f));
	
		if (true == pCamera->Get_IsFinish()) {
			m_isSimulation = false;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Play CutScene")) {
		CCamera_Manager_Tool::GetInstance()->Change_CameraType(CCamera_Manager_Tool::CUTSCENE);
		//CCamera_Manager_Tool::GetInstance()->Set_NextCutScene(m_CutSceneTags[m_iSelectedCutSceneNum]);
		CCamera* pCamera = CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::CUTSCENE);

		if (nullptr == pCamera)
			return;

		dynamic_cast<CCamera_CutScene*>(pCamera)->Set_NextCutScene(m_CutSceneTags[m_iSelectedCutSceneNum]);
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset CutScene Camera, Simulation")) {
		//CCamera* pCamera = CCamera_Manager_Tool::GetInstance()->Get_Camera(CCamera_Manager_Tool::CUTSCENE);

		//if (nullptr == pCamera)
		//	return;

		m_pSimulationCube->Set_Active(false);
	}
}

void CLevel_Camera_Tool::Save_Data_CutScene()
{
	if (true == m_isForClient) {

	}
	else {
		
	}
}

void CLevel_Camera_Tool::Save_Data_Arm()
{
}

void CLevel_Camera_Tool::Load_Data_CutScene()
{
	if (true == m_isForClient) {

	}
	else {

	}
}

void CLevel_Camera_Tool::Load_Data_Arm()
{
}

CLevel_Camera_Tool* CLevel_Camera_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Camera_Tool* pInstance = new CLevel_Camera_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Camera_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Camera_Tool::Free()
{
	for (auto& Point : m_KeyFrames) 
		Safe_Release(Point.second);

	if(nullptr != m_pCurKeyFrame)
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
