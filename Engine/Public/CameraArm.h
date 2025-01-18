#pragma once

#include "Engine_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class ENGINE_DLL CCameraArm final : public CBase
{
public:
	enum ARM_TYPE { DEFAULT, COPY, OTHER, ARM_TYPE_END };
	enum TARGET_STATE { RIGHT, UP, LOOK, POS, TARGET_STATE_END };
	enum ROTATE_TYPE { CROSS, NOT_CROSS, ROTATE_TYE_END };
	enum TIME_RATE_TYPE { EASE_IN, EASE_OUT, LERP, TIME_RATE_TYPE_END };	// Ease In -> 점점 빠르게 Ease Out -> 점점 느리게

	typedef struct tagCameraArmDesc
	{
		_float3				vArm = { 0.f, 0.f, -1.f };
		_float3				vPosOffset = { 0.f, 0.f, 0.f };
		_float3				vRotation;
		_float				fLength = 1.f;
		_wstring			wszArmTag = {};
		ARM_TYPE			eArmType = { DEFAULT };

		const _float4x4*	pTargetWorldMatrix = { nullptr };
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

	_float3				Get_Rotation() { return m_vRotation; }
	_float				Get_Length() { return m_fLength; }

	void				Set_Rotation(_vector _vRotation);
	void				Set_Length(_float _fLength) { m_fLength = _fLength; }
	void				Set_ArmType(_uint _eType) { m_eArmType = _eType; }
	void				Set_ArmTag(_wstring _wszArmTag) { m_wszArmTag = _wszArmTag; }
	void				Set_RotateType(_uint _iRotateType) { m_pNextArmData->iRotateType = _iRotateType; }
#endif

public:
	_wstring			Get_ArmTag() { return m_wszArmTag; }
	_vector				Get_ArmVector() { return XMLoadFloat3(&m_vArm); }
	_vector				Get_TargetState(TARGET_STATE _eTargetState) const { return XMLoadFloat4x4(m_pTargetWorldMatrix).r[_eTargetState]; }
	_uint				Get_RotateType() { return m_pNextArmData->iRotateType; }

	void				Set_NextArmData(ARM_DATA* _pData) { m_pNextArmData = _pData; }

	void				Change_Target(const _float4x4* _pTargetWorldMatrix) { m_pTargetWorldMatrix = _pTargetWorldMatrix; }
	_vector				Calculate_CameraPos(_float fTimeDelta);					// Arm과 Length에 따라 카메라 위치 결정
	_bool				Move_To_NextArm_Cross(_float _fTimeDelta);
	_bool				Move_To_NextArm_NotCross(_float _fTimeDelta);

private:
	ID3D11Device*		m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance*		m_pGameInstance = { nullptr };

private:
	_wstring			m_wszArmTag = {};
	_float3				m_vArm = {};
	_float3				m_vRotation = {};
	_float				m_fLength = {};

	// 이거 나중에 Target 월드 행렬 받으려면 Target File Tag 같은 거 필요할 수도
	const _float4x4*	m_pTargetWorldMatrix = { nullptr };

	_uint				m_eArmType = {};

	// Desire Arm
	ARM_DATA*			m_pNextArmData = { nullptr };
	_float				m_fSavedAngle = {};
	_float				m_fPreAngle = {};
	_bool				m_bSave = { false };

	_float3				m_SavedArm = {};

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
	void				Turn_ArmX(_float fAngle);
	void				Turn_ArmY(_float fAngle);
	_float				Calculate_Ratio();

public:
	static CCameraArm*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	CCameraArm*			Clone();
	virtual void		Free() override;
};

END