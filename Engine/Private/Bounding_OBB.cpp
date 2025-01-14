#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBounding(_pDevice, _pContext)
{
}

HRESULT CBounding_OBB::Initialize(const BOUND_DESC* _pBoundDesc)
{
	const BOUND_OBB_DESC* pDesc = static_cast<const BOUND_OBB_DESC*>(_pBoundDesc);

	
	/* 오일러 각 회전을 쿼터니언으로 변형. */
	_float4 vQuaternion{};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pOriginalBoundingBox = new BoundingOrientedBox(pDesc->vCenterPos, pDesc->vExtents, vQuaternion);
	m_pFinalBoundingBox = new BoundingOrientedBox(*m_pOriginalBoundingBox);

	return S_OK;
}

void CBounding_OBB::Update(_fmatrix _OwnerWorldMatrix)
{

	m_pOriginalBoundingBox->Transform(*m_pFinalBoundingBox, _OwnerWorldMatrix);
}

void CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _vector _vColor)
{
	DX::Draw(_pBatch, *m_pFinalBoundingBox, _vColor);
}

_bool CBounding_OBB::Intersect(CCollider::TYPE _eType, CBounding* _pTargetBounding)
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

_bool CBounding_OBB::Intersect_Ray(_fvector _vRayStart, _fvector _fRayDir, _float* _pOutDst)
{
	_bool isColl = false;
	_float fDst = 0.0f;
	isColl = m_pFinalBoundingBox->Intersects(_vRayStart, _fRayDir, fDst);

	if (true == isColl)
		*_pOutDst = fDst;

	return isColl;
}

_bool CBounding_OBB::Contains(_fvector _fTargetPos)
{
	_bool isColl = false;

	_float fDst = 0.0f;
	isColl = m_pFinalBoundingBox->Contains(_fTargetPos);

	return isColl;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const BOUND_DESC* _pBoundDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pBoundDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBounding_OBB::Free()
{
	Safe_Delete(m_pOriginalBoundingBox);
	Safe_Delete(m_pFinalBoundingBox);

	__super::Free();
}
