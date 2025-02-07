#pragma once
#include "Base.h"
#include "Transform_2D.h"
#include "Transform_3D.h"

BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CController_Transform final : public CBase
{
public:
	typedef struct tagControllerTransformDesc
	{
		// 2D, 3D 좌표계 여부 enum
		COORDINATE							eStartCoord = COORDINATE_LAST;
		// 좌표계 변환 가능 여부
		_bool								isCoordChangeEnable = false;
		// 2D Transform의 Desc, 사용하지 않는다면 채우지 않아도 됌.
		CTransform_2D::TRANSFORM_2D_DESC	tTransform2DDesc = {};
		// 2D Transform의 Desc, 사용하지 않는다면 채우지 않아도 됌.
		CTransform_3D::TRANSFORM_3D_DESC	tTransform3DDesc = {};

#pragma region Build Method
		void Build_2D_Transform(_float2 _fPos, _float2 _fScale = { 1.f,1.f }, _float _fMoveSpeed = 10.f,
			_float _fRotationSpeed = 0.f, _float fRotation = 0.f)
		{
			tTransform2DDesc.fSpeedPerSec = _fMoveSpeed;
			tTransform2DDesc.fRotationPerSec = _fRotationSpeed;
			tTransform2DDesc.vInitialPosition = { _fPos.x, _fPos.y, 0.0f };
			tTransform2DDesc.vInitialScaling = { _fScale.x, _fScale.y, 1.0f };
			tTransform2DDesc.fInitialRotation = fRotation;
		}

		void Build_3D_Transform(_float4x4 _matWorld)
		{
			tTransform3DDesc.isMatrix = true;
			tTransform3DDesc.matWorld = _matWorld;
		}


#pragma endregion

	}CON_TRANSFORM_DESC;

private:
	CController_Transform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CController_Transform() = default;

public:
	HRESULT					Initialize(CON_TRANSFORM_DESC* _pDesc);
	void							Update_AutoRotation(_float _fTimeDelta);
public: /* Change_Coordinate */
	HRESULT					Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr);

public: /* 2D, 3D */
	_bool					Go_Straight(_float _fTimeDelta);
	_bool					Go_Backward(_float _fTimeDelta);
	_bool					Go_Left(_float _fTimeDelta);
	_bool					Go_Right(_float _fTimeDelta);
	_bool					Go_Up(_float _fTimeDelta);
	_bool					Go_Down(_float _fTimeDelta);
	void					Go_Direction(_vector _vDirection, _float _fTimeDelta);
	void					Go_Direction(_vector _vDirection, _float _fSpeed, _float _fTimeDelta);

	void					Rotation(_float _fRadian, _fvector _vAxis = { 0.0f, 0.0f, 1.0f, 0.0f }); // 항등상태를 기준으로 지정한 각도로 회전한다.
	void					RotationXYZ(const _float3& _vRadianXYZ);
	void					RotationQuaternion(const _float3& _vRadianXYZ);
	void					RotationQuaternionW(const _float4& _vQuaternion);	//쿼터니언으로 회전

	void					Turn(_float _fRadian, _fvector _vAxis = { 0.0f, 0.0f, 1.0f, 0.0f }); // 기존 회전을 기준으로 추가로 정해진 속도로 회전한다.
	void					TurnAngle(_float _fRadian, _fvector _vAxis = { 0.0f, 1.0f, 0.0f, 0.0f });	// 기존 회전을 기준으로 정해진 각도만큼 회전한다
	_float					Compute_Distance(_fvector _vTargetPos); // 거리 계산 함수.
	HRESULT					Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public: /* 2D */

public: /* 3D */
	void					LookAt_3D(_fvector _vAt); // 타겟의 위치쪽으로 바라보는 (회전하는)
	_bool				MoveToTarget(_fvector _vTargetPos, _float _fTimeDelta);

public:
	// Get
	COORDINATE				Get_CurCoord() const { return m_eCurCoord; }
	CTransform*				Get_Transform() const;
	CTransform*				Get_Transform(COORDINATE _eCoordinate) const;
	_matrix					Get_WorldMatrix() const;
	_matrix					Get_WorldMatrix(COORDINATE _eCoordinate) const;
	const _float4x4* Get_WorldMatrix_Ptr() const;
	const _float4x4* Get_WorldMatrix_Ptr(COORDINATE _eCoordinate) const;
	_float3					Get_Scale() const;
	_vector					Get_State(CTransform::STATE _eState) const;
	_float					Get_SpeedPerSec() const;
	_float					Get_RotationPerSec() const;
	_bool					Is_CoordChangeEnable() const { return m_isCoordChangeEnable; }
	// Set
	void					Set_WorldMatrix(_fmatrix _WorldMatrix);
	void					Set_WorldMatrix(const _float4x4& _WorldMatrix);
	void					Set_Scale(_float _fX, _float _fY, _float _fZ);
	void					Set_Scale(const _float3& _vScale);
	void					Set_Scale(COORDINATE _eCoord, _float _fX, _float _fY, _float _fZ);
	void					Set_Scale(COORDINATE _eCoord, const _float3& _vScale);
	void					Set_State(CTransform::STATE _eState, _fvector _vState);
	void					Set_State(CTransform::STATE _eState, const _float4& _vState);
	void					Set_SpeedPerSec(_float _fSpeedPerSec);
	void					Set_RotationPerSec(_float _fRotationPerSec);
	void					Set_AutoRotationYDirection(_fvector _vRotYTarget);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	CGameInstance* m_pGameInstance = nullptr;

private:
	CTransform* m_pTransforms[COORDINATE::COORDINATE_LAST] = { nullptr, nullptr };

	COORDINATE				m_eCurCoord = COORDINATE_LAST;
	_bool					m_isCoordChangeEnable = false;

private:
	HRESULT					Ready_Transforms(CON_TRANSFORM_DESC* _pDesc);

public:
	static CController_Transform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CON_TRANSFORM_DESC* _pDesc);
	virtual void Free() override;
};
END
