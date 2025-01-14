#include "VIBuffer_Particle_Instance.h"

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(ID3D11Device* _pDeivce, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDeivce, _pContext)
{
}

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& _Prototype)
    : CVIBuffer(_Prototype)
    , m_pVBInstance{ _Prototype.m_pVBInstance }
    , m_InstanceBufferDesc{ _Prototype.m_InstanceBufferDesc }
    , m_InstanceInitialDesc{ _Prototype.m_InstanceInitialDesc }
    , m_iNumInstances{ _Prototype.m_iNumInstances }
    , m_iNumIndicesPerInstance{ _Prototype.m_iNumIndicesPerInstance }
    , m_iInstanceStride{ _Prototype.m_iInstanceStride }

{
    Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Particle_Instance::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Initialize(void* _pArg)
{
    m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance);

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Render()
{
    /* Instancing용 그리기 함수 */
    m_pContext->DrawIndexedInstanced(m_iNumIndicesPerInstance, m_iNumInstances, 0, 0, 0);
    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Bind_BufferDesc()
{
    ID3D11Buffer* pVertexBuffers[] = {
            m_pVB,
            m_pVBInstance,
    };

    _uint					iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceStride,

    };

    _uint					iOffsets[] = {
        0,
        0,
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}


void CVIBuffer_Particle_Instance::Free()
{
    Safe_Release(m_pVBInstance);

    __super::Free();
}
