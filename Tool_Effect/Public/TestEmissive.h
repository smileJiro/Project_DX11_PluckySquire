#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CShader;

END
class CTestEmissive : public CModelObject
{
private:
	CTestEmissive(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestEmissive(const CModelObject& _Prototype);
	virtual ~CTestEmissive() = default;

public:
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	_float4			m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_float4			m_vBloomColor = { 1.f, 1.f, 1.f, 1.f };

private:
	HRESULT			Bind_ShaderResources();



public:
	static CTestEmissive* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

public:
	HRESULT Cleanup_DeadReferences() override;
};

