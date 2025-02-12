#pragma once
#include "Collider.h"
BEGIN(Engine)
class CCollider_AABB;
class ENGINE_DLL CCollider_Circle : public CCollider
{
public:
	typedef struct tagColliderCircleDesc : public CCollider::COLLIDER_DESC
	{
		_float fRadius = 0.5f;

	}COLLIDER_CIRCLE_DESC;
protected:
	CCollider_Circle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider_Circle(const CCollider_Circle& _Prototype);
	virtual ~CCollider_Circle() = default;

public:
	HRESULT						Initialize_Prototype() override;
	HRESULT						Initialize(void* _pArg) override;
	void						Update(_float _fTimeDelta) override;
	void						Late_Update(_float _fTimeDelta) override;
	HRESULT						Render(_float2 _fRenderTargetSize) override;

public:
	_bool						Is_Collision(CCollider* _pOther) override;
	void						Update_OwnerTransform() override;
	void						Block(CCollider* _pOther);
	void						Block_AABB(CCollider_AABB* _pOther);
	void						Block_Circle(CCollider_Circle* _pOther);

private:
	_bool						Is_Collision_Circle(CCollider_Circle* _pOther);
	_bool						Is_Collision_AABB(CCollider_AABB* _pOther);

public:
	_float						Get_FinalRadius() const { return m_fFinalRadius; }
	void							Set_Radius(_float _fRadius) { m_fRadius = _fRadius; }
private:
	_float						m_fRadius = 0.0f;
	_float						m_fFinalRadius = 0.0f;

public:
	static CCollider_Circle*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent*			Clone(void* _pArg);
	void						Free() override;
};

END