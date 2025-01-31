#include "VIBuffer_Mesh_Particle.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include "iostream"
#include "Effect_Module.h"

CVIBuffer_Mesh_Particle::CVIBuffer_Mesh_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Mesh_Particle::CVIBuffer_Mesh_Particle(const CVIBuffer_Mesh_Particle& _Prototype)
	: CVIBuffer_Instance(_Prototype)
	, m_pInstanceVertices(_Prototype.m_pInstanceVertices)
	, m_iMaterialIndex(_Prototype.m_iMaterialIndex)
	, m_PreTransformMatrix(_Prototype.m_PreTransformMatrix)
#ifdef _DEBUG
	, m_pTexcoords(_Prototype.m_pTexcoords)
	, m_pTangents(_Prototype.m_pTangents)
	, m_iNumFaces(_Prototype.m_iNumFaces)
#endif // _DEBUG

{
	strcpy_s(m_szName, _Prototype.m_szName);
}


HRESULT CVIBuffer_Mesh_Particle::Initialize_Prototype(ifstream& _inFile, const json& _jsonBufferInfo, _fmatrix _PreTransformMatrix)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

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
#ifdef _DEBUG
	m_iNumFaces = iNumFaces;
#endif

	m_iNumIndices = iNumFaces * 3/* * m_iNumInstances*/;
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

	//if (1 < m_iNumInstances)
	//{
	//	for (size_t i = 1; i < m_iNumInstances; ++i)
	//	{
	//		for (size_t j = 0; j < iNumFaces; ++j)
	//		{
	//			pIndices[iNumIndices++] = m_vecIndexBuffer[j];
	//			pIndices[iNumIndices++] = m_vecIndexBuffer[j + 1];
	//			pIndices[iNumIndices++] = m_vecIndexBuffer[j + 2];
	//		}
	//	}
	//}



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

	// Scale 설정
	if (FAILED(Set_Float3Data(P_SCALE, _jsonBufferInfo, "Scale", m_pSetScales)))
		return E_FAIL;

	// Rotation 설정
	if (FAILED(Set_Float3Data(P_ROTATION, _jsonBufferInfo, "Rotation", m_pSetRotations)))
		return E_FAIL;

	// Position 설정
	if (FAILED(Set_Float3Data(P_POSITION, _jsonBufferInfo, "Position", m_pSetPositions)))
		return E_FAIL;

	// LifeTime 설정
	if (FAILED(Set_FloatData(P_LIFETIME, _jsonBufferInfo, "LifeTime", m_pSetLifeTimes)))
		return E_FAIL;

	// Color 설정
	if (FAILED(Set_Float4Data(P_COLOR, _jsonBufferInfo, "Color", m_pSetColors)))
		return E_FAIL;

	// Shape Data 설정
	if (FAILED(Set_ShapeData(_jsonBufferInfo)))
		return E_FAIL;

	// Spawn 설정
	if (_jsonBufferInfo.contains("Spawn"))
	{
		m_eSpawnType = _jsonBufferInfo["Spawn"]["Type"];
		m_fSpawnTime = _jsonBufferInfo["Spawn"]["Time"];
		//if (_jsonBufferInfo["Spawn"].contains("Count"))
		//	m_iSpawnCount = _jsonBufferInfo["Spawn"]["Count"];
	}

	for (size_t i = 0; i < m_iNumInstances; ++i)
	{
		Set_Instance(i);


	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;


#pragma endregion

	if (FAILED(Ready_Modules(_jsonBufferInfo)))
		return E_FAIL;

	if (FAILED(Initialize_Particles()))
		return E_FAIL;


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
	m_fAccSpawnTime += _fTimeDelta;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	if (SPAWN == m_eSpawnType)
	{
		while (m_fSpawnTime <= m_fAccSpawnTime)
		{
			// 시간이 충족되면 Spawn합니다.
			pVertices[m_iSpawnIndex] = m_pInstanceVertices[m_iSpawnIndex];

			m_iSpawnIndex = (m_iSpawnIndex + 1) % m_iNumInstances;
			m_fAccSpawnTime -= m_fSpawnTime;
		}

		for (_int i = 0; i < m_iNumInstances; ++i)
		{
			if (D3D11_FLOAT32_MAX == pVertices[i].vLifeTime.y)
				continue;

			for (auto& pModule : m_Modules)
			{
				if (pModule->Is_Init())
					continue;

				pModule->Update_Translations(_fTimeDelta, &pVertices[i].vTranslation, &pVertices[i].vVelocity, &pVertices[i].vAcceleration);
			}

			XMStoreFloat3(&pVertices[i].vVelocity, XMLoadFloat3(&pVertices[i].vVelocity) + XMLoadFloat3(&pVertices[i].vAcceleration) * _fTimeDelta);
			XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMLoadFloat3(&pVertices[i].vVelocity) * _fTimeDelta);
			pVertices[i].vLifeTime.y += _fTimeDelta;

			// TODO : Kill or Revive 등..
			if (pVertices[i].vLifeTime.y > pVertices[i].vLifeTime.x)
			{
				pVertices[i] = m_pInstanceVertices[i];
			}

		}

	}
	else if (BURST == m_eSpawnType)
	{
		if (m_fSpawnTime <= m_fAccSpawnTime)
		{
			for (_int i = 0; i < m_iNumInstances; i++)
			{
				if (m_iSpawnIndex != m_iNumInstances)
				{
					pVertices[i] = m_pInstanceVertices[i];
					pVertices[i].vLifeTime.y = 0.f;

					m_iSpawnIndex = m_iNumInstances;
				}

				for (auto& pModule : m_Modules)
				{
					if (pModule->Is_Init())
						continue;

					pModule->Update_Translations(_fTimeDelta, &pVertices[i].vTranslation, &pVertices[i].vVelocity, &pVertices[i].vAcceleration);
				}

				XMStoreFloat3(&pVertices[i].vVelocity, XMLoadFloat3(&pVertices[i].vVelocity) + XMLoadFloat3(&pVertices[i].vAcceleration) * _fTimeDelta);
				XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMLoadFloat3(&pVertices[i].vVelocity) * _fTimeDelta);
				pVertices[i].vLifeTime.y += _fTimeDelta;

				// TODO : Kill or Revive 등..
				if (pVertices[i].vLifeTime.y > pVertices[i].vLifeTime.x)
				{
					pVertices[i] = m_pInstanceVertices[i];
				}

			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Mesh_Particle::Reset_Buffers()
{
	m_iSpawnIndex = 0;
	m_fAccSpawnTime = 0.f;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		//if (SPAWN == m_eSpawnType)
		{
			pVertices[i].vLifeTime.y = D3D11_FLOAT32_MAX;
			ZeroMemory(&pVertices[i].vRight, sizeof(_float4));
			ZeroMemory(&pVertices[i].vUp, sizeof(_float4));
			ZeroMemory(&pVertices[i].vLook, sizeof(_float4));
		}
		//else if (BURST == m_eSpawnType)
		//{
		//	pVertices[i] = m_pInstanceVertices[i];
		//	pVertices[i].vLifeTime.y = 0.f;
		//}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
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

#ifdef _DEBUG
	m_pTexcoords = new _float2[m_iNumVertices];
	m_pTangents = new _float3[m_iNumVertices];
#endif


	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vPosition), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _PreTransformMatrix));

		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vNormal), sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _PreTransformMatrix));

		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vTexcoord), sizeof(_float2));
		_inFile.read(reinterpret_cast<char*>(&pVertices[i].vTangent), sizeof(_float3));
#ifdef _DEBUG
		m_pTexcoords[i] = pVertices[i].vTexcoord;
		m_pTangents[i] = pVertices[i].vTangent;
#endif
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), _PreTransformMatrix));


		m_vecVerticesPos.push_back(pVertices[i].vPosition);
		m_vecVerticesNormal.push_back(pVertices[i].vNormal);

	}

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, _PreTransformMatrix);


	Safe_Delete_Array(pVertices);

	return S_OK;
}

void CVIBuffer_Mesh_Particle::Set_Instance(_int _iIndex)
{
	_float3 vScale = Compute_ScaleValue();
	_float3 vRotation = Compute_RotationValue();
	_matrix matRotation = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z)));

	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vRight, matRotation.r[0] * vScale.x);
	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vUp, matRotation.r[1] * vScale.y);
	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vLook, matRotation.r[2] * vScale.z);

	Set_Position(_iIndex);

	m_pInstanceVertices[_iIndex].vLifeTime = _float2(
		Compute_LifeTime(),
		0.f
	);

	m_pInstanceVertices[_iIndex].vColor = Compute_ColorValue();
}

void CVIBuffer_Mesh_Particle::Set_Position(_int _iIndex)
{
	_vector vPosition = XMLoadFloat3(&Compute_PositionValue());

	_matrix matShape;
	matShape = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vShapeRotation.x), XMConvertToRadians(m_vShapeRotation.y), XMConvertToRadians(m_vShapeRotation.z));

	for (_int i = 0; i < 3; ++i)
	{
		matShape.r[i] *= *((_float*)(&m_vShapeScale) + i);
	}
	matShape.r[3] = XMVectorSetW(XMLoadFloat3(&m_vShapePosition), 1.f);


	switch (m_eShapeType)
	{
	case SPHERE:
	{
		vPosition += XMVector4Transform(Get_Sphere_Pos(m_ShapeDatas["Sphere_Radius"], m_ShapeDatas["Sphere_Surface"]), matShape);
		break;
	}
	case CYLINDER:
	{
		vPosition += XMVector4Transform(Get_Cylinder_Pos(m_ShapeDatas["Cylinder_Height"], m_ShapeDatas["Cylinder_Radius"], m_ShapeDatas["MidPoint"]), matShape);
		break;
	}
	case BOX:
	{
		_float3 vWHD = _float3(m_ShapeDatas["Width"], m_ShapeDatas["Height"], m_ShapeDatas["Depth"]);
		_float3 vMidPoint = _float3(m_ShapeDatas["MidX"], m_ShapeDatas["MidY"], m_ShapeDatas["MidZ"]);
		_bool isSurface = Is_ShapeData("Box_Surface");

		if (isSurface)
			vPosition += XMVector4Transform(Get_Box_Pos(vWHD, vMidPoint, isSurface, m_ShapeDatas["Box_Surface"]), matShape);
		else
			vPosition += XMVector4Transform(Get_Box_Pos(vWHD, vMidPoint, isSurface, 0.f), matShape);
		break;
	}
	case TORUS:
	{
		vPosition += XMVector4Transform(Get_Torus_Pos(m_ShapeDatas["LargeRadius"], m_ShapeDatas["HandleRadius"], m_ShapeDatas["U_Distribution"], m_ShapeDatas["V_Distribution"]), matShape);
		break;
	}
	case RING:
	{
		vPosition += XMVector4Transform(Get_Ring_Pos(m_ShapeDatas["Ring_Radius"], m_ShapeDatas["Coverage"], m_ShapeDatas["Ring_Distribution"]), matShape);
		break;
	}
	case CONE:
	{
		_float3 vAngle = _float3(m_ShapeDatas["Radial_Angle"], m_ShapeDatas["Inner_Angle"], m_ShapeDatas["Outer_Angle"]);
		vPosition += XMVector4Transform(Get_Cone_Pos(m_ShapeDatas["Flatten"], m_ShapeDatas["Length"], vAngle, m_ShapeDatas["Cone_Surface"]), matShape);
		break;
	}
	default:
		break;

	}

	vPosition = XMVectorSetW(vPosition, 1.f);
	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vTranslation, vPosition);
}

HRESULT CVIBuffer_Mesh_Particle::Initialize_Particles()
{
	for (auto& pModule : m_Modules)
	{
		if (false == pModule->Is_Init())
			continue;

		for (_int i = 0; i < m_iNumInstances; ++i)
		{
			pModule->Update_Translations(0.f, &m_pInstanceVertices[i].vTranslation, &m_pInstanceVertices[i].vVelocity, &m_pInstanceVertices[i].vAcceleration);
		}
	}


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
	
	return pInstance;
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
#ifdef _DEBUG
		Safe_Delete_Array(m_pTexcoords);
		Safe_Delete_Array(m_pTangents);
#endif
	}

#ifdef _DEBUG
	if (m_isToolReset)
	{
		Safe_Delete_Array(m_pInstanceVertices);
	}
#endif


	__super::Free();
}

#ifdef _DEBUG

HRESULT CVIBuffer_Mesh_Particle::Initialize_Prototype(ifstream& _inFile, _uint _iNumInstances, _fmatrix _PreTransformMatrix)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_inFile.read(reinterpret_cast<char*>(&m_iMaterialIndex), sizeof(_uint));

	_uint iNameLength = 0;
	_inFile.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
	_inFile.read(m_szName, iNameLength);
	m_szName[iNameLength] = '\0';

	// Mesh 정보 읽어들이기
#pragma region VERTEX_BUFFER
	m_iNumVertexBuffers = 2;
	m_iNumInstances = _iNumInstances;

	_inFile.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));

	m_iIndexStride = sizeof(_uint);
	_uint iNumFaces = 0;
	_inFile.read(reinterpret_cast<char*>(&iNumFaces), sizeof(_uint));
#ifdef _DEBUG
	m_iNumFaces = iNumFaces;
#endif

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
		Set_Instance(i);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(Initialize_Particles()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Mesh_Particle::Tool_Setting()
{
	__super::Tool_Setting();

}

void CVIBuffer_Mesh_Particle::Tool_Reset_Instance()
{
	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		Set_Instance(i);
	}

	if (FAILED(Initialize_Particles()))
	{
		MSG_BOX("Module 초기화 실패!!");
		return;
	}

	Reset_Buffers();

	m_isToolChanged = true;
}

void CVIBuffer_Mesh_Particle::Tool_Reset_Buffers()
{
	m_isToolReset = true;

	//Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Release(m_pVBInstance);


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	//m_iNumIndices = m_iNumFaces * 3 /** m_iNumInstances*/;

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = m_vecIndexBuffer[i];
		//pIndices[iNumIndices++] = m_vecIndexBuffer[i + 1];
		//pIndices[iNumIndices++] = m_vecIndexBuffer[i + 2];
	}
	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
	{
		MSG_BOX("Index Buffer 만들기 실패!!");
		Safe_Delete_Array(pIndices);
		return;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion
//
#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXMESHINSTANCE);
	m_iNumIndexCountPerInstance = m_iNumFaces * 3;

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTXMESHINSTANCE[m_iNumInstances];
	ZeroMemory(m_pInstanceVertices, m_iInstanceStride * m_iNumInstances);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		Set_Instance(i);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
	{
		MSG_BOX("Instance 버퍼 만들기 실패!!");
		return;
	}

	if (FAILED(Initialize_Particles()))
	{
		MSG_BOX("Module 초기화 실패!!");
		return;
	}

	Reset_Buffers();

	m_isToolChanged = true;

#pragma endregion
}

void CVIBuffer_Mesh_Particle::Tool_Update(_float _fTimeDelta)
{
	Tool_Adjust_Shape();
	Tool_Adjust_DefaultData();

	Tool_Modules();
}

HRESULT CVIBuffer_Mesh_Particle::Save_Buffer(json& _jsonBufferInfo)
{
	if (FAILED(__super::Save_Buffer(_jsonBufferInfo)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Mesh_Particle* CVIBuffer_Mesh_Particle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ifstream& _inFile, _uint _iNumInstances, _fmatrix _PreTransformMatrix)
{
	CVIBuffer_Mesh_Particle* pInstance = new CVIBuffer_Mesh_Particle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_inFile, _iNumInstances, _PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Mesh_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

#endif