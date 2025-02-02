#include "VIBuffer_PxDebug.h"
#include "GameInstance.h"

CVIBuffer_PxDebug::CVIBuffer_PxDebug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_PxDebug::CVIBuffer_PxDebug(const CVIBuffer_PxDebug& _Prototype)
	:CVIBuffer(_Prototype)
{
}

HRESULT CVIBuffer_PxDebug::Initialize_Prototype(_uint iNumLines)
{
	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXPOSCOLOR);
	m_iNumVertices = iNumLines * 2;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXPOSCOLOR* pVertices = new VTXPOSCOLOR[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSCOLOR) * m_iNumVertices);

	for (_uint i = 0; i < iNumLines; ++i)
	{
		/* Start Point */
		pVertices[i * 2].vPosition = XMFLOAT3(0.0f, 0.0f, 0.f);
		pVertices[i * 2].vColor = _float4(0.0f, 1.0f, 0.0f, 1.0f);

		/* End Point */
		pVertices[i * 2 + 1].vPosition = XMFLOAT3(0.0f, 0.0f, 0.f);
		pVertices[i * 2 + 1].vColor = _float4(0.0f, 1.0f, 0.0f, 1.0f);
	}


	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_PxDebug::Initialize(void* _pArg)
{
	
	return S_OK;
}

HRESULT CVIBuffer_PxDebug::Render()
{
	m_pContext->Draw(m_iNumRenderLines * 2, 0);
	return S_OK;
}

void CVIBuffer_PxDebug::Update_PxDebug(const PxRenderBuffer& _PxRenderBuffer)
{
	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	VTXPOSCOLOR* pVertices = static_cast<VTXPOSCOLOR*>(SubResource.pData);


	const PxDebugLine* pLines =  _PxRenderBuffer.getLines();
	m_iNumRenderLines = _PxRenderBuffer.getNbLines();
	for (_uint i = 0; i < m_iNumRenderLines; ++i)
	{
		if (m_iNumVertices / 2 <= i)
			break;

		/* Start Point */
		pVertices[i * 2].vPosition = _float3(pLines[i].pos0.x, pLines[i].pos0.y, pLines[i].pos0.z);
		pVertices[i * 2].vColor.x = ((pLines[i].color0 >> 16) & 0xFF) / 255.0f; // R
		pVertices[i * 2].vColor.y = ((pLines[i].color0 >> 8) & 0xFF) / 255.0f; // G
		pVertices[i * 2].vColor.z = ((pLines[i].color0 >> 0) & 0xFF) / 255.0f; // B
		pVertices[i * 2].vColor.w = 1.0f; // ((pLines[i].color0 >> 24) & 0xFF) / 255.0f; // A

		/* End Point */
		pVertices[i * 2 + 1].vPosition = _float3(pLines[i].pos1.x, pLines[i].pos1.y, pLines[i].pos1.z);
		pVertices[i * 2 + 1].vColor.x = ((pLines[i].color1 >> 16) & 0xFF) / 255.0f; // R
		pVertices[i * 2 + 1].vColor.y = ((pLines[i].color1 >> 8) & 0xFF) / 255.0f; // G
		pVertices[i * 2 + 1].vColor.z = ((pLines[i].color1 >> 0) & 0xFF) / 255.0f; // B
		pVertices[i * 2 + 1].vColor.w = 1.0f; // ((pLines[i].color0 >> 24) & 0xFF) / 255.0f; // A

	}
	m_pContext->Unmap(m_pVB, 0);
	
}

HRESULT CVIBuffer_PxDebug::Bind_BufferDesc()
{
	ID3D11Buffer* pVertexBuffers[] =
	{
		m_pVB,
	};
	_uint iVertexStrides[] =
	{
		m_iVertexStride,
	};
	_uint iVertexOffsets[] =
	{
		0,
	};

	/* VertexBuffer 만 바인딩 */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iVertexOffsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

CVIBuffer_PxDebug* CVIBuffer_PxDebug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iNumLines)
{
	CVIBuffer_PxDebug* pInstance = new CVIBuffer_PxDebug(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iNumLines)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_PxDebug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_PxDebug::Clone(void* _pArg)
{
	CVIBuffer_PxDebug* pInstance = new CVIBuffer_PxDebug(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_PxDebug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_PxDebug::Free()
{
	__super::Free();
}
