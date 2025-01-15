#include "Camera_Manager.h"

#include "GameInstance.h"
#include "Camera.h"

CCamera_Manager::CCamera_Manager()
{
}

HRESULT CCamera_Manager::Initialize()
{
	return S_OK;
}

void CCamera_Manager::Update(_float fTimeDelta)
{
	m_pCurrentArm->Update(fTimeDelta);
}

void CCamera_Manager::Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera)
{
	if (nullptr == _pCamera)
		return;

	m_eCurrentCameraType = _iCurrentCameraType;
	m_Cameras[_iCurrentCameraType] = _pCamera;
	m_Transforms[_iCurrentCameraType] = dynamic_cast<CTransform*>(_pCamera->Find_Component(TEXT("Transform")));
}

void CCamera_Manager::Add_Arm(CCameraArm* _pCameraArm)
{
}

void CCamera_Manager::Change_CameraMode(_uint _iCameraMode, _int _iNextMode)
{
}

void CCamera_Manager::Set_CameraPos(_vector _vCameraPos)
{
	m_Transforms[m_eCurrentCameraType]->Set_State(CTransform::STATE_POSITION, _vCameraPos);
}

void CCamera_Manager::Change_CameraArm(_wstring _wszArmTag)
{
	m_pCurrentArm = Find_Arm(_wszArmTag);
}

//void CCamera_Manager::Update_CameraPos()
//{
//}

CCameraArm* CCamera_Manager::Find_Arm(_wstring _wszArmTag)
{
	auto iter = m_Arms.find(_wszArmTag);

	if (iter == m_Arms.end())
		return nullptr;

	return iter->second;
}

CCamera_Manager* CCamera_Manager::Create(_uint _iNumLevels, _uint _iExcludeLevelID)
{
	return nullptr;
}

void CCamera_Manager::Free()
{
}
