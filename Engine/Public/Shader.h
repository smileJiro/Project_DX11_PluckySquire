#pragma once
#include "Component.h"
BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CShader(const CShader& _Prototype);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElementsDesc, const _uint _iNumElements);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	// 어떠한 Pass를 사용하여 Render를 수행할 것 인지 지정하고 그에 맞는 InputLayout을 IA에 전달.
	HRESULT Begin(_uint _iPassIndex);
	HRESULT Bind_RawValue(const _char* _pConstantName, const void* _pData, _uint _iLength);
	// 셰이더에 상수데이터를 바인딩. (셰이더에서 사용 중인 상수 데이터의 이름, 바인딩 할 행렬)
	HRESULT Bind_Matrix(const _char* _pConstantName, const _float4x4* _pMatrix);
	HRESULT Bind_Matrices(const _char* _pConstantName, const _float4x4* _pMatrix, _uint _iNumMatrices); /* 매트릭스 배열을 바인딩하는 함수. */
	HRESULT Bind_SRV(const _char* _pConstantName, ID3D11ShaderResourceView* _pSRV);
	HRESULT Bind_SRVs(const _char* _pConstantName, vector<ID3D11ShaderResourceView*>& _SRVs);

private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	_uint						m_iNumLayouts = { 0 }; // NumLayouts == NumPasses
	vector<ID3D11InputLayout*>	m_InputLayouts = {};
public:
	// Shader 객체 생성 시 ShaderFilePath, 해당 셰이더에 사용 될 Vertex struct의 ElementsDesc, NumElements를 전달한다.
	static CShader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElementsDesc, const _uint _iNumElements);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END