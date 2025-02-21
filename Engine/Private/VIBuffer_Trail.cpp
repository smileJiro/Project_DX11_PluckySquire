#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& _Prototype)
	: CVIBuffer(_Prototype)
	, m_iTriangleCount(_Prototype.m_iTriangleCount)
{
	m_pVertices = new VTXPOSTEX[m_iNumVertices];

	memcpy(m_pVertices, _Prototype.m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);
}


HRESULT CVIBuffer_Trail::Initialize_Prototype(_uint _iMaxVertexCount)
{
	if (4 > _iMaxVertexCount)
		return E_FAIL;

	// 멤버 변수에 값 저장. >>> 추후 IA에 전송할 데이터들을 미리 저장 해두는 것.
	m_iNumVertexBuffers = 1;

	// TODO: VTXPOSTEX가 맞을까 ? 
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumVertices = _iMaxVertexCount;

	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = _iMaxVertexCount;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT; // 16비트 정수형 타입
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	// trail 총 삼각형 개수
	m_iTriangleCount = m_iNumVertices - 2;

#pragma region VERTEX_BUFFER

	// 1. BufferDesc 값을 채운다.
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;


	// 2. SubResourceData 값을 채운다.
	m_pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	// Test
	m_pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	//m_pVertices[0].vTexcoord = _float2(0.f)

	ZeroMemory(&m_SubResourceDesc, sizeof(m_SubResourceDesc));
	m_SubResourceDesc.pSysMem = m_pVertices; // Buffer 생성시에는 pSysMem 만 사용한다.

	// 3. Buffer를 생성한다.
	if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
		return E_FAIL;
	
	// m_pVertices는 지워주지 않음.

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0; // 0으로 값을 채워도 상관 없는 이유 : 추후 IndexBuffer 데이터를 몇바이트 단위로 읽을 지에 대한 값은 따로 설정함.
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	// SubResourceData 채우기 //

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = i;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* _pArg)
{
	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iMaxVertexCount)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iMaxVertexCount)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* _pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	Safe_Delete_Array(m_pVertices);

	__super::Free();
}
