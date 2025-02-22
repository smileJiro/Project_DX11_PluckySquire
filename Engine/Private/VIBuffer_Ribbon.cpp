#include "VIBuffer_Ribbon.h"
#include "GameInstance.h"

CVIBuffer_Ribbon::CVIBuffer_Ribbon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Ribbon::CVIBuffer_Ribbon(const CVIBuffer_Ribbon& _Prototype)
	: CVIBuffer(_Prototype)
{
	if (nullptr != _Prototype.m_pVertices)
	{
		m_pVertices = new VTXPOS[m_iNumVertices];

		memcpy(m_pVertices, _Prototype.m_pVertices, sizeof(VTXPOS) * m_iNumVertices);
	}
}

// 기본 Beam형태, 정점은 오로지 2개만 존재합니다.
HRESULT CVIBuffer_Ribbon::Initialize_Prototype()
{
	// 멤버 변수에 값 저장. >>> 추후 IA에 전송할 데이터들을 미리 저장 해두는 것.
	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertices = 2;

	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = m_iNumVertices;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT; // 16비트 정수형 타입
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

#pragma region Vertices
	m_pVertices = new VTXPOS[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);

#pragma endregion Vertices

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0; // 0으로 값을 채워도 상관 없는 이유 : 추후 IndexBuffer 데이터를 몇바이트 단위로 읽을 지에 대한 값은 따로 설정함.
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	// SubResourceData 채우기 //

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
	_ushort iIndex = 0;


	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = (_ushort)i;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Ribbon::Initialize(void* _pArg)
{
#pragma region VERTEX_BUFFER

	// 1. BufferDesc 값을 채운다.
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;


	// 2. SubResourceData 값을 채운다.
	VTXTRAIL* pVertices = new VTXTRAIL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTRAIL) * m_iNumVertices);


	ZeroMemory(&m_SubResourceDesc, sizeof(m_SubResourceDesc));
	m_SubResourceDesc.pSysMem = pVertices; // Buffer 생성시에는 pSysMem 만 사용한다.

	// 3. Buffer를 생성한다.
	// m_pVertices는 지워주지 않음.
	if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	// 4. Buffer -> SRV 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.ElementWidth = m_iNumVertices;

	//if (FAILED(m_pDevice->CreateShaderResourceView(m_pVB, &SRVDesc, &m_pBufferSRV)))
	//	return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Ribbon::Bind_BufferDesc()
{
	return E_NOTIMPL;
}

HRESULT CVIBuffer_Ribbon::Render()
{
	return E_NOTIMPL;
}

CVIBuffer_Ribbon* CVIBuffer_Ribbon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CVIBuffer_Ribbon* pInstance = new CVIBuffer_Ribbon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Ribbon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Ribbon::Clone(void* _pArg)
{
	CVIBuffer_Ribbon* pInstance = new CVIBuffer_Ribbon(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Ribbon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Ribbon::Free()
{
}
