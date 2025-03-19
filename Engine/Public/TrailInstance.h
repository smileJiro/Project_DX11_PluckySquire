#pragma once
#include "Base.h"

BEGIN(Engine)
class CShader;
class C3DModel;
class ENGINE_DLL CTrailInstance final : public CBase
{
public:
	typedef struct tagTrailDesc
	{
		_float4x4 WorldMatrix;
		_float fTrailTime = 0.0f;
		_int iNumMeshes = 0;
		_float4 vTrailColor = {};
		_float4 vOuterColor = {};
		_float fBaseReflect = {0.f};
		_float fExp = {0.f};
		vector<array<_float4x4, 256>> MeshesBoneMatrices;
	}TRAIL_DESC;
private:
	CTrailInstance();
	virtual ~CTrailInstance() = default;

public:
	HRESULT Initialize(TRAIL_DESC* _pArg);
	/* Trail의 시간을 계산*/
	void Update(_float _fTimeDelta);
	HRESULT Render(CShader* _pShaderCom, C3DModel* _pModelCom);
	
private:
	/* Trail 렌더링에 필요한 데이터를 바인드  */
	HRESULT Bind_TrailMatrices(const _char* _pConstantName, _int _iMeshIndex, CShader* _pShaderCom);

	/* 해당 트레일을 그리기 전 1회만 호출하는, (색상, 시간 등등에 대한 데이터를 바인드. )*/
	HRESULT Bind_TrailResources(CShader* _pShaderCom);

public:
	// Get
	_bool Is_Finished() const { return m_isFinished; }
	
private:
	_float4x4 m_WorldMatrix = {};
	_bool m_isFinished = false;
	_float2 m_vTrailTime = {};
	_float4 m_vTrailColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	_float m_iNumMeshes = {};
	_float4 m_vOuterColor = { 1.f, 1.f, 1.f, 1.f };
	_float m_fBaseReflect = { 0.f };
	_float m_fExp = { 0.f };
	vector<array<_float4x4, 256>> m_MeshesBoneMatrices;

public:
	static CTrailInstance* Create(TRAIL_DESC* _pArg);
	virtual void Free() override;
};
END
