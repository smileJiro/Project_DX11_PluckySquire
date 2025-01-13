#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
CBounding_Sphere::CBounding_Sphere(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBounding(_pDevice, _pContext)
{
}

HRESULT CBounding_Sphere::Initialize(const BOUND_DESC* _pBoundDesc)
{
	const BOUND_SPHERE_DESC* pDesc = static_cast<const BOUND_SPHERE_DESC*>(_pBoundDesc);

	m_pOriginalBoundingBox = new BoundingSphere(pDesc->vCenterPos, pDesc->fRadius);
	m_pFinalBoundingBox = new BoundingSphere(*m_pOriginalBoundingBox);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix _OwnerWorldMatrix)
{
	m_pOriginalBoundingBox->Transform(*m_pFinalBoundingBox, _OwnerWorldMatrix);
}

void CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _vector _vColor)
{
	DX::Draw(_pBatch, *m_pFinalBoundingBox, _vColor);
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE _eType, CBounding* _pTargetBounding)
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

_bool CBounding_Sphere::Intersect_Ray(_fvector _vRayStart, _fvector _fRayDir, _float* _pOutDst)
{
	_bool isColl = false;
	_float fDst = 0.0f;
	isColl = m_pFinalBoundingBox->Intersects(_vRayStart, _fRayDir, fDst);

	if (true == isColl)
		*_pOutDst = fDst;

	return isColl;
}

_bool CBounding_Sphere::Contains(_fvector _vTargetPos)
{
	_bool isColl = false;
	/* 단순 현재 상대방의 위치가 나의 충돌체 안에 들어왔는 지 검사하는 함수. */
	isColl = m_pFinalBoundingBox->Contains(_vTargetPos);

	return isColl;
}


CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const BOUND_DESC* _pBoundDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounding_Sphere::Free()
{
	Safe_Delete(m_pOriginalBoundingBox);
	Safe_Delete(m_pFinalBoundingBox);

	__super::Free();
}
