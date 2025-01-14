#pragma once
#include "Base.h"


BEGIN(Engine)
class CPipeLine : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	HRESULT Initialize();
	void Update();

public:
	_float2 Transform_WorldToScreen(_fvector _vWorldPos, _float2 _fScreenSize);

public:
	// Get
	_matrix Get_TransformMatrix(D3DTRANSFORMSTATE _eState)
	{
		return XMLoadFloat4x4(&m_TransformMatrices[_eState]);
	}
	_float4x4 Get_TransformFloat4x4(D3DTRANSFORMSTATE _eState) 
	{
		return m_TransformMatrices[_eState];
	}
	_matrix Get_TransformInverseMatrix(D3DTRANSFORMSTATE _eState)
	{
		return XMLoadFloat4x4(&m_TransformInverseMatrices[_eState]);
	}
	_float4x4 Get_TransformInverseFloat4x4(D3DTRANSFORMSTATE _eState)
	{
		return m_TransformInverseMatrices[_eState];
	}

	// * 로 넘기는이유 : void* 로 데이터 쉐이더에 넘길거라 
	const _float4* Get_CamPosition() const 	{ return &m_vCamPosition; }

	_float* Get_NearZ() { return &m_vNearFarZ.x; }
	_float* Get_FarZ() { return &m_vNearFarZ.y; }
	// Set
	void Set_Transform(D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix) { XMStoreFloat4x4(&m_TransformMatrices[_eState], _TransformMatrix); }
	void Set_NearFarZ(_float2 _vNearFarZ) { m_vNearFarZ = _vNearFarZ; };

private:
	_float4x4				m_TransformMatrices[D3DTS_END];
	_float4x4				m_TransformInverseMatrices[D3DTS_END];

	_float4					m_vCamPosition = {};
	_float2					m_vNearFarZ = { 0.0f, 1000.f };

private:
	void Setting_PipeLineData();

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END