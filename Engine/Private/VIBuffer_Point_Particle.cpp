#include "VIBuffer_Point_Particle.h"
#include "GameInstance.h"

CVIBuffer_Point_Particle::CVIBuffer_Point_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Point_Particle::CVIBuffer_Point_Particle(const CVIBuffer_Point_Particle& _Prototype)
    : CVIBuffer_Instance(_Prototype)
    , m_pInstanceVertices(_Prototype.m_pInstanceVertices)
{
}

HRESULT CVIBuffer_Point_Particle::Initialize_Prototype(const json& _jsonBufferInfo)
{
	m_iNumVertexBuffers = 2;
	m_iNumInstances = _jsonBufferInfo["Count"];

	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = m_iNumInstances;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices->vPosition = XMFLOAT3(0.0f, 0.0f, 0.f);
	pVertices->vSize = XMFLOAT2(1.f, 1.f);

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);


	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
	m_iNumIndexCountPerInstance = 6;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstances];
	ZeroMemory(m_pInstanceVertices, m_iInstanceStride * m_iNumInstances);


	for (size_t i = 0; i < m_iNumInstances; i++)
	{

		m_pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[i].vTranslation =
			//_float4(m_pGameInstance->Compute_Random(0.5f, 1.f), m_pGameInstance->Compute_Random(0.5f, 1.f), 0.f, 1.f);
			_float4(i * 0.5f, i * 0.5f, 0.f, 1.f);

		m_pInstanceVertices[i].vLifeTime = _float2(
			1.f,
			0.f
		);

		m_pInstanceVertices[i].vUV = _float4(0.f, 0.f, 1.f, 1.f);
		//if (0 == i % 3)
		//	m_pInstanceVertices[i].vColor = _float4(0.f, 0.f, 1.f, 0.9f);
		//if (1 == i % 3)
		//	m_pInstanceVertices[i].vColor = _float4(0.f, 1.f, 0.f, 0.9f);
		//if (2 == i % 3)
		//	m_pInstanceVertices[i].vColor = _float4(1.f, 0.f, 0.f, 0.9f);

		
		//m_pInstanceVertices[i].vColor = _float4(m_pGameInstance->Compute_Random(0.5f, 1.f), m_pGameInstance->Compute_Random(0.5f, 1.f), m_pGameInstance->Compute_Random(0.5f, 1.f), 1.f);
		m_pInstanceVertices[i].vColor = _float4(1.f, 1.f, 1.f, 0.5f);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Particle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Point_Particle* CVIBuffer_Point_Particle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonBufferInfo)
{
	CVIBuffer_Point_Particle* pInstance = new CVIBuffer_Point_Particle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_jsonBufferInfo)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Particle::Clone(void* _pArg)
{
	CComponent* pInstance = new CVIBuffer_Point_Particle(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Particle::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);

	}
}
