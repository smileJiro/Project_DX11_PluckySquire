#pragma once
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Client)
class CCamera_2D : public CCamera
{
public:
	enum TARGET_STATE { TARGET_RIGHT, TARGET_UP, TARGET_LOOK, TARGET_POS, TARGET_STATE_END };

	enum CAMERA_2D_MODE
	{
		DEFAULT,
		MOVE_TO_NEXTARM,
		MOVE_TO_CUSTOMARM,
		FLIPPING_UP,
		PAUSE,
		FLIPPING_DOWN,
		NARRATION,
		RESET_TO_SETTINGPOINT,
		ZIPLINE,
		CAMERA_2D_MODE_END
	};

	enum DIRECTION_TYPE
	{
		HORIZON,	// 가로
		VERTICAL,	// 세로
		TYPE_END
	};

	enum FREEZE_TYPE
	{
		NONE,
		FREEZE_X,
		FREEZE_Z,
		FREEZE_TYPE_END
	};

	enum MAGNIFICATION_TYPE
	{
		HORIZON_NON_SCALE,
		HORIZON_SCALE,
		VERTICAL_NONE_SCALE,
		VERTICAL_SCALE,
		MAGNIFICATION_END
	};

	enum ARM_NORMAL_TYPE
	{
		DEFAULT_NORMAL = 0,
		CUSTOM_NORMAL = 1,
		NORMAL_MAP = 2,

		NORMAL_TYPE_END
	};

	enum FLIPPING_STATE
	{
		FLIPPING_NONE = 0x00,
		TURN_ARM = 0x01,
		CHANGE_LENGTH = 0x02,
		ALL_DONE = TURN_ARM | CHANGE_LENGTH
	};

	typedef struct tagCamera2DDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_2D_MODE			eCameraMode = { DEFAULT };
		_float3					vAtOffset = { 0.f, 0.f, 0.f };
		_float					fSmoothSpeed = {};

		const _float4x4* pTargetWorldMatrix = { nullptr };
	}CAMERA_2D_DESC;

private:
	CCamera_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_2D(const CCamera_2D& Prototype);
	virtual ~CCamera_2D() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

public:
	virtual _uint				Get_CameraMode() { return m_eCameraMode; }
	virtual INITIAL_DATA		Get_InitialData() override;

	void						Set_CameraMode(_uint _iCameraMode) { m_eCameraMode = (CAMERA_2D_MODE)_iCameraMode; }
	void						Set_Current_PortalID(_uint _iPortalID) { m_iPortalID = _iPortalID; }  // Portal 들어갈 때 Data 초기화하기 
	void						Set_InitialData(_fvector _vArm, _float _fLength, _fvector _vOffset, _uint _iZoomLevel);
	void						Set_InitialData(pair<ARM_DATA*, SUB_DATA*>* pData);
	void						Set_InitialData(_wstring _szSectionTag);
	void						Set_TrackingTime(_float _fTrackingTime) { m_fTrackingTime.x = _fTrackingTime; }

public:
	void						Add_CurArm(CCameraArm* _pCameraArm);
	void						Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData);
	void						Add_CustomArm(ARM_DATA _tArmData);

	_bool						Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID);

	virtual void				Switch_CameraView(INITIAL_DATA* _pInitialData = nullptr) override;
	virtual void				Change_Target(const _float4x4* _pTargetMatrix, _float _fChangingTime = 1.f) { m_pTargetWorldMatrix = _pTargetMatrix; m_fTargetChangingTime = { _fChangingTime, 0.f }; }
	virtual void				Change_Target(CGameObject* _pTarget, _float _fChangingTime = 1.f);
	virtual void				Turn_AxisY(_float _fTimeDelta) override;
	virtual void				Turn_AxisRight(_float _fTimeDelta) override;
	virtual void				Change_Length(_float _fTimeDelta) override;
	virtual void				Start_ResetArm_To_SettingPoint(_float _fResetTime) override;

private:
	const _float4x4*			m_pTargetWorldMatrix = { nullptr };
	CAMERA_2D_MODE				m_eCameraMode = { CAMERA_2D_MODE_END };
	CAMERA_2D_MODE				m_ePreCameraMode = { DEFAULT };
	COORDINATE					m_eTargetCoordinate = { COORDINATE_2D };

	_float						m_fSmoothSpeed = {};

	// TargetPos로 변환한 값 저장
	_float3						m_v2DTargetWorldPos = {};
	_float3						m_v2DPreTargetWorldPos = {};
	_float3						m_v2DFixedPos = {};
	_float3						m_vStartPos = {};			// Target 바뀌었을 때 Lerp하기 위한 변수

	// 이전 2D 좌표계 저장, 000일 때를 구하기 위해서
	_float2						m_v2DdMatrixPos = {};

	_float2						m_fTrackingTime = { 0.5f, 0.f };

	DIRECTION_TYPE				m_eDirectionType = { HORIZON };

	// Book
	_float						m_fFixedY = {};

	// Sketch Space
	NORMAL_DIRECTION			m_eCurSpaceDir = { NORMAL_DIRECTION::NONEWRITE_NORMAL };

	// 카메라 위치 고정
	_float2						m_fBasicRatio[MAGNIFICATION_END] = {};	// 렌더 타겟의 어느 정도 비율로 고정할 것인지 결정, 기본적으로 해야 함
	_uint						m_iFreezeMask = { NONE };
	MAGNIFICATION_TYPE			m_eMagnificationType = {};
	_float						m_fLengthValue = { 1.f };				// 보간해서 Camera Pos 길이 계산에 쓸 Value
	_float						m_fStartLengthValue = { 1.f };			// 보간 때 쓰는 첫 시작 Value
	_float						m_fOriginLengthValue = { 1.f };			// Section에서 받은 진짜 Length Value
	_float2						m_fLengthValueTime = { 0.5f, 0.f };		// 
	_uint						m_FlippingFlag = { FLIPPING_NONE };

	_uint						m_iPlayType = {};
	_bool						m_iNarrationPosType = { false };		// 나중에 int로 바꾸기, 지금은 true면 left

	// Arm
	map<_wstring, pair<ARM_DATA*, SUB_DATA*>>	m_ArmDatas;

	// Portal ID
	_uint						m_iPortalID = {};

	// Normal
	map<_wstring, _float3>		m_NormalTargets;

	// Zipline
	_float2						m_fZiplineTime = { 4.5f, 0.f };

private:
	void						Action_Mode(_float _fTimeDelta);
	void						Action_SetUp_ByMode();

	void						Defualt_Move(_float _fTimeDelta);
	void						Move_To_NextArm(_float _fTimeDelta);
	void						Move_To_CustomArm(_float _fTimeDelta);
	void						Flipping_Up(_float _fTimeDelta);
	void						Pause(_float _fTimeDelta);
	void						Flipping_Down(_float _fTimeDelta);
	void						Play_Narration(_float _fTimeDelta);
	void						Reset_To_SettingPoint(_float _fTimeDelta);
	void						Zipline(_float _fTimeDelta);
	
	void						Look_Target(_float fTimeDelta);
	_vector						Calculate_CameraPos(_float _fTimeDelta);
	void						Calculate_Book_Scroll();
	_bool						Change_LengthValue(_float _fTimeDelta);

	virtual	void				Switching(_float _fTimeDelta) override;
	void						Find_TargetPos();
	void						Check_MagnificationType();
	_bool						Is_Target_In_SketchSpace();

private:
	void						Set_LengthValue(_float _fStartLengthValue, _float _fOriginLengthValue) 
	{ 
		m_fStartLengthValue = _fStartLengthValue; 
		m_fOriginLengthValue = _fOriginLengthValue;
	};

private:
	pair<ARM_DATA*, SUB_DATA*>* Find_ArmData(_wstring _wszArmTag);

	void						Key_Input(_float _fTimeDelta);
#ifdef _DEBUG
	void						Imgui(_float _fTimeDelta);
#endif

public:
	static CCamera_2D*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};
END