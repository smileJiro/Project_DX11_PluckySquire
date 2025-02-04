#pragma once
#include "GameObject.h"
BEGIN(Engine)
class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum ZOOM_LEVEL { LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_5, LEVEL_6, LEVEL_7, LEVEL_8, LEVEL_9, LEVEL_10, ZOOM_LAST,};
	enum RATIO_TYPE { EASE_IN, EASE_OUT, LERP, RATIO_TYPE_LAST};
	enum SHAKE_TYPE { SHAKE_XY, SHAKE_X, SHAKE_Y, SHAKE_LAST };

public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		// 카메라 타입
		_uint			iCameraType;

		_float3			vEye = {};
		_float3			vAt = {};

		// 줌 레벨임 NORMAL이 기본값 임
		ZOOM_LEVEL		eZoomLevel = LEVEL_6;

		_float			fFovy = { 0.f };
		_float			fAspect = { 0.f };
		_float			fNear = { 0.f };
		_float			fFar = { 0.f };
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
	
protected:
	void Compute_PipeLineMatrices(); // 둘다 연산
	void Compute_ViewMatrix(); // 따로 연산
	void Compute_ProjMatrix();

public:
#ifdef _DEBUG
	_float	Get_Fovy(_uint _iZoomLevel) { return m_ZoomLevels[_iZoomLevel]; }
	_uint	Get_PreZoomLevel() { return m_iPreZoomLevel; }
	_uint	Get_CurrentZoomLevel() { return m_iCurZoomLevel; }
	_vector Get_AtOffset() { return XMLoadFloat3(&m_vAtOffset); }

	void	Set_ZoomLevel(_uint _iZoomLevel) { m_iCurZoomLevel = _iZoomLevel; }
	void	Set_AtOffset(_fvector _vAtOffset) { XMStoreFloat3(&m_vAtOffset, _vAtOffset); }
#endif

	// Get
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

	// Set
	void Set_Fovy(const _float _fFovy) { m_fFovy = _fFovy; }
	void Set_Aspect(const _float _fAspect) { m_fAspect = _fAspect; }
	void Set_NearZ(const _float _fNear) { m_fNear = _fNear; }
	void Set_FarZ(const _float _fFar) { m_fFar = _fFar; }
	void Set_CameraMoveSpeed(const _float _fMoveSpeed) { if (nullptr != m_pControllerTransform) m_pControllerTransform->Set_SpeedPerSec(_fMoveSpeed); }

public:
	virtual void	Change_Target(const _float4x4* _pTargetWorldMatrix) {};

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
	
	//_float3			m_vOffset = { 0.0f, 0.0f, 0.0f };
public:
	void		Start_Zoom(_float _fZoomTime, ZOOM_LEVEL _eZoomLevel, RATIO_TYPE _eRatioType);
	void		Start_Changing_AtOffset(_float _fAtOffsetTime, _vector _vNextAtOffset, _uint _iRatioType);
	void		Start_Shake_ByTime(_float _fShakeTime, _float _fShakeForce, _float _fShakeCycleTime = 0.05f, SHAKE_TYPE _ShakeType = SHAKE_TYPE::SHAKE_XY, _float _fDelayTime = 0.f);
	void		Start_Shake_ByCount(_float _fShakeTime, _float _fShakeForce, _int _iShakeCount, SHAKE_TYPE _ShakeType = SHAKE_TYPE::SHAKE_XY, _float _fDelayTime = 0.f);

public:
	//void		Start_Shake(SHAKE_TYPE _eType, _float _fShakeTime, _int _iShakeCount, _float _fPower = 1.0f);
	void		End_Shake();

protected:
	void		Action_Zoom(_float _fTimeDelta);
	void		Change_AtOffset(_float _fTimeDelta);
	//void		Action_Shake(_float _fTimeDelta);
	void		Action_Shake(_float _fTimeDelta);

	_float		Calculate_Ratio(_float2* _fTime, _float _fTimeDelta, _uint _iRatioType);

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



public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences(); // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

};

END