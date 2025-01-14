#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	_uint iPassIndex = {};

	if (LIGHT_DESC::TYPE_DIRECTOINAL == m_LightDesc.eType)
	{
		if (FAILED(_pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
		iPassIndex = (_uint)PASS_DEFERRED::LIGHT_DIRECTION;
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		if (FAILED(_pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(_pShader->Bind_RawValue("g_vLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
		iPassIndex = (_uint)PASS_DEFERRED::LIGHT_POINT;
	}

	if (FAILED(_pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(_pShader->Begin(iPassIndex)))
		return E_FAIL;

	return _pVIBuffer->Render();
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
