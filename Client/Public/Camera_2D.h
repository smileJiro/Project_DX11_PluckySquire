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
		MOVE_TO_DESIREPOS, 
		MOVE_TO_NEXTARM, 
		MOVE_TO_CUSTOMARM, 
		RETURN_TO_DEFUALT, 
		FLIPPING_UP,
		FLIPPING_PAUSE,
		FLIPPING_DOWN,
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

	typedef struct tagCamera2DDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_2D_MODE			eCameraMode = { DEFAULT };
		_float3					vAtOffset = { 0.f, 0.f, 0.f };
		_float					fSmoothSpeed = {};

		const _float4x4*				pTargetWorldMatrix = { nullptr };
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
	_uint						Get_CameraMode() { return m_eCameraMode; }
	virtual INITIAL_DATA		Get_InitialData() override;

	void						Set_CameraMode(_uint _iCameraMode) { m_eCameraMode = (CAMERA_2D_MODE)_iCameraMode; }

public:
	void						Add_CurArm(CCameraArm* _pCameraArm);
	void						Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData);
	void						Add_CustomArm(ARM_DATA _tArmData);

	_bool						Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID);

	virtual void				Switch_CameraView(INITIAL_DATA* _pInitialData = nullptr) override;
	virtual void				Change_Target(const _float4x4* _pTargetMatrix) { m_pTargetWorldMatrix = _pTargetMatrix; }
	virtual void				Change_Target(CGameObject* _pTarget);

private:
	const _float4x4* 			m_pTargetWorldMatrix = { nullptr };
	CAMERA_2D_MODE				m_eCameraMode = { CAMERA_2D_MODE_END };
	CAMERA_2D_MODE				m_ePreCameraMode = { DEFAULT };
	CCameraArm*					m_pCurArm = { nullptr };
	COORDINATE					m_eTargetCoordinate = { COORDINATE_2D };

	_float						m_fSmoothSpeed = {};

	// TargetPos로 변환한 값 저장
	_float3						m_v2DTargetWorldPos = {};
	_float3						m_v2DPreTargetWorldPos = {};
	_float3						m_v2DFixedPos = {};

	_float2						m_fTrackingTime = { 0.5f, 0.f };
	
	DIRECTION_TYPE				m_eDirectionType = { HORIZON };

	// Book
	_bool						m_isBook = { true };
	_float						m_fFixedY = {};

	// Arm
	map<_wstring, pair<ARM_DATA*, SUB_DATA*>>	m_ArmDatas;

	// Sketch Space
	NORMAL_DIRECTION			m_eCurSpaceDir = { NORMAL_DIRECTION::NONEWRITE_NORMAL };

	// 카메라 위치 고정
	_float2						m_fBasicRatio[MAGNIFICATION_END] = {};	// 렌더 타겟의 어느 정도 비율로 고정할 것인지 결정, 기본적으로 해야 함
	_uint						m_iFreezeMask = { NONE };
	MAGNIFICATION_TYPE			m_eMagnificationType = {};

	_uint						m_iPlayType = {};

	_bool						m_isChangeTarget = { false };

	// CustomArm
	ARM_DATA					m_CustomArmData = {};

private:
	void						Action_Mode(_float _fTimeDelta);
	void						Action_SetUp_ByMode();

	void						Defualt_Move(_float _fTimeDelta);
	void						Move_To_NextArm(_float _fTimeDelta);
	void						Move_To_DesirePos(_float _fTimeDelta);
	void						Move_To_CustomArm(_float _fTimeDelta);
	void						Return_To_Default(_float _fTimeDelta);
	void						Flipping_Up(_float _fTimeDelta);
	void						Flipping_Pause(_float _fTimeDelta);
	void						Flipping_Down(_float _fTimeDelta);
	void						Look_Target(_float fTimeDelta);

	_vector						Calculate_CameraPos(_float _fTimeDelta);
	virtual	void				Switching(_float _fTimeDelta) override;
	void						Find_TargetPos();
	void						Calculate_Book_Scroll();					
	void						Check_MagnificationType();
	void						Clamp_FixedPos();
	void						Check_TargetChange();

private:
	pair<ARM_DATA*, SUB_DATA*>* Find_ArmData(_wstring _wszArmTag);

#ifdef _DEBUG
	void						Key_Input(_float _fTimeDelta);
#endif

public:
	static CCamera_2D*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};
END