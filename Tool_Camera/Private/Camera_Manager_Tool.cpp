#include "stdafx.h"
#include "Camera_Manager_Tool.h"

#include "GameInstance.h"

#include "Camera_Target.h"

IMPLEMENT_SINGLETON(CCamera_Manager_Tool)

CCamera_Manager_Tool::CCamera_Manager_Tool()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager_Tool::Initialize()
{
	return S_OK;
}

void CCamera_Manager_Tool::Update()
{
}

void CCamera_Manager_Tool::Render()
{
	for (auto& Arm : m_Arms) 
		Arm.second->Render_Arm();
	
	if (nullptr != m_pCopyArm)
		m_pCopyArm->Render_Arm();

	if (nullptr != m_pCurrentArm)
		m_pCurrentArm->Render_Arm();
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

ARM_DATA* CCamera_Manager_Tool::Get_ArmData(_wstring _wszArmName)
{
	ARM_DATA* pData = Find_ArmData(_wszArmName);

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
	m_eCurrentCameraType = _iCurrentCameraType;

	CController_Transform* pTargetConTrans = m_Cameras[TARGET]->Get_ControllerTransform();
	CController_Transform* pFreeConTrans = m_Cameras[FREE]->Get_ControllerTransform();

	switch (m_eCurrentCameraType) {
	case FREE:
		m_Cameras[FREE]->Set_Active(true);
		m_Cameras[TARGET]->Set_Active(false);

		pFreeConTrans->Set_WorldMatrix(pTargetConTrans->Get_WorldMatrix());
		break;

	case TARGET:
		m_Cameras[FREE]->Set_Active(false);
		m_Cameras[TARGET]->Set_Active(true);
		break;
	}
}

void CCamera_Manager_Tool::Change_CameraTarget(const _float4x4* _pTargetWorldMatrix)
{
	if (nullptr != m_Cameras[TARGET] && TARGET == m_eCurrentCameraType) {
		dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Change_Target(_pTargetWorldMatrix);
	}
}

void CCamera_Manager_Tool::Set_NextArmData(_wstring _wszNextArmName)
{
	ARM_DATA* pData = Find_ArmData(_wszNextArmName);

	if (nullptr == pData)
		return;

	if (nullptr != m_Cameras[TARGET] && TARGET == m_eCurrentCameraType) {
		dynamic_cast<CCamera_Target*>(m_Cameras[TARGET])->Set_NextArmData(pData);
	}
}

void CCamera_Manager_Tool::Copy_Arm()
{
	if (nullptr != m_pCurrentArm) {
		if(nullptr != m_pCopyArm)
			Safe_Release(m_pCopyArm);
		
		m_pCopyArm = m_pCurrentArm->Clone();
	}
}

void CCamera_Manager_Tool::Add_CopyArm(_wstring _wszArmTag, ARM_DATA _pData)
{
	if (nullptr == m_pCopyArm)
		return;

	if (nullptr == Find_Arm(_wszArmTag)) {
		m_Arms.emplace(_wszArmTag, m_pCopyArm);
		m_pCopyArm->Set_ArmType(CCameraArm::OTHER);
		m_pCopyArm->Set_ArmTag(_wszArmTag);

		Add_ArmData(_wszArmTag, _pData);

		m_pCopyArm = nullptr;
	}
	else
		return;
}

void CCamera_Manager_Tool::Add_ArmData(_wstring _wszArmTag, ARM_DATA _pData)
{
	if (nullptr == Find_ArmData(_wszArmTag)) {
		ARM_DATA* pArmData = new ARM_DATA();

		XMStoreFloat3(&pArmData->vArm, m_pCopyArm->Get_ArmVector());
		pArmData->fLength = m_pCopyArm->Get_Length();
		pArmData->fMoveTimeAxisY = _pData.fMoveTimeAxisY;
		pArmData->fMoveTimeAxisRight = _pData.fMoveTimeAxisRight;
		pArmData->fLengthTime = _pData.fLengthTime;
		pArmData->iRotateType = _pData.iRotateType;
		pArmData->iTimeRateType = _pData.iTimeRateType;

		m_ArmDatas.emplace(_wszArmTag, pArmData);
	}
}

void CCamera_Manager_Tool::Edit_ArmInfo(_wstring _wszArmTag)
{
}

_float CCamera_Manager_Tool::Get_ArmLength(_bool _isCopyArm)
{
	if (true == _isCopyArm) {
		if (nullptr != m_pCopyArm)
			return m_pCopyArm->Get_Length();
	}
	else {
		if (nullptr != m_pCurrentArm)
			return m_pCurrentArm->Get_Length();
	}
		
}

void CCamera_Manager_Tool::Set_ArmRotation(_vector _vRotation, _bool _isCopyArm)
{
	if (true == _isCopyArm) {
		if(nullptr != m_pCopyArm)
			m_pCopyArm->Set_Rotation(_vRotation);
	}
	else {
		if(nullptr != m_pCurrentArm)
			m_pCurrentArm->Set_Rotation(_vRotation);
	}
}

void CCamera_Manager_Tool::Set_ArmLength(_float _fLength, _bool _isCopyArm)
{
	if (true == _isCopyArm) {
		if (nullptr != m_pCopyArm)
			m_pCopyArm->Set_Length(_fLength);
	}
	else {
		if (nullptr != m_pCurrentArm)
			m_pCurrentArm->Set_Length(_fLength);
	}
		
}

void CCamera_Manager_Tool::Set_CurrentArm(CCameraArm* _pCameraArm)
{
	if (nullptr == _pCameraArm)
		return;

	m_pCurrentArm = _pCameraArm;
	Safe_AddRef(m_pCurrentArm);

}

CCameraArm* CCamera_Manager_Tool::Find_Arm(_wstring _wszArmTag)
{
	auto iter = m_Arms.find(_wszArmTag);

	if (iter == m_Arms.end())
		return nullptr;

	return iter->second;
}

ARM_DATA* CCamera_Manager_Tool::Find_ArmData(_wstring _wszArmTag)
{
	auto iter = m_ArmDatas.find(_wszArmTag);

	if (iter == m_ArmDatas.end())
		return nullptr;

	return iter->second;
}

void CCamera_Manager_Tool::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Arm : m_Arms)
		Safe_Release(Arm.second);
	m_Arms.clear();

	for (auto& ArmData : m_ArmDatas)
		Safe_Delete(ArmData.second);
	m_ArmDatas.clear();

	Safe_Release(m_pCurrentArm);
	Safe_Release(m_pCopyArm);

	__super::Free();
}
