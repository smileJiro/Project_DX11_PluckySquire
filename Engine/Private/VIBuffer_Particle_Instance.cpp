#include "VIBuffer_Particle_Instance.h"

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(ID3D11Device* _pDeivce, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDeivce, _pContext)
{
}

CVIBuffer_Particle_Instance::CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& _Prototype)
    : CVIBuffer(_Prototype)
    , m_pVBInstance{ _Prototype.m_pVBInstance } /* 얕은 복사 하긴하지만 코드의 통일성 때문인 실제 데이터는 사본 객체 생성 후 할당되기때문에 nullptr 얕은복사임. */
    , m_InstanceBufferDesc{ _Prototype.m_InstanceBufferDesc }
    , m_InstanceInitialDesc{ _Prototype.m_InstanceInitialDesc }
    , m_iNumInstances{ _Prototype.m_iNumInstances }
    , m_iNumIndicesPerInstance{ _Prototype.m_iNumIndicesPerInstance }
    , m_iInstanceStride{ _Prototype.m_iInstanceStride }
    , m_pInstanceVertices{ _Prototype.m_pInstanceVertices }
    , m_pSpeeds{ _Prototype.m_pSpeeds }
    , m_isLoop{ _Prototype.m_isLoop }
    , m_vPivot{ _Prototype.m_vPivot }
{
    Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Particle_Instance::Initialize_Prototype()
{
    /* 원형 객체 생성 시에는 해당 클래스를 상속 받은 클래스들 각각 자기 자신의 VertexBuffer + IndexBuffer를 생성하고, InstancingBuffer 를 생성하기 위한 초기 작업을 수행함. */
    return S_OK;
}

HRESULT CVIBuffer_Particle_Instance::Initialize(void* _pArg)
{
    /* 사본 객체 생성 시에, 원형 객체에 저장되어있던 Instancing Buffer 관련 데이터를 기반으로 실제 CreateBuffer()를 수행한다. (Instancing Buffer만 이시점에 Create한다. ) */
    /* 원형 객체 생성 시에 InstancingBuffer까지 Create 해두게되면, 해당 원형객체를 얕은복사 받은 모든 객체들이 Instancing Buffer를 공유하게 됌. 다 똑같이 움직인다는 것임. */
    /* 그렇다고 원형 객체 생성 시에 Instancing Buffer에 대한 작업을 아무것도 안하면, 매번 사본객체 생성 시마다 세팅해주고 준비해줘야하는 과정이 너무 많아, 중복 작업 */
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

HRESULT CVIBuffer_Particle_Instance::Create_InstanceBuffer()
{
    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

_bool CVIBuffer_Particle_Instance::Drop(_float _fTimeDelta)
{
    /* 눈처럼 입자들이 랜덤한 속도, 랜덤한 크기로 떨어지는 */
    D3D11_MAPPED_SUBRESOURCE SubResource = {};

    /* Prototype에서 미리 저장해둔 Instance의 초기값 , 0, 쓰기모드지만, 기존에 데이터가 있었더라도 덮어쓰지 않기에 이전 데이터를 확인가능하다. */
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXINSTANCE* pVertices = static_cast<VTXINSTANCE*>(SubResource.pData);

    /* 모든 Instance들에 대해 순회하며 각각의 속도 만큼 떨어트린다. */
    for (size_t i = 0; i < m_iNumInstances; ++i)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * _fTimeDelta;
        pVertices[i].vLifeTime.y += _fTimeDelta; /* y : TimeAcc*/

        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            /* 지정한 생존 시간이 지났다면, */
            if (true == m_isLoop)
            {
                /* Loop Paticle 이라면, */
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.0f;
            }
            else
            {
                /* 만약 루프객체가 아니라면, */
                // 끝났다는 알림
                return true;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);

    return false;
}

_bool CVIBuffer_Particle_Instance::Spread(_float _fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource = {};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXINSTANCE* pVertices = static_cast<VTXINSTANCE*>(SubResource.pData);
    _int isFinishedCount = 0;
    for (size_t i = 0; i < m_iNumInstances; ++i)
    {
        /* 현재 Paticle들의 위치와, Pibot을 기준으로 방향벡터를 구한다. */
        _vector vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.0f));
        
        //// 간단한 회전상태 정의.
        //XMStoreFloat4(&pVertices[i].vUp, vDir * 0.1f);
        //pVertices[i].vRight.x = XMVectorGetY(vDir * 0.1f);
        //pVertices[i].vRight.y = XMVectorGetX(vDir * 0.1f) * -1.0f;
        //pVertices[i].vRight.z = XMVectorGetZ(vDir * 0.1f);
        //pVertices[i].vRight.w = 0.f;

        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vDir * m_pSpeeds[i] * _fTimeDelta);

        pVertices[i].vLifeTime.y += _fTimeDelta; /* y : TimeAcc*/

        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            /* 지정한 생존 시간이 지났다면, */
            if (true == m_isLoop)
            {
                /* Loop Paticle 이라면, */
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.0f;
            }
            else
            {
                /* 만약 루프객체가 아니라면, */
                // 끝났다는 알림
                ++isFinishedCount;

            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);

    if (isFinishedCount == m_iNumInstances)
    {
        return true;
    }
        

    return false;
}

_bool CVIBuffer_Particle_Instance::SpreadAndDrop(_float _fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource = {};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXINSTANCE* pVertices = static_cast<VTXINSTANCE*>(SubResource.pData);
    _int isFinishedCount = 0;
    for (size_t i = 0; i < m_iNumInstances; ++i)
    {


        pVertices[i].vLifeTime.y += _fTimeDelta; /* y : TimeAcc*/

        _float fTimeRatio = pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x;
        if (0.12f >= fTimeRatio)
        {
            /* Spread */
            _vector vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.0f));
            XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vDir * m_pSpeeds[i] * _fTimeDelta);
            //pVertices[i].vTranslation.y -= m_pSpeeds[i] * 0.5f * _fTimeDelta;
        }
        else if (0.45f >= fTimeRatio)
        {

        }
        else if(0.6f >= fTimeRatio)
        {
            /* Drop */
            pVertices[i].vTranslation.y -= m_pSpeeds[i] * 1.5f * _fTimeDelta;
            
        }
        else if (0.65f >= fTimeRatio)
        {
            
            /* Spread */
           _vector vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.0f));
           XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vDir * m_pSpeeds[i] * _fTimeDelta * 0.3f);
            /* Drop */
            pVertices[i].vTranslation.y -= m_pSpeeds[i] * -0.7f * _fTimeDelta;


        }
        else if (0.9f >= fTimeRatio)
        {
            /* Drop */
            pVertices[i].vTranslation.y -= m_pSpeeds[i] * 1.0f * _fTimeDelta;

        }
        //else if (0.72f >= fTimeRatio)
        //{
        //    /* Drop */
        //    pVertices[i].vTranslation.y -= m_pSpeeds[i] * -0.2f * _fTimeDelta;
        //
        //}
        //else if (1.0f >= fTimeRatio)
        //{
        //    /* Drop */
        //    pVertices[i].vTranslation.y -= m_pSpeeds[i] * 0.5f * _fTimeDelta;
        //
        //}


        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            /* 지정한 생존 시간이 지났다면, */
            if (true == m_isLoop)
            {
                /* Loop Paticle 이라면, */
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.0f;
            }
            else
            {
                /* 만약 루프객체가 아니라면, */
                // 끝났다는 알림
                ++isFinishedCount;

            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);

    if (isFinishedCount == m_iNumInstances)
    {
        return true;
    }


    return false;
}

void CVIBuffer_Particle_Instance::Start_Particle()
{
    D3D11_MAPPED_SUBRESOURCE SubResource = {};
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXINSTANCE* pVertices = static_cast<VTXINSTANCE*>(SubResource.pData);
    _int isFinishedCount = 0;
    for (size_t i = 0; i < m_iNumInstances; ++i)
    {
        pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
        pVertices[i].vLifeTime.y = 0.0f;
    }

    m_pContext->Unmap(m_pVBInstance, 0);

}

void CVIBuffer_Particle_Instance::Free()
{
    /* 사본 객체가 아닐때 (원형 객체 일때만)*/
    if (false == m_isCloned)
    {
        Safe_Delete_Array(m_pSpeeds);
        Safe_Delete_Array(m_pInstanceVertices); /* 행렬 정보를 저장하던 Instancing Buffer 해제 */
    }


    Safe_Release(m_pVBInstance);

    __super::Free();
}
