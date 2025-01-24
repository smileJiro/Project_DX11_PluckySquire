#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
    : m_pGameInstance( CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	/* 절두체를 구성하기위한 여덟개점을 만들자. */
	/* 투영스페이스 공간에 존재한다. */
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);
    return S_OK;
}

void CFrustum::Update()
{
	_matrix		ViewMatrixInverse = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW);
	_matrix		ProjMatrixInverse = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_PROJ);

	for (size_t i = 0; i < 8; ++i)
	{
		/* 뷰 스페이스 까지 이동 */
		XMStoreFloat3(&m_vPoints_InWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInverse));
		/* 월드 스페이스 까지 이동 */
		XMStoreFloat3(&m_vPoints_InWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints_InWorld[i]), ViewMatrixInverse));
	}

	// Out : Planes_InWorld
	Make_Planes(m_vPoints_InWorld, m_vPlanes_InWorld);
}

_bool CFrustum::isIn_InWorldSpace(_fvector _vWorldPos, _float _fRange)
{
	for (size_t i = 0; i < 6; ++i)
	{
		/*	a b c d
			x y z 1	*/
		/* ax + by + cz + d = ? */
		if (_fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanes_InWorld[i]), _vWorldPos)))
			return false;
	}

    return true;
}

void CFrustum::Make_Planes(const _float3* pPoints, _float4* pOutPlanes)
{
	/* +x */
	XMStoreFloat4(&pOutPlanes[0], XMPlaneFromPoints(
	XMLoadFloat3(&pPoints[1]),
	XMLoadFloat3(&pPoints[5]),
	XMLoadFloat3(&pPoints[6])));

	/* -x */
	XMStoreFloat4(&pOutPlanes[1], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[3])));

	/* +y */
	XMStoreFloat4(&pOutPlanes[2], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[1])));

	/* -y */
	XMStoreFloat4(&pOutPlanes[2], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[1])));

	/* +z */
	XMStoreFloat4(&pOutPlanes[4], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[5]),
		XMLoadFloat3(&pPoints[4]),
		XMLoadFloat3(&pPoints[7])));

	/* -z */
	XMStoreFloat4(&pOutPlanes[5], XMPlaneFromPoints(
		XMLoadFloat3(&pPoints[0]),
		XMLoadFloat3(&pPoints[1]),
		XMLoadFloat3(&pPoints[2])));
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	Safe_Release(m_pGameInstance);

	__super::Free();
}
