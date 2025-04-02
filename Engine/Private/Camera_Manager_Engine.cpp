#include "Camera_Manager_Engine.h"

#include "GameInstance.h"
#include "Camera.h"

CCamera_Manager_Engine::CCamera_Manager_Engine()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager_Engine::Initialize()
{
	for (auto& Camera : m_Cameras)
		Camera = nullptr;

	return S_OK;
}

void CCamera_Manager_Engine::Update(_float fTimeDelta)
{

}

#ifdef NDEBUG
void CCamera_Manager_Engine::Render()
{

}
#endif

_vector CCamera_Manager_Engine::Get_CameraVector(CTransform::STATE _eState)
{
	CController_Transform* pConTrans = m_Cameras[m_eCurrentCameraType]->Get_ControllerTransform();
	return pConTrans->Get_State(_eState);
}

void CCamera_Manager_Engine::Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera)
{
	if (nullptr == _pCamera)
		return;

	m_eCurrentCameraType = _iCurrentCameraType;
	m_Cameras[_iCurrentCameraType] = _pCamera;

	Safe_AddRef(m_Cameras[_iCurrentCameraType]);
}

void CCamera_Manager_Engine::Change_CameraType(_uint _iCurrentCameraType)
{
	m_eCurrentCameraType = _iCurrentCameraType;

	CController_Transform* pTargetConTrans = m_Cameras[TARGET]->Get_ControllerTransform();
	CController_Transform* pFreeConTrans = m_Cameras[FREE]->Get_ControllerTransform();

	switch (m_eCurrentCameraType) {
	case FREE:
		m_Cameras[FREE]->Set_Active(true);
		m_Cameras[TARGET]->Set_Active(false);
		
		pFreeConTrans->Set_WorldMatrix(pTargetConTrans->Get_WorldMatrix());
		m_Cameras[FREE]->Compute_FocalLength();
		m_Cameras[FREE]->Bind_DofConstBuffer();
		break;
	
	case TARGET:
		m_Cameras[FREE]->Set_Active(false);
		m_Cameras[TARGET]->Set_Active(true);	


		m_Cameras[TARGET]->Compute_FocalLength();
		m_Cameras[TARGET]->Bind_DofConstBuffer();
		break;
	}
}

CCamera_Manager_Engine* CCamera_Manager_Engine::Create()
{
	CCamera_Manager_Engine* pInstance = new CCamera_Manager_Engine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCamera_Manager_Engine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Manager_Engine::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& Camera : m_Cameras) 
		Safe_Release(Camera);

	__super::Free();
}
