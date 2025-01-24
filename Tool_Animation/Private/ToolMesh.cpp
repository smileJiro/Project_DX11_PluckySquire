#include "stdafx.h"
#include "ToolMesh.h"

CToolMesh::CToolMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CMesh(_pDevice, _pContext)
{
}




HRESULT CToolMesh::Ready_VertexBuffer_For_NonAnim(ifstream& _inFile, _fmatrix _PreTransformMatrix)
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
	m_Vertices.resize(m_iNumVertices);
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
		m_Vertices[i].vPosition = pVertices->vPosition;
		m_Vertices[i].vNormal = pVertices->vNormal;
		m_Vertices[i].vTexcoord = pVertices->vTexcoord;
		m_Vertices[i].vTangent = pVertices->vTangent;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CToolMesh::Ready_VertexBuffer_For_Anim(ifstream& _inFile, C3DModel* _pModel)
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

	m_Vertices.resize(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
		m_vecVerticesPos.push_back(pVertices[i].vPosition);
		m_vecVerticesNormal.push_back(pVertices[i].vNormal);
		m_Vertices[i].vPosition = pVertices->vPosition;
		m_Vertices[i].vNormal = pVertices->vNormal;
		m_Vertices[i].vTexcoord = pVertices->vTexcoord;
		m_Vertices[i].vTangent = pVertices->vTangent;
	}
	_inFile.read(reinterpret_cast<char*>(&m_iNumBones), sizeof(_uint));
	m_BoneInfos.resize(m_iNumBones);
	for (_uint curMeshBoneIdx = 0; curMeshBoneIdx < m_iNumBones; curMeshBoneIdx++)
	{

		_inFile.read(reinterpret_cast<char*>(&m_BoneInfos[curMeshBoneIdx].iNameSize), sizeof(_uint));
		_inFile.read(m_BoneInfos[curMeshBoneIdx].szName, m_BoneInfos[curMeshBoneIdx].iNameSize);
		m_BoneInfos[curMeshBoneIdx].szName[m_BoneInfos[curMeshBoneIdx].iNameSize] = '\0';

		m_BoneIndices.push_back(_pModel->Get_BoneIndex(m_BoneInfos[curMeshBoneIdx].szName));

		_inFile.read(reinterpret_cast<char*>(&m_BoneInfos[curMeshBoneIdx].matOffset), sizeof(_float4x4));
		_float4x4	OffsetMatrixTranspose = {};
		XMStoreFloat4x4(&OffsetMatrixTranspose, XMMatrixTranspose(XMLoadFloat4x4(&m_BoneInfos[curMeshBoneIdx].matOffset)));
		m_BoneOffsetMatrices.push_back(OffsetMatrixTranspose);


		//정점마다 어떤 본에 얼마만큼 영향받는지 정보를 읽어온다.
		_inFile.read(reinterpret_cast<char*>(&m_BoneInfos[curMeshBoneIdx].iNumVertices), sizeof(_uint));
		m_BoneInfos[curMeshBoneIdx].WeightInfos.resize(m_BoneInfos[curMeshBoneIdx].iNumVertices);
		for (_uint curBoneVertex = 0; curBoneVertex < m_BoneInfos[curMeshBoneIdx].iNumVertices; curBoneVertex++)
		{
			pair<_uint, _float>& tWeightPair = m_BoneInfos[curMeshBoneIdx].WeightInfos[curBoneVertex];

			_inFile.read(reinterpret_cast<char*>(&tWeightPair.first), sizeof(_uint));

			_inFile.read(reinterpret_cast<char*>(&tWeightPair.second), sizeof(_float));
			//하나의 정점이 영향받을 수 있는 본의 갯수가 최대 4개라고 정함.
			//아무 정보도 들어가지 않으면 0이므로, x부터 0임을 확인하고 순서대로 채워넣는다.
			_uint iVertexIdx = tWeightPair.first;
			_float fWeight = tWeightPair.second;
			if (0.f == pVertices[iVertexIdx].vBlendWeights.x)
			{
				pVertices[iVertexIdx].vBlendIndices.x = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.x = fWeight;
			}
			else if (0.f == pVertices[iVertexIdx].vBlendWeights.y)
			{
				pVertices[iVertexIdx].vBlendIndices.y = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.y = fWeight;
			}
			else if (0.f == pVertices[iVertexIdx].vBlendWeights.z)
			{
				pVertices[iVertexIdx].vBlendIndices.z = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.z = fWeight;
			}
			else if (0.f == pVertices[iVertexIdx].vBlendWeights.z)
			{
				pVertices[iVertexIdx].vBlendIndices.w = curMeshBoneIdx;
				pVertices[iVertexIdx].vBlendWeights.w = fWeight;
			}
		}
	}
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.push_back(_pModel->Get_BoneIndex(m_szName));

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

void CToolMesh::Export(ofstream& _outfile, _bool _bAnim)
{
	_outfile.write(reinterpret_cast<const char*>(&m_iMaterialIndex), sizeof(_uint));
	string strName = m_szName;
	_uint iCount = strName.length();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));

	_outfile.write(strName.c_str(), iCount);

	_outfile.write(reinterpret_cast<const char*>(&m_iNumVertices), sizeof(_uint));
	iCount = m_iNumIndices / 3;
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		_outfile.write(reinterpret_cast<const char*>(&m_Vertices[i].vPosition), sizeof(_float3));
		_outfile.write(reinterpret_cast<const char*>(&m_Vertices[i].vNormal), sizeof(_float3));
		_outfile.write(reinterpret_cast<const char*>(&m_Vertices[i].vTexcoord), sizeof(_float2));		
		_outfile.write(reinterpret_cast<const char*>(&m_Vertices[i].vTangent), sizeof(_float3));
	}
	if (_bAnim)
	{
		_outfile.write(reinterpret_cast<const char*>(&m_iNumBones), sizeof(_uint));

		for (_uint i = 0; i < m_iNumBones; i++)
		{

			_outfile.write(reinterpret_cast<const char*>(&m_BoneInfos[i].iNameSize), sizeof(_uint));
			_outfile.write(m_BoneInfos[i].szName, m_BoneInfos[i].iNameSize);

			_outfile.write(reinterpret_cast<const char*>(&m_BoneInfos[i].matOffset), sizeof(_float4x4));

			_outfile.write(reinterpret_cast<const char*>(&m_BoneInfos[i].iNumVertices), sizeof(_uint));
			for (size_t j = 0; j < m_BoneInfos[i].iNumVertices; j++)
			{
				pair<_uint, _float>& tWeightPair = m_BoneInfos[i].WeightInfos[j];
				_outfile.write(reinterpret_cast<const char*>(&tWeightPair.first), sizeof(_uint));
				_outfile.write(reinterpret_cast<const char*>(&tWeightPair.second), sizeof(_float));
			}

		}

	}

	_uint		iNumIndices = { 0 };
	for (_uint i = 0; i < m_iNumIndices/3; ++i)
	{
		_outfile.write(reinterpret_cast<char*>(&m_vecIndexBuffer[iNumIndices++]), sizeof(_uint));
		_outfile.write(reinterpret_cast<char*>(&m_vecIndexBuffer[iNumIndices++]), sizeof(_uint));
		_outfile.write(reinterpret_cast<char*>(&m_vecIndexBuffer[iNumIndices++]), sizeof(_uint));
	}
}

CToolMesh* CToolMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, C3DModel::ANIM_TYPE _eModelType, C3DModel* pModel, ifstream& inFile, _fmatrix PreTransformMatrix)
{
	CToolMesh* pInstance = new CToolMesh(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_eModelType, pModel, inFile, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : ToolMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolMesh::Free()
{
	__super::Free();
}
