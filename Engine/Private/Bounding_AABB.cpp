#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBounding(_pDevice, _pContext)
{
}

HRESULT CBounding_AABB::Initialize(const BOUND_DESC* _pBoundDesc)
{
	const BOUND_AABB_DESC* pDesc = static_cast<const BOUND_AABB_DESC*>(_pBoundDesc);

	m_pOriginalBoundingBox = new BoundingBox(pDesc->vCenterPos, pDesc->vExtents);
	m_pFinalBoundingBox = new BoundingBox(*m_pOriginalBoundingBox);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix _OwnerWorldMatrix)
{

	/* Rotation을 막자 */

	_matrix TransformMatrix = _OwnerWorldMatrix;
	TransformMatrix.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * XMVector3Length(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * XMVector3Length(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * XMVector3Length(TransformMatrix.r[2]);

	/* 회전에 대한 정보는 제외한 행렬을 설정한다. (AABB는 회전하지 않는 사각형 )*/
	m_pOriginalBoundingBox->Transform(*m_pFinalBoundingBox, TransformMatrix);
}

void CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _vector _vColor)
{
	DX::Draw(_pBatch, *m_pFinalBoundingBox, _vColor);
}

_bool CBounding_AABB::Intersect(CCollider::TYPE _eType, CBounding* _pTargetBounding)
{
	_bool isColl = false;

	switch (_eType)
	{
	case CCollider::SPHERE:
		isColl = m_pFinalBoundingBox->Intersects(*static_cast<CBounding_Sphere*>(_pTargetBounding)->Get_FinalBoundingBox_Ptr());
		break;
	case CCollider::AABB:
		isColl = m_pFinalBoundingBox->Intersects(*static_cast<CBounding_AABB*>(_pTargetBounding)->Get_FinalBoundingBox_Ptr());
		break;
	case CCollider::OBB:
		isColl = m_pFinalBoundingBox->Intersects(*static_cast<CBounding_OBB*>(_pTargetBounding)->Get_FinalBoundingBox_Ptr());
		break;
	default:
		break;
	}

	return isColl;
}

_bool CBounding_AABB::Intersect_Ray(_fvector _vRayStart, _fvector _fRayDir, _float* _pOutDst)
{
	_bool isColl = false;

	_float fDst = 0.0f;
	isColl = m_pFinalBoundingBox->Intersects(_vRayStart, _fRayDir, fDst);

	if (true == isColl)
		*_pOutDst = fDst;


	return isColl;
}

_bool CBounding_AABB::Contains(_fvector _fTargetPos)
{
	_bool isColl = false;

	_float fDst = 0.0f;
	isColl = m_pFinalBoundingBox->Contains(_fTargetPos);


	return isColl;
}


CBounding_AABB* CBounding_AABB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const BOUND_DESC* _pBoundDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounding_AABB::Free()
{
	Safe_Delete(m_pOriginalBoundingBox);
	Safe_Delete(m_pFinalBoundingBox);

	__super::Free();
}
