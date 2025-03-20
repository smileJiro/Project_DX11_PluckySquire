#pragma once
#include "GameObject.h"
BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum ZOOM_LEVEL { LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_5, LEVEL_6, LEVEL_7, LEVEL_8, LEVEL_9, LEVEL_10, ZOOM_LAST,};
	enum SHAKE_TYPE { SHAKE_XY, SHAKE_X, SHAKE_Y, SHAKE_LAST };
	enum FADE_TYPE { FADE_IN, FADE_OUT, FADE_LAST };
	enum WHITE_FADE { WHITE_IN, WHITE_OUT, WHITE_LAST };
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		// 카메라 타입
		_uint			iCameraType;

		_float3			vEye = {};
		_float3			vAt = {};

		// 줌 레벨임 NORMAL이 기본값 임
		ZOOM_LEVEL		eZoomLevel = LEVEL_6;

		_float			fFovy = { 62.f };
		_float			fAspect = { 16/9.f };
		_float			fNear = { 0.1f };
		_float			fFar = { 500.f };

		// Arm
		_float3			vArm = {};
		_float			fLength = {};

		// Dof 기본값 
		_float			fSensorHeight = 24.0f;
		_float			fAperture = 2.8f; // 조리개 크기
		_float			fFocusDistance = 50.0f; // 초점 평면거리
		_float			fDofBrightness = 0.9f;
		_float			fBaseBlurPower = 0.05f;
		_float3			vBlurColor = { 1.0f, 1.0f, 1.0f };
	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera(const CCamera& _Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	HRESULT Load_DOF(const _wstring& _strJsonPath);

protected:
	void Compute_PipeLineMatrices(); // 둘다 연산
	void Compute_ViewMatrix(); // 따로 연산
	void Compute_ProjMatrix();

public:
	_float	Get_Fovy(_uint _iZoomLevel) { return m_ZoomLevels[_iZoomLevel]; }
	_uint	Get_PreZoomLevel() { return m_iPreZoomLevel; }
	_uint	Get_CurrentZoomLevel() { return m_iCurZoomLevel; }
	_vector Get_AtOffset() { return XMLoadFloat3(&m_vAtOffset); }

	void	Set_ZoomLevel(_uint _iZoomLevel) { m_iCurZoomLevel = _iZoomLevel; m_fFovy = m_ZoomLevels[_iZoomLevel]; }
	void	Set_AtOffset(_fvector _vAtOffset) { XMStoreFloat3(&m_vAtOffset, _vAtOffset); }
	// Get
	CONST_DOF Get_DofBufferData() const { return m_tDofData; }
	_uint Get_CamType() const { return m_eCameraType; }
	_float Get_Fovy() const { return m_fFovy; }
	_float Get_Aspect() const { return m_fAspect; }
	_float Get_NearZ() const { return m_fNear; }
	_float Get_FarZ() const { return m_fFar; }
	_float Get_CameraMoveSpeed() const
	{ 
		if (nullptr != m_pControllerTransform) 
		return m_pControllerTransform->Get_SpeedPerSec();

		return 0.0f; 
	}

	class CCameraArm* Get_Arm() { return m_pCurArm; }
	_bool	Is_Switching() { return m_isInitialData; }

	// Set
	void Set_DofBufferData(const CONST_DOF& _tDofConstData, _bool _isUpdate = false);
	void Set_Fovy(const _float _fFovy) { m_fFovy = _fFovy; }
	void Set_Aspect(const _float _fAspect) { m_fAspect = _fAspect; }
	void Set_NearZ(const _float _fNear) { m_fNear = _fNear; }
	void Set_FarZ(const _float _fFar) { m_fFar = _fFar; }
	void Set_CameraMoveSpeed(const _float _fMoveSpeed) { if (nullptr != m_pControllerTransform) m_pControllerTransform->Set_SpeedPerSec(_fMoveSpeed); }
	void Off_Shake() { m_isShake = false; }
public:
	virtual void	Change_Target(const _float4x4* _pTargetWorldMatrix, _float _fChangingTime = 1.f) {};
	virtual void	Change_Target(CGameObject* _pTarget, _float _fChangingTime = 1.f) {};
	virtual void	Switch_CameraView(INITIAL_DATA* _pInitialData = nullptr) {};
	virtual INITIAL_DATA Get_InitialData() { return INITIAL_DATA(); };
	virtual _uint	Get_CameraMode() { return _uint(); }

	void			Create_Arm(CAMERA_DESC* _pDesc);

protected:
	/* 뷰, 투영 행렬을 구성하는 기능 */
	/* 만들어 낸 뷰, 투영 행렬을 PipeLine Class 에 저장. */
	/* 뷰 행렬은 Transform으로 대체 */
	/* 투영 -> 별도로 만들어주어야한다. */
	_uint			m_eCameraType = {};
	_float			m_fFovy = { 0.f };
	_float			m_fAspect = { 0.f };
	_float			m_fNear = { 0.f };
	_float			m_fFar = { 0.f };

public:
	void		Start_Zoom(_float _fZoomTime, ZOOM_LEVEL _eZoomLevel, RATIO_TYPE _eRatioType);
	void		Start_Changing_AtOffset(_float _fAtOffsetTime, _vector _vNextAtOffset, _uint _iRatioType);
	void		Start_Shake_ByTime(_float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime = 0.05f, SHAKE_TYPE _ShakeType = SHAKE_TYPE::SHAKE_XY, _float _fDelayTime = 0.f);
	void		Start_Shake_ByCount(_float _fShakeTime, _float _fShakeForce, _int _iShakeCount, SHAKE_TYPE _ShakeType = SHAKE_TYPE::SHAKE_XY, _float _fDelayTime = 0.f);
	void		Start_PostProcessing_Fade(FADE_TYPE _eFadeType, _float _fFadeTime = 1.0f);
	void		Set_FadeRatio(_float _fFadeRatio, _bool _isUpdate = false);
	void		Set_FadeRatio_White(_float _fFadeRatio, _bool _isUpdate = false);

	void		Start_Turn_AxisY(_float _fTurnTime, _float _fMinRotationPerSec, _float _fMaxRotationPerSec);
	void		Start_Turn_AxisY(_float _fTurnTime, _float _fAngle, _uint _iRatioType);
	void		Start_Turn_AxisRight(_float _fTurnTime, _float _fMinRotationPerSec, _float _fMaxRotationPerSec);
	void		Start_Turn_AxisRight(_float _fTurnTime, _float _fAngle, _uint _iRatioType);
	void		Start_Changing_ArmLength(_float _fLengthTime, _float _fLength, RATIO_TYPE _eRatioType);
	virtual void		Start_Changing_ArmVector(_float _fChangingTime, _fvector _vNextArm, RATIO_TYPE _eRatioType);
	virtual void		Start_ResetArm_To_SettingPoint(_float _fResetTime) {};

	void		Set_ResetData();
	//void		Reset_CameraArm();

	RETURN_ARMDATA		Save_ArmData();
	virtual void		Load_SavedArmData(RETURN_ARMDATA& _tSavedData, _float _fLoadTime) {};

public:/* Dof 값 조절 후 Bind_DofBuffer() 호출 시 적용 */
	HRESULT		Compute_FocalLength();
	HRESULT		Bind_DofConstBuffer();

public:
	void		End_Shake();

protected:
	void			Action_Zoom(_float _fTimeDelta);
	void			Change_AtOffset(_float _fTimeDelta);
	void			Action_Shake(_float _fTimeDelta);
	void			Action_PostProcessing_Fade(_float _fTimeDelta);
	
	virtual void	Turn_AxisY(_float _fTimeDelta);
	virtual void	Turn_AxisY_Angle(_float _fTimeDelta);
	virtual void	Turn_AxisRight(_float _fTimeDelta);
	virtual void	Turn_AxisRight_Angle(_float _fTimeDelta);
	virtual void	Turn_Vector(_float _fTimeDelta);
	virtual void	Change_Length(_float _fTimeDelta);
	virtual void	Reset_To_SettingPoint(_float _fTimeDelta) {};
	virtual void	Action_Load_SavedArmData(_float _fTimeDelta) {};

	virtual void	Switching(_float _fTimeDelta) {};

public:
	_bool			Turn_Camera_AxisY(_float _fAngle, _float _fTurnTime = 1.f, _float _fTimeDelta = 1.f, _uint _iRatioType = LERP);
	_bool			Turn_Camera_AxisRight(_float _fAngle, _float _fTurnTime = 1.f, _float _fTimeDelta = 1.f, _uint _iRatioType = LERP);

protected: /* Zoom */
	_float		m_ZoomLevels[(_uint)ZOOM_LAST] = {};
	_bool		m_isZoomOn = false;

	RATIO_TYPE	m_eRatioType = RATIO_TYPE_LAST;
	_uint		m_iPreZoomLevel = LEVEL_6;
	_uint		m_iCurZoomLevel = LEVEL_6;

	_float2		m_fZoomTime = {};
	_float		m_fStartFovy = {};

protected: // Change AtOffset
	_float3						m_vAtOffset = {};
	_float3						m_vStartAtOffset = {};
	_float3						m_vNextAtOffset = {};
	_bool						m_isChangingAtOffset = { false };
	_float2						m_fAtOffsetTime = {};
	_uint						m_iOffsetRatioType = {};

protected: /* Shake */
	SHAKE_TYPE					m_eShakeType = {};
	_bool						m_isShake = { false };
	_float2						m_fShakeTime = {};
	_float2						m_fShakeCycleTime = {};
	_float2						m_fDelayTime = {};
	_float						m_fShakeForce = {};
	_float3						m_vShakeOffset = {};

protected: /*Target Change*/
	_bool						m_isTargetChanged = { false };
	_float2						m_fTargetChangingTime = { 1.f, 0.f };
	COORDINATE					m_eTargetCoordinate = { COORDINATE_2D };

protected: // Initial
	INITIAL_DATA				m_tInitialData = {};
	_bool						m_isInitialData = { false };
	_float2						m_InitialTime = { 0.3f, 0.f };

protected: // EventTag (ex CutScene_1 ...)
	_wstring					m_szEventTag = {};

protected:
	CONST_DOF					m_tDofData = {};
	ID3D11Buffer*				m_pConstDofBuffer = nullptr;
	_bool						m_isBindConstBuffer = false;

protected:
	FADE_TYPE					m_eFadeType = FADE_TYPE::FADE_LAST;
	_float2						m_vFadeTime = { 1.0f, 0.0f };


public:
	void Start_WhiteFadeOut(_float _fTime);
	void Start_WhiteFadeIn(_float _fTime);

protected:
	_int						m_isWhiteFade = 0;

protected:
	class CCameraArm*			m_pCurArm = { nullptr };
	_bool						m_isChangingLength = { false };
	_bool						m_isTurnAxisY = { false };
	_bool						m_isTurnAxisY_Angle = { false };
	_bool						m_isTurnAxisRight = { false };
	_bool						m_isTurnAxisRight_Angle = { false };
	_bool						m_isTurnVector = { false };

	// CustomArm
	ARM_DATA					m_CustomArmData = {};
	_float						m_fTurnAngle = {};			// 원하는 각도로 돌기 위한 변수
	_uint						m_iTurnAngleRatioType = {};

	// 해당 지점으로 카메라 복구
	RETURN_ARMDATA				m_ResetArmData = {};
	_float2						m_fResetTime = {};

	// Save 기능
	RETURN_ARMDATA				m_SaveArmData = {};
	_float2						m_fLoadTime = {};

protected:
	// Camera 회전
	_float						m_fPreLookAngle = {};
	_float2						m_fCameraTurnTime = {};
	_bool						m_isStartTurn = { false };


private:
	HRESULT						Ready_DofConstData(CAMERA_DESC* _pDesc);

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences(); // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)
};

END