#include "VIBuffer_Instance.h"
#include "GameInstance.h"


CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& _Prototype)
	: CVIBuffer(_Prototype)
	, m_InstanceBufferDesc(_Prototype.m_InstanceBufferDesc)
	, m_InstanceInitialDesc(_Prototype.m_InstanceInitialDesc)
	, m_iNumInstances(_Prototype.m_iNumInstances)
	, m_iNumIndexCountPerInstance(_Prototype.m_iNumIndexCountPerInstance)
	, m_iInstanceStride(_Prototype.m_iInstanceStride)

	, m_eShapeType(_Prototype.m_eShapeType)
	, m_ShapeDatas(_Prototype.m_ShapeDatas)
	, m_SetDatas(_Prototype.m_SetDatas)
	, m_pSetScales(_Prototype.m_pSetScales)
	, m_pSetRotations(_Prototype.m_pSetRotations)
	, m_pSetPositions(_Prototype.m_pSetPositions)
	, m_pSetLifeTimes(_Prototype.m_pSetLifeTimes)
	, m_pSetColors(_Prototype.m_pSetColors)
	, m_ShapeMatrix(_Prototype.m_ShapeMatrix)
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	m_SetDatas.resize(DATAEND, UNSET);

	m_pSetScales = new _float3[2];
	m_pSetRotations = new _float3[2];
	m_pSetPositions = new _float3[2];
	m_pSetLifeTimes = new _float[2];
	m_pSetColors = new _float4[2];


	XMStoreFloat4x4(&m_ShapeMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* _pArg)
{
	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Render()
{
	m_pContext->DrawIndexedInstanced(m_iNumIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_BufferDesc()
{
	ID3D11Buffer* pVertexBuffer[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	// 시작 슬롯, Vertex Buffer의 개수, Vertex Buffer 배열 주소, 각 Vertex Buffer byte 크기 주소, 오프셋
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
	// Index Buffer, Index Buffer의 크기, 오프셋
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	// 형상 (삼각형, 점,,,)
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}



HRESULT CVIBuffer_Instance::Set_ShapeData(const json& _jsonInfo)
{
	if (false == _jsonInfo.contains("ShapeLocation") && false == _jsonInfo["ShapeLocation"].contains("Shape"))
		return E_FAIL;

	m_eShapeType = _jsonInfo["ShapeLocation"]["Shape"];
	
	switch (m_eShapeType)
	{
	case SPHERE:
	{
		if (false == _jsonInfo["ShapeLocation"].contains("Radius"))
			return E_FAIL;
		m_ShapeDatas.emplace("Radius", _jsonInfo["ShapeLocation"]["Radius"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Surface"))
			return E_FAIL;
		m_ShapeDatas.emplace("Surface", _jsonInfo["ShapeLocation"]["Surface"]);

		break;
	}
	case CYLINDER:
	{
		if (false == _jsonInfo["ShapeLocation"].contains("Radius"))
			return E_FAIL;
		m_ShapeDatas.emplace("Radius", _jsonInfo["ShapeLocation"]["Radius"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Height"))
			return E_FAIL;
		m_ShapeDatas.emplace("Height", _jsonInfo["ShapeLocation"]["Height"]);

		if (false == _jsonInfo["ShapeLocation"].contains("MidPoint"))
			return E_FAIL;
		m_ShapeDatas.emplace("MidPoint", _jsonInfo["ShapeLocation"]["MidPoint"]);

		break;
	}
	case BOX:
	{
		if (false == _jsonInfo["ShapeLocation"].contains("Width"))
			return E_FAIL;
		m_ShapeDatas.emplace("Width", _jsonInfo["ShapeLocation"]["Width"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Height"))
			return E_FAIL;
		m_ShapeDatas.emplace("Height", _jsonInfo["ShapeLocation"]["Height"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Depth"))
			return E_FAIL;
		m_ShapeDatas.emplace("Depth", _jsonInfo["ShapeLocation"]["Depth"]);

		if (false == _jsonInfo["ShapeLocation"].contains("MidX"))
			return E_FAIL;
		m_ShapeDatas.emplace("MidX", _jsonInfo["ShapeLocation"]["MidX"]);

		if (false == _jsonInfo["ShapeLocation"].contains("MidY"))
			return E_FAIL;
		m_ShapeDatas.emplace("MidY", _jsonInfo["ShapeLocation"]["MidY"]);

		if (false == _jsonInfo["ShapeLocation"].contains("MidZ"))
			return E_FAIL;
		m_ShapeDatas.emplace("MidZ", _jsonInfo["ShapeLocation"]["MidZ"]);

		if (_jsonInfo["ShapeLocation"].contains("Surface"))
		{
			m_ShapeDatas.emplace("Surface", _jsonInfo["ShapeLocation"]["Surface"]);
		}
		break;
	}
	case TORUS:
	{
		if (false == _jsonInfo["ShapeLocation"].contains("LargeRadius"))
			return E_FAIL;
		m_ShapeDatas.emplace("LargeRadius", _jsonInfo["ShapeLocation"]["LargeRadius"]);

		if (false == _jsonInfo["ShapeLocation"].contains("HandleRadius"))
			return E_FAIL;
		m_ShapeDatas.emplace("HandleRadius", _jsonInfo["ShapeLocation"]["HandleRadius"]);

		if (false == _jsonInfo["ShapeLocation"].contains("U_Distribution"))
			return E_FAIL;
		m_ShapeDatas.emplace("U_Distribution", clamp((_float)(_jsonInfo["ShapeLocation"]["U_Distribution"]), 0.f, 1.f));

		if (false == _jsonInfo["ShapeLocation"].contains("V_Distribution"))
			return E_FAIL;
		m_ShapeDatas.emplace("V_Distribution", clamp((_float)(_jsonInfo["ShapeLocation"]["V_Distribution"]), 0.f, 1.f));

		break;
	}
	case RING:
	{
		if (false == _jsonInfo["ShapeLocation"].contains("Radius"))
			return E_FAIL;
		m_ShapeDatas.emplace("Radius", _jsonInfo["ShapeLocation"]["Radius"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Coverage"))
			return E_FAIL;
		m_ShapeDatas.emplace("Coverage", clamp((_float)_jsonInfo["ShapeLocation"]["Coverage"], 0.f, 1.f));

		if (false == _jsonInfo["ShapeLocation"].contains("U_Distribution"))
			return E_FAIL;
		m_ShapeDatas.emplace("U_Distribution", clamp((_float)_jsonInfo["ShapeLocation"]["U_Distribution"], 0.f, 1.f));


		break;
	}
	case CONE:
	{
		if (false == _jsonInfo["ShapeLocation"].contains("Length"))
			return E_FAIL;
		m_ShapeDatas.emplace("Length", _jsonInfo["ShapeLocation"]["Length"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Outer_Angle"))
			return E_FAIL;
		m_ShapeDatas.emplace("Outer_Angle", _jsonInfo["ShapeLocation"]["Outer_Angle"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Inner_Angle"))
			return E_FAIL;
		m_ShapeDatas.emplace("Inner_Angle", _jsonInfo["ShapeLocation"]["Inner_Angle"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Radial_Angle"))
			return E_FAIL;
		m_ShapeDatas.emplace("Radial_Angle", _jsonInfo["ShapeLocation"]["Radial_Angle"]);

		if (false == _jsonInfo["ShapeLocation"].contains("Surface"))
			return E_FAIL;
		m_ShapeDatas.emplace("Surface", clamp((_float)(_jsonInfo["ShapeLocation"]["Surface"]), 0.f, 1.f));

		if (false == _jsonInfo["ShapeLocation"].contains("Flatten"))
			return E_FAIL;
		m_ShapeDatas.emplace("Flatten", _jsonInfo["ShapeLocation"]["Flatten"]);

		break;
	}
	case SHAPE_NONE:
	{
		break;
	}

	}

	if (false == _jsonInfo["ShapeLocation"].contains("Matrix") && 16 > _jsonInfo["ShapeLocation"]["Matrix"].size())
		return E_FAIL;

	for (_int i = 0; i < 16; ++i)
	{
		*((_float*)(&m_ShapeMatrix) + i) = _jsonInfo["ShapeLocation"]["Matrix"][i];
	}

	return S_OK;
}

_vector  CVIBuffer_Instance::Get_Sphere_Pos(_float _fRadius, _float _fSurface)
{
	/* fSurface : 중앙에서 해당 비율만큼 떨어진 곳에서부터 만들어진다.
		0 = 중앙에서부터, 1 = 무조건 표면에서만 생성 */
	_vector vReturn;
	_fSurface = clamp(_fSurface, 0.f, 1.f);

	vReturn = XMVectorSetW(XMVector3Normalize(XMVectorSet(m_pGameInstance->Compute_Random(-1.f, 1.f), m_pGameInstance->Compute_Random(-1.f, 1.f), m_pGameInstance->Compute_Random(-1.f, 1.f), 0.f))
		* _fRadius * m_pGameInstance->Compute_Random(_fSurface, 1.f), 1.f);

	return vReturn;
}

_vector CVIBuffer_Instance::Get_Cylinder_Pos(_float _fHeight, _float _fRadius, _float _fMidPoint)
{
	/* fMidPoint : 중앙점, 0 = 원기둥의 바닥, 1 = 원기둥의 윗면 */
	_vector vReturn;

	_float fOutY = m_pGameInstance->Compute_Random(0.f, _fHeight) - _fHeight * _fMidPoint;
	_vector vBase = XMVector3Normalize(XMVectorSet(m_pGameInstance->Compute_Random(-1.f, 1.f), 0.f, m_pGameInstance->Compute_Random(-1.f, 1.f), 0.f))
		* _fRadius * m_pGameInstance->Compute_Random_Normal();

	vReturn = XMVectorSetW(XMVectorSetY(vBase, fOutY), 1.f);

	return vReturn;
}

_vector CVIBuffer_Instance::Get_Box_Pos(const _float3& _vWHD, const _float3& _vMidPoint, _bool _isSurface, _float _fSurface)
{	
	/* fSurface : 표면에서 그려지는데, 두께는 어느정도인가 ? */
	/* fMidPoint : 중점으로부터 얼마만큼 떨어져 있는가 ? */
	/* vWHD : Widht, Height, Depth */

	_vector vReturn;
	_float fX, fY, fZ;

	if (_isSurface)
	{
		fX = m_pGameInstance->Compute_Random(0.f, _vWHD.x);
		fY = m_pGameInstance->Compute_Random(0.f, _vWHD.y);
		fZ = m_pGameInstance->Compute_Random(0.f, _vWHD.z);
		_int iRandValue = rand() % 3;
		if (0 == iRandValue)
		{
			iRandValue = rand() % 2;
			if (iRandValue)
				fX = _vWHD.x;
			else
				fX = 0.f;

		}
		else if (1 == iRandValue)
		{
			iRandValue = rand() % 2;
			if (iRandValue)
				fY = _vWHD.y;
			else
				fY = 0.f;

		}
		else if (2 == iRandValue)
		{
			iRandValue = rand() % 2;
			if (iRandValue)
				fZ = _vWHD.z;
			else
				fZ = 0.f;

		}

		fX += m_pGameInstance->Compute_Random(-_fSurface * 0.5f, _fSurface * 0.5f) - _vWHD.x * _vMidPoint.x;
		fY += m_pGameInstance->Compute_Random(-_fSurface * 0.5f, _fSurface * 0.5f) - _vWHD.y * _vMidPoint.y;
		fZ += m_pGameInstance->Compute_Random(-_fSurface * 0.5f, _fSurface * 0.5f) - _vWHD.z * _vMidPoint.z;

	}
	else
	{
		fX = m_pGameInstance->Compute_Random(0.f, _vWHD.x) - _vWHD.x * _vMidPoint.x;
		fY = m_pGameInstance->Compute_Random(0.f, _vWHD.y) - _vWHD.y * _vMidPoint.y;
		fZ = m_pGameInstance->Compute_Random(0.f, _vWHD.z) - _vWHD.z * _vMidPoint.z;


	}

	vReturn = XMVectorSet(fX, fY, fZ, 1.f);

	return vReturn;
}

_vector CVIBuffer_Instance::Get_Torus_Pos(_float _fLargeRadius, _float _fHandleRadius, _float _fU, _float _fV)
{
	// Torus : 도넛 형태
	/* LargeRadius : 도넛의 큰 반지름 */
	/* HandleRadius : 도넛 링의 크기, */
	/* U : 큰 도넛의 각도 느낌? */
	/* V : 도넛 링의 각도 느낌? */
	_vector vReturn;

	_float fHandleAngle = XM_2PI - m_pGameInstance->Compute_Random(0.f, XM_2PI * (1.f - _fV));
	_vector vHandleLocal = XMVectorSet(cosf(fHandleAngle), sinf(fHandleAngle), 0.f, 1.f) * m_pGameInstance->Compute_Random(0.f, _fHandleRadius);
	
	_float fLargeAngle = XM_2PI - m_pGameInstance->Compute_Random(0.f, XM_2PI * (1.f - _fU));
	_matrix matTotal = XMMatrixIdentity();
	matTotal.r[3] = XMVectorSetX(matTotal.r[3], _fLargeRadius);
	matTotal = matTotal * XMMatrixRotationY(fLargeAngle);
	
	vReturn = XMVector3TransformCoord(vHandleLocal, matTotal);

	return vReturn;
}

_vector CVIBuffer_Instance::Get_Ring_Pos(_float _fRadius, _float _fCoverage, _float _fU)
{
	/* Coverage: 중앙으로 치고드는 정도, 1 = 원형 */
	/* fU : 링의 최대 각도 느낌? */
	_vector vReturn;

	_fRadius = (1.f - m_pGameInstance->Compute_Random(0.f, _fCoverage)) * _fRadius;
	
	_float fAngle = XM_2PI - m_pGameInstance->Compute_Random(0.f, XM_2PI * (1.f - _fU));	

	vReturn = XMVectorSet(_fRadius * cosf(fAngle), 0.f, _fRadius * sinf(fAngle), 1.f);
	return vReturn;
}

_vector CVIBuffer_Instance::Get_Cone_Pos(_float _fFlatten, _float _fLength, const _float3& _vAngle, _float _fSurface)
{
	/* Flatten : 평평하냐 vs 아니냐, _bool로 쓰임 */
	/* Length : 콘의 길이 */
	/* Angle : Angle, Inner Angle, Outer Angle,
		Angle : 콘의 커버 각도, 360도 이상일 때 전반을 다룸
		Inner Angle : 콘의 안 각도
		OuterAngle : 콘의 바깥 각도
	*/
	/* Surface : 아래에서부터 위까지 다루는 정도*/

	_vector vReturn;

	// 평평함
	if (0.5f < _fFlatten)
	{
		_float fY = _fLength * m_pGameInstance->Compute_Random(_fSurface, 1.f);
		_float fCoverAngle = m_pGameInstance->Compute_Random(XMConvertToRadians(_vAngle.y) * 0.5f, XMConvertToRadians(_vAngle.z) * 0.5f);
		_float fX = fY * tanf(clamp(fCoverAngle, -263.f, 263.f));
		vReturn = XMVectorSet(fX, fY, 0.f, 0.f);
	}

	// 원형
	else
	{
		_float  fLength = _fLength * m_pGameInstance->Compute_Random(_fSurface, 1.f);
		_float fCoverAngle = m_pGameInstance->Compute_Random(XMConvertToRadians(_vAngle.y) * 0.5f, XMConvertToRadians(_vAngle.z) * 0.5f);
		vReturn = XMVectorSet(sinf(fCoverAngle), cosf(fCoverAngle), 0.f, 0.f) * fLength;
	}
	

	vReturn = XMVector3Transform(vReturn, XMMatrixRotationY(m_pGameInstance->Compute_Random(0.f, XMConvertToRadians(_vAngle.x))));

	return vReturn;
}



_float CVIBuffer_Instance::Compute_FloatValue(SETTING_TYPE _eType, _float _fArg1, _float _fArg2)
{
	_float fReturn;
	if (RANDOMLINEAR == _eType || RANDOMRANGE == _eType)
	{
		fReturn = m_pGameInstance->Compute_Random(_fArg1, _fArg2);
	}
	else
	{
		fReturn = _fArg1;
	}

	return fReturn;
}

_float2 CVIBuffer_Instance::Compute_Float2Value(SETTING_TYPE _eType, const _float2& _vArg1, const _float2& _vArg2)
{
	_float2 vReturn;
	if (RANDOMLINEAR == _eType)
	{
		XMStoreFloat2(&vReturn, XMVectorLerp(XMLoadFloat2(&_vArg1), XMLoadFloat2(&_vArg2), m_pGameInstance->Compute_Random_Normal()));
	}
	else if (RANDOMRANGE == _eType)
	{
		vReturn = _float2(m_pGameInstance->Compute_Random(_vArg1.x, _vArg2.x), m_pGameInstance->Compute_Random(_vArg1.y, _vArg2.y));
	}
	else
	{
		vReturn = _vArg1;
	}

	return vReturn;
}

_float3 CVIBuffer_Instance::Compute_Float3Value(SETTING_TYPE _eType, const _float3& _vArg1, const _float3& _vArg2)
{
	_float3 vReturn;
	if (RANDOMLINEAR == _eType)
	{
		XMStoreFloat3(&vReturn, XMVectorLerp(XMLoadFloat3(&_vArg1), XMLoadFloat3(&_vArg2), m_pGameInstance->Compute_Random_Normal()));
	}
	else if (RANDOMRANGE == _eType)
	{
		vReturn = _float3(m_pGameInstance->Compute_Random(_vArg1.x, _vArg2.x), 
			m_pGameInstance->Compute_Random(_vArg1.y, _vArg2.y),
			m_pGameInstance->Compute_Random(_vArg1.z, _vArg2.z)
		);
	}
	else
	{
		vReturn = _vArg1;
	}

	return vReturn;
}

_float4 CVIBuffer_Instance::Compute_Float4Value(SETTING_TYPE _eType, const _float4& _vArg1, const _float4& _vArg2)
{
	_float4 vReturn;
	if (RANDOMLINEAR == _eType)
	{
		XMStoreFloat4(&vReturn, XMVectorLerp(XMLoadFloat4(&_vArg1), XMLoadFloat4(&_vArg2), m_pGameInstance->Compute_Random_Normal()));
	}
	else if (RANDOMRANGE == _eType)
	{
		vReturn = _float4(m_pGameInstance->Compute_Random(_vArg1.x, _vArg2.x),
			m_pGameInstance->Compute_Random(_vArg1.y, _vArg2.y),
			m_pGameInstance->Compute_Random(_vArg1.z, _vArg2.z),
			m_pGameInstance->Compute_Random(_vArg1.w, _vArg2.w)
		);
	}
	else
	{
		vReturn = _vArg1;
	}

	return vReturn;
}

HRESULT CVIBuffer_Instance::Set_FloatData(_Out_ _float* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float* _pSaveDatas)
{
	if (false == _jsonInfo.contains(_szTag))
		return E_FAIL;
	else
	{
		if (_jsonInfo[_szTag].contains("SetType"))
		{
			m_SetDatas[eData] = _jsonInfo[_szTag]["SetType"];
			if (false == _jsonInfo[_szTag].contains("Arg1"))
				return E_FAIL;
			_pSaveDatas[0] = _jsonInfo[_szTag]["Arg1"];
			if (_jsonInfo[_szTag].contains("Arg2"))
				_pSaveDatas[1] = _jsonInfo[_szTag]["Arg2"];
			else
				_pSaveDatas[1] = _jsonInfo[_szTag]["Arg1"];
		}
	}

	if (_pSetData)
	{
		*_pSetData = Compute_FloatValue(m_SetDatas[eData], _pSaveDatas[0], _pSaveDatas[1]);
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Set_Float2Data(_Out_ _float2* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float2* _pSaveDatas)
{
	if (false == _jsonInfo.contains(_szTag))
		return E_FAIL;
	else
	{
		if (_jsonInfo[_szTag].contains("SetType"))
		{
			m_SetDatas[eData] = _jsonInfo[_szTag]["SetType"];
			if (false == _jsonInfo[_szTag].contains("Arg1"))
				return E_FAIL;

			_float2 vArg = { 0.f, 0.f  };
			for (_int i = 0; i < 2; ++i)
				*((_float*)(&vArg) + i) = _jsonInfo[_szTag]["Arg1"][i];
			_pSaveDatas[0] = vArg;

			if (_jsonInfo[_szTag].contains("Arg2"))
			{
				for (_int i = 0; i < 2; ++i)
					*((_float*)(&vArg) + i) = _jsonInfo[_szTag]["Arg2"][i];
			}
			_pSaveDatas[1] = vArg;

		}
	}

	if (_pSetData)
	{
		*_pSetData = Compute_Float2Value(m_SetDatas[eData], _pSaveDatas[0], _pSaveDatas[1]);
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Set_Float3Data(_float3* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float3* _pSaveDatas)
{
	if (false == _jsonInfo.contains(_szTag))
		return E_FAIL;
	else
	{
		if (_jsonInfo[_szTag].contains("SetType"))
		{
			m_SetDatas[eData] = _jsonInfo[_szTag]["SetType"];
			if (false == _jsonInfo[_szTag].contains("Arg1"))
				return E_FAIL;

			_float3 vArg = { 0.f, 0.f, 0.f };
			for (_int i = 0; i < 3; ++i)
				*((_float*)(&vArg) + i) = _jsonInfo[_szTag]["Arg1"][i];
			_pSaveDatas[0] = vArg;

			if (_jsonInfo[_szTag].contains("Arg2"))
			{
				for (_int i = 0; i < 3; ++i)
					*((_float*)(&vArg) + i) = _jsonInfo[_szTag]["Arg2"][i];
			}
			_pSaveDatas[1] = vArg;

		}
	}

	if (_pSetData)
	{
		*_pSetData = Compute_Float3Value(m_SetDatas[eData], _pSaveDatas[0], _pSaveDatas[1]);
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance::Set_Float4Data(_Out_ _float4* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float4* _pSaveDatas)
{
	if (false == _jsonInfo.contains(_szTag))
		return E_FAIL;
	else
	{
		if (_jsonInfo[_szTag].contains("SetType"))
		{
			m_SetDatas[eData] = _jsonInfo[_szTag]["SetType"];
			if (false == _jsonInfo[_szTag].contains("Arg1"))
				return E_FAIL;

			_float4 vArg = { 0.f, 0.f, 0.f, 0.f };
			for (_int i = 0; i < 4; ++i)
				*((_float*)(&vArg) + i) = _jsonInfo[_szTag]["Arg1"][i];
			_pSaveDatas[0] = vArg;

			if (_jsonInfo[_szTag].contains("Arg2"))
			{
				for (_int i = 0; i < 4; ++i)
					*((_float*)(&vArg) + i) = _jsonInfo[_szTag]["Arg2"][i];
			}
			_pSaveDatas[1] = vArg;
				
		}
	}

	if (_pSetData)
	{
		*_pSetData = Compute_Float4Value(m_SetDatas[eData], _pSaveDatas[0], _pSaveDatas[1]);
	}

	return S_OK;
}






void CVIBuffer_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSetScales);
		Safe_Delete_Array(m_pSetRotations);
		Safe_Delete_Array(m_pSetPositions);
		Safe_Delete_Array(m_pSetLifeTimes);
		Safe_Delete_Array(m_pSetColors);
	}

}
