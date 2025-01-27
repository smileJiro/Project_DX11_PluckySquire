#pragma once
#include "GameObject.h"
#include "EffectTool_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(EffectTool)

class CEffect_Reference : public CGameObject
{
private:
	CEffect_Reference(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect_Reference(const CEffect_Reference& _Prototype);
	virtual ~CEffect_Reference() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();

public:
	static CEffect_Reference* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences() override;
};

END