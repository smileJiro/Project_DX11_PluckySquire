#include "Mesh.h"
#include "3DModel.h"
#include "Shader.h"
#include "Bone.h"
#include "Engine_Function.h"
#include "iostream"

CMesh::CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

HRESULT CMesh::Initialize_Prototype(C3DModel::TYPE eModelType, C3DModel* pModel, ifstream& inFile, _fmatrix PreTransformMatrix)
{

	inFile.read(reinterpret_cast<char*>(&m_iMaterialIndex), sizeof(_uint));

	_uint iNameLength = 0;
	inFile.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
	inFile.read(m_szName, iNameLength);
	m_szName[iNameLength] = '\0';


	m_iNumVertexBuffers = 1;
	inFile.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));
	m_iIndexStride = sizeof(_uint);
	_uint iNumFaces = 0;
	inFile.read(reinterpret_cast<char*>(&iNumFaces), sizeof(_uint));
	m_iNumIndices = iNumFaces * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER	

	HRESULT hr = C3DModel::NONANIM == eModelType ? Ready_VertexBuffer_For_NonAnim(inFile, PreTransformMatrix) : Ready_VertexBuffer_For_Anim(inFile, pModel);

	if (FAILED(hr))
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
		inFile.read(reinterpret_cast<char*>(&pIndices[iNumIndices++]), sizeof(_uint));
		m_vecIndexBuffer.push_back(pIndices[iNumIndices -1]);
		inFile.read(reinterpret_cast<char*>(&pIndices[iNumIndices++]), sizeof(_uint));
		m_vecIndexBuffer.push_back(pIndices[iNumIndices -1]);
		inFile.read(reinterpret_cast<char*>(&pIndices[iNumIndices++]), sizeof(_uint));
		m_vecIndexBuffer.push_back(pIndices[iNumIndices -1]);

	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* _pShader, const _char* _pConstantName, const vector<CBone*>& _pBones)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * 256);

	_uint		iSize = m_BoneIndices.size();

	for (size_t i = 0; i < m_BoneIndices.size(); ++i)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_BoneOffsetMatrices[i]) * _pBones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return _pShader->Bind_Matrices(_pConstantName, m_BoneMatrices,  256);

}

void CMesh::ReSet_OffsetMarix()
{
	for (auto& i : m_BoneOffsetMatrices)
	{
		XMStoreFloat4x4(&i, XMMatrixIdentity());
	}
}


HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(ifstream& inFile, _fmatrix PreTransformMatrix)
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
		inFile.read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		inFile.read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		inFile.read(reinterpret_cast<char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		inFile.read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));


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

HRESULT CMesh::Ready_VertexBuffer_For_Anim(ifstream& inFile, C3DModel* pModel)
{


	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;



	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		inFile.read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		inFile.read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		inFile.read(reinterpret_cast<char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		inFile.read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
	}
	inFile.read(reinterpret_cast<char*>(&m_iNumBones), sizeof(_uint));

	for (_uint curMeshBoneIdx = 0; curMeshBoneIdx < m_iNumBones; curMeshBoneIdx++)
	{
		_uint iNameLength = 0;
		inFile.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
		char szName[MAX_PATH] = "";
		inFile.read(szName, iNameLength);
		szName[iNameLength] = '\0';

		m_BoneIndices.push_back(pModel->Get_BoneIndex(szName));

		_float4x4	OffsetMatrix = {};
		inFile.read(reinterpret_cast<char*>(&OffsetMatrix), sizeof(_float4x4));
		//cout << OffsetMatrix._11 << " " << OffsetMatrix._12 << " " << OffsetMatrix._13 << " " << OffsetMatrix._14 << endl;
		//cout << OffsetMatrix._21 << " " << OffsetMatrix._22 << " " << OffsetMatrix._23 << " " << OffsetMatrix._24 << endl;
		//cout << OffsetMatrix._31 << " " << OffsetMatrix._32 << " " << OffsetMatrix._33 << " " << OffsetMatrix._34 << endl;
		//cout << OffsetMatrix._41 << " " << OffsetMatrix._42 << " " << OffsetMatrix._43 << " " << OffsetMatrix._44 << endl;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
		m_BoneOffsetMatrices.push_back(OffsetMatrix);


		//정점마다 어떤 본에 얼마만큼 영향받는지 정보를 읽어온다.
		_uint iNumVertices;
		inFile.read(reinterpret_cast<char*>(&iNumVertices), sizeof(_uint));
		for (_uint curBoneVertex = 0; curBoneVertex < iNumVertices; curBoneVertex++)
		{
			//
			_uint iVertexIdx = 0;
			inFile.read(reinterpret_cast<char*>(&iVertexIdx), sizeof(_uint));
			_float fWeight = 0.f;
			inFile.read(reinterpret_cast<char*>(&fWeight), sizeof(_float));
			//하나의 정점이 영향받을 수 있는 본의 갯수가 최대 4개라고 정함.
			//아무 정보도 들어가지 않으면 0이므로, x부터 0임을 확인하고 순서대로 채워넣는다.
			if (0 == pVertices[iVertexIdx].vBlendWeights.x)
			{
				pVertices[iVertexIdx].vBlendIndices.x = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.x = fWeight;
			}
			else if (0 == pVertices[iVertexIdx].vBlendWeights.y)
			{
				pVertices[iVertexIdx].vBlendIndices.y = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.y = fWeight;
			}
			else if (0 == pVertices[iVertexIdx].vBlendWeights.z)
			{
				pVertices[iVertexIdx].vBlendIndices.z = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.z = fWeight;
			}
			else if (0 == pVertices[iVertexIdx].vBlendWeights.z)
			{
				pVertices[iVertexIdx].vBlendIndices.w = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.w = fWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_BoneOffsetMatrices.push_back(OffsetMatrix);

	}
	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, C3DModel::TYPE eModelType, C3DModel* pModel, ifstream& inFile, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModel, inFile, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* _pArg)
{
	/* 클론 함수는 사용하지 않는다. CMesh 객체는 얕은 복사가 아닌 깊은 복사를 통해 구현해야한다. */
    return nullptr;
}

void CMesh::Free()
{
	__super::Free();
}
