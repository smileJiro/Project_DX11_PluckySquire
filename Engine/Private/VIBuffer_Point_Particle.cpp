#include "VIBuffer_Point_Particle.h"
#include "GameInstance.h"

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
	if (FAILED(Set_Float3Data(nullptr, P_SCALE, _jsonBufferInfo, "Scale", m_pSetScales)))
		return E_FAIL;

	// Rotation 설정
	if (FAILED(Set_Float3Data(nullptr, P_ROTATION, _jsonBufferInfo, "Rotation", m_pSetRotations)))
		return E_FAIL;

	// Position 설정
	if (FAILED(Set_Float3Data(nullptr, P_POSITION, _jsonBufferInfo, "Position", m_pSetPositions)))
		return E_FAIL;

	// LifeTime 설정
	if (FAILED(Set_FloatData(nullptr, P_LIFETIME, _jsonBufferInfo, "LifeTime", m_pSetLifeTimes)))
		return E_FAIL;

	// Color 설정
	if (FAILED(Set_Float4Data(nullptr, P_COLOR, _jsonBufferInfo, "Color", m_pSetColors)))
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
		_vector vPosition = XMLoadFloat3(&Compute_Float3Value(m_SetDatas[P_POSITION], m_pSetPositions[0], m_pSetPositions[1]));
		switch (m_eShapeType)
		{
		case SPHERE:
		{
			vPosition += Get_Sphere_Pos(m_ShapeDatas["Radius"], m_ShapeDatas["Surface"]);
			break;
		}
		case CYLINDER:
		{
			vPosition += Get_Cylinder_Pos(m_ShapeDatas["Height"], m_ShapeDatas["Radius"], m_ShapeDatas["MidPoint"]);
			break;
		}
		case BOX:
		{
			_float3 vWHD = _float3(m_ShapeDatas["Width"], m_ShapeDatas["Height"], m_ShapeDatas["Depth"]);
			_float3 vMidPoint = _float3(m_ShapeDatas["MidX"], m_ShapeDatas["MidY"], m_ShapeDatas["MidZ"]);
			_bool isSurface = Is_Data("Surface");

			if (isSurface)
				vPosition += Get_Box_Pos(vWHD, vMidPoint, isSurface, m_ShapeDatas["Surface"]);
			else
				vPosition += Get_Box_Pos(vWHD, vMidPoint, isSurface, 0.f);
			break;
		}
		case TORUS:
		{
			vPosition += Get_Torus_Pos(m_ShapeDatas["LargeRadius"], m_ShapeDatas["HandleRadius"], m_ShapeDatas["U_Distribution"], m_ShapeDatas["V_Distribution"]);
			break;
		}
		case RING:
		{
			vPosition += Get_Ring_Pos(m_ShapeDatas["Radius"], m_ShapeDatas["Coverage"], m_ShapeDatas["U_Distribution"]);
			break;
		}
		case CONE:
		{
			_float3 vAngle = _float3(m_ShapeDatas["Radial_Angle"], m_ShapeDatas["Inner_Angle"], m_ShapeDatas["Outer_Angle"]);
			vPosition += Get_Cone_Pos(m_ShapeDatas["Flatten"], m_ShapeDatas["Length"], vAngle, m_ShapeDatas["Surface"]);
			break;
		}
		default:
			break;
		
	}
		// TODO : 추가적인 Transform 설정이 필요하다
		_float3 vScale = Compute_Float3Value(m_SetDatas[P_SCALE], m_pSetScales[0], m_pSetScales[1]);
		_float3 vRotation = Compute_Float3Value(m_SetDatas[P_ROTATION], m_pSetRotations[0], m_pSetRotations[1]);
		_matrix matRotation = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z)));
		
		XMStoreFloat4(&m_pInstanceVertices[i].vRight, matRotation.r[0] * vScale.x);
		XMStoreFloat4(&m_pInstanceVertices[i].vUp, matRotation.r[1] * vScale.y);
		XMStoreFloat4(&m_pInstanceVertices[i].vLook, matRotation.r[2] * vScale.z);
		XMStoreFloat4(&m_pInstanceVertices[i].vTranslation, XMVector4Transform(vPosition, XMLoadFloat4x4(&m_ShapeMatrix)));


		m_pInstanceVertices[i].vLifeTime = _float2(
			Compute_FloatValue(m_SetDatas[P_LIFETIME], m_pSetLifeTimes[0], m_pSetLifeTimes[1]),
			0.f
		);

		// UV Logic 설정
		if (m_isRandomUV)
		{
			
		}
		else
		{
			Set_UV(&m_pInstanceVertices[i].vUV, 0);
		}



		m_pInstanceVertices[i].vColor = Compute_Float4Value(m_SetDatas[P_COLOR], m_pSetColors[0], m_pSetColors[1]);
	}

	ZeroMemory(&m_InstanceInitialDesc, sizeof m_InstanceInitialDesc);
	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Particle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
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
}
