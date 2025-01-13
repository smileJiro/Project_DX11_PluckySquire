#pragma once
#include "Bounding.h"

BEGIN(Engine)
class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc: public CBounding::BOUND_DESC
	{
		_float3		vExtents;
		_float3		vRotation; /* 사용자 입장에서는 x,y,z, 에 대한 오일러각으로, 실제 구현부는 쿼터니언으로  */
	}BOUND_OBB_DESC;
private:
	/* Bounding Box에 대한 데이터는 콜라이더의 사본 객체 생성 시 채워 줄 것. */
	CBounding_OBB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual HRESULT Initialize(const BOUND_DESC* _pBoundDesc);
	virtual void Update(_fmatrix _OwnerWorldMatrix) override;
	virtual void Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _vector _vColor) override;
	virtual _bool Intersect(CCollider::TYPE _eType, CBounding* _pTargetBounding) override;
	virtual _bool Intersect_Ray(_fvector _vRayStart, _fvector _fRayDir, _float* _pOutDst) override;
	virtual _bool Contains(_fvector _fTargetPos) override;
public:
	const BoundingOrientedBox* Get_FinalBoundingBox_Ptr() const { return m_pFinalBoundingBox; }
private: /* BoundingBox : AABB에 사용 될 구조체 타입 */
	BoundingOrientedBox* m_pOriginalBoundingBox;
	BoundingOrientedBox* m_pFinalBoundingBox;

public:
	static CBounding_OBB* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const BOUND_DESC* _pBoundDesc);
	virtual void Free();
};
END
