#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Trail;
class CTexture;
END

class CTestTrail : public CGameObject
{
private:
	CTestTrail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestTrail(const CTestTrail& _Prototype);
	virtual ~CTestTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Trail* m_pBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	_float m_fAccTime = 0.f;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();

public:
	static CTestTrail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences() override;
};

