#pragma once

#include "Engine_Defines.h"
#include "Transform.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CController_Transform;
class CCamera;
class CCameraArm;
END

BEGIN(Engine)
class ENGINE_DLL CCamera_Manager_Engine : public CBase
{
public:
	enum CAMERA_TYPE { FREE, TARGET, CAMERA_TYPE_END };

private:
	CCamera_Manager_Engine();
	virtual ~CCamera_Manager_Engine() = default;

public:
	HRESULT				Initialize();
	void				Update(_float fTimeDelta);

#ifdef NDEBUG
	void				Render();
#endif

public:
	CCamera*			Get_CurrentCamera() { return m_Cameras[m_eCurrentCameraType]; }
	CCamera*			Get_Camera(_uint _iCameraType) { return m_Cameras[_iCameraType]; }
	_vector				Get_CameraVector(CTransform::STATE _eState);						// 현재 카메라 Right, Up, Look, Pos 가져오는 함수
	_uint				Get_CameraType() { return m_eCurrentCameraType; }					

public:
	void				Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera);			// Free Camera, Target Camera 셋팅(처음 한 번)
				
	void				Change_CameraType(_uint _iCurrentCameraType);

private:
	CGameInstance*							m_pGameInstance = { nullptr };

private:
	CCamera*								m_Cameras[CAMERA_TYPE_END];
	_uint									m_eCurrentCameraType = { CAMERA_TYPE_END };

public:
	static CCamera_Manager_Engine*	Create();
	virtual void					Free();
};

END