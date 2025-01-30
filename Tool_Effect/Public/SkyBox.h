#pragma once
#include "GameObject.h"
#include "EffectTool_Defines.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END

BEGIN(EffectTool)

class CSkyBox : public CGameObject
{
private:
	CSkyBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSkyBox(const CSkyBox& _Prototype);
	virtual ~CSkyBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CSkyBox* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
	HRESULT Cleanup_DeadReferences() override;

	// CGameObject을(를) 통해 상속됨
};

END