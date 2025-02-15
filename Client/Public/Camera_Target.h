#pragma once
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Client)
class CCamera_Target  : public CCamera
{
public:
	enum CAMERA_MODE { DEFAULT, MOVE_TO_NEXTARM, RETURN_TO_PREARM, RETURN_TO_DEFUALT, CAMERA_MODE_END };
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
		FREEZE_END
	};

	typedef struct tagReturnSubData
	{
		_uint		iZoomLevel;
		_int		iTriggerID = {};
		_float3		vAtOffset;
	}RETURN_SUBDATA;

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

#ifdef _DEBUG
	_float3						Get_ArmRotation();
#endif
public:
	_uint						Get_CameraMode() { return m_eCameraMode; }
	virtual INITIAL_DATA		Get_InitialData() override;

public:
	void						Add_CurArm(CCameraArm* _pCameraArm);
	void						Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData);
	
	_bool						Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID);
	void						Set_PreArmDataState(_int _iTriggerID, _bool _isReturn);
	void						Set_CameraMode(_uint _iCameraMode, _int iNextCameraMode = -1) { m_eCameraMode = (CAMERA_MODE)_iCameraMode; m_iNextCameraMode = iNextCameraMode; }
	void						Set_FreezeEnter(_uint _iFreezeMask, _fvector _vExitArm, _int _iTriggerID);
	void						Set_FreezeExit(_uint _iFreezeMask, _int _iTriggerID);
	void						Set_EnableLookAt(_bool _isEnableLookAt) { m_isEnableLookAt = _isEnableLookAt; }

	void						Change_Target(const _float4x4* _pTargetWorldMatrix) override;
	virtual void				Switch_CameraView(INITIAL_DATA* _pInitialData = nullptr) override;

private:
	const _float4x4*			m_pTargetWorldMatrix = { nullptr };
	_float3						m_vPreTargetPos = {};

	CAMERA_MODE					m_eCameraMode = { CAMERA_MODE_END };
	CAMERA_MODE					m_ePreCameraMode = { DEFAULT };
	_int						m_iNextCameraMode = { -1 };

	_float						m_fSmoothSpeed = {};

	// Arm
	map<_wstring, pair<ARM_DATA*, SUB_DATA*>>	m_ArmDatas;
	CCameraArm*					m_pCurArm = { nullptr };

	// Freeze
	_uint						m_iFreezeMask = {};
	_bool						m_isFreezeExit = { false };
	_float2						m_fFreezeExitTime = { 0.4f, 0.f };
	_float3						m_vFreezeEnterPos = {};

	// LookAt
	_bool						m_isEnableLookAt = { true };
	
	list<pair<_float3, _uint>>	m_FreezeExitArms = {};
	_float3						m_vCurFreezeExitArm = {};

	//_uint						m_iPreFreeze = {};

	// PreArm Return
	list<pair<RETURN_SUBDATA, _bool>> m_PreSubArms;
	_int						m_iCurTriggerID = {};

private:
	void						Key_Input(_float _fTimeDelta);

	void						Action_Mode(_float _fTimeDelta);
	void						Action_SetUp_ByMode();

	void						Defualt_Move(_float _fTimeDelta);
	void						Move_To_NextArm(_float _fTimeDelta);
	void						Look_Target(_fvector _vTargetPos, _float _fTimeDelta);
	void						Move_To_PreArm(_float _fTimeDelta);

	_vector						Calculate_CameraPos(_vector* _pLerpTargetPos, _float _fTimeDelta);
	virtual	void				Switching(_float _fTimeDelta) override;
	void						Set_Arm_From_Freeze();

private:
	pair<ARM_DATA*, SUB_DATA*>* Find_ArmData(_wstring _wszArmTag);

public:
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END
