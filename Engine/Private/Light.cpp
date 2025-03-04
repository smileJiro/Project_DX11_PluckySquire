#include "Light.h"
#include "GameInstance.h"
#include "DebugDraw.h"

_int CLight::s_iShadowLightID = 0;

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

	// Shadow Light 여부 판단 후 행렬 계산 작업 수행.
	if (true == (_bool)m_tLightConstData.isShadow)
	{
		/* 0. Shadow ID 부여 */
		m_iShadowLightID = s_iShadowLightID++;

		/* 1. RenderTarget을 만든다, DSV는 DSV_Shadow 사용한다(메인앱에서 생성했음). */
		_wstring strShadowRTTag = TEXT("Target_Shadow_");
		strShadowRTTag += to_wstring(m_iShadowLightID);
		m_pGameInstance->Add_RenderTarget(strShadowRTTag, (_uint)SHADOWMAP_X, (_uint)SHADOWMAP_Y, DXGI_FORMAT_R32_FLOAT, _float4(1.0f,0.0f,0.0f,0.0f), &m_pShadowRenderTarget);
		Safe_AddRef(m_pShadowRenderTarget);
		/* 2. 자기 자신을 shadow rendergroup에 등록한다. */
		Compute_ViewProjMatrix();

		m_pGameInstance->Add_ShadowLight(this);
	}

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
HRESULT CLight::Render_Light(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	if (false == m_isActive)
		return S_OK;

	_uint iPassIndex = {};
	
	switch (m_eType)
	{
	case Engine::LIGHT_TYPE::POINT:
		iPassIndex = (_uint)PASS_DEFERRED::PBR_LIGHT_POINT;
		break;
	case Engine::LIGHT_TYPE::DIRECTOINAL:
		iPassIndex = (_uint)PASS_DEFERRED::PBR_LIGHT_DIRECTIONAL;
		break;
	case Engine::LIGHT_TYPE::SPOT:
		iPassIndex = (_uint)PASS_DEFERRED::PBR_LIGHT_SPOT;
		break;
	default:
		return E_FAIL;
	}

	if(true == (_bool)m_tLightConstData.isShadow)
	{
		_pShader->Bind_Matrix("g_LightViewMatrix", &m_ViewMatrix);
		_pShader->Bind_Matrix("g_LightProjMatrix", &m_ProjMatrix);
		if (FAILED(_pShader->Bind_SRV("g_ShadowMapTexture", m_pShadowRenderTarget->Get_SRV())))
			return E_FAIL;
	}

	if(FAILED(_pShader->Bind_ConstBuffer("BasicDirectLightConstData", m_pLightConstbuffer)))
		return E_FAIL;

	if (FAILED(_pShader->Begin(iPassIndex)))
		return E_FAIL;

	_pVIBuffer->Render();


#ifdef _DEBUG
	m_pGameInstance->Add_BaseDebug(this);
#endif // _DEBUG


	return S_OK;
}

void CLight::Update(_float _fTimeDelta)
{

}

#ifdef _DEBUG

HRESULT CLight::Render_Base_Debug()
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pBatch->Begin();
	BoundingSphere sphere = {};
	sphere.Center = m_tLightConstData.vPosition;
	sphere.Radius = 2.0f;

	static _vector m_LightDebugColor = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);
	static _vector m_LightSelectDebugColor = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	if(true == m_isSelect)
		DX::Draw(m_pBatch, sphere, m_LightSelectDebugColor);
	else
		DX::Draw(m_pBatch, sphere, m_LightDebugColor);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG


HRESULT CLight::Compute_ViewProjMatrix()
{
	if (LIGHT_TYPE::POINT == m_eType)
		return E_FAIL;

	if (LIGHT_TYPE::DIRECTOINAL == m_eType)
	{
		_vector vLightDir = XMVector3Normalize(XMLoadFloat3(&m_tLightConstData.vDirection));
		_vector vEye = vLightDir * -1.0f * 70.f;
		XMStoreFloat3(&m_tLightConstData.vPosition, vEye);
		_vector vAt = vEye + vLightDir;
		_vector vWorldUp = { 0.0f, 1.0f, 0.0f, 0.0f };

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixLookAtLH(XMVectorSetW(XMLoadFloat3(&m_tLightConstData.vPosition), 1.0f), vAt, vWorldUp));

		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovy), SHADOWMAP_X / SHADOWMAP_Y, m_vNearFarPlane.x, m_vNearFarPlane.y));

		m_tLightConstData.LightViewProjMatrix = XMLoadFloat4x4(&m_ViewMatrix) * XMLoadFloat4x4(&m_ProjMatrix);

	}

	if (LIGHT_TYPE::SPOT == m_eType)
	{
		_vector vLightDir = XMVector3Normalize(XMLoadFloat3(&m_tLightConstData.vDirection));
		_vector vEye = XMVectorSetW(XMLoadFloat3(&m_tLightConstData.vPosition), 1.0f);
		_vector vAt = vEye + vLightDir;
		_vector vWorldUp = { 0.0f, 1.0f, 0.0f, 0.0f };

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixLookAtLH(vEye, vAt, vWorldUp));

		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovy), SHADOWMAP_X / SHADOWMAP_Y, m_vNearFarPlane.x, m_vNearFarPlane.y));

		m_tLightConstData.LightViewProjMatrix = XMLoadFloat4x4(&m_ViewMatrix) * XMLoadFloat4x4(&m_ProjMatrix);

	}

	return S_OK;
}

void CLight::Set_Shadow(_bool _isShadow)
{
	if (true == _isShadow)
	{
		/* 1. Shadow를 켠 경우 */
		if (true == (_bool)m_tLightConstData.isShadow)
			/* 2. 만약 원래 true였던 경우, */
			return;
		m_tLightConstData.isShadow = _isShadow;

		if (nullptr == m_pShadowRenderTarget)
		{
			/* RenderTarget이 없는 경우, (첫 쉐도우 생성 요청.)*/

			/* 0. Shadow ID 부여 */
			m_iShadowLightID = s_iShadowLightID++;

			/* 1. RenderTarget을 만든다, DSV는 DSV_Shadow 사용한다(메인앱에서 생성했음). */
			_wstring strShadowRTTag = TEXT("Target_Shadow_");
			strShadowRTTag += to_wstring(m_iShadowLightID);
			m_pGameInstance->Add_RenderTarget(strShadowRTTag, (_uint)SHADOWMAP_X,(_uint)SHADOWMAP_Y, DXGI_FORMAT_R32_FLOAT, _float4(1.0f, 0.0f, 0.0f, 0.0f), &m_pShadowRenderTarget);
			/* 2. 자기 자신을 shadow rendergroup에 등록한다. */
		}
		else
		{
			/* RenderTarget이 이미 존재하는 경우 */
		}


		Compute_ViewProjMatrix();

		m_pGameInstance->Add_ShadowLight(this);
	}
	else
	{
		/* shdow를 끄라고 요청한 경우 */
		if (false == m_tLightConstData.isShadow)
			/* 이미 쉐도우가 없는 경우 */
			return;
		
		m_tLightConstData.isShadow = false;
		m_pGameInstance->Remove_ShadowLight(m_iShadowLightID);
	}

	Update_LightConstBuffer();
	
}

HRESULT CLight::Set_LightConstData_AndUpdateBuffer(const CONST_LIGHT& _LightConstData)
{
	m_tLightConstData = _LightConstData;

	if (FAILED(Update_LightConstBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLight::Update_LightConstBuffer()
{
	if (FAILED(m_pGameInstance->UpdateConstBuffer(m_tLightConstData, m_pLightConstbuffer)))
		return E_FAIL;

	return S_OK;
}


void CLight::Active_OnEnable()
{
}

void CLight::Active_OnDisable()
{
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

	if (true == m_tLightConstData.isShadow)
	{
		_wstring strShadowRTTag = TEXT("Target_Shadow_");
		strShadowRTTag += to_wstring(m_iShadowLightID);
		m_pGameInstance->Erase_RenderTarget(strShadowRTTag);
		Safe_Release(m_pShadowRenderTarget);

	}


	Safe_Release(m_pLightConstbuffer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
