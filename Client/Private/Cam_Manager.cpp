#include "stdafx.h"
#include "Cam_Manager.h"
#include "Camera_Target.h"
#include "GameInstance.h"



IMPLEMENT_SINGLETON(CCam_Manager);

CCam_Manager::CCam_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCam_Manager::Change_Target(CGameObject* _pNewTarget)
{
    if (nullptr == m_pTargetCamera)
        return E_FAIL;
    
    return m_pTargetCamera->Change_Target(_pNewTarget);
}

HRESULT CCam_Manager::Change_SubTarget(CGameObject* _pNewSubTarget)
{
    if (nullptr == m_pTargetCamera)
        return E_FAIL;

    return m_pTargetCamera->Change_SubTarget(_pNewSubTarget);
}

void CCam_Manager::Release_SubTarget()
{
    if (nullptr == m_pTargetCamera)
        return;
    m_pTargetCamera->Release_SubTarget();
}

HRESULT CCam_Manager::Change_Cam(CAM_TYPE _eType)
{
    if (nullptr == m_pTargetCamera )
        return E_FAIL;

    if (m_eCamType == _eType)
        return S_OK;

    m_eCamType = _eType;
    switch (m_eCamType)
    {
    case Client::CCam_Manager::CAM_TARGET:
        m_pTargetCamera->Active_On();
        break;
    }

    return S_OK;
}

void CCam_Manager::ZoomIn(CAM_TYPE _eType, _float _fZoomTime)
{
    switch (m_eCamType)
    {
    case Client::CCam_Manager::CAM_TARGET:
        m_pTargetCamera->ZoomIn(_fZoomTime);
        break;

    }
}

void CCam_Manager::ZoomIn(CAM_TYPE _eType, _float _fZoomTime, _uint _iZoomLevel)
{
    switch (m_eCamType)
    {
    case Client::CCam_Manager::CAM_TARGET:
        m_pTargetCamera->ZoomIn(_fZoomTime, (CCamera::ZOOM_LEVEL)_iZoomLevel);
        break;

    }
}

void CCam_Manager::ZoomOut(CAM_TYPE _eType, _float _fZoomTime)
{
    switch (m_eCamType)
    {
    case Client::CCam_Manager::CAM_TARGET:
        m_pTargetCamera->ZoomOut(_fZoomTime);
        break;

    }
}

void CCam_Manager::ZoomOut(CAM_TYPE _eType, _float _fZoomTime, _uint _iZoomLevel)
{
    switch (m_eCamType)
    {
    case Client::CCam_Manager::CAM_TARGET:
        m_pTargetCamera->ZoomOut(_fZoomTime, (CCamera::ZOOM_LEVEL)_iZoomLevel);
        break;

    }
}

void CCam_Manager::Start_Assassinate()
{
    if (nullptr == m_pTargetCamera)
        return;

    m_pTargetCamera->Start_Assassinate();
}

void CCam_Manager::End_Assassinate()
{
    if (nullptr == m_pTargetCamera)
        return;

    m_pTargetCamera->End_Assassinate();
}

void CCam_Manager::Start_Shake(CAM_TYPE _eCamType, CCamera::SHAKE_TYPE _eType, _float _fShakeTime, _int _iShakeCount, _float _fPower)
{
    switch (_eCamType)
    {
    case Client::CCam_Manager::CAM_TARGET:
        if (nullptr == m_pTargetCamera)
            return;
        m_pTargetCamera->Start_Shake(_eType, _fShakeTime, _iShakeCount, _fPower);
        break;
    default:
        break;
    }
}

void CCam_Manager::Level_Exit()
{
    Safe_Release(m_pTargetCamera);
    m_pTargetCamera = nullptr;

}

HRESULT CCam_Manager::Set_TargetCamera(CCamera_Target* _pCamera)
{
    if (nullptr != m_pTargetCamera)
    {
        Safe_Release(m_pTargetCamera);
    }
    m_pTargetCamera = _pCamera;
    Safe_AddRef(m_pTargetCamera);

    return S_OK;
}


_vector CCam_Manager::Get_TargetCamLook()
{
    if (nullptr == m_pTargetCamera)
    {
        assert(nullptr);
    }

    return m_pTargetCamera->Get_TargetCamLook();
}



void CCam_Manager::Free()
{
    Safe_Release(m_pTargetCamera);
    m_pTargetCamera = nullptr;


    Safe_Release(m_pGameInstance);
    __super::Free();
}
