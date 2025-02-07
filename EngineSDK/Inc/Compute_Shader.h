#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCompute_Shader : public CComponent
{
private:
	CCompute_Shader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCompute_Shader(const CCompute_Shader& _Prototype);
	virtual ~CCompute_Shader() = default;

public:
	HRESULT Initialize_Prototype(const _tchar* _pShaderFilePath, const _char* _szEntry);
	virtual HRESULT Initialize(void* _pArg) override;


public:
	HRESULT Compute(ID3D11UnorderedAccessView* _pUAV, _uint _iThreadGroupCountX, _uint _iThreadGroupCountY, _uint _iThreadGroupCountZ);


private:
	ID3D11ComputeShader* m_pComputeShader = { nullptr };

public:
	static CCompute_Shader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath, const _char* _szEntry);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END