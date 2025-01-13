#pragma once
#include "Component.h"

/* 객체에 씌울 충돌체이다. */
/* 각각의 충돌체는 ColliderType에 따라 구, AABB, OBB 충돌을 수행한다. */
BEGIN(Engine)
class CBounding;
class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { SPHERE, AABB, OBB, TYPE_LAST };
	enum USE { USE_NORMAL, USE_ATTACK, USE_HIT, USE_DETECTION, USE_LAST };
	enum ISCOLLISION { NONE, COLLISION, COLLISION_RAY, CONTAINS, LAST };
public:
	typedef struct tagColliderDesc
	{
		USE eUse;
	}COLLIDER_DESC;
private:
	CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider(const CCollider& _Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE _eColliderType); /* 렌더링 관련 변수만 초기화 */
	virtual HRESULT Initialize(void* _pArg) override; /* 실질적인 충돌체에 대한 타입을 지정하고 타입에 맞는 CBounding을 생성. */
	virtual void	Update(_fmatrix _OwnerWorldMatrix);

#ifdef _DEBUG
	virtual HRESULT Render();
#endif

public:
	virtual _bool	Intersect(const CCollider* _pTargetCollider);
	_bool			Intersect_Ray(_fvector _vRayStart, _fvector _vRayDir, _float* _pOutDst);
	_bool			Contains(_fvector _vTargetPos);
public:
	// Get
	CBounding*		Get_Bounding_Ptr() { return m_pBounding; }
	TYPE			Get_ColliderType() const { return m_eType; };
	USE				Get_ColliderUse() const { return m_eUse; };
	_uint			Get_ColliderID() const { return m_iColliderID; }
	_bool			Is_Collision() const { return m_eCollision == ISCOLLISION::COLLISION ?  true : false; }
	// Set
	void			Set_Collision(ISCOLLISION _isCollision) { m_eCollision = _isCollision; }
	void			Set_OffsetMatrix(_fmatrix _OffsetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, _OffsetMatrix); };
private:
	static _uint	g_iNextColliderID;
private:
	TYPE			m_eType = TYPE::TYPE_LAST;
	USE				m_eUse = USE::USE_LAST;
	_uint			m_iColliderID = 0; /* 고유 아이디 값. */
	CBounding*		m_pBounding = nullptr;
	_float4x4		m_OffsetMatrix = {};
protected:
	ISCOLLISION		m_eCollision = NONE;


#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
#endif // _DEBUG

public:
	static CCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE _eColliderType);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END