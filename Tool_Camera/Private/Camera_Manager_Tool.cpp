#include "stdafx.h"
#include "Camera_Manager_Tool.h"

#include "GameInstance.h"

#include "Camera_Target.h"
#include "Camera_CutScene.h"

IMPLEMENT_SINGLETON(CCamera_Manager_Tool)

CCamera_Manager_Tool::CCamera_Manager_Tool()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager_Tool::Initialize()
{
	for (auto& Camera : m_Cameras) {
		Camera = nullptr;
	}

	return S_OK;
}

void CCamera_Manager_Tool::Update()
{
}

void CCamera_Manager_Tool::Render()
{

}

void CCamera_Manager_Tool::Clear()
{
	for (auto& ArmData : m_ArmDatas) {
		Safe_Delete(ArmData.second.first);
		Safe_Delete(ArmData.second.second);
	}
	m_ArmDatas.clear();

	for (auto& Camera : m_Cameras) {
		Safe_Release(Camera);
		Camera = nullptr;
	}


	Safe_Release(m_pCurrentArm);
	m_pCurrentArm = nullptr;
}

_vector CCamera_Manager_Tool::Get_CameraVector(CTransform::STATE _eState)
{
	CController_Transform* pConTrans = m_Cameras[m_eCurrentCameraType]->Get_ControllerTransform();
	return pConTrans->Get_State(_eState);
}

void CCamera_Manager_Tool::Get_ArmNames(vector<_wstring>* _vecArmNames)
{
	_vecArmNames->clear();

	for (auto& ArmName : m_ArmDatas) {
		_vecArmNames->push_back(ArmName.first);
	}
}

pair<ARM_DATA*, SUB_DATA*>* CCamera_Manager_Tool::Get_ArmData(_wstring _wszArmName)
{
	pair<ARM_DATA*, SUB_DATA*>* pData = Find_ArmData(_wszArmName);

	if (nullptr == pData)
		return nullptr;

	return pData;
}

void CCamera_Manager_Tool::Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera)
{
	if (nullptr == _pCamera)
		return;

	m_eCurrentCameraType = _iCurrentCameraType;
	m_Cameras[_iCurrentCameraType] = _pCamera;

	Safe_AddRef(m_Cameras[_iCurrentCameraType]);
}

void CCamera_Manager_Tool::Change_CameraMode(_uint _iCameraMode, _int _iNextMode)
{
	if (TARGET == m_eCurrentCameraType) {
		dynamic_cast<CCamera_Target*>(m_Cameras[m_eCurrentCameraType])->Set_CameraMode(_iCameraMode, _iNextMode);
	}
	else
		return;
}


void CCamera_Manager_Tool::Change_CameraType(_uint _iCurrentCameraType)
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
			if (nullptr == m_Cameras[TARGET])
				return;
			CController_Transform* pTargetConTrans = m_Cameras[TARGET]->Get_ControllerTransform();
			CController_Transform* pFreeConTrans = m_Cameras[FREE]->Get_ControllerTransform();
			pFreeConTrans->Set_WorldMatrix(pTargetConTrans->Get_WorldMatrix());
		}
	}

	m_eCurrentCameraType = _iCurrentCameraType;
}

void CCamera_Manager_Tool::Change_CameraTarget(const _float4x4* _pTargetWorldMatrix)
{
	if (nullptr != m_Cameras[TARGET] && TARGET == m_eCurrentCameraType) {
		dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Change_Target(_pTargetWorldMatrix);
	}
}

void CCamera_Manager_Tool::Set_NextArmData(_wstring _wszNextArmName)
{
	pair<ARM_DATA*, SUB_DATA*>* pData = Find_ArmData(_wszNextArmName);

	if (nullptr == pData)
		return;

	if (nullptr != m_Cameras[TARGET] && TARGET == m_eCurrentCameraType) {
		dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Set_NextArmData(pData->first, pData->second);
	}
}

void CCamera_Manager_Tool::Start_Zoom(CAMERA_TYPE _eCameraType, _float _fZoomTime, _uint _iZoomLevel, _uint _iRatioType)
{
	m_Cameras[_eCameraType]->Start_Zoom(_fZoomTime, (CCamera::ZOOM_LEVEL)_iZoomLevel, (RATIO_TYPE)_iRatioType);
}

void CCamera_Manager_Tool::Start_Changing_AtOffset(CAMERA_TYPE _eCameraType, _float _fOffsetTime, _vector _vNextOffset, _uint _iRatioType)
{
	if (FREE == m_eCurrentCameraType)
		return;

	m_Cameras[_eCameraType]->Start_Changing_AtOffset(_fOffsetTime, _vNextOffset, _iRatioType);
}

void CCamera_Manager_Tool::Start_Shake_ByTime(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime, _uint _iShakeType, _float _fDelayTime)
{
	if (FREE == m_eCurrentCameraType)
		return;

	m_Cameras[_eCameraType]->Start_Shake_ByTime(_fShakeTime, _fShakeForce, _fShakeCycleTime, (CCamera::SHAKE_TYPE)_iShakeType, _fDelayTime);
}

void CCamera_Manager_Tool::Start_Shake_ByCount(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _uint _iShakeCount, _uint _iShakeType, _float _fDelayTime)
{
	if (FREE == m_eCurrentCameraType)
		return;

	m_Cameras[_eCameraType]->Start_Shake_ByCount(_fShakeTime, _fShakeForce, _iShakeCount, (CCamera::SHAKE_TYPE)_iShakeType, _fDelayTime);
}
//
//void CCamera_Manager_Tool::Add_NextArm_Info(_wstring _wszArmTag, ARM_DATA _pData)
//{
//	Add_ArmData(_wszArmTag, _pData);
//}

void CCamera_Manager_Tool::Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData)
{
	if (nullptr == Find_ArmData(_wszArmTag)) {
		m_ArmDatas.emplace(_wszArmTag, make_pair(_pArmData, _pSubData));
	}
	else {
		Safe_Delete(_pArmData);
		Safe_Delete(_pSubData);
	}
}

void CCamera_Manager_Tool::Add_CutScene(_wstring _wszCutSceneTag, vector<CCutScene_Sector*> _vecCutScene)
{
	if (nullptr == m_Cameras[CUTSCENE])
		return;

	dynamic_cast<CCamera_CutScene*>(m_Cameras[CUTSCENE])->Add_CutScene(_wszCutSceneTag, _vecCutScene);
}

void CCamera_Manager_Tool::Edit_ArmInfo(_wstring _wszArmTag)
{
}

void CCamera_Manager_Tool::Reset_CurrentArm(_fvector vArm, _float fLength, _fvector _vAtOffset, _int _iZoomLevel)
{
	m_pCurrentArm->Set_Length(fLength);
	m_pCurrentArm->Set_ArmVector(vArm);

	m_Cameras[TARGET]->Set_AtOffset(_vAtOffset);
	m_Cameras[TARGET]->Set_ZoomLevel(_iZoomLevel);
}

_float CCamera_Manager_Tool::Get_ArmLength()
{
	if (nullptr != m_pCurrentArm)
		return m_pCurrentArm->Get_Length();
		
	return _float();
}

_uint CCamera_Manager_Tool::Get_ZoomLevel()
{
	if (nullptr == m_Cameras[TARGET])
		return -1;

	return m_Cameras[TARGET]->Get_CurrentZoomLevel();
}

_float3 CCamera_Manager_Tool::Get_CurrentArmVector()
{
	_float3 vArm;
	XMStoreFloat3(&vArm, m_pCurrentArm->Get_ArmVector());
	return vArm;
}

void CCamera_Manager_Tool::Set_ArmRotation(_vector _vRotation)
{
	if (nullptr != m_pCurrentArm)
		m_pCurrentArm->Set_Rotation(_vRotation);
}

void CCamera_Manager_Tool::Set_ArmLength(_float _fLength)
{
	if (nullptr != m_pCurrentArm)
		m_pCurrentArm->Set_Length(_fLength);
		
}

void CCamera_Manager_Tool::Set_CurrentArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurrentArm = _pCameraArm;
	Safe_AddRef(m_pCurrentArm);

}

void CCamera_Manager_Tool::Set_CurrentArmVector(_float3 _vCurArmVector)
{
	if (nullptr == m_pCurrentArm)
		return;

	m_pCurrentArm->Set_ArmVector(XMLoadFloat3(&_vCurArmVector));
}

void CCamera_Manager_Tool::Set_ZoomLevel(_uint _iZoomLevel)
{
	if (nullptr == m_Cameras[TARGET])
		return;

	m_Cameras[TARGET]->Set_ZoomLevel(_iZoomLevel);
}

//CCameraArm* CCamera_Manager_Tool::Find_Arm(_wstring _wszArmTag)
//{
//	auto iter = m_Arms.find(_wszArmTag);
//
//	if (iter == m_Arms.end())
//		return nullptr;
//
//	return iter->second;
//}

pair<ARM_DATA*, SUB_DATA*>* CCamera_Manager_Tool::Find_ArmData(_wstring _wszArmTag)
{
	auto iter = m_ArmDatas.find(_wszArmTag);

	if (iter == m_ArmDatas.end())
		return nullptr;

	return &(iter->second);
}

void CCamera_Manager_Tool::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& ArmData : m_ArmDatas) {
		Safe_Delete(ArmData.second.first);
		Safe_Delete(ArmData.second.second);
	}
	m_ArmDatas.clear();

	for (auto& Camera : m_Cameras)
		Safe_Release(Camera);

	Safe_Release(m_pCurrentArm);

	__super::Free();
}
