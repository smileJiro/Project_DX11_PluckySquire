#include "stdafx.h"
#include "Level_Camera_Tool.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Test_Player.h"
#include "Test_Terrain.h"

#include "Camera_Free.h"
#include "Camera_Target.h"
#include "Camera_Manager.h"
#include "Arm_Manager.h"

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
	if (KEY_DOWN(KEY::N)) {
		m_pGameInstance->Change_CameraType(CCamera_Manager::FREE);
	}
	else if (KEY_DOWN(KEY::M)) {
		m_pGameInstance->Change_CameraType(CCamera_Manager::TARGET);
	}

	Show_CameraTool();
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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Camera_Target"),
		LEVEL_CAMERA_TOOL, _strLayerTag, &pCamera, &TargetDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Camera(CCamera_Manager::TARGET, dynamic_cast<CCamera*>(pCamera));

	m_pGameInstance->Change_CameraType(CCamera_Manager::FREE);

	Create_Arms();

	return S_OK;
}

HRESULT CLevel_Camera_Tool::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{
	CGameObject** pGameObject = nullptr;

	CTest_Player::CONTAINEROBJ_DESC Desc;
	Desc.iCurLevelID = LEVEL_CAMERA_TOOL;
	Desc.tTransform2DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
	Desc.tTransform2DDesc.vScaling = _float3(150.f, 150.f, 150.f);

	Desc.tTransform3DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
	Desc.tTransform3DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);

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

	TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	TerrainDesc.strModelPrototypeTag_3D = TEXT("Prototype_Component_Model_WoodenPlatform_01");

	TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	TerrainDesc.tTransform3DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
	TerrainDesc.tTransform3DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CAMERA_TOOL, TEXT("Prototype_GameObject_Test_Terrain"), LEVEL_CAMERA_TOOL, _strLayerTag, &TerrainDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Camera_Tool::Show_CameraTool()
{
	ImGui::Begin("Test");
	ImGui::Text("Current Arm");
	
	// Roteate
	Rotate_Arm(m_isCopyArm);

	// Length
	Change_ArmLength(m_isCopyArm);

	// CheckBox
	ImGui::NewLine();
	ImGui::Checkbox("Rotate CopyArm", &m_isCopyArm);
	ImGui::SameLine(155.f);

	// Copy
	if (ImGui::Button("Create CopyArm")) {
		CArm_Manager::GetInstance()->Copy_Arm();
	}

	// Add CopyArm
	if (ImGui::Button("Add CopyArm")) {
		ImGui::SameLine(155.f);
		Add_CopyArm();
	}

	ImGui::End();
}

void CLevel_Camera_Tool::Create_Arms()
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Ptr(LEVEL_CAMERA_TOOL, TEXT("Layer_Player"), 0);
	_vector vPlayerLook = pPlayer->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);

	CCameraArm::CAMERA_ARM_DESC Desc{};

	XMStoreFloat3(&Desc.vArm, -vPlayerLook);
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	Desc.vRotation = { XMConvertToRadians(60.f), XMConvertToRadians(30.f), 0.f };
	Desc.fLength = 20.f;
	Desc.wszArmTag = TEXT("Player_Arm");
	Desc.pTargetWorldMatrix = pPlayer->Get_ControllerTransform()->Get_WorldMatrix_Ptr();

	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);


	CCamera_Target* pTarget = dynamic_cast<CCamera_Target*>(m_pGameInstance->Get_Camera(CCamera_Manager::TARGET));

	pTarget->Add_Arm(pArm);
	CArm_Manager::GetInstance()->Set_CurrentArm(pArm);
}

void CLevel_Camera_Tool::Rotate_Arm(_bool _isCopyArm)
{
	_vector vCamPos = XMLoadFloat4((m_pGameInstance->Get_CamPosition()));
	_vector vArmRotation = {};

	ImGui::Text("Camera Position: %.2f, %.2f, %.2f", XMVectorGetX(vCamPos), XMVectorGetY(vCamPos), XMVectorGetZ(vCamPos));
	ImGui::NewLine();

	ImGui::Text("Ratation Value: %.2f", m_fRotationValue);
	ImGui::SameLine();
	ImGui::DragFloat("##Rotate", &m_fRotationValue, 0.1f, 0.f, 10.f);

	ImGui::Text("RotationValueX: %.2f", m_fRotationValue);
	ImGui::SameLine();
	if (ImGui::Button("-X Angle") || ImGui::IsItemActive()) // 누르고 있는 동안 계속 동작
		vArmRotation = XMVectorSetX(vArmRotation, -m_fRotationValue);
	ImGui::SameLine();
	if (ImGui::Button("+X Angle") || ImGui::IsItemActive())
		vArmRotation = XMVectorSetX(vArmRotation, m_fRotationValue);

	ImGui::Text("RotationValueY: %.2f", m_fRotationValue);
	ImGui::SameLine();
	if (ImGui::Button("-Y Angle") || ImGui::IsItemActive()) // 누르고 있는 동안 계속 동작
		vArmRotation = XMVectorSetY(vArmRotation, -m_fRotationValue);
	ImGui::SameLine();
	if (ImGui::Button("+Y Angle") || ImGui::IsItemActive())
		vArmRotation = XMVectorSetY(vArmRotation, m_fRotationValue);

	ImGui::Text("RotationValueZ: %.2f", m_fRotationValue);
	ImGui::SameLine();
	if (ImGui::Button("-Z Angle") || ImGui::IsItemActive()) // 누르고 있는 동안 계속 동작
		vArmRotation = XMVectorSetZ(vArmRotation, -m_fRotationValue);
	ImGui::SameLine();
	if (ImGui::Button("+Z Angle") || ImGui::IsItemActive())
		vArmRotation = XMVectorSetZ(vArmRotation, m_fRotationValue);

	_vector vRadianRotation = XMVectorSet(XMConvertToRadians(XMVectorGetX(vArmRotation)), XMConvertToRadians(XMVectorGetY(vArmRotation)), XMConvertToRadians(XMVectorGetZ(vArmRotation)), 0.f);

	CArm_Manager::GetInstance()->Set_ArmRotation(vRadianRotation, _isCopyArm);
}

void CLevel_Camera_Tool::Change_ArmLength(_bool _isCopyArm)
{
	ImGui::NewLine();
	ImGui::Text("Length Value: %.2f", m_fLengthValue);
	ImGui::SameLine();
	ImGui::DragFloat("##Length", &m_fLengthValue, 0.1f, 0.f, 10.f);

	_float fArmLength = CArm_Manager::GetInstance()->Get_ArmLength(_isCopyArm);
	_bool bActive = false;

	ImGui::Text("Arm Length: %.2f  ", m_fLengthValue);
	ImGui::SameLine();
	if (ImGui::Button("- Length") || ImGui::IsItemActive()) {// 누르고 있는 동안 계속 동작
		fArmLength -= m_fLengthValue;
		CArm_Manager::GetInstance()->Set_ArmLength(fArmLength, _isCopyArm);
	}
	ImGui::SameLine();
	if (ImGui::Button("+ Length") || ImGui::IsItemActive()) {
		fArmLength += m_fLengthValue;
		CArm_Manager::GetInstance()->Set_ArmLength(fArmLength, _isCopyArm);
	}
}

void CLevel_Camera_Tool::Add_CopyArm()
{
	//ImGui::NewLine();

	ImGui::InputText("CopyArm Tag", m_szCopyArmName, MAX_PATH);
	CArm_Manager::GetInstance()->Add_CopyArm(m_pGameInstance->StringToWString(m_szCopyArmName));
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
