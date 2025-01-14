#pragma once
#include "Transform.h"

/* Transform Component */
// 1. 객체의 월드 좌표계 내에서의 상태를 표현한다. (m_WorldMatrix)
// 2. 객체의 월드 좌표계 내에서의 상태 변환의 기능을 수행한다. (Move, Rotation, Move_To_Target 등 )
// 3. Shader에게 필요한 WorldMatrix를 제공한다.
// 4. Transform Component는 예외적으로. 모든 GameObeject가 반드시 소유하게 한다.

BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CTransform_3D final : public CTransform // 상황에 따라 final이 빠질 여지도 있나?
{
public:
	typedef struct tagTransform3DDesc : public CTransform::TRANSFORM_DESC
	{

	}TRANSFORM_3D_DESC;
private:
	CTransform_3D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTransform_3D(const CTransform_3D& _Prototype);
	virtual ~CTransform_3D() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* _pArg);

public:
	_bool				MoveToTarget(_fvector _vTargetPos, _float _fTimeDelta);

public:
	void				LookAt(_fvector _vAt); // 타겟의 위치쪽으로 바라보는 (회전하는)
	void				Update_AutoRotationY(_float _fTimeDelta);

public:
	_float				Compute_Distance_Scaled(CTransform_3D* const _vTargetPos) const; // 크기가 반영 된 상대와의 거리.

public:
	// Get
	const _float3*		Get_AutoRotationYDirection_Ptr() const { return &m_vAutoRotationYDirection; }

	// Set
	void				Set_AutoRotationYDirection(_fvector _vRotYTarget)  { XMStoreFloat3(&m_vAutoRotationYDirection, XMVector3Normalize(_vRotYTarget)); }

private: /* For. Auto Rotation Y */
	_float3				m_vAutoRotationYDirection = { 0.0f, 0.0f, 1.0f };
	_float				m_fCurAngle = 0.0f;
	_float				m_fTargetAngle = 0.0f;

public:
	static CTransform_3D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END