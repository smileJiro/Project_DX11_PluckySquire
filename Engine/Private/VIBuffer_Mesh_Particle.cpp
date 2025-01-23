#include "VIBuffer_Mesh_Particle.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include "iostream"

CVIBuffer_Mesh_Particle::CVIBuffer_Mesh_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Mesh_Particle::CVIBuffer_Mesh_Particle(const CVIBuffer_Mesh_Particle& _Prototype)
	: CVIBuffer_Instance(_Prototype)
	, m_pInstanceVertices(_Prototype.m_pInstanceVertices)
	, m_iMaterialIndex(_Prototype.m_iMaterialIndex)
{
	strcpy_s(m_szName, _Prototype.m_szName);
}


HRESULT CVIBuffer_Mesh_Particle::Initialize_Prototype(ifstream& _inFile, const json& _jsonBufferInfo, _fmatrix _PreTransformMatrix)
{
	_inFile.read(reinterpret_cast<char*>(&m_iMaterialIndex), sizeof(_uint));

	_uint iNameLength = 0;
	_inFile.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
	_inFile.read(m_szName, iNameLength);
	m_szName[iNameLength] = '\0';

	// Mesh 정보 읽어들이기
#pragma region VERTEX_BUFFER
	m_iNumVertexBuffers = 2;
	m_iNumInstances = _jsonBufferInfo["Count"];

	_inFile.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));

	m_iIndexStride = sizeof(_uint);
	_uint iNumFaces = 0;
	_inFile.read(reinterpret_cast<char*>(&iNumFaces), sizeof(_uint));
	m_iNumIndices = iNumFaces * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (FAILED(Ready_VertexBuffer(_inFile, _PreTransformMatrix)))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };


	for (size_t i = 0; i < iNumFaces; i++)
	{
		_inFile.read(reinterpret_cast<char*>(&pIndices[iNumIndices++]), sizeof(_uint));
		m_vecIndexBuffer.push_back(pIndices[iNumIndices - 1]);
		_inFile.read(reinterpret_cast<char*>(&pIndices[iNumIndices++]), sizeof(_uint));
		m_vecIndexBuffer.push_back(pIndices[iNumIndices - 1]);
		_inFile.read(reinterpret_cast<char*>(&pIndices[iNumIndices++]), sizeof(_uint));
		m_vecIndexBuffer.push_back(pIndices[iNumIndices - 1]);

	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXMESHINSTANCE);
	m_iNumIndexCountPerInstance = iNumFaces * 3;

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTXMESHINSTANCE[m_iNumInstances];
	ZeroMemory(m_pInstanceVertices, m_iInstanceStride * m_iNumInstances);


	for (size_t i = 0; i < m_iNumInstances; ++i)
	{
		m_pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[i].vTranslation =
			_float4(i * 0.5f, i * 0.5f, 0.f, 1.f);

		m_pInstanceVertices[i].vLifeTime = _float2(
			1.f,
			0.f
		);
		m_pInstanceVertices[i].vColor = _float4(1.f, 1.f, 1.f, 0.5f);

	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;


#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Particle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Mesh_Particle::Update(_float _fTimeDelta)
{
}

HRESULT CVIBuffer_Mesh_Particle::Ready_VertexBuffer(ifstream& _inFile, _fmatrix _PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _PreTransformMatrix));

		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _PreTransformMatrix));

		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), _PreTransformMatrix));


		m_vecVerticesPos.push_back(pVertices[i].vPosition);
		m_vecVerticesNormal.push_back(pVertices[i].vNormal);

	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CVIBuffer_Mesh_Particle* CVIBuffer_Mesh_Particle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ifstream& _inFile, const json& _jsonBufferInfo, _fmatrix _PreTransformMatrix)
{
	CVIBuffer_Mesh_Particle* pInstance = new CVIBuffer_Mesh_Particle(_pDevice, _pContext);
	
	if (FAILED(pInstance->Initialize_Prototype(_inFile, _jsonBufferInfo, _PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Mesh_Particle");
		Safe_Release(pInstance);
	}
	
	return nullptr;
}


CComponent* CVIBuffer_Mesh_Particle::Clone(void* _pArg)
{
	CComponent* pInstance = new CVIBuffer_Mesh_Particle(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Mesh_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Particle::Free()
{

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
	}

	__super::Free();
}
