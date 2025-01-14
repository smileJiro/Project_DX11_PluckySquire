#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer_Particle_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& _Prototype)
    : CVIBuffer_Particle_Instance(_Prototype)
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const PARTICLE_DESC& _ParticleDesc)
{
	m_iNumVertexBuffers = 2;
	m_iNumInstances = _ParticleDesc.iNumInstances;


	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1; /* 정점의 개수는 1개이겠지. */

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = m_iNumInstances; /* 인덱스 개수는 인스턴스 수 만큼.*/
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; /* Point List */

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	/* 정점이 1개라 */
	pVertices->vPosition = XMFLOAT3(0.0f, 0.0f, 0.f);
	pVertices->vSize = XMFLOAT2(1.f, 1.f);

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion // VERTEX_BUFFER


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices); /* 어짜피 0번 인덱스 정점만 사용하니까 ZeroMemmory해서 한번에 채우자. */


	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion // INDEX_BUFFER 

#pragma region INSTANCE_BUFFER

	m_iInstanceStride = sizeof(VTXINSTANCE);
	m_iNumIndicesPerInstance = 6; /* 동일하게 정점 6개당 1개의 Instance Buffer 할당. */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTXINSTANCE[m_iNumInstances];
	ZeroMemory(m_pInstanceVertices, m_iInstanceStride * m_iNumInstances);

	m_isLoop = _ParticleDesc.isLoop;
	m_pSpeeds = new _float[m_iNumInstances];
	m_vPivot = _ParticleDesc.vPivot;

	for (_uint i = 0; i < m_iNumInstances; i++)
	{
		m_pSpeeds[i] = m_pGameInstance->Compute_Random(_ParticleDesc.vSpeed.x, _ParticleDesc.vSpeed.y);

		_float		fSize = m_pGameInstance->Compute_Random(_ParticleDesc.vSize.x, _ParticleDesc.vSize.y);
		
		m_pInstanceVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
		m_pInstanceVertices[i].vTranslation =
			_float4(
				m_pGameInstance->Compute_Random(_ParticleDesc.vCenter.x - _ParticleDesc.vRange.x * 0.5f, _ParticleDesc.vCenter.x + _ParticleDesc.vRange.x * 0.5f),
				m_pGameInstance->Compute_Random(_ParticleDesc.vCenter.y - _ParticleDesc.vRange.y * 0.5f, _ParticleDesc.vCenter.y + _ParticleDesc.vRange.y * 0.5f),
				m_pGameInstance->Compute_Random(_ParticleDesc.vCenter.z - _ParticleDesc.vRange.z * 0.5f, _ParticleDesc.vCenter.z + _ParticleDesc.vRange.z * 0.5f),
				1.f);

		m_pInstanceVertices[i].vLifeTime = _float2(
			m_pGameInstance->Compute_Random(_ParticleDesc.vLifeTime.x, _ParticleDesc.vLifeTime.y),
			0.f
		);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion //INSTANCE_BUFFER


	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* _pArg)
{
	/* Prototype 기반 데이터로 Instance Buffer 생성. */
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const PARTICLE_DESC& _ParticleDesc)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_ParticleDesc)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Particle_Point::Clone(void* _pArg)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{

	__super::Free();
}
