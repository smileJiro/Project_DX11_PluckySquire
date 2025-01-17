#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent(_pDevice, _pContext)
    , m_pVB(nullptr)
    , m_pIB(nullptr)
    , m_BufferDesc{}
    , m_SubResourceDesc{}
    , m_iVertexStride(0)
    , m_iNumVertices(0)
    , m_iNumVertexBuffers(0)
    , m_iIndexStride(0)
    , m_iNumIndices(0)
    , m_vecVerticesPos{}
    , m_vecIndexBuffer{}
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& _Prototype)
    : CComponent(_Prototype)
    , m_pVB(_Prototype.m_pVB)
    , m_pIB(_Prototype.m_pIB)
    , m_BufferDesc{ _Prototype.m_BufferDesc }
    , m_SubResourceDesc{ _Prototype.m_SubResourceDesc }
    , m_iVertexStride(_Prototype.m_iVertexStride)
    , m_iNumVertices(_Prototype.m_iNumVertices)
    , m_iNumVertexBuffers(_Prototype.m_iNumVertexBuffers)
    , m_iIndexStride(_Prototype.m_iIndexStride)
    , m_iNumIndices(_Prototype.m_iNumIndices)
    , m_eIndexFormat(_Prototype.m_eIndexFormat)
    , m_ePrimitiveTopology(_Prototype.m_ePrimitiveTopology)
    , m_vecVerticesPos{ _Prototype .m_vecVerticesPos }
    , m_vecIndexBuffer{ _Prototype .m_vecIndexBuffer}
{
    Safe_AddRef(m_pVB);
    Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer::Bind_BufferDesc()
{
    // 버퍼에 대한 정보를 여기서 장치에 세팅 
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

    // IA에 VertexBuffer 전송

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iVertexOffsets);
    // IA에 IndexBuffer 전송
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    // IA에 PrimitiveTopology 전송
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** _ppOut)
{
    m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceDesc, _ppOut);
    return S_OK;
}

void CVIBuffer::Free()
{
    __super::Free();

    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
}
