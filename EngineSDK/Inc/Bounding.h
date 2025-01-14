#pragma once
#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

/* 1. dx 에서 제공하는 충돌 관련 구조체들이 공통된 부모가 없어서, 관리하기가 영 번거롭다. */
/* 그래서 우리는 공통 된 부모 클래스를 만들고 이 부모클래스를 통해 관리 할 예정. */

/* 오직 각각의 충돌체의 부모 역할만 하기 위해서 만들어진 클래스이다, 특별한 기능은 없다. 그리기 관련된 코드만 존재. */
BEGIN(Engine)
class CBounding abstract : public CBase
{

public:
	typedef struct tagBoundingDesc : CCollider::COLLIDER_DESC
	{
		_float3 vCenterPos;
	}BOUND_DESC;

protected:
	CBounding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_fmatrix _OwnerWorldMatrix) = 0;
	virtual void Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _vector _vColor) = 0;
	virtual _bool Intersect(CCollider::TYPE _eType, CBounding* _pTargetBounding) = 0;
	virtual _bool Intersect_Ray(_fvector _vRayStart, _fvector _fRayDir, _float* _pOutDst) = 0;
	virtual _bool Contains(_fvector _vTargetPos) = 0;
	
protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;


public:
	virtual void Free() override;
};
END
