#include "Shadow.h"

CShadow::CShadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
{
    Safe_AddRef(_pDevice);
    Safe_AddRef(_pContext);
}

void CShadow::SetUp_TargetShadowMatrix(_fvector _vLightDirection, _fvector _vWorldPos, _float _fDistance, _float2 _vNearFar)
{
	_vector vAt = {};
	_vector vLightDir = _vLightDirection;
	_vector vEye = _vWorldPos;
	vEye -= XMVector3Normalize(vLightDir) * _fDistance;
	vAt = vEye + XMVector3Normalize(vLightDir);
	SetUp_TransformMatrix(CShadow::D3DTS_VIEW, XMMatrixLookAtLH(vEye, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));


	SetUp_TransformMatrix(CShadow::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), static_cast<_float>(g_iShadowWidth) / g_iShadowHeight, _vNearFar.x, _vNearFar.y));
}

HRESULT CShadow::Initialize()
{
    return S_OK;
}

void CShadow::Update()
{
}

CShadow* CShadow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShadow* pInstance = new CShadow(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CShadow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
