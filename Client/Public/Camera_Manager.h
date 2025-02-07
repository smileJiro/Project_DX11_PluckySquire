#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Transform.h"
#include "Camera.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

public:
	enum CAMERA_TYPE { FREE, TARGET, CUTSCENE, TARGET_2D, CAMERA_TYPE_END };

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT				Initialize();
	void				Update(_float fTimeDelta);

public:
	CCamera*			Get_CurrentCamera() { return m_Cameras[m_eCurrentCameraType]; }
	CCamera*			Get_Camera(_uint _iCameraType) { return m_Cameras[_iCameraType]; }
	_vector				Get_CameraVector(CTransform::STATE _eState);						// 현재 카메라 Right, Up, Look, Pos 가져오는 함수
	_uint				Get_CameraType() { return m_eCurrentCameraType; }

#ifdef _DEBUG
	void				Start_ZoomIn();
	void				Start_ZoomOut();
#endif

public:
	void				Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera);			// Free Camera, Target Camera 셋팅(처음 한 번)
	
	void				Change_CameraMode(_uint _iCameraMode, _int _iNextMode = -1);		// 카메라 모드 전환(아마 Target Camera만 적용)							
	void				Change_CameraType(_uint _iCurrentCameraType, _bool _isInitialData = false, _float _fInitialTime = 0.3f);
	void				Change_CameraTarget(const _float4x4* _pTargetWorldMatrix);
	
	_bool				Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID);
	_bool				Set_NextCutSceneData(_wstring _wszCutSceneName);
	void				Set_PreArmDataState(_int _iTriggerID, _bool _isReturn);
	void				Set_Freeze(_uint _iFreezeType);

	void				Start_Zoom(CAMERA_TYPE _eCameraType, _float _fZoomTime, _uint _iZoomLevel, _uint _iRatioType);
	void				Start_Changing_AtOffset(CAMERA_TYPE _eCameraType, _float _fOffsetTime, _vector _vNextOffset, _uint _iRatioType);
	void				Start_Shake_ByTime(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime = 0.05f, _uint _iShakeType = CCamera::SHAKE_XY, _float _fDelayTime = 0.f);
	void				Start_Shake_ByCount(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _uint _iShakeCount, _uint _iShakeType = CCamera::SHAKE_XY, _float _fDelayTime = 0.f);	

public:
	void				Load_ArmData();
	void				Load_CutSceneData();

private:
	CGameInstance*		m_pGameInstance = { nullptr };

private:
	CCamera*			m_Cameras[CAMERA_TYPE_END];
	_uint				m_eCurrentCameraType = { CAMERA_TYPE_END };

private:
	void				Add_ArmData(_wstring wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData);
	void				Add_CutScene(_wstring _wszCutSceneTag, pair<_float2, vector<CUTSCENE_DATA>> _CutSceneData);

public:
	static CCamera_Manager* Create();
	virtual void			Free();
};
END