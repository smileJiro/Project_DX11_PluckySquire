#pragma once
#include "GameObject.h"
BEGIN(Engine)
class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum STATE { STATE_NONE, STATE_ZOOM_IN, STATE_ZOOM_OUT, STATE_LAST };
	enum TYPE { TARGET, FREE, LAST };
	enum ZOOM_LEVEL { IN_LEVEL2, IN_LEVEL1, NORMAL, OUT_LEVEL1, OUT_LEVEL2, ZOOM_LAST,};
	enum SHAKE_TYPE { SHAKE_XY, SHAKE_X, SHAKE_Y, SHAKE_LAST };
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		// 카메라 타입
		TYPE			eCameraType;

		_float3			vEye = {};
		_float3			vAt = {};

		// 줌 레벨임 NORMAL이 기본값 임
		ZOOM_LEVEL		eZoomLevel = NORMAL;

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
	// Get
	TYPE Get_CamType() const { return m_eCameraType; }
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
protected:
	/* 뷰, 투영 행렬을 구성하는 기능 */
	/* 만들어 낸 뷰, 투영 행렬을 PipeLine Class 에 저장. */
	/* 뷰 행렬은 Transform으로 대체 */
	/* 투영 -> 별도로 만들어주어야한다. */
	TYPE			m_eCameraType = TYPE::LAST;
	_float			m_fFovy = { 0.f };
	_float			m_fAspect = { 0.f };
	_float			m_fNear = { 0.f };
	_float			m_fFar = { 0.f };
	
	_float3			m_vOffset = { 0.0f, 0.0f, 0.0f };
public:
	void	 ZoomIn(_float _fZoomTime);
	void	 ZoomIn(_float _fZoomTime, ZOOM_LEVEL _eLevel);
	void	 ZoomOut(_float _fZoomTime);
	void	 ZoomOut(_float _fZoomTime, ZOOM_LEVEL _eLevel);

public:
	void	 Start_Shake(SHAKE_TYPE _eType, _float _fShakeTime, _int _iShakeCount, _float _fPower = 1.0f);
	void	 End_Shake();

protected:
	void	 Action_Zoom(_float _fTimeDelta);
	void	 Action_Shake(_float _fTimeDelta);

protected:
	STATE	m_eCamState = STATE::STATE_NONE;

protected: /* Zoom */
	_float	 m_fStartFovy = 0.0f;
	_float	 m_arrZoomFovy[(_uint)ZOOM_LAST];
	_uint	 m_iPreZoomLevel = NORMAL;
	_uint	 m_iCurZoomLevel = NORMAL;
	_bool	 m_bZoomOn = false;
	_float2	 m_vZoomTime = { 1.0f, 0.0f };

protected: /* Shake */
	SHAKE_TYPE	m_eShakeType = SHAKE_TYPE::SHAKE_LAST;
	_bool		m_isShake = false;
	_float2		m_vShakeCycleTime = { 0.1f, 0.0f };
	_float2		m_vShakeTime = { 1.0f, 0.0f };
	_int		m_iShakeCount = 0;
	_int		m_iShakeCountAcc = 0;
	_float		m_fShakePower = 0.0f;

	_float3		m_vShakeMovement = {0.0f, 0.0f, 0.0f};


public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences(); // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

};

END