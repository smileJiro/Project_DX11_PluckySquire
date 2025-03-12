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
	void				Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);

public:
	CCamera*			Get_CurrentCamera() { return m_Cameras[m_eCurrentCameraType]; }
	CCamera*			Get_Camera(_uint _iCameraType) { return m_Cameras[_iCameraType]; }
	_vector				Get_CameraVector(CTransform::STATE _eState);						// 현재 카메라 Right, Up, Look, Pos 가져오는 함수
	_uint				Get_CameraType() { return m_eCurrentCameraType; }
	_uint				Get_CameraMode(_uint _iCameraType);
	_uint				Get_CurCameraMode();

#ifdef _DEBUG

#endif
	void				Start_ZoomIn();
	void				Start_ZoomOut();
	void				Imgui_Dof();
public:
	void				Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera);			// Free Camera, Target Camera 셋팅(처음 한 번)
	
	void				Change_CameraMode(_uint _iCameraMode);		// 카메라 모드 전환(아마 Target Camera만 적용)							
	void				Change_CameraType(_uint _iCurrentCameraType, _bool _isInitialData = false, _float _fInitialTime = 0.3f);
	void				Change_CameraTarget(const _float4x4* _pTargetWorldMatrix, _float _fChangingTime = 1.f);
	void				Change_CameraTarget(CGameObject* _pTarget, _float _fChangingTime = 1.f);

	_bool				Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID);
	_bool				Set_NextCutSceneData(_wstring _wszCutSceneName);
	void				Set_PreArmDataState(_int _iTriggerID, _bool _isReturn);
	void				Set_FreezeEnter(_uint _iFreezeMask, _fvector _vExitArm, _int _iTriggerID);
	void				Set_FreezeExit(_uint _iFreezeMask, _int _iTriggerID);

	// 예시 코드: Camera Target KeyInput 보면 됨
	// Zoom
	// 1. Zoom Level에 따라서 Zoom을 함
	// 2. Ratio Type으로 보간할 때 어느 속도로 할지 결정
	// 3. EASE_IN: 점점 빠르게, EASE_OUT: 점점 느리게, LERP: 기본 보간, EASE_IN_OUT: 빨라지다가 다시 느리게
	void				Start_Zoom(CAMERA_TYPE _eCameraType, _float _fZoomTime, _uint _iZoomLevel, _uint _iRatioType);
	void				Start_ZoomIn(CAMERA_TYPE _eCameraType, _float _fZoomTime, _uint _iRatioType);
	void				Start_ZoomIn(_float _fZoomTime, _uint _iRatioType);
	void				Start_ZoomOut(CAMERA_TYPE _eCameraType, _float _fZoomTime, _uint _iRatioType);
	void				Start_ZoomOut(_float _fZoomTime, _uint _iRatioType);

	// AtOffset
	// 1. Target Camera는 기본적으로 Target을 바라보지만, 타겟을 기준으로 해당 Offset만큼 떨어진 곳을 보도록 조절한다
	void				Start_Changing_AtOffset(CAMERA_TYPE _eCameraType, _float _fOffsetTime, _vector _vNextOffset, 
		_uint _iRatioType);

	// Shake 
	// 1. Cycle Time은 설정한 Cycle Time마다 Shake를 준다 (0.02라면 0.02초마다 한 번씩 흔들림)
	// 2. 총 시간 동안 내가 지정한 Count만큼 흔들린다
	// 3. SHAKE_X: X축으로만 흔들기, SHAKE_Y: Y축으로만 흔들기, SHAKE_XY: 사방으로 흔들기
	// 4. 딜레이 구현 안 함
	void				Start_Shake_ByTime(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime = 0.05f, _uint _iShakeType = CCamera::SHAKE_XY, _float _fDelayTime = 0.f);
	void				Start_Shake_ByCount(CAMERA_TYPE _eCameraType, _float _fShakeTime, _float _fShakeForce, _uint _iShakeCount, _uint _iShakeType = CCamera::SHAKE_XY, _float _fDelayTime = 0.f);	
	
	// Arm 회전
	// 1. AxisY는 Y축을 기준으로 회전하며, AxisRight는 위아래로 까딱이는 회전을 한다(Up Vector와 현재 Arm의 외적값이 회전축임)
	// 2. 내가 설정한 시간만큼 회전을 한다
	// 3. 회전하는 속도를 RotationPerSec를 인자로 받아서 설정한다(Min 20, Max 30일 경우 점점 빠르게 회전하며 반대일 경우 점점 느리게 회전한다)
	// 4. 반대로 돌고 싶은 경우 -를 붙인다

	// 1. 원하는 각도를 원하는 시간 동안 돌린다
	void				Start_Turn_AxisY(CAMERA_TYPE _eCameraType, _float _fTurnTime, _float _fMinRotationPerSec, _float _fMaxRotationPerSec);
	void				Start_Turn_AxisY(CAMERA_TYPE _eCameraType, _float _fTurnTime, _float _fAngle, _uint _iRatioType = LERP);
	void				Start_Turn_AxisRight(CAMERA_TYPE _eCameraType, _float _fTurnTime, _float _fMinRotationPerSec, _float _fMaxRotationPerSec);
	void				Start_Turn_AxisRight(CAMERA_TYPE _eCameraType, _float _fTurnTime, _float _fAngle, _uint _iRatioType = LERP);

	// ArmVector 변경
	void				Start_Turn_ArmVector(CAMERA_TYPE _eCameraType, _float _fTurnTime, _fvector _vNextVector, _uint _iRatioType);

	// Length 조절
	// 1. 정해진 시간만큼 Length를 조절한다
	// 2. Decrease -> _fDecreaseValue만큼 Length를 줄인다
	// 3. Increase -> _fIncreaseValue만큼 Length를 늘린다
	void				Start_Changing_ArmLength(CAMERA_TYPE _eCameraType, _float _fLengthTime, _float _fLength, _uint _iRatioType);
	void				Start_Changing_ArmLength_Decrease(CAMERA_TYPE _eCameraType, _float _fLengthTime, _float _fDecreaseValue, _uint _iRatioType);
	void				Start_Changing_ArmLength_Increase(CAMERA_TYPE _eCameraType, _float _fLengthTime, _float _fIncreaseValue, _uint _iRatioType);

	// Camera 원복
	// 1. 원복하고 싶은 시점에 Set_ResetData 함수를 호출해서 현재 상태를 저장한다
	void				Set_ResetData(CAMERA_TYPE _eCameraType);

	// 2. 원하는 시점으로 _fResetTime 동안 원복하고 싶을 때 호출한다
	void				Start_ResetArm_To_SettingPoint(CAMERA_TYPE _eCameraType, _float _fResetTime);

	// Camera 중간 저장
	// 1. 중간 저장하고 싶은 시점에 Save_ArmData 함수를 호출해서 현재 상태를 저장한다
	RETURN_ARMDATA		Save_ArmData(CAMERA_TYPE _eCameraType = TARGET);

	// 2. 원하는 시점으로 _fLoadTime 동안 원복하고 싶을 때 호출한다
	void				Load_SavedArmData(RETURN_ARMDATA& _tSavedData, _float _fLoadTime, CAMERA_TYPE _eCameraType = TARGET);


	void				Start_FadeIn(_float _fFadeTime = 1.0f);
	void				Start_FadeOut(_float _fFadeTime = 1.0f);
	void				Set_FadeRatio(_uint _eCameraType, _float _fFadeRatio, _bool _isUpdate);

public:
	void				Load_ArmData(LEVEL_ID _eLevelID);
	void				Load_CutSceneData(LEVEL_ID _eLevelID);
	HRESULT				Load_DOF(const _wstring& _strJsonPath, _uint _iCameraType);

private:
	CGameInstance*		m_pGameInstance = { nullptr };

private:
	CCamera*			m_Cameras[CAMERA_TYPE_END];
	_uint				m_eCurrentCameraType = { (_uint)FREE };

	_int				m_iCurLevelID = {};

private:
	void				Add_ArmData(_uint _iCameraType, _wstring wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData);
	void				Add_CutScene(_wstring _wszCutSceneTag, pair<CUTSCENE_SUB_DATA, vector<CUTSCENE_DATA*>> _CutSceneData);
	void				Load_ArmData(_uint _iCameraType, _wstring _szFilePath);

public:
	static CCamera_Manager* Create();
	virtual void			Free();
};
END