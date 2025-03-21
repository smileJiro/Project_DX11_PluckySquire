#pragma once

#include "Engine_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class ENGINE_DLL CCameraArm final : public CBase
{
public:
	enum TARGET_STATE { RIGHT, UP, LOOK, POS, TARGET_STATE_END };

	enum MOVEMENT_FLAGS
	{
		RESET_FLAG = 0,
		DONE_Y_ROTATE = 1,
		DONE_RIGHT_ROTATE = 1 << 1,
		DONE_LENGTH_MOVE = 2 << 1,
		ALL_DONE_MOVEMENT = DONE_Y_ROTATE | DONE_RIGHT_ROTATE | DONE_LENGTH_MOVE,
		ALL_DONE_MOVEMENT_VECTOR = DONE_Y_ROTATE | DONE_LENGTH_MOVE
	};

	typedef struct tagCameraArmDesc : public CTransform_3D::TRANSFORM_3D_DESC
	{
		_float3				vArm = { 0.f, 0.f, -1.f };
		_float3				vPosOffset = { 0.f, 0.f, 0.f };
		_float3				vRotation;
		_float				fLength = 1.f;
	}CAMERA_ARM_DESC;

#ifdef NDEBUG
	typedef struct tagLineBuffer
	{
		_float3				vPos;
		_float4				vColor;
	} LINE_BUFFER;
#endif

private:
	CCameraArm(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCameraArm(const CCameraArm& Prototype);
	virtual ~CCameraArm() = default;

public:
	HRESULT				Initialize(void* pArg);
	HRESULT				Initialize_Clone();
	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);

	_float3				Get_Rotation() { return m_vRotation; }
	_float				Get_Length() { return m_fLength; }

	void				Set_Rotation(_vector _vRotation);
	void				Set_Length(_float _fLength) { m_fLength = _fLength; }
	void				Set_ArmVector(_vector _vArm);
	void				Set_StartInfo();
	void				Set_PreArmAngle(_float _fAngle) { m_fPreArmAngle = _fAngle; }
	void				Set_ArmTurnTime(_float2 _fArmTurnTime) { m_fArmTurnTime = _fArmTurnTime; }

public:
	_vector				Get_ArmVector() { return XMLoadFloat3(&m_vArm); }
	CTransform_3D*		Get_TransformCom() { return m_pTransform; }
	_float				Get_ReturnTime() { return m_fReturnTime.x; }
	_bool				Get_IsInPreArmData(_int _iTriggerID, pair<RETURN_ARMDATA, _bool>* _pPreArmData = nullptr);

	void				Set_NextArmData(ARM_DATA* _pData, _int _iTriggerID);
	void				Set_PreArmDataState(_int _iTriggerID, _bool _isReturn);	// 돌아갈지 안 돌아갈지에 따라 삭제 혹은 이동

	_bool				Move_To_NextArm_ByVector(_float _fTimeDelta, _bool _isBook = false);
	_bool				Move_To_CustomArm(ARM_DATA* _pCustomData, _float _fTimeDelta, _bool _isUsingVector = false);
	_bool				Move_To_PreArm(_float _fTimeDelta);						// Stack에 저장해둔 Arm으로
	_bool				Move_To_FreezeExitArm(_float _fRatio, _fvector _vFreezeExitArm, _float _fFreezeExitLength);
	_bool				Reset_To_SettingPoint(_float _fRatio, _fvector _vSettingPoint, _float _fSettingLength);

	_bool				Turn_Vector(ARM_DATA* _pCustomData, _float _fTimeDelta = 1.f);
	_bool				Turn_AxisY(ARM_DATA* _pCustomData, _float _fTimeDelta);
	_bool				Turn_AxisRight(ARM_DATA* _pCustomData, _float _fTimeDelta);
	_bool				Turn_AxisY(_float _fAngle, _float _fTimeDelta = 1.f, _uint _iRatioType = LERP);
	_bool				Turn_AxisRight(_float _fAngle, _float _fTimeDelta = 1.f, _uint _iRatioType = LERP);
	_bool				Change_Length(ARM_DATA* _pCustomData, _float _fTimeDelta);

	_bool				Turn_AxisRight_Angle(_float _fAngle);

private:
	ID3D11Device*		m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance*		m_pGameInstance = { nullptr };
	CTransform_3D*		m_pTransform = { nullptr };

private:
	_float3				m_vArm = {};
	_float3				m_vRotation = {};
	_float				m_fLength = {};

	_float				m_fStartLength = {};
	_float3				m_vStartArm = {};

	// Desire Arm
	ARM_DATA*			m_pNextArmData = { nullptr };

	// Rotate Flag
	_uint				m_iMovementFlags = RESET_FLAG;

	// Return
	deque<pair<RETURN_ARMDATA, _bool>> m_PreArms;		// Return Data, 현재 Return 중인지 아닌지 Check
	_float2				m_fReturnTime = { 1.2f, 0.f };
	_int				m_iCurTriggerID = {};

	// Arm 회전
	_float				m_fPreArmAngle = {};
	_float2				m_fArmTurnTime = {};

public:
	static CCameraArm*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	CCameraArm*			Clone();
	virtual void		Free() override;
};

END