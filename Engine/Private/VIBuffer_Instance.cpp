#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& _Prototype)
	: CVIBuffer(_Prototype)
	, m_InstanceBufferDesc(_Prototype.m_InstanceBufferDesc)
	, m_InstanceInitialDesc(_Prototype.m_InstanceInitialDesc)
	, m_iNumInstances(_Prototype.m_iNumInstances)
	, m_iNumIndexCountPerInstance(_Prototype.m_iNumIndexCountPerInstance)
	, m_iInstanceStride(_Prototype.m_iInstanceStride)
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* _pArg)
{
	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Render()
{
	m_pContext->DrawIndexedInstanced(m_iNumIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_BufferDesc()
{
	ID3D11Buffer* pVertexBuffer[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	// 시작 슬롯, Vertex Buffer의 개수, Vertex Buffer 배열 주소, 각 Vertex Buffer byte 크기 주소, 오프셋
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
	// Index Buffer, Index Buffer의 크기, 오프셋
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	// 형상 (삼각형, 점,,,)
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}


void CVIBuffer_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
