#include "Camera_Manager.h"

#include "GameInstance.h"
#include "Camera.h"

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

}

#ifdef _DEBUG
void CCamera_Manager::Render()
{

}
#endif

_vector CCamera_Manager::Get_CameraVector(CTransform::STATE _eState)
{
	CController_Transform* pConTrans = m_Cameras[m_eCurrentCameraType]->Get_ControllerTransform();
	return pConTrans->Get_State(_eState);
}

void CCamera_Manager::Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera)
{
	if (nullptr == _pCamera)
		return;

	m_eCurrentCameraType = _iCurrentCameraType;
	m_Cameras[_iCurrentCameraType] = _pCamera;

	Safe_AddRef(m_Cameras[_iCurrentCameraType]);
}

void CCamera_Manager::Change_CameraMode(_uint _iCameraMode, _int _iNextMode)
{
	if (TARGET == m_eCurrentCameraType) {
		m_Cameras[m_eCurrentCameraType]->Set_CameraMode(_iCameraMode, _iNextMode);
	}
	else
		return;
}

void CCamera_Manager::Set_CameraPos(_vector _vCameraPos, _vector _vTargetPos)
{
	m_Cameras[m_eCurrentCameraType]->Set_TargetPos(_vTargetPos);

	CController_Transform* pConTrans = m_Cameras[m_eCurrentCameraType]->Get_ControllerTransform();
	pConTrans->Set_State(CTransform::STATE_POSITION, _vCameraPos);
}

#ifdef _DEBUG
void CCamera_Manager::Set_Rotation(_vector vRotation)
{
	m_Cameras[TARGET]->Set_Rotation(vRotation);
}
#endif

void CCamera_Manager::Change_CameraType(_uint _iCurrentCameraType)
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

void CCamera_Manager::Change_CameraTarget(const _float4x4* _pTargetWorldMatrix)
{
	if (nullptr != m_Cameras[TARGET] && TARGET == m_eCurrentCameraType) {
		m_Cameras[TARGET]->Change_Target(_pTargetWorldMatrix);
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
