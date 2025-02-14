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
		_wstring			wszArmTag = {};
		//ARM_TYPE			eArmType = { DEFAULT };

		//const _float4x4*	pTargetWorldMatrix = { nullptr };
	}CAMERA_ARM_DESC;

#ifdef _DEBUG
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

#ifdef _DEBUG
	void				Render_Arm();
#endif
	_float3				Get_Rotation() { return m_vRotation; }
	_float				Get_Length() { return m_fLength; }

	void				Set_Rotation(_vector _vRotation);
	void				Set_Length(_float _fLength) { m_fLength = _fLength; }
	void				Set_ArmTag(_wstring _wszArmTag) { m_wszArmTag = _wszArmTag; }
	void				Set_ArmVector(_vector _vArm);
	void				Set_DesireVector();		// 최종 벡터 저장하는용
public:
	_wstring			Get_ArmTag() { return m_wszArmTag; }
	_vector				Get_ArmVector() { return XMLoadFloat3(&m_vArm); }
	CTransform_3D*		Get_TransformCom() { return m_pTransform; }
	_float				Get_ReturnTime() { return m_fReturnTime.x; }
	_bool				Get_IsInPreArmData(_int _iTriggerID, pair<RETURN_ARMDATA, _bool>* _pPreArmData = nullptr);

	void				Set_NextArmData(ARM_DATA* _pData, _int _iTriggerID);
	void				Set_PreArmDataState(_int _iTriggerID, _bool _isReturn);	// 돌아갈지 안 돌아갈지에 따라 삭제 혹은 이동

	_bool				Move_To_NextArm(_float _fTimeDelta);
	_bool				Move_To_NextArm_ByVector(_float _fTimeDelta);
	_bool				Move_To_PreArm(_float _fTimeDelta);						// Stack에 저장해둔 Arm으로

private:
	ID3D11Device*		m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance*		m_pGameInstance = { nullptr };
	CTransform_3D*		m_pTransform = { nullptr };

private:
	// 이거 나중에 Target 월드 행렬 받으려면 Target File Tag 같은 거 필요할 수도
	//const _float4x4*	m_pTargetWorldMatrix = { nullptr };

	_wstring			m_wszArmTag = {};
	_float3				m_vArm = {};
	_float3				m_vRotation = {};
	_float				m_fLength = {};

	_float				m_fStartLength = {};
	_float3				m_vStartArm = {};

	_float				m_fRotationValue = { 1.f };

	// Desire Arm
	ARM_DATA*			m_pNextArmData = { nullptr };

	// Rotate Flag
	_uint				m_iMovementFlags = RESET_FLAG;

	// Return
	deque<pair<RETURN_ARMDATA, _bool>> m_PreArms;		// Return Data, 현재 Return 중인지 아닌지 Check
	_float2				m_fReturnTime = { 1.f, 0.f };
	_int				m_iCurTriggerID = {};

	// Line
#ifdef _DEBUG
	LINE_BUFFER			m_vertices[2];

	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11DepthStencilState*				m_pDepthStencilState = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };
#endif

private:
#ifdef _DEBUG
	HRESULT				Set_PrimitiveBatch();
#endif
	void				Set_WorldMatrix();

	void				Turn_ArmX(_float fAngle);
	void				Turn_ArmY(_float fAngle);
	_float				Calculate_Ratio(_float2* _fTime, _float _fTimeDelta, _uint _iRatioType);
	_bool				Check_IsNear_ToDesireArm(_float _fTimeDelta);

public:
	static CCameraArm*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	CCameraArm*			Clone();
	virtual void		Free() override;
};

END