#pragma once
#include "Collider.h"
BEGIN(Engine)
class CCollider_Circle;
class ENGINE_DLL CCollider_AABB final : public CCollider
{
public:
	typedef struct tagColliderAABBDesc : public CCollider::COLLIDER_DESC
	{
		// 가로 세로 반길이
		_float2 vExtents = {};
	}COLLIDER_AABB_DESC;
private:
	CCollider_AABB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider_AABB(const CCollider_AABB& _Prototype);
	virtual ~CCollider_AABB() = default;

public:
	HRESULT			Initialize_Prototype() override;
	HRESULT			Initialize(void* _pArg) override;
	void			Update(_float _fTimeDelta) override;
	void			Late_Update(_float _fTimeDelta) override;

#ifdef _DEBUG
	HRESULT			Render(_float2 _fRenderTargetSize) override;
#endif // _DEBUG

public:
	_bool			Is_Collision(CCollider* _pOther) override;
	virtual _bool	Is_ContainsPoint(_float2 _vPosition)override;
	void			Block(CCollider* _pOther);
	void			Block_AABB(CCollider_AABB* _pOther);
	void			Block_Circle(CCollider_Circle* _pOther);

public:
	void			Update_OwnerTransform() override;

private:
	_bool					Is_Collision_AABB(CCollider_AABB* _pOther);
	_bool					Is_Collision_Circle(CCollider_Circle* _pOther);

public:
	_float2					Get_LT();
	_float2					Get_RB();

public:
	// Get 
	_float2					Get_FinalExtents() const { return m_vFinalExtents; }
private:
	_float2					m_vExtents = {}; // 가로 세로 반길이
	_float2					m_vFinalExtents = {};

public:
	static CCollider_AABB* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	void Free() override;
};

END