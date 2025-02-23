#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& _Prototype)
	: CVIBuffer(_Prototype)
{
}


HRESULT CVIBuffer_Trail::Initialize_Prototype(_uint _iMaxVertexCount)
{
	//if (4 > _iMaxVertexCount || 0 != _iMaxVertexCount % 2)
	//	return E_FAIL;

	// 멤버 변수에 값 저장. >>> 추후 IA에 전송할 데이터들을 미리 저장 해두는 것.
	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXTRAIL);
	m_iNumVertices = _iMaxVertexCount;

	m_iIndexStride = sizeof(_uint);
	//m_iNumIndices = m_iTriangleCount * 3;
	m_iNumIndices = m_iNumVertices;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT; // 16비트 정수형 타입
	//m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	// trail 총 삼각형 개수

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

	//for (_uint i = 0; i < m_iTriangleCount; i += 2)
	//{
	//	pIndices[iIndex++] = i + 3;
	//	pIndices[iIndex++] = i + 1;
	//	pIndices[iIndex++] = i;

	//	pIndices[iIndex++] = i + 2;
	//	pIndices[iIndex++] = i + 3;
	//	pIndices[iIndex++] = i;
	//}

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

HRESULT CVIBuffer_Trail::Initialize(void* _pArg)
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

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pVB, &SRVDesc, &m_pBufferSRV)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

void CVIBuffer_Trail::Update(_float _fTimeDelta)
{
	if (1 < m_iCurVertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE		SubResource{};
		m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
		VTXTRAIL* pVertices = static_cast<VTXTRAIL*>(SubResource.pData);
		
		for (_uint i = 0; i < m_iCurVertexCount; )
		{
			pVertices[i].vLifeTime.y += _fTimeDelta;

			// 삭제
			if (pVertices[i].vLifeTime.x < pVertices[i].vLifeTime.y)
			{
				for (_int j = i; j < (_int)(m_iCurVertexCount) - 1; ++j)
				{
					pVertices[j].vPosition = pVertices[j + 1].vPosition;
					pVertices[j].vLifeTime = pVertices[j + 1].vLifeTime;

				}
				m_iCurVertexCount -= 1;

			}
			// 그다음 !
			else
			{
				++i;
			}

		}

		m_pContext->Unmap(m_pVB, 0);
	}
}


//}

HRESULT CVIBuffer_Trail::Bind_BufferDesc()
{
	m_pContext->VSSetShaderResources(0, 1, &m_pBufferSRV);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
	m_pContext->Draw(_uint(m_iCurVertexCount), 0);

	ID3D11ShaderResourceView* nullSRV[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	m_pContext->VSSetShaderResources(0, 6, nullSRV); // VS에서도 해제

	return S_OK;
}

void CVIBuffer_Trail::Add_Point(const _float4x4* _pWorldMatrix, _fvector _vPosition, _float _fLifeTime)
{
	if (nullptr == _pWorldMatrix)
		return;

	_float3 vTranslate;
	_matrix WorldMatrix = XMLoadFloat4x4(_pWorldMatrix);

	for (_int i = 0; i < 3; ++i)
		WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);

	XMStoreFloat3(&vTranslate, XMVector3TransformCoord(_vPosition, WorldMatrix));

	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXTRAIL* pVertices = static_cast<VTXTRAIL*>(SubResource.pData);

	// 최대로 가질 수보다 적을 경우.
	if (m_iNumVertices > m_iCurVertexCount)
	{
		pVertices[m_iCurVertexCount].vPosition = vTranslate;
		pVertices[m_iCurVertexCount].vLifeTime.y = 0.f;
		pVertices[m_iCurVertexCount].vLifeTime.x = _fLifeTime;

		m_iCurVertexCount += 1;
	}
	// 최대 수에 해당할 경우
	else
	{
		for (_uint i = 0; i < m_iCurVertexCount - 1; ++i)
		{
			pVertices[i].vPosition = pVertices[i + 1].vPosition;
			pVertices[i].vLifeTime = pVertices[i + 1].vLifeTime;
		}
		pVertices[m_iCurVertexCount].vPosition = vTranslate;
		pVertices[m_iCurVertexCount].vLifeTime.y = 0.f;
		pVertices[m_iCurVertexCount].vLifeTime.x = _fLifeTime;
	}

	m_pContext->Unmap(m_pVB, 0);
}

//void CVIBuffer_Trail::Delete_Point()
//{
//	//if (0 < m_iCurVertexCount)
//	//{
//	//	D3D11_MAPPED_SUBRESOURCE		SubResource{};
//	//	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
//	//	VTXTRAIL* pVertices = static_cast<VTXTRAIL*>(SubResource.pData);
//	//	
//	//	if (1 < m_iCurVertexCount)
//	//	{
//	//		for (_uint i = 0; i < m_iCurVertexCount - 1; ++i)
//	//		{
//	//			pVertices[i].vPosition = pVertices[i + 1].vPosition;
//	//		}
//	//	}
//	//	else
//	//	{
//	//		pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
//	//	}		
//	//
//	//	m_pContext->Unmap(m_pVB, 0);
//	//
//	//	m_iCurVertexCount -= 1;
//	//}
//}


CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iMaxVertexCount)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iMaxVertexCount)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* _pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	Safe_Release(m_pBufferSRV);

	__super::Free();
}
