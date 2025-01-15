#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CCamera;
class CCameraArm;
END

BEGIN(Engine)
class ENGINE_DLL CCamera_Manager : public CBase
{
public:
	enum CAMERA_TYPE { FREE, TARGET, CAMERA_TYPE_END };

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT				Initialize();
	void				Update(_float fTimeDelta);

public:
	CCamera*			Get_CurrentCamera() { return m_Cameras[m_eCurrentCameraType]; }

public:
	void				Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera);	// Free Camera, Target Camera 셋팅(처음 한 번)
	void				Add_Arm(CCameraArm* _pCameraArm);
	
	void				Change_CameraMode(_uint _iCameraMode, _int _iNextMode = -1);	// 카메라 모드 전환(아마 Target Camera만 적용)
	void				Change_CameraArm(_wstring _wszArmTag);

	void				Set_CameraPos(_vector _vCameraPos);							// CameraArm이 호출해서 Camera의 위치 설정
	
private:
	CGameInstance*							m_pGameInstance = { nullptr };

private:
	map<_wstring, CCameraArm*>				m_Arms;
	CCameraArm*								m_pCurrentArm = { nullptr };

	CCamera*								m_Cameras[CAMERA_TYPE_END];
	CTransform*								m_Transforms[CAMERA_TYPE_END];
	_uint									m_eCurrentCameraType = { CAMERA_TYPE_END };

private:
	//void				Update_CameraPos();
	CCameraArm*			Find_Arm(_wstring _wszArmTag);

public:
	static CCamera_Manager* Create(_uint _iNumLevels, _uint _iExcludeLevelID);
	virtual void Free();
};

END