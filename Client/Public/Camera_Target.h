#pragma once
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Client)
class CCamera_Target  : public CCamera
{
public:
	enum CAMERA_MODE { DEFAULT, MOVE_TO_NEXTARM, RETURN_TO_PREARM, RETURN_TO_DEFUALT, MOVE_TO_CUSTOMARM, RESET_TO_SETTINGPOINT, LOAD_SAVED_ARMDATA, BOSS, CAMERA_MODE_END };
	enum TARGET_STATE { TARGET_RIGHT, TARGET_UP, TARGET_LOOK, TARGET_POS, TARGET_STATE_END };

	typedef struct tagCameraTargetDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_MODE				eCameraMode;
		_float3					vAtOffset;
		_float					fSmoothSpeed;

		const _float4x4*		pTargetWorldMatrix = { nullptr };
	}CAMERA_TARGET_DESC;

	enum FREEZE
	{
		RESET = 0x00,
		FREEZE_X = 0x01,
		FREEZE_Z = 0x02,
		FREEZE_Y = 0x04,

		FREEZE_END
	};

	typedef struct tagReturnSubData
	{
		_uint		iZoomLevel;
		_int		iTriggerID = {};
		_float3		vAtOffset;
	}RETURN_SUBDATA;

	typedef struct tagFreezeExitData
	{
		_float3		vFreezeExitArm = {};
		_float		fFreezeExitLength = {};
	}FREEZE_EXITDATA;

private:
	CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Target(const CCamera_Target& Prototype);
	virtual ~CCamera_Target() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

public:
	virtual _uint				Get_CameraMode() { return m_eCameraMode; }
	virtual INITIAL_DATA		Get_InitialData() override;
	_wstring					Get_DefaultArm_Tag();
	const _float4x4*			Get_TargetMatrix() { return m_pTargetWorldMatrix ; }
#pragma region Tool용
	void						Set_LookAt(_bool _isLookAt)
	{
		m_isLookAt = _isLookAt;
	}
	pair<ARM_DATA*, SUB_DATA*>* Get_ArmData(_wstring _wszArmName);
	map<_wstring, pair<ARM_DATA*, SUB_DATA*>>* Get_ArmDatas() { return &m_ArmDatas; };
	void						Get_ArmNames(vector<_wstring>* _vecArmNames);
#pragma endregion

public:
	void						Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData);

	_bool						Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID);
	void						Set_PreArmDataState(_int _iTriggerID, _bool _isReturn);
	void						Set_CustomArmData(ARM_DATA& _tArmData);

	void						Set_CameraMode(_uint _iCameraMode) { m_eCameraMode = (CAMERA_MODE)_iCameraMode; }
	void						Set_FreezeEnter(_uint _iFreezeMask, _fvector _vExitArm, _int _iTriggerID);
	void						Set_FreezeExit(_uint _iFreezeMask, _int _iTriggerID);
	void						Set_EnableLookAt(_bool _isEnableLookAt);

	void						Set_InitialData(_wstring _szSectionTag, _uint _iPortalIndex); // Portal 나갈 때 Data 초기화하기 
	void						Set_InitialData(pair<ARM_DATA*, SUB_DATA*>* pData);
	void						Set_InitialData(_fvector _vArm, _float _fLength, _fvector _vOffset, _uint _iZoomLevel);

	virtual void				Switch_CameraView(INITIAL_DATA* _pInitialData = nullptr) override;
	virtual void				Change_Target(const _float4x4* _pTargetWorldMatrix, _float _fChangingTime = 1.f) override;
	virtual void				Change_Target(CGameObject* _pTarget, _float _fChangingTime = 1.f) override;
	virtual void				Start_ResetArm_To_SettingPoint(_float _fResetTime) override;
	virtual void				Start_Changing_ArmVector(_float _fChangingTime, _fvector _vNextArm, RATIO_TYPE _eRatioType) override;
	virtual void				Load_SavedArmData(RETURN_ARMDATA& _tSavedData, _float _fLoadTime);

	void						Load_InitialArmTag();

private:
	const _float4x4*			m_pTargetWorldMatrix = { nullptr };
	_float3						m_vPreTargetPos = {};
	_float3						m_vFreezeOffset = {};
	_float3						m_vPreFreezeOffset = {};
	_float3						m_vStartFreezeOffset = {};

	CAMERA_MODE					m_eCameraMode = { CAMERA_MODE_END };
	CAMERA_MODE					m_ePreCameraMode = { DEFAULT };

	_float						m_fSmoothSpeed = {};

	// Arm
	map<_wstring, pair<ARM_DATA*, SUB_DATA*>>	m_ArmDatas;

	// Freeze
	_uint						m_iFreezeMask = {};
	_float3						m_vFreezeEnterPos = {};

	_bool						m_isFreezeExit = { false };
	_bool						m_isFreezeExitReturn = { false };	// Freeze Exit Arm으로 돌아가기
	_bool						m_isFreezeOffsetReturn = { false };
	_float2						m_fFreezeOffsetTime = { 4.f, 0.f };
	_float2						m_fFreezeExitReturnTime = { 5.f, 0.f };
	_bool						m_isUsingFreezeOffset = { true };

	// LookAt
	_bool						m_isEnableLookAt = { true };
	_bool						m_isExitLookAt = { false };
	_float2						m_fLookTime = {0.4f, 0.f };
	_float3						m_vStartLookVector = {};
	
	list<pair<FREEZE_EXITDATA, _uint>>	m_FreezeExitDatas;
	FREEZE_EXITDATA				m_vCurFreezeExitData = {};

	// PreArm Return
	list<pair<RETURN_SUBDATA, _bool>> m_PreSubArms;
	_int						m_iCurTriggerID = {};

	// Portal Initial Data
	map<_wstring, vector<_wstring>> m_SkspInitialTags;

	// 2D 객체 Target Change 하기 위한 변수
	_wstring					m_szTargetSectionTag;
	_float3						m_vStartPos = {};			// Target 바뀌었을 때 Lerp하기 위한 변수

	// Boss
	const _float4x4*			m_pPivotWorldMatrix = { nullptr };		// WorldMatrirx를 구할 수 없을 때 사용
	_float						m_fPivotRatio = {};

#pragma region Tool용
	_bool						m_isLookAt = { true };
#pragma endregion

private:
	void						Key_Input(_float _fTimeDelta);

	void						Action_Mode(_float _fTimeDelta);
	void						Action_SetUp_ByMode();

	void						Defualt_Move(_float _fTimeDelta);
	void						Move_To_NextArm(_float _fTimeDelta);
	void						Move_To_PreArm(_float _fTimeDelta);
	void						Move_To_CustomArm(_float _fTimeDelta);
	void						Move_To_ExitArm(_float _fTimeDelta);
	virtual void				Reset_To_SettingPoint(_float _fTimeDelta) override;
	virtual void				Action_Load_SavedArmData(_float _fTimeDelta) override;

	void						Move_In_BossStage(_float _fTimeDelta);

	_vector						Calculate_CameraPos(_vector* _pLerpTargetPos, _float _fTimeDelta);
	void						Calculate_FreezeOffset(_vector* _pTargetPos);
	void						Look_Target(_fvector _vTargetPos, _float _fTimeDelta);
	virtual	void				Switching(_float _fTimeDelta) override;
	void						Change_FreezeOffset(_float _fTimeDelta);

//#ifdef _DEBUG
	void						Imgui(_float _fTimeDelta);
//#endif
private:
	pair<ARM_DATA*, SUB_DATA*>* Find_ArmData(_wstring _wszArmTag);

public:
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END
