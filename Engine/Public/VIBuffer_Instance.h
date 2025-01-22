#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance : public CVIBuffer
{
public:
	enum SHAPE_TYPE { SPHERE, CYLINDER, BOX, TORUS, RING, CONE, SHAPE_NONE };
	enum SETTING_TYPE { UNSET, DIRECTSET, RANDOMLINEAR, RANDOMRANGE  };
	enum PARTICLE_DATA { P_SCALE, P_ROTATION, P_POSITION, P_LIFETIME, P_COLOR, DATAEND };

protected:
	CVIBuffer_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& _Prototype);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual HRESULT Render();

	virtual HRESULT Bind_BufferDesc();

public:
	_uint	Get_NumInstance() const { return m_iNumInstances; }

protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };		// Instance 전용 버퍼
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};			// Instance Buffer을 만들기 위한 Buffer Desc
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialDesc = {};			// Instance Buffer 초기 설정 Desc
	_uint						m_iNumInstances = {};				// Instance 개수
	_uint						m_iNumIndexCountPerInstance = {};	// Instance 한 개당 Index 할당 개수
	_uint						m_iInstanceStride = {};				// Instance Buffer 자료형의 크기

protected:
	SHAPE_TYPE							m_eShapeType = SHAPE_NONE; // 초기 스폰 모양
	map<const _string, _float>			m_ShapeDatas;
	_float4x4							m_ShapeMatrix = {};

protected:
	vector<SETTING_TYPE> m_SetDatas;			// 세팅 정보들

	_float3* m_pSetScales = { nullptr };		// 세팅 크기
	_float3* m_pSetRotations = { nullptr };		// 세팅 회전
	_float3* m_pSetPositions = { nullptr };		// 세팅 포지션
	_float*	 m_pSetLifeTimes = { nullptr };		// 세팅 수명 시간
	_float4* m_pSetColors = { nullptr };		// 세팅 색깔
	

protected:
	/*
	* 모양에 따른 랜덤포지션 세팅
	* 주요한 설명은 함수 내에서
	*/
	HRESULT		Set_ShapeData(const json& _jsonInfo);

	_vector		Get_Sphere_Pos(_float _fRadius, _float _fSurface);
	_vector		Get_Cylinder_Pos(_float _fHeight, _float _fRadius, _float _fMidPoint);
	_vector		Get_Box_Pos(const _float3& _vWHD, const _float3& _vMidPoint, _bool _isSurface, _float _fSurface);
	_vector		Get_Torus_Pos(_float _fLargeRadius, _float _fHandleRadius, _float _fU, _float _fV);
	_vector		Get_Ring_Pos(_float _fRadius, _float _fCoverage, _float _fU);
	_vector		Get_Cone_Pos(_float _fFlatten, _float _fLength, const _float3& _vAngle, _float _fSurface);

protected:
	_float		Compute_FloatValue(SETTING_TYPE _eType, _float _fArg1, _float _fArg2);
	_float2		Compute_Float2Value(SETTING_TYPE _eType, const _float2& _vArg1, const _float2& _vArg2);
	_float3		Compute_Float3Value(SETTING_TYPE _eType, const _float3& _vArg1, const _float3& _vArg2);
	_float4		Compute_Float4Value(SETTING_TYPE _eType, const _float4& _vArg1, const _float4& _vArg2);
	
	HRESULT		Set_FloatData(_Out_ _float* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float* _pSaveDatas);
	HRESULT		Set_Float2Data(_Out_ _float2* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float2* _pSaveDatas);
	HRESULT		Set_Float3Data(_Out_ _float3* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float3* _pSaveDatas);
	HRESULT		Set_Float4Data(_Out_ _float4* _pSetData, PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float4* _pSaveDatas);


protected:
	_bool		Is_Data(const _string& _strTag)
	{
		auto iter = m_ShapeDatas.find(_strTag);
		if (iter == m_ShapeDatas.end())
			return false;
		return true;
	}

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;

};

END

BEGIN(Engine)

NLOHMANN_JSON_SERIALIZE_ENUM(CVIBuffer_Instance::SHAPE_TYPE, {
{CVIBuffer_Instance::SHAPE_TYPE::SPHERE, "SPHERE"},
{CVIBuffer_Instance::SHAPE_TYPE::CYLINDER, "CYLINDER"},
{CVIBuffer_Instance::SHAPE_TYPE::BOX, "BOX"},
{CVIBuffer_Instance::SHAPE_TYPE::TORUS, "TORUS"},
{CVIBuffer_Instance::SHAPE_TYPE::RING, "RING"},
{CVIBuffer_Instance::SHAPE_TYPE::CONE, "CONE"},
{CVIBuffer_Instance::SHAPE_TYPE::SHAPE_NONE, "NONE"},

	});

NLOHMANN_JSON_SERIALIZE_ENUM(CVIBuffer_Instance::SETTING_TYPE, {
{CVIBuffer_Instance::SETTING_TYPE::UNSET, "UNSET"},
{CVIBuffer_Instance::SETTING_TYPE::DIRECTSET, "DIRECTSET"},
{CVIBuffer_Instance::SETTING_TYPE::RANDOMLINEAR, "RANDOMLINEAR"},
{CVIBuffer_Instance::SETTING_TYPE::RANDOMRANGE, "RANDOMRANGE"},

	});

END