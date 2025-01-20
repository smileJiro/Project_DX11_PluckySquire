#include "stdafx.h"
#include "Level_Camera_Tool.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Test_Player.h"
#include "Test_Terrain.h"

#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_Manager_Tool.h"

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

	return S_OK;
}

void CLevel_Camera_Tool::Update(_float _fTimeDelta)
{
	// Change Camera Free  Or Target
	if (KEY_DOWN(KEY::TAB)) {
		_uint iCurCameraType = CCamera_Manager_Tool::GetInstance()->Get_CameraType();
		iCurCameraType ^= 1;
		CCamera_Manager_Tool::GetInstance()->Change_CameraType(iCurCameraType);
	}

	Show_CameraTool();
	Show_ArmInfo();
}

HRESULT CLevel_Camera_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Camera Tool Level"));
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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &Desc)))
		return E_FAIL;

	CCamera_Manager_Tool::GetInstance()->Add_Camera(CCamera_Manager_Tool::FREE, dynamic_cast<CCamera*>(pCamera));

	// Target Camera
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	TargetDesc.fSmoothSpeed = 5.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;
	TargetDesc.vAtOffset = { 0.f, 3.f, 0.f };

	TargetDesc.fFovy = XMConvertToRadians(30.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.fNear = 0.1f;
	TargetDesc.fFar = 1000.f;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_Target"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &TargetDesc)))
		return E_FAIL;

	CCamera_Manager_Tool::GetInstance()->Add_Camera(CCamera_Manager_Tool::TARGET, dynamic_cast<CCamera*>(pCamera));
	CCamera_Manager_Tool::GetInstance()->Change_CameraType(CCamera_Manager_Tool::FREE);

	Create_Arms();

	return S_OK;
}

HRESULT CLevel_Camera_Tool::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CTest_Player::CONTAINEROBJ_DESC Desc;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	Desc.tTransform2DDesc.vIniitialPosition = _float3(0.0f, 0.0f, 0.0f);
	Desc.tTransform2DDesc.vInitialScaling = _float3(150.f, 150.f, 150.f);

	Desc.tTransform3DDesc.vIniitialPosition = _float3(0.0f, 0.0f, 0.0f);
	Desc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

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

	TerrainDesc.tTransform3DDesc.vIniitialPosition = _float3(0.0f, 0.0f, 0.0f);
	TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Terrain"), LEVEL_CAMERA_TOOL, _strLayerTag, &TerrainDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Camera_Tool::Show_CameraTool()
{
	ImGui::Begin("Test");
	
	// Roteate
	Rotate_Arm();

	// Length
	Change_ArmLength();

	// CheckBox
	ImGui::NewLine();
	
	ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.3f, 0.0f));
	ImGui::SameLine();
	ImGui::Text("Change Next ArmPos Value");
	ImGui::Separator();

	//ImGui::Checkbox("Rotate CopyArm", &m_isCopyArm);
	//ImGui::SameLine(155.f);

	// Copy
	//if (ImGui::Button("Create CopyArm")) {
	//	CCamera_Manager_Tool::GetInstance()->Copy_Arm();
	//}

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

		CCamera_Manager_Tool::GetInstance()->Add_NextArm_Info(m_pGameInstance->StringToWString(m_szCopyArmName), tData);
		CCamera_Manager_Tool::GetInstance()->Get_ArmNames(&m_ArmNames);
	}

	// Edit CopyArm
	ImGui::SameLine();

	if (ImGui::Button("Edit CopyArm")) {
		Edit_CopyArm();
	}

	// Play Move
	Set_MovementInfo();
	
	// Reset Current Arm Pos
	Reset_CurrentArmPos();

	ImGui::End();
}

void CLevel_Camera_Tool::Show_ArmInfo()
{
	ImGui::Begin("Arm Info");

	Show_SelectedArmData();

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

void CLevel_Camera_Tool::Rotate_Arm()
{
	//ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) * 0.4f, 0.0f));
	//ImGui::SameLine();
	//ImGui::Text("Change Value of Arm");
	//ImGui::Separator();

	//_vector vCamPos = XMLoadFloat4((m_pGameInstance->Get_CamPosition()));
	//_vector vArmRotation = {};

	//ImGui::Text("Camera Position: %.2f, %.2f, %.2f", XMVectorGetX(vCamPos), XMVectorGetY(vCamPos), XMVectorGetZ(vCamPos));
	//ImGui::NewLine();

	//ImGui::Text("Ratation Value: %.2f", m_fRotationValue);
	//ImGui::SameLine();
	//ImGui::DragFloat("##Rotate", &m_fRotationValue, 0.1f, 0.f, 10.f);

	//ImGui::Text("RotationValueX: %.2f", m_fRotationValue);
	//ImGui::SameLine();
	//if (ImGui::Button("-X Angle") || ImGui::IsItemActive()) // 누르고 있는 동안 계속 동작
	//	vArmRotation = XMVectorSetX(vArmRotation, -m_fRotationValue);
	//ImGui::SameLine();
	//if (ImGui::Button("+X Angle") || ImGui::IsItemActive())
	//	vArmRotation = XMVectorSetX(vArmRotation, m_fRotationValue);

	//ImGui::Text("RotationValueY: %.2f", m_fRotationValue);
	//ImGui::SameLine();
	//if (ImGui::Button("-Y Angle") || ImGui::IsItemActive()) // 누르고 있는 동안 계속 동작
	//	vArmRotation = XMVectorSetY(vArmRotation, -m_fRotationValue);
	//ImGui::SameLine();
	//if (ImGui::Button("+Y Angle") || ImGui::IsItemActive())
	//	vArmRotation = XMVectorSetY(vArmRotation, m_fRotationValue);

	//ImGui::Text("RotationValueZ: %.2f", m_fRotationValue);
	//ImGui::SameLine();
	//if (ImGui::Button("-Z Angle") || ImGui::IsItemActive()) // 누르고 있는 동안 계속 동작
	//	vArmRotation = XMVectorSetZ(vArmRotation, -m_fRotationValue);
	//ImGui::SameLine();
	//if (ImGui::Button("+Z Angle") || ImGui::IsItemActive())
	//	vArmRotation = XMVectorSetZ(vArmRotation, m_fRotationValue);

	//_vector vRadianRotation = XMVectorSet(XMConvertToRadians(XMVectorGetX(vArmRotation)), XMConvertToRadians(XMVectorGetY(vArmRotation)), XMConvertToRadians(XMVectorGetZ(vArmRotation)), 0.f);

	//CCamera_Manager_Tool::GetInstance()->Set_ArmRotation(vRadianRotation);
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

	ImGui::Text("Length: %.2f        ", m_fLength);
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

	
	ImGui::Text("Min Rotation Per Sec AxisY: %.2f        ", m_fRotationPerSecAxisY.x);
	ImGui::SameLine();
	ImGui::DragFloat("##Min Rotation Per Sec AxisY", &m_fRotationPerSecAxisY.x, 0.1f, -360.f, 360.f);
	ImGui::Text("Max Rotation Per Sec AxisY: %.2f        ", m_fRotationPerSecAxisY.y);
	ImGui::SameLine();
	ImGui::DragFloat("##Max Rotation Per Sec AxisY", &m_fRotationPerSecAxisY.y, 0.1f, -360.f, 360.f);

	ImGui::Text("Min Rotation Per Sec AxisRight: %.2f        ", m_fRotationPerSecAxisRight.x);
	ImGui::SameLine();
	ImGui::DragFloat("##Min Rotation Per Sec AxisRight", &m_fRotationPerSecAxisRight.x, 0.1f, -360.f, 360.f);
	ImGui::Text("Max Rotation Per Sec AxisRight: %.2f        ", m_fRotationPerSecAxisRight.y);
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
	__super::Free();
}
