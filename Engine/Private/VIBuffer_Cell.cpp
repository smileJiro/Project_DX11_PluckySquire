#include "VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& _Prototype)
	: CVIBuffer(_Prototype)
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* _pPoints)
{
	/* 정점 세개에 대한 Buffer를 구성해준다. */
	/* 사실상 Cell 전용 Buffer인데 이처럼 삼각형 1개당 1개의 버퍼를 가지는 구조는 그리 좋은 구조는 아니다. 그만큼 각각의 Draw Call이 빈번하게 호출되기 때문 */
	/* 하지만 Debug Mode에서만 사용 할 것이니까. */
	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertices = 3;

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = 4; /* 라인 스트립으로 그리기 때문에 0,1,2,0 >> 4개의 IndexBuffer가 필요하다. */
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; /* 라인 스트립을 사용하여 그린다. */

#pragma region VertexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	/* 매개변수로 들어온 Points[] 자체를 곧바로 Buffer의 Position으로 설정한다. */
	for (size_t i = 0; i < m_iNumVertices; i++)
		pVertices[i].vPosition = _pPoints[i];

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region IndexBuffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	/* 라인 스트립이라 IndexBuffer가 4개 */
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void* _pArg)
{
    return S_OK;
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pPoints)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* _pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{


    __super::Free();
}
