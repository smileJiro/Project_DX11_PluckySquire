#include "DebugDraw_For_Client.h"
#include "DebugDraw.h"
#include "GameInstance.h"

CDebugDraw_For_Client::CDebugDraw_For_Client(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CDebugDraw_For_Client::CDebugDraw_For_Client(const CDebugDraw_For_Client& _Prototype)
	: CComponent(_Prototype)
{
#ifdef _DEBUG
	m_pInputLayout = _Prototype.m_pInputLayout;
	m_pBatch = _Prototype.m_pBatch;
	m_pEffect = _Prototype.m_pEffect;
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CDebugDraw_For_Client::Initialize_Prototype()
{
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

HRESULT CDebugDraw_For_Client::Initialize(void* _pArg)
{
	return S_OK;
}

#ifdef _DEBUG

HRESULT CDebugDraw_For_Client::Render_Frustum(BoundingFrustum& _Frustum, _float4& _vDebugColor)
{
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

	DX::Draw(m_pBatch, _Frustum, XMLoadFloat4(&_vDebugColor));

	m_pBatch->End();

	return S_OK;
}
HRESULT CDebugDraw_For_Client::Render_Ring(_fvector _vPosition, _fvector _vMajorAxis, _fvector _vMinorAxis, _gvector _vDebugColor)
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::DrawRing(m_pBatch, _vPosition, _vMajorAxis, _vMinorAxis, _vDebugColor);

	m_pBatch->End();

	return S_OK;
}
#endif


CDebugDraw_For_Client* CDebugDraw_For_Client::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDebugDraw_For_Client* pInstance = new CDebugDraw_For_Client(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDebugDraw_For_Client");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDebugDraw_For_Client* CDebugDraw_For_Client::Clone(void* _pArg)
{
	CDebugDraw_For_Client* pInstance = new CDebugDraw_For_Client(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CDebugDraw_For_Client");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebugDraw_For_Client::Free()
{
#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif

	__super::Free();
}
