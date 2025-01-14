#include "VIBuffer_Particle_Rect.h"
#include "GameInstance.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer_Particle_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& _Prototype)
    : CVIBuffer_Particle_Instance(_Prototype)
{
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(const PARTICLE_DESC& _ParticleDesc)
{
    /* 기본적인 VertexBuffer, IndexBuffer를 Create하고, InstancingBuffer에 대한 정보들을 세팅한다 >> InstancingBufferCreate는 사본객체 생성 시 별도로 한다. */
    m_iNumVertexBuffers = 2; // Vertex Buffer 개수 
    m_iNumInstances = _ParticleDesc.iNumInstances; // Instance 개수 

    /* 그리기위한 기본 VetexBuffer 정보 */
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iNumVertices = 4;

    /* 그리기 위한 기본 IndexBuffer 정보 */
    m_iIndexStride = sizeof(_ushort);
    m_iNumIndices = 6 * m_iNumInstances; // 인덱스 6개당 사각형 하나이다. >>> 여기에 Instance 개수만큼 곱하여 할당. 
    m_eIndexFormat = DXGI_FORMAT_R16_UINT; // 2바이트 _uint
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

    /* 정점버퍼의 기본 정보를 세팅 */
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = m_iVertexStride;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;

    /* 정점버퍼의 Position, Texcoord 정보 세팅. */
    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    pVertices[0].vPosition = XMFLOAT3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = XMFLOAT2(0.f, 0.f);

    pVertices[1].vPosition = XMFLOAT3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = XMFLOAT2(1.f, 0.f);

    pVertices[2].vPosition = XMFLOAT3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = XMFLOAT2(1.f, 1.f);

    pVertices[3].vPosition = XMFLOAT3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = XMFLOAT2(0.f, 1.f);

    /* 곧바로 Create Buffer */
    ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
    m_SubResourceDesc.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion // VERTEX_BUFFER

#pragma region INDEX_BUFFER
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    /* 인덱스버퍼의 기본 정보를 세팅 */
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;

    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    /* Instance의 개수만큼 Loop 돌며, IndexBuffer를 할당한다. */
    _uint			iNumIndices = { 0 };
    for (_uint i = 0; i < m_iNumInstances; ++i)
    {
        pIndices[iNumIndices++] = 0;
        pIndices[iNumIndices++] = 1;
        pIndices[iNumIndices++] = 2;

        pIndices[iNumIndices++] = 0;
        pIndices[iNumIndices++] = 2;
        pIndices[iNumIndices++] = 3;
    }
    /* 곧바로 인덱스 버퍼 Create. */
    ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceDesc.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion // INDEX_BUFFER


#pragma region INSTANCING_BUFFER
    m_iInstanceStride = sizeof(VTXINSTANCE); // 행렬 정보를 담고있는 구조체 
    m_iNumIndicesPerInstance = 6; // 인덱스 6개당 1개의 인스턴스 버퍼 데이터를 할당한다는 의미. 

    ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
    /* 인스턴싱 버퍼 기본 정보 세팅 */
    m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances; // 구조체 크기 * 인스턴스 개수
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;  // 타입은 VertexBuffer로 한다. dx는 VertexBuffer여러개를 셰이더에 바인딩 가능. 
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // Dynamic으로 설정하여, Instancing Buffer에 있는 vRight, vUp, vLook, vTranslation을 조작하여 파티클들을 움직이게 할 것임. 
    m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride; // InstanceBufferStride 
    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Runtime에 조작하여 데이터를 변경할 의도가 있다. map, unmap 사용 >>> Write로 한다. 
    m_InstanceBufferDesc.MiscFlags = 0;

    m_pInstanceVertices = new VTXINSTANCE[m_iNumInstances];
    ZeroMemory(m_pInstanceVertices, m_iInstanceStride * m_iNumInstances);

    m_isLoop = _ParticleDesc.isLoop;              // Particle Effect Data
    m_vPivot = _ParticleDesc.vPivot;              // Particle Effect Data
    m_pSpeeds = new _float[m_iNumInstances];      // Particle Effect Data

    for (_uint i = 0; i < m_iNumInstances; ++i)
    {
        m_pSpeeds[i] = m_pGameInstance->Compute_Random(_ParticleDesc.vSpeed.x, _ParticleDesc.vSpeed.y);

        /* 초기 값 세팅 : 일단은 크기가 10인 정육면체 안에 랜덤하게 할당되게 해보자. */
        _float		fSize = m_pGameInstance->Compute_Random(_ParticleDesc.vSize.x, _ParticleDesc.vSize.y);
        m_pInstanceVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
        m_pInstanceVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
        m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
        /* 범위 내의 랜덤한 값으로 Posis*/
        m_pInstanceVertices[i].vTranslation = _float4(
            m_pGameInstance->Compute_Random(_ParticleDesc.vCenter.x - _ParticleDesc.vRange.x * 0.5f, _ParticleDesc.vCenter.x + _ParticleDesc.vRange.x * 0.5f),
            m_pGameInstance->Compute_Random(_ParticleDesc.vCenter.y - _ParticleDesc.vRange.y * 0.5f, _ParticleDesc.vCenter.y + _ParticleDesc.vRange.y * 0.5f),
            m_pGameInstance->Compute_Random(_ParticleDesc.vCenter.z - _ParticleDesc.vRange.z * 0.5f, _ParticleDesc.vCenter.z + _ParticleDesc.vRange.z * 0.5f),
            1.f);


        /* 범위 내의 랜덤한 값으로 생존 시간 설정. */
        m_pInstanceVertices[i].vLifeTime = _float2(
            m_pGameInstance->Compute_Random(_ParticleDesc.vLifeTime.x, _ParticleDesc.vLifeTime.y),
            0.f);
    }

    /* 곧바로 Instancing Buffer를 할당하는 것이 아니라, Create 하기위한 데이터를 미리 세팅 후 해당 데이터를 얕은복사. 사본 객체 생성 시 Create */
    ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
    m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion // INSTANCING_BUFFER


    return S_OK;
}

HRESULT CVIBuffer_Particle_Rect::Initialize(void* _pArg)
{
    /* 부모의 Initialze에서 Instancing Buffer Create */
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    return S_OK;
}

CVIBuffer_Particle_Rect* CVIBuffer_Particle_Rect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const PARTICLE_DESC& _ParticleDesc)
{
    CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_ParticleDesc)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Particle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Particle_Rect::Clone(void* _pArg)
{
    CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Particle_Rect::Free()
{


    __super::Free();
}
