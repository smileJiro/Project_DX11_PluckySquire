#pragma once
#include "Collider_Circle.h"
BEGIN(Engine)
class ENGINE_DLL CCollider_Fan :
    public CCollider_Circle
{
public:
	typedef struct tagColliderFanDesc : public CCollider_Circle::COLLIDER_CIRCLE_DESC
	{
		_float fRadianAngle = XMConvertToRadians(180.f);
		_float2 vDirection = { 1.f,0.f };
	}COLLIDER_FAN_DESC;
private:
	CCollider_Fan(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider_Fan(const CCollider_Fan& _Prototype);
	virtual ~CCollider_Fan() = default;

public:
	//원 충돌을 먼저 적용 후, 상대 콜라이더의 중점이 부채꼴 안이면 충돌
	virtual HRESULT Initialize(void* _pArg)override;
    virtual _bool	Is_Collision(CCollider* _pOther) override;
	virtual _bool	Is_ContainsPoint(_float2 _vPosition) override;
	virtual void Late_Update(_float _fTimeDelta)override;
	virtual void	 Update_OwnerTransform() override;

	void Set_RadianAngle(_float _fRadianAngle) { m_fRadianAngle = _fRadianAngle; }
	void Set_Direction(_float2 _vDirection) { m_vDirection = _vDirection; }
private:
	_float m_fRadianAngle = XMConvertToRadians(180.f);
	_float2 m_vDirection = { 1.f,0.f };

public:
	static CCollider_Fan* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	void						Free() override;
};

END