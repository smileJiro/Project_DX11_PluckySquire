#include "VIBuffer_Point_Particle.h"
#include "GameInstance.h"
#include "Particle_Module.h"

CVIBuffer_Point_Particle::CVIBuffer_Point_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Point_Particle::CVIBuffer_Point_Particle(const CVIBuffer_Point_Particle& _Prototype)
	: CVIBuffer_Instance(_Prototype)
	, m_pInstanceVertices(_Prototype.m_pInstanceVertices)
	, m_isAnim(_Prototype.m_isAnim)
	, m_isRandomUV(_Prototype.m_isRandomUV)
	, m_fStartIndex(_Prototype.m_fStartIndex)
	, m_fAnimTime(_Prototype.m_fAnimTime)
	, m_fAnimCount(_Prototype.m_fAnimCount)
	, m_vUVPerAnim(_Prototype.m_vUVPerAnim)
{
}

HRESULT CVIBuffer_Point_Particle::Initialize_Prototype(const json& _jsonBufferInfo)
{
	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iNumInstances = _jsonBufferInfo["Count"];

	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = m_iNumInstances;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);	

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices->vPosition = XMFLOAT3(0.0f, 0.0f, 0.f);
	pVertices->vSize = XMFLOAT2(1.f, 1.f);

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);


	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
	m_iNumIndexCountPerInstance = 6;

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstances];
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

	// UV 설정
	if (false == _jsonBufferInfo.contains("UVSet"))
		return E_FAIL;

	m_isAnim = _jsonBufferInfo["UVSet"]["Anim"];
	m_isRandomUV = _jsonBufferInfo["UVSet"]["Random"];
	m_vUVPerAnim.x = _jsonBufferInfo["UVSet"]["UV"][0];
	m_vUVPerAnim.y = _jsonBufferInfo["UVSet"]["UV"][1];
	m_fStartIndex = _jsonBufferInfo["UVSet"]["Start"];
	m_fAnimCount = _jsonBufferInfo["UVSet"]["Count"];

	if (m_isAnim && false == _jsonBufferInfo["UVSet"].contains("Time"))
	{
		m_fAnimTime = _jsonBufferInfo["UVSet"]["Time"];
	}

	// Shape Data 설정
	if (FAILED(Set_ShapeData(_jsonBufferInfo)))
		return E_FAIL;	


	for (size_t i = 0; i < m_iNumInstances; i++)
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



HRESULT CVIBuffer_Point_Particle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;


	return S_OK;
}

void CVIBuffer_Point_Particle::Update(_float _fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (_int i = 0; i < m_iNumInstances; i++)
	{
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

	m_pContext->Unmap(m_pVBInstance, 0);

}

void CVIBuffer_Point_Particle::Set_UV(_Out_ _float4* _pOutUV, _float _fIndex)
{
	if (nullptr == _pOutUV)
		return;

	_float fLeft = m_vUVPerAnim.x * (_fIndex);
	_float fTop = m_vUVPerAnim.y * (_int)(fLeft / 1.f);
	
	fLeft = fLeft - (_int)fLeft;

	*_pOutUV = _float4(fLeft, fTop, fLeft + m_vUVPerAnim.x, fTop + m_vUVPerAnim.y);
}

void CVIBuffer_Point_Particle::Set_Position(_int _iIndex)
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

	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vTranslation, vPosition);
}

void CVIBuffer_Point_Particle::Set_Instance(_int _iIndex)
{
	// TODO : 추가적인 Transform 설정이 필요하다
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

	// UV Logic 설정
	if (m_isRandomUV)
	{

	}
	else
	{
		Set_UV(&m_pInstanceVertices[_iIndex].vUV, 0);
	}



	m_pInstanceVertices[_iIndex].vColor = Compute_ColorValue();
}

HRESULT CVIBuffer_Point_Particle::Initialize_Particles()
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



CVIBuffer_Point_Particle* CVIBuffer_Point_Particle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonBufferInfo)
{
	CVIBuffer_Point_Particle* pInstance = new CVIBuffer_Point_Particle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_jsonBufferInfo)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Particle::Clone(void* _pArg)
{
	CComponent* pInstance = new CVIBuffer_Point_Particle(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Particle::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
	}

#ifdef _DEBUG
	if (m_isToolReset)
	{
		Safe_Delete_Array(m_pInstanceVertices);
	}
#endif

}

#ifdef _DEBUG
void CVIBuffer_Point_Particle::Tool_Setting()
{
	__super::Tool_Setting();

}
void CVIBuffer_Point_Particle::Tool_Reset_Instance()
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

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstances; i++)
	{
		pVertices[i] = m_pInstanceVertices[i];
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}
void CVIBuffer_Point_Particle::Tool_Reset_Buffers()
{
	m_isToolReset = true;

	Safe_Release(m_pIB);
	Safe_Release(m_pVBInstance);


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_iNumIndices = m_iNumInstances;

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);


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

#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
	m_iNumIndexCountPerInstance = 6;

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstances];
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


#pragma endregion

}

void CVIBuffer_Point_Particle::Tool_Update(_float _fTimeDelta)
{
	Tool_Adjust_Shape();
	Tool_Adjust_DefaultData();

	Tool_Modules();


}

// Buffer 정보 저장
HRESULT CVIBuffer_Point_Particle::Save_Buffer(json& _jsonBufferInfo)
{
	if (FAILED(__super::Save_Buffer(_jsonBufferInfo)))
		return E_FAIL;

	_jsonBufferInfo["UVSet"]["Anim"] = m_isAnim;
	_jsonBufferInfo["UVSet"]["Random"] = m_isRandomUV;
	_jsonBufferInfo["UVSet"]["Count"] = m_fAnimCount;
	_jsonBufferInfo["UVSet"]["Start"] = m_fStartIndex;
	_jsonBufferInfo["UVSet"]["Time"] = m_fAnimTime;

	_jsonBufferInfo["UVSet"]["UV"].push_back(m_vUVPerAnim.x);
	_jsonBufferInfo["UVSet"]["UV"].push_back(m_vUVPerAnim.y);


	return S_OK;

}

// m_pVBInstance까지 Create 실행합니다.
HRESULT CVIBuffer_Point_Particle::Initialize_Prototype(_uint _iNumInstances)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iNumInstances = _iNumInstances;

	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iIndexStride = sizeof(_ushort);
	m_iNumIndices = m_iNumInstances;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices->vPosition = XMFLOAT3(0.0f, 0.0f, 0.f);
	pVertices->vSize = XMFLOAT2(1.f, 1.f);

	ZeroMemory(&m_SubResourceDesc, sizeof m_SubResourceDesc);
	m_SubResourceDesc.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);


	ZeroMemory(&m_SubResourceDesc, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceDesc.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
	m_iNumIndexCountPerInstance = 6;

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);

	/* 정점버퍼를 몇 바이트 할당할까요? */
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstances];
	ZeroMemory(m_pInstanceVertices, m_iInstanceStride * m_iNumInstances);


	for (size_t i = 0; i < m_iNumInstances; i++)
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
CVIBuffer_Point_Particle* CVIBuffer_Point_Particle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iNumInstances)
{
	CVIBuffer_Point_Particle* pInstance = new CVIBuffer_Point_Particle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iNumInstances)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

#endif