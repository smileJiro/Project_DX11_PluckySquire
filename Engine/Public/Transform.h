#pragma once
#include "Component.h"

BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CTransform abstract : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
	typedef struct tagTransformDesc
	{
		// 초당 이동속도
		_float	fSpeedPerSec = {};
		// 초당 회전속도
		_float	fRotationPerSec = {};
		// 초기화 위치
		_float3 vPosition = { 0.0f, 0.0f, 0.0f };

		// 초기화 크기
		_float3 vScaling = { 1.0f, 1.0f, 1.0f };
	}TRANSFORM_DESC;

protected:
	CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTransform(const CTransform& _Prototype);
	virtual ~CTransform() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);

public:
	virtual _bool			Go_Straight(_float _fTimeDelta);
	virtual _bool			Go_Backward(_float _fTimeDelta);
	_bool					Go_Left(_float _fTimeDelta); 
	_bool					Go_Right(_float _fTimeDelta); 
	_bool					Go_Up(_float _fTimeDelta); 
	_bool					Go_Down(_float _fTimeDelta);
	void						Go_Direction(_vector _vDirection, _float _fTimeDelta);

	void					Rotation(_float _fRadian, _fvector _vAxis = { 0.0f, 0.0f, 1.0f, 0.0f }); // 항등상태를 기준으로 지정한 각도로 회전한다.
	void					RotationZ(_float _vRadianZ);
	virtual void			RotationXYZ(const _float3& _vRadianXYZ);
	virtual void			RotationQuaternion(const _float3& _vRadianXYZ);
	virtual void			RotationQuaternionW(const _float4& _vQuaternion);	//쿼터니언으로 회전

	void					Turn(_float _fTimeDelta, _fvector _vAxis = { 0.0f, 0.0f, 1.0f, 0.0f }); // 기존 회전을 기준으로 추가로 정해진 속도로 회전한다.
	void					TurnZ(_float _fTimeDelta);

	_float					Compute_Distance(_fvector _vTargetPos) const; // 거리 계산 함수.

public:
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName) const;

public:
	// Get
	_matrix					Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }
	const _float4x4*		Get_WorldMatrix_Ptr() const { return &m_WorldMatrix; }
	_float3					Get_Scale(); const
	_vector					Get_State(STATE _eState) const { return XMLoadFloat4x4(&m_WorldMatrix).r[_eState]; }
	_float					Get_SpeedPerSec() const { return m_fSpeedPerSec; };
	_float					Get_RotationPerSec() const { return m_fRotationPerSec; }

	// Set
	void					Set_WorldMatrix(_fmatrix _WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, _WorldMatrix); }
	void					Set_WorldMatrix(const _float4x4& _WorldMatrix) { m_WorldMatrix = _WorldMatrix; }
	void					Set_Scale(_float _fX, _float _fY, _float _fZ);
	void					Set_Scale(const _float3& _vScale);
	void					Set_State(STATE _eState, _fvector _vState) { XMStoreFloat4((_float4*)&m_WorldMatrix.m[_eState], _vState); }
	void					Set_State(STATE _eState, const _float4& _vState) { memcpy(m_WorldMatrix.m[_eState], &_vState, sizeof(_float4)); }
	void					Set_SpeedPerSec(_float _fSpeedPerSec) { m_fSpeedPerSec = _fSpeedPerSec; };
	void					Set_RotationPerSec(_float _fRotationPerSec) { m_fRotationPerSec = _fRotationPerSec; };

protected:
	_float4x4				m_WorldMatrix = {};
	_float					m_fSpeedPerSec = {};
	_float					m_fRotationPerSec = {};

public:
	virtual void Free() override;

};
END
