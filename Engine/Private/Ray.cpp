#include "Ray.h"
#include "GameInstance.h"
CRay::CRay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
	, m_vStartPos{}
	, m_vDir{}
{
	
}

CRay::CRay(const CRay& _Prototype)
	: CComponent(_Prototype)
	, m_vStartPos{ _Prototype.m_vStartPos }
	, m_vDir{ _Prototype.m_vDir }
{
}

HRESULT CRay::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CRay::Initialize(void* _pArg)
{
	__super::Initialize(_pArg);

	RAY_DESC* pDesc = static_cast<RAY_DESC*>(_pArg);
	m_vStartPos = pDesc->fStartPos;
	m_vDir = pDesc->fDir;
	m_fViewportX = pDesc->fViewportWidth;
	m_fViewportY = pDesc->fViewportHeight;

	return S_OK;
}

_bool CRay::Compute_Intersect_Triangle(_fvector _v0, _fvector _v1, _fvector _v2, _float& _fDist)
{
	return TriangleTests::Intersects(XMVectorSetW(XMLoadFloat3(&m_vStartPos), 1.0f), XMLoadFloat3(&m_vDir), _v0, _v1, _v2, _fDist);
}

_bool CRay::Compute_IntersectCoord_Triangle(_fvector _v0, _fvector _v1, _fvector _v2, _float3& _vIntersectCoord)
{
	_bool bResult = false;
	_float fDist = 0.0f;
	_vector vRayStart = XMVectorSetW(XMLoadFloat3(&m_vStartPos), 1.0f);
	_vector vRayDir = XMLoadFloat3(&m_vDir);
	bResult = TriangleTests::Intersects(vRayStart, XMVector3Normalize(vRayDir), _v0, _v1, _v2, fDist);


	if (bResult)
	{
		XMStoreFloat3(&_vIntersectCoord, vRayStart + (vRayDir * fDist));
	}

	return bResult;
}

HRESULT CRay::Update_RayInfoFromCursor(_float2 _vCursorPos, _fmatrix _InverseViewMatrix, _cmatrix _InverseProjMatrix)
{
	// Cam Position
	XMStoreFloat3(&m_vStartPos, _InverseViewMatrix.r[3]);

	// Ray Direction
	_vector vRayDir = XMVectorSet((_vCursorPos.x * 2.0f / m_fViewportX ) - 1.0f,
								  (_vCursorPos.y * -2.0f / m_fViewportY ) + 1.0f, 1.0f, 1.0f);
	vRayDir = XMVector3TransformCoord(vRayDir, _InverseProjMatrix); // 스크린좌표를 view space좌표로 이동.
	vRayDir = XMVector3TransformNormal(vRayDir, _InverseViewMatrix); // 이때부터 vRayDir는 Direction 값이 됌.
	XMStoreFloat3(&m_vDir, XMVector3Normalize(vRayDir));


	/* 굳이 투영좌표의 역행렬을 곱하지 않아도, 투영행렬의 _11, _22 원소만이 x, y 정점에 영향을 주는 좌표이다. */
	/* 그래서 아래와 같은 방식으로 코드를 작성해도 식은 성립한다. */

	//_float4x4 matProj = CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	//_vector vRayDir3 = XMVectorSet(((_vCursorPos.x * 2.0f / m_fViewportX) - 1.0f) / matProj._11,
	//							 ((_vCursorPos.y * -2.0f / m_fViewportY) + 1.0f) /  matProj._22, 1.0f, 1.0f);
	////vRayDir = XMVector3TransformCoord(vRayDir, _InverseProjMatrix);
	//vRayDir3 = XMVector3TransformNormal(vRayDir3, _InverseViewMatrix);
	//XMStoreFloat3(&m_vDir, XMVector3Normalize(vRayDir3));


	return S_OK;
}

CRay* CRay::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRay* pInstance = new CRay(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRay::Clone(void* _pArg)
{
	CRay* pInstance = new CRay(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CRay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRay::Free()
{
	__super::Free();
}
