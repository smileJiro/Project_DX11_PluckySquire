#pragma once
#include "GameObject.h"
#include "EffectTool_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
END

BEGIN(EffectTool)

/* 뒷배경에 해당하는 더미 오브젝트 */
class CObject_Background : public CGameObject
{
private:
	CObject_Background(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CObject_Background(const CObject_Background& _Prototype);
	virtual ~CObject_Background() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Rect*		m_pBufferCom = { nullptr };

	_float4				m_vColor = { 1.f, 1.f, 1.f, 1.f };

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();

public:
	static CObject_Background* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences() override;

};

END