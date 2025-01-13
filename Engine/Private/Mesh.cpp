#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
	, m_szName{}
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE _eModelType, class CModel* _pModel, const aiMesh* _pAIMesh, _matrix _PreTransformMatrix)
{
	/* 0. 어떤 Material로 그릴지 저장. */
	m_iMaterialIndex = _pAIMesh->mMaterialIndex;

	/* 1. pAIMesh->mName.data : 해당 메쉬의 이름을 char* 타입 */
	strcpy_s(m_szName, _pAIMesh->mName.data);

	m_iNumVertexBuffers = 1;
	m_iNumVertices = _pAIMesh->mNumVertices;
	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = _pAIMesh->mNumFaces * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex_Buffer
	/* AnimModel인지 nonAnimModel인지에 따라서, 별도의 초기화 과정을 거친다 : nonanim은 pretransform미리곱하고, animmodel은 별도로 본정보 정리해야함.*/
	/* VertexBuffer 정보만 차이가 있다. */
	HRESULT hr = CModel::TYPE::NONANIM == _eModelType ? Ready_VertexBuffer_For_NonAnim(_pAIMesh, _PreTransformMatrix) : Ready_VertexBuffer_For_Anim(_pAIMesh, _pModel);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion // Vertex_Buffer

#pragma region Index_Buffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	// 삼각형 면 개수만큼 순회.
	for (size_t i = 0; i < _pAIMesh->mNumFaces; ++i)
	{
		/* mFaces[] : 면 객체 배열 우리는 타입을 삼각형으로 지정하여, Face는 각각 3개의 인덱스를 가진다.*/
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = _pAIMesh->mFaces[i].mIndices[2];

		m_vecIndexBuffer.push_back(_pAIMesh->mFaces[i].mIndices[0]);
		m_vecIndexBuffer.push_back(_pAIMesh->mFaces[i].mIndices[1]);
		m_vecIndexBuffer.push_back(_pAIMesh->mFaces[i].mIndices[2]);
	}

	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion // Index_Buffer


    return S_OK;
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE _eModelType, FBX_MESH& _tMesh, _matrix _PreTransformMatrix)
{
	/* 0. 어떤 Material로 그릴지 저장. */
	m_iMaterialIndex = _tMesh.iMaterialIndex;

	/* 1. pAIMesh->mName.data : 해당 메쉬의 이름을 char* 타입 */
	strcpy_s(m_szName, _tMesh.szName);

	m_iNumVertexBuffers = 1;

	/* 2. 버텍스 구조체 타입(Tangent 추가), pAIMesh->mNumVertices (버텍스 개수) */
	m_iNumVertices = _tMesh.iNumVertices;
	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = _tMesh.iNumFaces * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Vertex_Buffer

	HRESULT hr = CModel::TYPE::NONANIM == _eModelType ? Ready_VertexBuffer_For_NonAnim(_tMesh, _PreTransformMatrix) : Ready_VertexBuffer_For_Anim(_tMesh);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion // Vertex_Buffer

#pragma region Index_Buffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	//_uint* pIndices = {};
	//ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
	//pIndices = _tMesh.pIndices;

	_uint		iNumIndices = { 0 };

	// 삼각형 면 개수만큼 순회.

	if (_eModelType == CModel::TYPE::NONANIM)
	{
		for (size_t i = 0; i < m_iNumIndices; ++i)
		{
			m_vecIndexBuffer.push_back(_tMesh.pIndices[i]);
		}
	}
	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = _tMesh.pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(_tMesh.pIndices);
	
#pragma endregion // Index_Buffer


	return S_OK;
}

HRESULT CMesh::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* _pShader, const _char* _pConstantName, const vector<CBone*>& _vecBones)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * 256);

	for (size_t i = 0; i < m_BoneIndices.size(); ++i)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_BoneOffsetMatrices[i]) * _vecBones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return _pShader->Bind_Matrices(_pConstantName, m_BoneMatrices, 256);
}

HRESULT CMesh::Copy_BoneMatrices(array<_float4x4, 256>* _pOutBoneMatrices)
{
	memcpy(_pOutBoneMatrices->data(), m_BoneMatrices, sizeof(m_BoneMatrices));
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	/* 3. 버텍스 버퍼에 값을 채워주자. */
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		/* 4. mVertices : aiVector3D 타입이다 .실수형 타입 3개가 있다고 보면되는데, 우리의 xmfloat과는 오버로딩 되지 않아서 멤카피를 한다. */
		memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		/* 주의! : dx에서도 TextureCoord는 총8개 까지 사용 가능하다. 이차원 배열로 구성되어있는데, [텍스쳐쿠드인덱스][i] 인 것이다.*/
		if(nullptr != _pAIMesh->mTextureCoords[0])
			memcpy(&pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));


		/* 모델에서 던져준, LinearTransform Matrix를 곱해준다. */
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _PreTransformMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _PreTransformMatrix)));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), _PreTransformMatrix)));

		/* 피킹용 데이터 저장 */
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

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const aiMesh* _pAIMesh, CModel* _pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));

		/* 피킹용 데이터 저장 */
		m_vecVerticesPos.push_back(pVertices[i].vPosition);
		m_vecVerticesNormal.push_back(pVertices[i].vNormal);
	}

	/* Bone에 대한 데이터 정리. (가장 중요. 메쉬클래스 내의 m_BoneIndices 벡터에 자신이 영향받는 Bone의 인덱스를 저장함.) */
	/* 그렇게 저장 된 m_BoneIndices 벡터의 인덱스와 동기화 하여 자기자신에게 영향을 주는 Bone의 상태행렬(CombinedMatrix)를 저장할 것임. 그리고 그걸 쉐이더에 바인딩. */
	/* 추가로, 자기 자신에게 영향을 주는 Bone을 순회하며, 각각의 Bone이 영향을 끼치는 Vertex에 대한 정보를 Assimp를 통해 받아오고, Vertex에게 어떤 Bone에게 영향을 받는지 인덱스 정보를 전달함. */
	// https://www.notion.so/42-Animation-Model_4-120b1e26c8a8801d95bcc4ef9b21a718

	/* 1. 자기자신에게 영향을 주는 Bone의 개수를 구한다. */
	m_iNumBones = _pAIMesh->mNumBones;

	/* 2. aiBone : 자기자신(메쉬)에게 영향을 주는 뼈에 대한 정보를 담을 것임. */
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		// 현재 메쉬에게 영향을 주는 본들을 가져오는 작업,
		aiBone* pAIBone = _pAIMesh->mBones[i];

		/* 추가 : OffsetMatrix 가져오기. : Offset Data는 한번 설정하면 절대 바뀌지 않고, 메쉬클래스 외에는 사용하는 곳이 없음. */
		_float4x4 OffsetMatrix = {};
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix))); /* 전치 주의 */
		/* PushBack 하더라도 상관없는 이유 : 어짜피 나에게 영향있는 본들중 0번부터 순회하는 루틴이고, Offset또 같은 순서대로 저장되어야함. */
		m_BoneOffsetMatrices.push_back(OffsetMatrix);

		// 나에게 영향을 주는 본의 이름을 기준으로 Model 클래스에 정의 된 m_Bones에서의 인덱스를 구함.
		m_BoneIndices.push_back(_pModel->Find_BoneIndex(pAIBone->mName.data));

		// 현재 이 뼈가 영향을 주고있는 Vertex의 개수만큼 순회.
		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			// pAIBone->mWeights[j] : 현재 pAIBone 이 영향을 주고있는 Vertex. >>> mVertexID : VertexBuffer에 저장된 인덱스를 리턴.
			if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				// 현재 j번째 정점에 Blendweight가 0이라는 것은, x 원소에 저장된 BlendIndices가 없다는 의미.(비어있다. == 데이터를 추가할 수 있다.)
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i; /* 현재 영향을 주고있는 뼈들 중 i번째 뼈 인덱스를 넘긴 것. m_BoneIndices 의 인덱스와 동일. */
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight; 
			}
			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				// 만약 x 원소에 이미 데이터가 들어있다면, y가 비어있는지 체크
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				// 만약 y 원소에 이미 데이터가 들어있다면, z가 비어있는지 체크
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else if (0 == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				// 만약 y 원소에 이미 데이터가 들어있다면, w가 비어있는지 체크
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}

		}
	}

	/* 영향을 주는 Bone이 없는 메쉬 클래스를 위한 루틴. (배치용 Bone을 찾아 해당 Bone을 m_BoneIndices, m_OffsetMatrices에 추가.) */
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		/* 현재 메쉬의 이름과 동일한 Bone을 찾는다. >>> 이게 배치용 본들의 특징임 */
		m_BoneIndices.push_back(_pModel->Find_BoneIndex(m_szName));

		_float4x4 OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		/* indices, offsetMatrix 모두 pushback 해서 인덱스 맞춰줌. */
		m_BoneOffsetMatrices.push_back(OffsetMatrix);
	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const FBX_MESH& _tMeshDesc, _fmatrix _PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	/* 3. 버텍스 버퍼에 값을 채워주자. */
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		/* 4. mVertices : aiVector3D 타입이다 .실수형 타입 3개가 있다고 보면되는데, 우리의 xmfloat과는 오버로딩 되지 않아서 멤카피를 한다. */
		pVertices[i].vPosition = _tMeshDesc.vecVertices[i].vPosition;
		pVertices[i].vNormal = _tMeshDesc.vecVertices[i].vNormal;
		pVertices[i].vTexcoord = _tMeshDesc.vecVertices[i].vTexcoord;
		pVertices[i].vTangent = _tMeshDesc.vecVertices[i].vTangent;

		/* 모델에서 던져준, LinearTransform Matrix를 곱해준다. */
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _PreTransformMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _PreTransformMatrix)));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), _PreTransformMatrix)));

		/* 피킹용 데이터 저장 */
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

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const FBX_MESH& _tMeshDesc)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		/* 4. mVertices : aiVector3D 타입이다 .실수형 타입 3개가 있다고 보면되는데, 우리의 xmfloat과는 오버로딩 되지 않아서 멤카피를 한다. */
		pVertices[i].vPosition = _tMeshDesc.vecVertices[i].vPosition;
		pVertices[i].vNormal = _tMeshDesc.vecVertices[i].vNormal;
		pVertices[i].vTexcoord = _tMeshDesc.vecVertices[i].vTexcoord;
		pVertices[i].vTangent = _tMeshDesc.vecVertices[i].vTangent;
		pVertices[i].vBlendIndices = _tMeshDesc.vecVertices[i].vBlendIndices;
		pVertices[i].vBlendWeights = _tMeshDesc.vecVertices[i].vBlendWeights;
	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	/* 영향주는 본 인덱스 및 매트릭스 설정 */
	m_iNumBones = _tMeshDesc.iNumBones;
	m_BoneIndices.resize(m_iNumBones);
	m_BoneOffsetMatrices.resize(m_iNumBones);
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		m_BoneIndices[i] = _tMeshDesc.vecBoneIndices[i];
		m_BoneOffsetMatrices[i] = _tMeshDesc.vecOffsetMatrix[i];
	}


	return S_OK;

}

CMesh* CMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::TYPE _eModelType, class CModel* _pModel, const aiMesh* _pAIMesh, _matrix _PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_eModelType, _pModel, _pAIMesh, _PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::TYPE _eModelType, FBX_MESH& _tMesh, _matrix _PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_eModelType, _tMesh, _PreTransformMatrix)))
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

HRESULT CMesh::ConvertToBinary(HANDLE _hFile, DWORD* _dwByte, const aiMesh* _pAIMesh, CModel* _pModel)
{
	if (!WriteFile(_hFile, m_szName, sizeof(m_szName), _dwByte, nullptr))
		return E_FAIL;

	if (!WriteFile(_hFile, &m_iMaterialIndex, sizeof(_uint), _dwByte, nullptr))
		return E_FAIL;

	if (!WriteFile(_hFile, &m_iNumBones, sizeof(_uint), _dwByte, nullptr))
		return E_FAIL;

	for (size_t i = 0; i < m_BoneIndices.size(); ++i)
	{
		if (!WriteFile(_hFile, &m_BoneIndices[i], sizeof(_uint), _dwByte, nullptr))
			return E_FAIL;
	}

	for (size_t i = 0; i < m_BoneOffsetMatrices.size(); ++i)
	{
		if (!WriteFile(_hFile, &m_BoneOffsetMatrices[i], sizeof(_float4x4), _dwByte, nullptr))
			return E_FAIL;
	}


	if (!WriteFile(_hFile, &m_iNumVertices, sizeof(_uint), _dwByte, nullptr))
		return E_FAIL;
	vector<VTXANIMMESH> vecAnimMesh;
	vecAnimMesh.resize(m_iNumVertices);
	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&vecAnimMesh[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&vecAnimMesh[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&vecAnimMesh[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&vecAnimMesh[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));

		vecAnimMesh[i].vBlendIndices = { 0, 0, 0, 0 };
		vecAnimMesh[i].vBlendWeights = { 0, 0, 0, 0 };
	}

	for (size_t i = 0; i < _pAIMesh->mNumBones; ++i)
	{
		aiBone* pAIBone = _pAIMesh->mBones[i];

		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			if (0 == vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0 == vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0 == vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0 == vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				vecAnimMesh[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}
	

	for (size_t i = 0; i < vecAnimMesh.size(); ++i)
	{
		if (!WriteFile(_hFile, &vecAnimMesh[i].vPosition, sizeof(_float3), _dwByte, nullptr))
			return E_FAIL;
		if (!WriteFile(_hFile, &vecAnimMesh[i].vNormal, sizeof(_float3), _dwByte, nullptr))
			return E_FAIL;
		if (!WriteFile(_hFile, &vecAnimMesh[i].vTexcoord, sizeof(_float2), _dwByte, nullptr))
			return E_FAIL;
		if (!WriteFile(_hFile, &vecAnimMesh[i].vTangent, sizeof(_float3), _dwByte, nullptr))
			return E_FAIL;
		if (!WriteFile(_hFile, &vecAnimMesh[i].vBlendIndices, sizeof(XMUINT4), _dwByte, nullptr))
			return E_FAIL;
		if (!WriteFile(_hFile, &vecAnimMesh[i].vBlendWeights, sizeof(_float4), _dwByte, nullptr))
			return E_FAIL;
	}


	if (!WriteFile(_hFile, &_pAIMesh->mNumFaces, sizeof(_uint), _dwByte, nullptr))
		return E_FAIL;
	for (size_t i = 0; i < _pAIMesh->mNumFaces; ++i)
	{
		if (!WriteFile(_hFile, &_pAIMesh->mFaces[i].mIndices[0], sizeof(_uint), _dwByte, nullptr))
			return E_FAIL;
		if (!WriteFile(_hFile, &_pAIMesh->mFaces[i].mIndices[1], sizeof(_uint), _dwByte, nullptr))
			return E_FAIL;
		if (!WriteFile(_hFile, &_pAIMesh->mFaces[i].mIndices[2], sizeof(_uint), _dwByte, nullptr))
			return E_FAIL;
	}


	return S_OK;
}
