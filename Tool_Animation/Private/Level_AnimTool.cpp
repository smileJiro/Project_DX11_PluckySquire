#include "stdafx.h"
#include "Level_AnimTool.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Test2DModel.h"
#include "Test3DModel.h"
#include "TestModelObject.h"
#include "Camera_Target.h"
#include "CameraArm.h"
#include "TestTerrain.h"

CLevel_AnimTool::CLevel_AnimTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_AnimTool::Initialize()
{
	m_fDefault2DCamSize = m_pGameInstance->Get_RT_Size(L"Target_Book_2D");

	Ready_Lights();
	//Ready_Layer_TestTerrain(TEXT("Terrain"));
	//Create_Camera(TEXT("Camera"));
	SetWindowText(g_hWnd, TEXT("애니메이션 툴입니다."));
	return S_OK;
}

void CLevel_AnimTool::Update(_float _fTimeDelta)
{
	ImGui::Begin("Animation Tool");
	LOADMODEL_TYPE	eLoadModelTYpe = LOADMODEL_TYPE::LOAD_LAST;
	std::wstring wstrSelectedPath;

	if (ImGui::Button("Open Model File")) {
		wstrSelectedPath = OpenFileDialog(TEXT("Model3D Files\0 * .model\0Model2D Files\0 * .model2d")); // 파일 다이얼로그 호출
		if (!wstrSelectedPath.empty()) {
			if (wstrSelectedPath.find(L".model") != std::wstring::npos)
			{
				eLoadModelTYpe = LOAD_3D;
			}
			else if (wstrSelectedPath.find(L".model2d") != std::wstring::npos)
			{
				eLoadModelTYpe = LOAD_2D;
			}
		}
	}
	if (ImGui::Button("Open Raw 2DModel Data")) {
		wstrSelectedPath = OpenDirectoryDialog(); // 폴더 다이얼로그 호출
		if (!wstrSelectedPath.empty())
		{
			eLoadModelTYpe = LOAD_RAW2D;
		}
	}
	ImGui::End();

	if(LOAD_LAST != eLoadModelTYpe)
	{
		if (SUCCEEDED(Load_Model(eLoadModelTYpe, wstrSelectedPath)))
		{
			lstrcpyW(m_szLoadingText, wstrSelectedPath.c_str());
			Create_Camera(TEXT("Camera"), m_pTestModelObj);
			Set_Animation(0);
		}
	}
	if (m_pTestModelObj)
	{
		_float fMove = (_float)MOUSE_MOVE(MOUSE_AXIS::Z) / 100.f;
		if (COORDINATE_3D == m_pTestModelObj->Get_CurCoord())
		{
			m_fZoomMultiplier += fMove * m_f3DZoomSpeed * _fTimeDelta;
			m_pTargetCam->Set_Fovy(m_fDefault3DCamFovY * m_fZoomMultiplier);
		}
		else
		{
			m_fZoomMultiplier += fMove * m_f2DZoomSpeed * _fTimeDelta;
			m_pTestModelObj->Set_2DProjMatrix(XMMatrixOrthographicLH((_float)m_fDefault2DCamSize.x * m_fZoomMultiplier, (_float)m_fDefault2DCamSize.y * m_fZoomMultiplier, 0.0f, 1.0f));
		}
	}
}

HRESULT CLevel_AnimTool::Render()
{
#ifdef _DEBUG
	if (m_pTestModelObj)
		SetWindowText(g_hWnd, m_szLoadingText);
#endif

	return S_OK;
}

void CLevel_AnimTool::Update_Imgui()
{
}

void CLevel_AnimTool::Set_Animation(_uint _iAnimIdx, _bool _bLoop)
{
	m_pTestModelObj->Switch_Animation(_iAnimIdx);
	m_pTestModelObj->Set_AnimationLoop(m_pTestModelObj->Get_CurCoord(), _iAnimIdx, _bLoop);
}

HRESULT CLevel_AnimTool::Ready_Lights()
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

HRESULT CLevel_AnimTool::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{

	/* Test Terrain */
	CTestTerrain::MODELOBJECT_DESC TerrainDesc{};

	TerrainDesc.eStartCoord = COORDINATE_3D;
	TerrainDesc.iCurLevelID = LEVEL_ANIMTOOL;
	TerrainDesc.isCoordChangeEnable = false;
	TerrainDesc.iModelPrototypeLevelID_3D = LEVEL_ANIMTOOL;
	TerrainDesc.strModelPrototypeTag_3D = TEXT("WoodenPlatform_01");
	TerrainDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");

	TerrainDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

	TerrainDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	TerrainDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	TerrainDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	TerrainDesc.tTransform3DDesc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_ANIMTOOL, TEXT("Prototype_GameObject_TestTerrain"), LEVEL_ANIMTOOL, _strLayerTag, (CGameObject**) & m_pTestTerrain, &TerrainDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AnimTool::Create_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{

	//CGameObject* pCamera = nullptr;

	// Free Camera
	/*CCamera_Free::CAMERA_FREE_DESC Desc{};

	Desc.fMouseSensor = 0.1f;

	Desc.fFovy = XMConvertToRadians(60.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);
	Desc.eZoomLevel = CCamera::LEVEL_6;*/


	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_ANIMTOOL, TEXT("Prototype_GameObject_Camera_Free"),
	//	LEVEL_ANIMTOOL, _strLayerTag, &pCamera, &Desc)))
	//	return E_FAIL;
	//	

	// Target Camera
	assert(nullptr != m_pTestModelObj);
	if(m_pTargetCam)
		m_pTargetCam->Set_Dead();
	m_pTargetCam = nullptr;
	CCamera_Target::CAMERA_TARGET_DESC TargetDesc{};

	TargetDesc.fSmoothSpeed = 5.f;
	TargetDesc.eCameraMode = CCamera_Target::DEFAULT;

	TargetDesc.fFovy = XMConvertToRadians(60.f);
	TargetDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	TargetDesc.fNear = 0.1f;
	TargetDesc.fFar = 1000.f;
	TargetDesc.vEye = _float3(0.f, 10.f, -7.f);
	TargetDesc.vAt = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_ANIMTOOL, TEXT("Prototype_GameObject_Camera_Target"),
		LEVEL_ANIMTOOL, _strLayerTag, (CGameObject**)&m_pTargetCam, &TargetDesc)))
		return E_FAIL;

	_vector vTargetLook = _pTarget->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);
	CCameraArm::CAMERA_ARM_DESC Desc{};

	XMStoreFloat3(&Desc.vArm, vTargetLook);
	Desc.vPosOffset = { 0.f, 0.f, 0.f };
	Desc.vRotation = { XMConvertToRadians(30.f), XMConvertToRadians(0.f), 0.f };
	Desc.fLength = 10.f;
	Desc.wszArmTag = TEXT("Cam_Arm");
	Desc.pTargetWorldMatrix = _pTarget->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
	CCameraArm* pArm = CCameraArm::Create(m_pDevice, m_pContext, &Desc);

	m_pTargetCam->Add_Arm(pArm);

	return S_OK;
}

HRESULT CLevel_AnimTool::Load_Model(LOADMODEL_TYPE _eType, wstring _wstrPath)
{
	assert(LOAD_LAST != _eType);
	assert(!_wstrPath.empty());

	SetWindowText(g_hWnd, TEXT("모델(을)를 로딩중입니다."));

	CTestModelObject::TESTMODELOBJ_DESC tModelObjDesc{};
	tModelObjDesc.isCoordChangeEnable = false;
	tModelObjDesc.iCurLevelID = LEVEL_ANIMTOOL;
	switch (_eType)
	{
	case AnimTool::CLevel_AnimTool::LOAD_3D:
		tModelObjDesc.tTransform3DDesc.vInitialPosition = _float3(0, 0, 0);
		tModelObjDesc.tTransform3DDesc.vInitialScaling = _float3(1, 1, 1);
		tModelObjDesc.eStartCoord = COORDINATE_3D;
		tModelObjDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
		tModelObjDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
		break;
	case AnimTool::CLevel_AnimTool::LOAD_2D:
	case AnimTool::CLevel_AnimTool::LOAD_RAW2D:
		tModelObjDesc.tTransform2DDesc.vInitialPosition = _float3(0, 0, 0);
		tModelObjDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
		tModelObjDesc.eStartCoord = COORDINATE_2D;
		tModelObjDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
		tModelObjDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
		break;
	case AnimTool::CLevel_AnimTool::LOAD_LAST:
		break;
	default:
		break;
	}


	CModel* pTmpModel = nullptr;
	switch (_eType)
	{
	case AnimTool::CLevel_AnimTool::LOAD_3D:
	{
		XMMATRIX matPretransform = XMMatrixScaling(1 / 150.0f, 1 / 150.0f, 1 / 150.0f);
		matPretransform *= XMMatrixRotationAxis(_vector{ 0,1,0,0 }, XMConvertToRadians(180));
		pTmpModel = CTest3DModel::Create(m_pDevice, m_pContext, WstringToString(_wstrPath).c_str(), matPretransform);
		break;
	}
	case AnimTool::CLevel_AnimTool::LOAD_2D:
	{
		pTmpModel = CTest2DModel::Create(m_pDevice, m_pContext, false, WstringToString(_wstrPath).c_str());
		break;
	}
	case AnimTool::CLevel_AnimTool::LOAD_RAW2D:
	{
		pTmpModel = CTest2DModel::Create(m_pDevice, m_pContext, true, WstringToString(_wstrPath).c_str());
		break;
	}
	case AnimTool::CLevel_AnimTool::LOAD_LAST:
	default:
		break;
	}
	if (pTmpModel)
	{
		if(FAILED(pTmpModel->Initialize(nullptr)))
		{
			Safe_Release(pTmpModel);
			return E_FAIL;
		}
		if (m_pTestModelObj)
			m_pTestModelObj->Set_Dead();

		tModelObjDesc.pModel = pTmpModel;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_TestModelObject"), LEVEL_ANIMTOOL, TEXT("MainModel"), (CGameObject**)&m_pTestModelObj, &tModelObjDesc)))
		{
			Safe_Release(pTmpModel);
			return E_FAIL;
		}
		

		return S_OK;
	}
	else
		return E_FAIL;
}

CLevel_AnimTool* CLevel_AnimTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_AnimTool* pInstance = new CLevel_AnimTool(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_AnimTool");
		Safe_Release(pInstance);
	}
	return pInstance;

}

void CLevel_AnimTool::Free()
{
	//Safe_Release(m_pTestModelObj);
	__super::Free();
}
