#include "VIBuffer_Point_Particle.h"
#include "GameInstance.h"
#include "Translation_Module.h"
#include "Keyframe_Module.h"

#include "Compute_Shader.h"

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

HRESULT CVIBuffer_Point_Particle::Initialize_Prototype(const json& _jsonBufferInfo, _float _fSpawnRate)
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



	for (_uint i = 0; i < m_iNumInstances; i++)
	{
		Set_Instance(i, _fSpawnRate);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion




	return S_OK;
}

HRESULT CVIBuffer_Point_Particle::Initialize_Module(CEffect_Module* _pModule)
{
	CEffect_Module::DATA_APPLY eData = _pModule->Get_ApplyType();
	CEffect_Module::MODULE_TYPE eType = _pModule->Get_Type();

	if (CEffect_Module::MODULE_TYPE::MODULE_KEYFRAME == eType)
	{
		if (CEffect_Module::DATA_APPLY::COLOR == eData)
		{
			_pModule->Update_ColorKeyframe((_float*)m_pInstanceVertices, m_iNumInstances, 18, 16, 33);
		}
	}
	else if (CEffect_Module::MODULE_TYPE::MODULE_TRANSLATION == eType)
	{
		if (CEffect_Module::DATA_APPLY::TRANSLATION == eData)
		{
			_pModule->Update_Translations(0.f, (_float*)m_pInstanceVertices, m_iNumInstances, 12, 26, 29, 16, 33);
		}
	}


	return S_OK;
}



HRESULT CVIBuffer_Point_Particle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	D3D11_BUFFER_DESC BufferDesc = {};
	ZeroMemory(&BufferDesc, sizeof D3D11_BUFFER_DESC);

	BufferDesc.ByteWidth = m_iNumInstances * m_iInstanceStride;
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | // Compute Shader
		D3D11_BIND_SHADER_RESOURCE;   // Vertex Shader
	BufferDesc.StructureByteStride = m_iInstanceStride;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA BufferData = {};
	ZeroMemory(&BufferData, sizeof D3D11_SUBRESOURCE_DATA);
	BufferData.pSysMem = m_pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &BufferData, &m_pBuffer)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	ZeroMemory(&UAVDesc, sizeof D3D11_UNORDERED_ACCESS_VIEW_DESC);
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.NumElements = m_iNumInstances;
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBuffer, &UAVDesc, &m_pUAV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.BufferEx.NumElements = m_iNumInstances;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBuffer, &SRVDesc, &m_pSRV)))
		return E_FAIL;	



	ZeroMemory(&BufferDesc, sizeof D3D11_BUFFER_DESC);
	m_pBuffer->GetDesc(&BufferDesc);

	m_pBufferInitial = nullptr;
	HRESULT hr = m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pBufferInitial);
	if (FAILED(hr))
		return hr;
	m_pContext->CopyResource(m_pBufferInitial, m_pBuffer);

	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.NumElements = m_iNumInstances;

	hr = m_pDevice->CreateShaderResourceView(m_pBufferInitial, &SRVDesc, &m_pSRVInitial);
	if (FAILED(hr))
		return E_FAIL;


	Reset_Buffers();

	return S_OK;
}

void CVIBuffer_Point_Particle::Begin_Update(_float _fTimeDelta)
{
	//m_fAccSpawnTime += _fTimeDelta;


	//D3D11_MAPPED_SUBRESOURCE		SubResource{};
	//m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	//m_pUpdateVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	//if (SPAWN == m_eSpawnType)
	//{
	//	while (m_fSpawnTime <= m_fAccSpawnTime)
	//	{
	//		// TODO: 임시 코드입니다.
	//		if (m_pUpdateVertices[m_iSpawnIndex].vLifeTime.y > 0.01f)
	//		{
	//			m_iSpawnIndex = (m_iSpawnIndex + 1) % m_iNumInstances;
	//			m_fAccSpawnTime -= m_fSpawnTime;
	//			continue;
	//		}
	//		// 시간이 충족되면 Spawn합니다.
	//		m_pUpdateVertices[m_iSpawnIndex] = m_pInstanceVertices[m_iSpawnIndex];
	//		// LifeTime의 설정은 Spawn시기에 맞게 설정합니다.
	//		m_pUpdateVertices[m_iSpawnIndex].vLifeTime.y = m_fAccSpawnTime - m_fSpawnTime;

	//		if (m_pSpawnMatrix)
	//		{
	//			_matrix matSpawn = XMLoadFloat4x4(m_pSpawnMatrix);
	//			for (_int i = 0; i < 3; ++i)
	//				matSpawn.r[i] = XMVector3Normalize(matSpawn.r[i]);

	//			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vRight, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vRight), matSpawn));
	//			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vUp, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vUp), matSpawn));
	//			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vLook, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vLook), matSpawn));
	//			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vTranslation), matSpawn));
	//		}

	//		m_iSpawnIndex = (m_iSpawnIndex + 1) % m_iNumInstances;
	//		m_fAccSpawnTime -= m_fSpawnTime;
	//	}
	//}
	//else if (BURST == m_eSpawnType)
	//{
	//}
}

void CVIBuffer_Point_Particle::Spawn_Burst(_float _fTimeDelta, const _float4x4* _pSpawnMatrix)
{
	for (_int i = 0; i < m_iNumInstances; i++)
	{
		m_pUpdateVertices[i] = m_pInstanceVertices[i];
		m_pUpdateVertices[i].vLifeTime.y = 0.f;

		if (_pSpawnMatrix)
		{
			_matrix matSpawn = XMLoadFloat4x4(_pSpawnMatrix);
			for (_int i = 0; i < 3; ++i)
				matSpawn.r[i] = XMVector3Normalize(matSpawn.r[i]);

			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vRight, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vRight), matSpawn));
			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vUp, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vUp), matSpawn));
			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vLook, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vLook), matSpawn));
			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vTranslation), matSpawn));
		}
	}
	
	//m_iSpawnIndex = m_iNumInstances;
}

void CVIBuffer_Point_Particle::Spawn_Rate(_float _fTimeDelta, _float _fSpawnRate, const _float4x4* _pSpawnMatrix)
{
	_float fSpawnThreshold = 60.f / max(_fSpawnRate, 0.01f);
	m_fSpawnRemainTime += _fTimeDelta;

	while (fSpawnThreshold <= m_fSpawnRemainTime)
	{
		// Pooling 혹은 Kill 여부에 따라
		// Kill = -1 * FloatMax, Pool = 0.f
		if (m_pUpdateVertices[m_iSpawnIndex].vLifeTime.y < 0.f)
		{
			m_iSpawnIndex = (m_iSpawnIndex + 1) % m_iNumInstances;
			m_fSpawnRemainTime -= fSpawnThreshold;
			continue;
		}

		// 시간이 충족되면 Spawn합니다.
		m_pUpdateVertices[m_iSpawnIndex] = m_pInstanceVertices[m_iSpawnIndex];
		// LifeTime의 설정은 Spawn시기에 맞게 설정합니다.
		m_pUpdateVertices[m_iSpawnIndex].vLifeTime.y = m_fSpawnRemainTime - fSpawnThreshold;

		if (_pSpawnMatrix)
		{
			_matrix matSpawn = XMLoadFloat4x4(_pSpawnMatrix);
			for (_int i = 0; i < 3; ++i)
				matSpawn.r[i] = XMVector3Normalize(matSpawn.r[i]);

			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vRight, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vRight), matSpawn));
			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vUp, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vUp), matSpawn));
			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vLook, XMVector3TransformNormal(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vLook), matSpawn));
			XMStoreFloat4(&m_pUpdateVertices[m_iSpawnIndex].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&m_pUpdateVertices[m_iSpawnIndex].vTranslation), matSpawn));
		}

		m_iSpawnIndex = (m_iSpawnIndex + 1) % m_iNumInstances;
		m_fSpawnRemainTime -= fSpawnThreshold;
	}
}

void CVIBuffer_Point_Particle::Update_Buffer(_float _fTimeDelta, _bool _isPooling)
{
	for (_int i = 0; i < m_iNumInstances; i++)
	{
		if (0.f < m_pUpdateVertices[i].vLifeTime.y)
		{
			m_pUpdateVertices[i].vLifeTime.y += _fTimeDelta;

			XMStoreFloat3(&m_pUpdateVertices[i].vVelocity, XMLoadFloat3(&m_pUpdateVertices[i].vVelocity) + XMLoadFloat3(&m_pUpdateVertices[i].vAcceleration) * _fTimeDelta);
			XMStoreFloat4(&m_pUpdateVertices[i].vTranslation, XMLoadFloat4(&m_pUpdateVertices[i].vTranslation) + XMLoadFloat3(&m_pUpdateVertices[i].vVelocity) * _fTimeDelta);

			if (m_pUpdateVertices[i].vLifeTime.y > m_pUpdateVertices[i].vLifeTime.x)
			{
				m_pUpdateVertices[i] = m_pInstanceVertices[i];
				ZeroMemory(&m_pUpdateVertices[i].vRight, sizeof(_float4));
				ZeroMemory(&m_pUpdateVertices[i].vUp, sizeof(_float4));
				ZeroMemory(&m_pUpdateVertices[i].vLook, sizeof(_float4));

				if (false == _isPooling)
				{
					m_pUpdateVertices[i].vLifeTime.y = D3D11_FLOAT32_MAX * -1.f;
				}

			}
		}

	}

}

void CVIBuffer_Point_Particle::End_Update(_float _fTimeDelta)
{
	

	m_pContext->Unmap(m_pVBInstance, 0);

}


void CVIBuffer_Point_Particle::Reset_Buffers()
{
	//m_iSpawnIndex = 0;
	////m_fSpawnRemainTime = 0.f;

	//D3D11_MAPPED_SUBRESOURCE		SubResource{};

	//m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	//VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	//for (size_t i = 0; i < m_iNumInstances; i++)
	//{
	//	// TODO : 
	//	//if (SPAWN == m_eSpawnType)
	//	{
	//		pVertices[i].vLifeTime.y = 0.f;
	//		ZeroMemory(&pVertices[i].vRight, sizeof(_float4));
	//		ZeroMemory(&pVertices[i].vUp, sizeof(_float4));
	//		ZeroMemory(&pVertices[i].vLook, sizeof(_float4));
	//	}
	//	//else if (BURST == m_eSpawnType)
	//	//{
	//	//	pVertices[i] = m_pInstanceVertices[i];
	//	//	pVertices[i].vLifeTime.y = 0.f;
	//	//}
	//}

	//m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Particle::Begin_Compute(CCompute_Shader* _pCShader)
{
	_pCShader->Set_SRVs(&m_pSRVInitial, 1);
	_pCShader->Set_UAVs(&m_pUAV, 1);
}

void CVIBuffer_Point_Particle::Update_All(_float _fTimeDelta, CCompute_Shader* _pCShader)
{
	_pCShader->Compute((_uint)(ceil(m_iNumInstances / 256.f)), 1, 1);
}



void CVIBuffer_Point_Particle::Update_Translation(_float _fTimeDelta, CEffect_Module* _pTranslationModule)
{
	_pTranslationModule->Update_Translations(_fTimeDelta, (_float*)m_pUpdateVertices, m_iNumInstances, 12, 26, 29, 16, 32);
	//for (_int i = 0; i < m_iNumInstances; ++i)
	//	_pTranslationModule->Update_Translations(_fTimeDelta, &m_pUpdateVertices[i].vTranslation, &m_pUpdateVertices[i].vVelocity, &m_pUpdateVertices[i].vAcceleration);
}

void CVIBuffer_Point_Particle::Update_Translation(_float _fTimeDelta, CEffect_Module* _pTranslationModule, CCompute_Shader* _pCShader)
{
	_int iEntry = _pTranslationModule->Update_Translation(_fTimeDelta, _pCShader);
	if (0 <= iEntry)
	{
		_pCShader->Begin(iEntry);
		Begin_Compute(_pCShader);
		_pCShader->Compute((_uint)(ceil(m_iNumInstances / 256.f)), 1, 1);
		_pCShader->End_Compute();
	}

}

void CVIBuffer_Point_Particle::Update_ColorKeyframe(CEffect_Module* _pColorModule)
{
	_pColorModule->Update_ColorKeyframe((_float*)m_pUpdateVertices, m_iNumInstances, 18, 16, 32);
}

void CVIBuffer_Point_Particle::Update_ScaleKeyframe(CEffect_Module* _pColorModule)
{
	_pColorModule->Update_ScaleKeyframe((_float*)m_pUpdateVertices, m_iNumInstances, 0, 4, 8, 16, 32);

}

void CVIBuffer_Point_Particle::Spawn_Rate(CCompute_Shader* _pCShader)
{
	_pCShader->Compute((_uint)(ceil(m_iNumInstances / 256.f)), 1, 1);
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
	vPosition = XMVectorSetW(vPosition, 1.f);
	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vTranslation, vPosition);
}

void CVIBuffer_Point_Particle::Set_Instance(_int _iIndex, _float _fSpawnRate)
{
	_float3 vScale = Compute_ScaleValue();
	_float3 vRotation = Compute_RotationValue();
	_matrix matRotation = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z)));

	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vRight, matRotation.r[0] * vScale.x);
	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vUp, matRotation.r[1] * vScale.y);
	XMStoreFloat4(&m_pInstanceVertices[_iIndex].vLook, matRotation.r[2] * vScale.z);

	Set_Position(_iIndex);

	_float fSpawnTime = 60.f / max(_fSpawnRate, 0.01f);

	m_pInstanceVertices[_iIndex].vLifeTime = _float2(
		Compute_LifeTime(),
		0.f - fSpawnTime * _iIndex
	);

	// UV Logic 설정
	if (m_isRandomUV)
	{

	}
	else
	{
		Set_UV(&m_pInstanceVertices[_iIndex].vUV, 0);
	}

	m_pInstanceVertices[_iIndex].fAlive = 0.f;

	m_pInstanceVertices[_iIndex].vColor = Compute_ColorValue();
}




CVIBuffer_Point_Particle* CVIBuffer_Point_Particle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonBufferInfo, _float _fSpawnRate)
{
	CVIBuffer_Point_Particle* pInstance = new CVIBuffer_Point_Particle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_jsonBufferInfo, _fSpawnRate)))
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



	__super::Free();
}

#ifdef _DEBUG
void CVIBuffer_Point_Particle::Tool_Setting()
{
	__super::Tool_Setting();

}
void CVIBuffer_Point_Particle::Tool_Reset_Instance(_float _fSpawnRate)
{
	for (_uint i = 0; i < m_iNumInstances; i++)
	{
		Set_Instance(i, _fSpawnRate);
	}

	// TODO: Initialize 하는 부분
	/*if (FAILED(Initialize_Particles()))
	{
		MSG_BOX("Module 초기화 실패!!");
		return;
	}*/
	
	Reset_Buffers();

	m_isToolChanged = true;

	//D3D11_MAPPED_SUBRESOURCE		SubResource{};
	//m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	//VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	//for (size_t i = 0; i < m_iNumInstances; i++)
	//{
	//	pVertices[i] = m_pInstanceVertices[i];
	//}

	//m_pContext->Unmap(m_pVBInstance, 0);
}
void CVIBuffer_Point_Particle::Tool_Reset_Buffers(_float _fSpawnRate)
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

	for (_uint i = 0; i < m_iNumInstances; i++)
	{
		Set_Instance(i, _fSpawnRate);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
	{
		MSG_BOX("Instance 버퍼 만들기 실패!!");
		return;
	}


	Reset_Buffers();

	m_isToolChanged = true;

#pragma endregion

}

void CVIBuffer_Point_Particle::Tool_Update(_float _fTimeDelta)
{
	Tool_Adjust_Shape();
	Tool_Adjust_DefaultData();


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
HRESULT CVIBuffer_Point_Particle::Initialize_Prototype(_uint _iNumInstances, _float _fSpawnRate)
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


	for (_uint i = 0; i < m_iNumInstances; i++)
	{
		Set_Instance(i, _fSpawnRate);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;


	return S_OK;
}
CVIBuffer_Point_Particle* CVIBuffer_Point_Particle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iNumInstances, _float _fSpawnRate)
{
	CVIBuffer_Point_Particle* pInstance = new CVIBuffer_Point_Particle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iNumInstances, _fSpawnRate)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

#endif