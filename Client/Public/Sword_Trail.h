#pragma once
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Trail;
class CTexture;
END

BEGIN(Client)

class CSword_Trail : public CPartObject
{
private:
	CSword_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSword_Trail(const CSword_Trail& _Prototype);
	virtual ~CSword_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void		Add_Point(_fvector _vPosition);
	void		Delete_Effect();

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
	static CSword_Trail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences() override;
};

END