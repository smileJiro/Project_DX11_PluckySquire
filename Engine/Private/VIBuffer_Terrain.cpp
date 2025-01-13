#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& _Prototype)
    : CVIBuffer(_Prototype)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* _pArg)
{
    VIBUFFER_TERRAIN_DESC* pDesc = static_cast<VIBUFFER_TERRAIN_DESC*>(_pArg);
    m_fVerticesItv = pDesc->fVerticesItv;
#pragma region Height_Map_And_Data_Setting

    // Read Height Map
    _ulong dwByte = {};
    HANDLE hFile = CreateFile(pDesc->pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    BITMAPFILEHEADER fh{};
    BITMAPINFOHEADER ih{};
    _uint* pPixels = { nullptr };
    _bool bResult = false;
    bResult = ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
    if (!bResult)
    {
        MSG_BOX("HeightMap fh Read Failed");
        return E_FAIL;
    }
    bResult = ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);
    if (!bResult)
    {
        MSG_BOX("HeightMap ih Read Failed");
        return E_FAIL;
    }
    m_iNumVerticesX = ih.biWidth;
    m_iNumVerticesZ = ih.biHeight;

    // 해당 높이맵의 픽셀 수 만큼. 정점 개수 할당.
    pPixels = new _uint[m_iNumVerticesX * m_iNumVerticesZ];

    bResult = ReadFile(hFile, pPixels, sizeof(_uint) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);
    if (!bResult)
    {
        MSG_BOX("HeightMap Pixels Read Failed");
        return E_FAIL;
        Safe_Delete_Array(pPixels);
    }

    m_iNumVertexBuffers = 1;

    m_iVertexStride = sizeof(VTXNORTEX);
    m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

    m_iIndexStride = sizeof(_uint);
    m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 6;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma endregion // Height_Map_And_Data_Setting

#pragma region VertexBuffer
    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
    m_vecVerticesPos.clear();
    m_vecVerticesPos.reserve(m_iNumVertices);
    for (size_t i = 0; i < m_iNumVerticesX; ++i)
    {
        for (size_t j = 0; j < m_iNumVerticesZ; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = _float3(j * m_fVerticesItv, (pPixels[iIndex] & 0x000000ff) / 10.f, i * m_fVerticesItv);
            m_vecVerticesPos.push_back(pVertices[iIndex].vPosition); // Ray 피킹을 위한 포지션 정보 저장.
            pVertices[iIndex].vTexcoord = _float2((float)j / ((float)m_iNumVerticesX - 1.0f), -(float)i / ((float)m_iNumVerticesZ - 1.0f));

        }
    }

#pragma endregion // VertexBuffer

#pragma region IndexBuffer
    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
    m_vecIndexBuffer.clear();
    m_vecIndexBuffer.reserve(m_iNumIndices);
    _uint iIndicesCount = 0;

    // 여기서 반복문의 횟수는 해당 버텍스들로 이루어진 Rect의 수이다. 
    for (size_t i = 0; i < m_iNumVerticesX - 1; ++i)
    {
        for (size_t j = 0; j < m_iNumVerticesZ - 1; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j; 

            _uint iIndices[] =
            {
                iIndex + m_iNumVerticesX,
                iIndex + m_iNumVerticesX + 1,
                iIndex + 1,
                iIndex,
            };
            _vector vSrc, vDst, vNormal;

            pIndices[iIndicesCount++] = iIndices[0];
            pIndices[iIndicesCount++] = iIndices[1];
            pIndices[iIndicesCount++] = iIndices[2];
            m_vecIndexBuffer.push_back(iIndices[0]);
            m_vecIndexBuffer.push_back(iIndices[1]);
            m_vecIndexBuffer.push_back(iIndices[2]);


            vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

           
            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);


            pIndices[iIndicesCount++] = iIndices[0];
            pIndices[iIndicesCount++] = iIndices[2];
            pIndices[iIndicesCount++] = iIndices[3];
            m_vecIndexBuffer.push_back(iIndices[0]);
            m_vecIndexBuffer.push_back(iIndices[2]);
            m_vecIndexBuffer.push_back(iIndices[3]);

            vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));


            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
        }
    }
    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
    }



#pragma endregion // IndexBuffer
    /* VertexBuffer Create */
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    /* 정점버퍼를 몇 바이트 할당할까요? */
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = m_iVertexStride;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    // 다 채웠으면 해당 데이터를 기반으로 SubResourceDesc 채우고 이를 통해 Create Buffer
    ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
    m_SubResourceDesc.pSysMem = pVertices;
    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    /* IndexBuffer Create */
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    /* 인덱스버퍼를 몇 바이트 할당할까요? */
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceDesc.pSysMem = pIndices;
    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);
    Safe_Delete_Array(pPixels);
    
    return S_OK;

}

HRESULT CVIBuffer_Terrain::Remake_Buffer_Height(void* _pArg)
{
    if (nullptr != m_pVB)
        Safe_Release(m_pVB);
        
    if (nullptr != m_pIB)
        Safe_Release(m_pIB);

    VIBUFFER_TERRAIN_DESC* pDesc = static_cast<VIBUFFER_TERRAIN_DESC*>(_pArg);
    m_fVerticesItv = pDesc->fVerticesItv;
#pragma region Height_Map_And_Data_Setting

    // Read Height Map
    _ulong dwByte = {};
    HANDLE hFile = CreateFile(pDesc->pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    BITMAPFILEHEADER fh{};
    BITMAPINFOHEADER ih{};
    _uint* pPixels = { nullptr };
    _bool bResult = false;

    // 파일 포인터를시작위치로 : 한 함수내에서 같은파일을 중복하여 읽을 때 사용하는 함수.
    // SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);

    bResult = ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
    if (!bResult)
    {
        MSG_BOX("HeightMap fh Read Failed");
        return E_FAIL;
    }
    bResult = ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);
    if (!bResult)
    {
        MSG_BOX("HeightMap ih Read Failed");
        return E_FAIL;
    }
    m_iNumVerticesX = ih.biWidth;
    m_iNumVerticesZ = ih.biHeight;

    // 해당 높이맵의 픽셀 수 만큼. 정점 개수 할당.
    pPixels = new _uint[m_iNumVerticesX * m_iNumVerticesZ];

    bResult = ReadFile(hFile, pPixels, sizeof(_uint) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);
    if (!bResult)
    {
        MSG_BOX("HeightMap Pixels Read Failed");
        return E_FAIL;
        Safe_Delete_Array(pPixels);
    }

    // 핸들을 제대로 닫아야함. 중복해서 같은 파일을 호출 시 문제 발생.
    CloseHandle(hFile);

    m_iNumVertexBuffers = 1;

    m_iVertexStride = sizeof(VTXNORTEX);
    m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

    m_iIndexStride = sizeof(_uint);
    m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 6;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma endregion // Height_Map_And_Data_Setting

#pragma region VertexBuffer

    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
    m_vecVerticesPos.clear();
    m_vecVerticesPos.reserve(m_iNumVertices);
    for (size_t i = 0; i < m_iNumVerticesX; ++i)
    {
        for (size_t j = 0; j < m_iNumVerticesZ; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = _float3(j * m_fVerticesItv, (pPixels[iIndex] & 0x000000ff) / 10.f, i * m_fVerticesItv);
            m_vecVerticesPos.push_back(pVertices[iIndex].vPosition); // Ray 피킹을 위한 포지션 정보 저장.
            pVertices[iIndex].vTexcoord = _float2((float)j / ((float)m_iNumVerticesX - 1.0f), -(float)i / ((float)m_iNumVerticesZ - 1.0f));

        }
    }

#pragma endregion // VertexBuffer

#pragma region IndexBuffer
    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
    m_vecIndexBuffer.clear();
    m_vecIndexBuffer.reserve(m_iNumIndices);
    _uint iIndicesCount = 0;

    // 여기서 반복문의 횟수는 해당 버텍스들로 이루어진 Rect의 수이다. 
    for (size_t i = 0; i < m_iNumVerticesX - 1; ++i)
    {
        for (size_t j = 0; j < m_iNumVerticesZ - 1; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            _uint iIndices[] =
            {
                iIndex + m_iNumVerticesX,
                iIndex + m_iNumVerticesX + 1,
                iIndex + 1,
                iIndex,
            };
            _vector vSrc, vDst, vNormal;

            pIndices[iIndicesCount++] = iIndices[0];
            pIndices[iIndicesCount++] = iIndices[1];
            pIndices[iIndicesCount++] = iIndices[2];
            m_vecIndexBuffer.push_back(iIndices[0]);
            m_vecIndexBuffer.push_back(iIndices[1]);
            m_vecIndexBuffer.push_back(iIndices[2]);

            vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));


            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);


            pIndices[iIndicesCount++] = iIndices[0];
            pIndices[iIndicesCount++] = iIndices[2];
            pIndices[iIndicesCount++] = iIndices[3];
            m_vecIndexBuffer.push_back(iIndices[0]);
            m_vecIndexBuffer.push_back(iIndices[2]);
            m_vecIndexBuffer.push_back(iIndices[3]);

            vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));


            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
        }
    }
    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
    }



#pragma endregion // IndexBuffer
    /* VertexBuffer Create */
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    /* 정점버퍼를 몇 바이트 할당할까요? */
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = m_iVertexStride;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    // 다 채웠으면 해당 데이터를 기반으로 SubResourceDesc 채우고 이를 통해 Create Buffer
    ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
    m_SubResourceDesc.pSysMem = pVertices;
    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    /* IndexBuffer Create */
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    /* 인덱스버퍼를 몇 바이트 할당할까요? */
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceDesc.pSysMem = pIndices;
    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);
    Safe_Delete_Array(pPixels);

    return S_OK;
}

HRESULT CVIBuffer_Terrain::Remake_Buffer_Flat(void* _pArg)
{
    if (nullptr != m_pVB)
        Safe_Release(m_pVB);

    if (nullptr != m_pIB)
        Safe_Release(m_pIB);

    VIBUFFER_TERRAIN_DESC* pDesc = static_cast<VIBUFFER_TERRAIN_DESC*>(_pArg);
    m_fVerticesItv = pDesc->fVerticesItv;
    m_iNumVerticesX = pDesc->fVerticesX;
    m_iNumVerticesZ = pDesc->fVerticesZ;
    

    m_iNumVertexBuffers = 1;

    m_iVertexStride = sizeof(VTXNORTEX);
    m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

    m_iIndexStride = sizeof(_uint);
    m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 6;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VertexBuffer

    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
    m_vecVerticesPos.clear();
    m_vecVerticesPos.reserve(m_iNumVertices);
    for (_uint i = 0; i < m_iNumVerticesZ; ++i)
    {
        for (_uint j = 0; j < m_iNumVerticesX; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = _float3(j * m_fVerticesItv, pDesc->fHeightY, i * m_fVerticesItv);
            m_vecVerticesPos.push_back(pVertices[iIndex].vPosition); // Ray 피킹을 위한 포지션 정보 저장.
            pVertices[iIndex].vTexcoord = _float2((float)j / ((float)m_iNumVerticesX - 1.0f), -(float)i / ((float)m_iNumVerticesZ - 1.0f));

        }
    }
   
#pragma endregion // VertexBuffer

#pragma region IndexBuffer
    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
    m_vecIndexBuffer.clear();
    m_vecIndexBuffer.reserve(m_iNumIndices);
    _uint iIndicesCount = 0;

    // 여기서 반복문의 횟수는 해당 버텍스들로 이루어진 Rect의 수이다. 
    for (size_t i = 0; i < m_iNumVerticesX - 1; ++i)
    {
        for (size_t j = 0; j < m_iNumVerticesZ - 1; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            _uint iIndices[] =
            {
                iIndex + m_iNumVerticesX,
                iIndex + m_iNumVerticesX + 1,
                iIndex + 1,
                iIndex,
            };
            _vector vSrc, vDst, vNormal;

            pIndices[iIndicesCount++] = iIndices[0];
            pIndices[iIndicesCount++] = iIndices[1];
            pIndices[iIndicesCount++] = iIndices[2];
            m_vecIndexBuffer.push_back(iIndices[0]);
            m_vecIndexBuffer.push_back(iIndices[1]);
            m_vecIndexBuffer.push_back(iIndices[2]);

            vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));


            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            

            pIndices[iIndicesCount++] = iIndices[0];
            pIndices[iIndicesCount++] = iIndices[2];
            pIndices[iIndicesCount++] = iIndices[3];
            m_vecIndexBuffer.push_back(iIndices[0]);
            m_vecIndexBuffer.push_back(iIndices[2]);
            m_vecIndexBuffer.push_back(iIndices[3]);

            vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));


            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
        }
    }

    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
    }
    
#pragma endregion // IndexBuffer
    /* Vertex Buffer */
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = m_iVertexStride;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
    m_SubResourceDesc.pSysMem = pVertices;
    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;


    /* Index Buffer */
    ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceDesc.pSysMem = pIndices;
    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);


    return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(_pDevice, _pContext);
    if(FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* _pArg)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Terrain::Free()
{


    __super::Free();
}
