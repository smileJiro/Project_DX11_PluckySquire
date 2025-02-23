#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Beam;
class CTexture;
END

class CTestBeam : public CGameObject
{
private:
	CTestBeam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestBeam(const CTestBeam& _Prototype);
	virtual ~CTestBeam() = default;


public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Beam*		m_pBufferCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };

private:
	_float4				m_vColor = { 1.f, 1.f, 1.f, 1.f  };

private:
	HRESULT				Bind_ShaderResources();
	HRESULT				Ready_Components();

public:
	static CTestBeam* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences() override;
};

