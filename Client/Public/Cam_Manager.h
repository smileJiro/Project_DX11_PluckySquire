#pragma once
#include "Base.h"
#include "Camera.h"
BEGIN(Engine)
class CCamera;
class CGameInstance;
END
BEGIN(Client)
class CCamera_Target;

class CCam_Manager final : public CBase
{
	DECLARE_SINGLETON(CCam_Manager);
public:
	enum CAM_TYPE { CAM_TARGET,  CAM_LAST };
private:
	CCam_Manager();
	virtual ~CCam_Manager() = default;
public: /* Target_Camera */
	HRESULT Change_Target(CGameObject* _pNewTarget);
	HRESULT Change_SubTarget(CGameObject* _pNewSubTarget);
	void Release_SubTarget();

public:
	HRESULT Change_Cam(CAM_TYPE _eType);
public:
	void ZoomIn(CAM_TYPE _eType, _float _fZoomTime);
	void ZoomIn(CAM_TYPE _eType, _float _fZoomTime, _uint _iZoomLevel);
	void ZoomOut(CAM_TYPE _eType, _float _fZoomTime);
	void ZoomOut(CAM_TYPE _eType, _float _fZoomTime, _uint _iZoomLevel);
	void Start_Assassinate();
	void End_Assassinate();

public:
	void Start_Shake(CAM_TYPE _eCamType, CCamera::SHAKE_TYPE _eType, _float _fShakeTime, _int _iShakeCount, _float _fPower = 1.0f);

public:
	void	Level_Exit();
public:
	HRESULT Set_TargetCamera(CCamera_Target* _pCamera);

	CCamera_Target* Get_TargetCamera() { return m_pTargetCamera; }
	_vector Get_TargetCamLook();

private:
	CGameInstance* m_pGameInstance;
private:
	CAM_TYPE m_eCamType = CAM_TYPE::CAM_LAST;
private:
	CCamera_Target* m_pTargetCamera = nullptr;
public:
	void Free() override;

};

END