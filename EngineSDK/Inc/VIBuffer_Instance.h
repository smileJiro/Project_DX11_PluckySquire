#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance : public CVIBuffer
{
private:
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
	virtual void	Update(_float _fTimeDelta) = 0;
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
	_float3 m_vShapeScale;
	_float3 m_vShapeRotation;
	_float3 m_vShapePosition;

protected:
	vector<SETTING_TYPE> m_SetDatas;			// 세팅 정보들

	_float3* m_pSetScales = { nullptr };		// 세팅 크기
	_float3* m_pSetRotations = { nullptr };		// 세팅 회전
	_float3* m_pSetPositions = { nullptr };		// 세팅 포지션
	_float*	 m_pSetLifeTimes = { nullptr };		// 세팅 수명 시간
	_float4* m_pSetColors = { nullptr };		// 세팅 색깔
	
protected:
	_float3* m_pSpeeds = { nullptr }; // 각 파티클의 속도
	_float3* m_pForces = { nullptr }; // 각 파티클의 힘

protected:
	vector<class CParticle_Module*> m_Modules;

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
	/* 기본으로 설정되는 데이터들 */	
	_float3		Compute_ScaleValue();
	_float3		Compute_RotationValue();
	_float3		Compute_PositionValue();
	_float		Compute_LifeTime();
	_float4		Compute_ColorValue();

protected:
	HRESULT		Set_FloatData(PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float* _pSaveDatas);
	HRESULT		Set_Float2Data(PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float2* _pSaveDatas);
	HRESULT		Set_Float3Data(PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float3* _pSaveDatas);
	HRESULT		Set_Float4Data(PARTICLE_DATA eData, const json& _jsonInfo, const _char* _szTag, _float4* _pSaveDatas);

protected:
	HRESULT		Ready_Modules(const json _jsonInfo);

protected:
	_bool		Is_ShapeData(const _string& _strTag)
	{
		auto iter = m_ShapeDatas.find(_strTag);
		if (iter == m_ShapeDatas.end())
			return false;
		return true;
	}

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;

#ifdef _DEBUG
public:
	virtual void Tool_Setting();
	virtual void Tool_Update(_float _fTimeDelta);

protected:
	_bool								m_isToolReset = { false };
	map<const _string, _float>			m_ToolShapeDatas;

protected:
	void				 Tool_Adjust_DefaultData();
	void				 Tool_Adjust_Shape();
	void				 Tool_Create_ShapeData();

	virtual void		 Tool_Reset_Instance() {}
	virtual void		 Tool_Reset_Buffers() {} // Count 자체가 바뀌어버린 경우

	_bool		Is_ToolData(const _string& _strTag)
	{
		auto iter = m_ToolShapeDatas.find(_strTag);
		if (iter == m_ToolShapeDatas.end())
			return false;
		return true;
	}

	void		Tool_InsertData(const _string& _strTag, _float _fData)
	{
		auto iter = m_ToolShapeDatas.find(_strTag);
		if (iter == m_ToolShapeDatas.end())
		{
			m_ShapeDatas.emplace(_strTag, _fData);
			return;
		}

		m_ShapeDatas.emplace(_strTag, iter->second);
	}

	static _bool s_isShapeChange;
	static _int s_iShapeFlags;
#endif

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