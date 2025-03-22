#pragma once
#include "Component.h"
BEGIN(Engine)
class CGameObject;
class CCollider_AABB;
class ENGINE_DLL CCollider abstract:  public CComponent
{
public:
	enum TYPE { CIRCLE_2D, AABB_2D, TYPE_LAST };
public:
	typedef struct tagColliderDesc
	{
		CGameObject*	pOwner = nullptr;

		_float2			vPosition = {};
		_float2			vOffsetPosition = {};
		_float2			vScale = { 1.0f, 1.0f };
		_bool			isBlock = false;
		_bool			isTrigger = false;
		_uint			iCollisionGroupID = {};
		_uint			iColliderUse = { 0 };
	}COLLIDER_DESC;
protected:
	CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider(const CCollider& _Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

#ifdef _DEBUG

	virtual HRESULT			Render();
	virtual HRESULT			Render(_float2 _fRenderTargetSize);
#endif // _DEBUG


public:
	virtual _bool			Is_Collision(CCollider* _pOther) = 0;
	virtual _bool			Is_ContainsPoint(_float2 _vPosition) = 0;
	virtual void			Update_OwnerTransform() = 0;
	virtual void			Block(CCollider* _pOther) { return; }// 블록등록한 애들이 otherobject를 밀어내게 .Circle, AABB 각각 밀어내기 코드가 존재해야겠지/...
public:
	virtual _bool			Compute_NearestPoint_AABB(CCollider_AABB* _pOtherAABB, _float2* _pOutPos = nullptr, _float2* _pContactVector_NotNormalize = nullptr) { return false; }

public:
	// Get
	TYPE					Get_Type() const { return m_eType; }
	_uint					Get_CollisionGroupID() { return m_iCollisionGroupID; }
	_uint					Get_ColliderID() const { return m_iColliderID; }
	_uint					Get_ColliderUse() const { return m_iColliderUse; }
	CGameObject*			Get_Owner() const { return m_pOwner; }
	_float2					Get_Position() const { return m_vPosition; }
	_bool					Is_Block() const { return m_isBlock; }
	_bool					Is_Trigger() const { return m_isTrigger; }
	virtual _bool			Get_CollisionPoint(CCollider* _pOther, _float2* pOutPoint) abstract;
	virtual _bool			Get_CollisionNormal(CCollider* _pOther, _float2* pOutNormal) abstract;
	// Set 
	void						Set_CollisionGroupID(_uint _iGroupID) { m_iCollisionGroupID = _iGroupID; }
	void						Set_Offset(_float2 _vOffset) { m_vOffsetPosition = _vOffset; }
	void						Set_Block(_bool _isBlock) { m_isBlock = _isBlock; }
	void					Set_Scale(_float2 _vScale) { m_vScale = _vScale; }
#ifdef _DEBUG
	void					Set_DebugColor(const _float4& _vDebugColor) { m_vDebugColor = _vDebugColor; }
#endif // _DEBUG

protected:
	static _uint			s_iNextColliderID;

protected:
	_uint					m_iCollisionGroupID = 0;
	_uint					m_iColliderID = 0;
	_uint					m_iColliderUse = 0;
	TYPE					m_eType = TYPE::TYPE_LAST;
	_bool					m_isTrigger = false;
	_bool					m_isBlock = false;
protected:
	CGameObject*			m_pOwner = nullptr;

	_float2					m_vPosition = {};
	_float2					m_vOffsetPosition = {};
	_float2					m_vScale = {};


#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
	BasicEffect*							m_pEffect = nullptr;
	ID3D11InputLayout*						m_pInputLayout = nullptr;
	_float4									m_vDebugColor = { 0.0f, 1.0f, 0.0f, 1.0f };
#endif // _DEBUG

public:
	virtual void Free() override;

};

END

//
//
//void XM_CALLCONV DX::DrawRing(PrimitiveBatch<VertexPositionColor>* batch,
//	FXMVECTOR origin,
//	FXMVECTOR majorAxis,
//	FXMVECTOR minorAxis,
//	GXMVECTOR color)
//{
//	static const size_t c_ringSegments = 32;
//
//	VertexPositionColor verts[c_ringSegments + 1];
//
//	FLOAT fAngleDelta = XM_2PI / float(c_ringSegments);
//	// Instead of calling cos/sin for each segment we calculate
//	// the sign of the angle delta and then incrementally calculate sin
//	// and cosine from then on.
//	XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
//	XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
//	XMVECTOR incrementalSin = XMVectorZero();
//	static const XMVECTORF32 s_initialCos =
//	{
//		{ { 1.f, 1.f, 1.f, 1.f } }
//	};
//	XMVECTOR incrementalCos = s_initialCos.v;
//	for (size_t i = 0; i < c_ringSegments; i++)
//	{
//		XMVECTOR pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
//		pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
//		XMStoreFloat3(&verts[i].position, pos);
//		XMStoreFloat4(&verts[i].color, color);
//		// Standard formula to rotate a vector.
//		XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
//		XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
//		incrementalCos = newCos;
//		incrementalSin = newSin;
//	}
//	verts[c_ringSegments] = verts[0];
//
//	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
//}