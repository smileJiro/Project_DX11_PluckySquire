#include "stdafx.h"
#include "VIBuffer_CellLine.h"

CVIBuffer_CellLine::CVIBuffer_CellLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_CellLine::CVIBuffer_CellLine(const CVIBuffer_CellLine& Prototype)
	: CVIBuffer(Prototype)
{
}

HRESULT CVIBuffer_CellLine::Initialize_Prototype(const _float3* pPoints)
{
	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertices = 12;

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = 18;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

#pragma region VERTEX_BUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
		pVertices[i].vPosition = pPoints[i];

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(CVIBuffer::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth =m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);


	for (_uint i = 0; i < 3; i++)
	{
		for (_uint j = 0; j < 3; j++)
		{
			pIndices[(i * 6) + (j * 2 )] = i * 4;
			pIndices[(i * 6) + (j * 2 ) +1] = i * 4 + 1 + j;
		}
	}

	//pIndices[1] = 1;
	//pIndices[2] = 2;
	//pIndices[3] = 3;
	//pIndices[4] = 4;
	//pIndices[5] = 5;
	//pIndices[6] = 6;

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_CellLine::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_CellLine* CVIBuffer_CellLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
	CVIBuffer_CellLine* pInstance = new CVIBuffer_CellLine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_CellLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_CellLine::Clone(void* pArg)
{
	CVIBuffer_CellLine* pInstance = new CVIBuffer_CellLine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_CellLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_CellLine::Free()
{
	__super::Free();

}
