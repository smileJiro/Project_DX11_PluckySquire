#include "stdafx.h"
#include "../Public/Camera_Manager.h"

#include "GameInstance.h"

#include "Camera_Target.h"
#include "Camera_CutScene.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize()
{
	for (auto& Camera : m_Cameras)
		Camera = nullptr;

	return S_OK;
}

void CCamera_Manager::Update(_float fTimeDelta)
{
	_int a = 0;
}

_vector CCamera_Manager::Get_CameraVector(CTransform::STATE _eState)
{
	CController_Transform* pConTrans = m_Cameras[m_eCurrentCameraType]->Get_ControllerTransform();
	return pConTrans->Get_State(_eState);
}

void CCamera_Manager::Start_ZoomIn()
{
	_uint iZoomLevel = m_Cameras[m_eCurrentCameraType]->Get_CurrentZoomLevel();

	if (iZoomLevel <= CCamera::LEVEL_1)
		return;

	m_Cameras[m_eCurrentCameraType]->Start_Zoom(0.5f, (CCamera::ZOOM_LEVEL)(iZoomLevel - 1), CCamera::EASE_IN);
}

void CCamera_Manager::Start_ZoomOut()
{
	_uint iZoomLevel = m_Cameras[m_eCurrentCameraType]->Get_CurrentZoomLevel();

	if (iZoomLevel >= CCamera::LEVEL_10)
		return;

	m_Cameras[m_eCurrentCameraType]->Start_Zoom(0.5f, (CCamera::ZOOM_LEVEL)(iZoomLevel + 1), CCamera::EASE_IN);
}

void CCamera_Manager::Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera)
{
	if (nullptr == _pCamera)
		return;

	m_eCurrentCameraType = _iCurrentCameraType;
	m_Cameras[_iCurrentCameraType] = _pCamera;

	Safe_AddRef(m_Cameras[_iCurrentCameraType]);
}

void CCamera_Manager::Add_ArmData(_wstring wszArmTag, ARM_DATA _pData)
{
	if (nullptr == m_Cameras[TARGET])
		return;

	dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Add_ArmData(wszArmTag, _pData);
}

void CCamera_Manager::Add_CutScene(_wstring _wszCutSceneTag, pair<_float2, vector<CUTSCENE_DATA>> _CutSceneData)
{
	if (nullptr == m_Cameras[CUTSCENE])
		return;

	dynamic_cast<CCamera_CutScene*>(m_Cameras[CUTSCENE])->Add_CutScene(_wszCutSceneTag, _CutSceneData);
}

void CCamera_Manager::Change_CameraMode(_uint _iCameraMode, _int _iNextMode)
{
	if (TARGET == m_eCurrentCameraType) {
		dynamic_cast<CCamera_Target*>(m_Cameras[m_eCurrentCameraType])->Set_CameraMode(_iCameraMode, _iNextMode);
	}
	else
		return;
}

void CCamera_Manager::Change_CameraType(_uint _iCurrentCameraType)
{
	if (nullptr == m_Cameras[_iCurrentCameraType])
		return;

	for (auto& Camera : m_Cameras) {
		if (nullptr == Camera)
			continue;

		if (_iCurrentCameraType == Camera->Get_CamType())
			Camera->Set_Active(true);
		else
			Camera->Set_Active(false);

		if (FREE == _iCurrentCameraType) {
			CController_Transform* pTargetConTrans = m_Cameras[TARGET]->Get_ControllerTransform();
			CController_Transform* pFreeConTrans = m_Cameras[FREE]->Get_ControllerTransform();
			pFreeConTrans->Set_WorldMatrix(pTargetConTrans->Get_WorldMatrix());
		}
	}

	m_eCurrentCameraType = _iCurrentCameraType;
}

void CCamera_Manager::Change_CameraTarget(const _float4x4* _pTargetWorldMatrix)
{
	if (nullptr == m_Cameras[TARGET] || TARGET != m_eCurrentCameraType)
		return;

	dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Change_Target(_pTargetWorldMatrix);
}

void CCamera_Manager::Set_NextArmData(_wstring _wszNextArmName)
{
	if (nullptr == m_Cameras[TARGET] || TARGET != m_eCurrentCameraType)
		return;

	dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Set_NextArmData(_wszNextArmName);
}

void CCamera_Manager::Set_NextCutSceneData(_wstring _wszCutSceneName, CUTSCENE_INITIAL_DATA* _pInitialData)
{
	if (nullptr == m_Cameras[CUTSCENE] || CUTSCENE != m_eCurrentCameraType)
		return;

	dynamic_cast<CCamera_CutScene*>(m_Cameras[CUTSCENE])->Set_NextCutScene(_wszCutSceneName, _pInitialData);
}

void CCamera_Manager::Start_Zoom(CAMERA_TYPE _eCameraType, _float _fZoomTime, _uint _iZoomLevel, _uint _iRatioType)
{
}

void CCamera_Manager::Start_Changing_AtOffset(CAMERA_TYPE _eCameraType, _float _fOffsetTime, _vector _vNextOffset, _uint _iRatioType)
{
}

void CCamera_Manager::Start_Shake_ByTime(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime, _uint _iShakeType, _float _fDelayTime)
{
}

void CCamera_Manager::Start_Shake_ByCount(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _uint _iShakeCount, _uint _iShakeType, _float _fDelayTime)
{
}

void CCamera_Manager::Load_ArmData()
{
}

void CCamera_Manager::Load_CutSceneData()
{
	_wstring wszLoadPath = L"../Bin/DataFiles/Camera/CutSceneData/Test.bin";

	std::ifstream inFile(wszLoadPath, std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
	}

	_uint iSize = {};

	inFile.read(reinterpret_cast<char*>(&iSize), sizeof(_uint));

	for (_uint i = 0; i < iSize; ++i) {
		
		pair<_float2, vector<CUTSCENE_DATA>> CutSceneData;

		// CutScene Tag 읽기
		_uint strLength = {};
		inFile.read(reinterpret_cast<char*>(&strLength), sizeof(_uint));
		_wstring CutSceneTag;
		CutSceneTag.resize(strLength);
		inFile.read(reinterpret_cast<char*>(&CutSceneTag[0]), strLength * sizeof(wchar_t));

		// Time 읽기
		inFile.read(reinterpret_cast<char*>(&CutSceneData.first), sizeof(_float2));

		// Data Struct 읽기
		_uint iDataSize = {};
		inFile.read(reinterpret_cast<char*>(&iDataSize), sizeof(_uint));

		for (_uint j = 0; j < iDataSize; ++j) {
			
			CUTSCENE_DATA tData = {};

			inFile.read(reinterpret_cast<char*>(&tData.vPosition), sizeof(_float3));
			inFile.read(reinterpret_cast<char*>(&tData.vRotation), sizeof(_float3));
			inFile.read(reinterpret_cast<char*>(&tData.vAtOffset), sizeof(_float3));
			inFile.read(reinterpret_cast<char*>(&tData.fFovy), sizeof(_float));

			CutSceneData.second.push_back(tData);
		}

		Add_CutScene(CutSceneTag, CutSceneData);
	}
}

CCamera_Manager* CCamera_Manager::Create()
{
	CCamera_Manager* pInstance = new CCamera_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCamera_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Camera : m_Cameras)
		Safe_Release(Camera);

	__super::Free();
}
