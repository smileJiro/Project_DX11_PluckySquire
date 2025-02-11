#include "Light.h"
#include "GameInstance.h"
#include "DebugDraw.h"
CLight::CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LIGHT_TYPE _eLightType)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
	, m_eType(_eLightType)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLight::Initialize(const CONST_LIGHT& _LightDesc)
{
	m_tLightConstData = _LightDesc;

	// Create ConstantBuffer
	if (FAILED(m_pGameInstance->CreateConstBuffer(m_tLightConstData, D3D11_USAGE_DYNAMIC, &m_pLightConstbuffer)))
		return E_FAIL;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	const void* pShaderByteCode = nullptr;
	size_t iShaderByteCodeLength = 0;

	/* m_pEffect 쉐이더의 옵션 자체를 Color 타입으로 변경해주어야 정상적인 동작을 한다. (기본 값은 Texture )*/
	m_pEffect->SetVertexColorEnabled(true);
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CLight::Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{

	if (KEY_DOWN(KEY::F))
	{
		if (m_eType == LIGHT_TYPE::POINT)
		{
			m_tLightConstData.vPosition = { 0.0f, 20.f, 0.0f };
			m_tLightConstData.fFallOutStart = 1.f;
			m_tLightConstData.fFallOutEnd = 30.f;
			m_tLightConstData.vDiffuse = { 0.0f, 0.0f, 9.0f, 1.0f };
			m_tLightConstData.vSpecular = { 0.0f, 0.0f, 1.0f, 1.0f };
			m_pGameInstance->UpdateConstBuffer(m_tLightConstData, m_pLightConstbuffer);
		}
		if (m_eType == LIGHT_TYPE::DIRECTOINAL)
		{
			m_tLightConstData.vDirection = { 0.0f, -1.0f, 1.0f };
			m_tLightConstData.vRadiance = _float3(1.0f, 1.0f, 1.0f);
			m_tLightConstData.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.0f);
			m_tLightConstData.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.0f);
			m_tLightConstData.vSpecular = _float4(0.1f, 0.1f, 0.1f, 1.0f);
			m_pGameInstance->UpdateConstBuffer(m_tLightConstData, m_pLightConstbuffer);
		}
	}


	_uint iPassIndex = {};
	
	switch (m_eType)
	{
	case Engine::LIGHT_TYPE::POINT:
		iPassIndex = (_uint)PASS_DEFERRED::PBR_LIGHT_POINT;
		break;
	case Engine::LIGHT_TYPE::DIRECTOINAL:
		iPassIndex = (_uint)PASS_DEFERRED::PBR_LIGHT_DIRECTIONAL;
		break;
	default:
		return E_FAIL;
	}

	if(FAILED(_pShader->Bind_ConstBuffer("BasicDirectLightConstData", m_pLightConstbuffer)))
		return E_FAIL;

	if (FAILED(_pShader->Begin(iPassIndex)))
		return E_FAIL;

	_pVIBuffer->Render();


	//m_pEffect->SetWorld(XMMatrixIdentity());
	//m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	//m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	//
	//m_pEffect->Apply(m_pContext);
	//m_pContext->IASetInputLayout(m_pInputLayout);
	//m_pBatch->Begin();
	//BoundingSphere sphere = {};
	//sphere.Center = m_tLightConstData.vPosition;
	//sphere.Radius = 2.0f;
	//DX::Draw(m_pBatch, sphere, XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
	//
	//m_pBatch->End();
	return S_OK;
}

HRESULT CLight::Set_LightConstData_AndUpdateBuffer(const CONST_LIGHT& _LightConstData)
{
	m_tLightConstData = _LightConstData;

	if(FAILED(m_pGameInstance->UpdateConstBuffer(m_tLightConstData, m_pLightConstbuffer)))
		return E_FAIL;
	return S_OK;
}


CLight* CLight::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CONST_LIGHT& _LightDesc, LIGHT_TYPE _eLightType)
{
	CLight* pInstance = new CLight(_pDevice, _pContext, _eLightType);

	if (FAILED(pInstance->Initialize(_LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
#endif // _DEBUG

	Safe_Release(m_pLightConstbuffer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
