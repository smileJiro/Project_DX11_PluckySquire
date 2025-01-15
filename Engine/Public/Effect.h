#pragma once
#include "GameObject.h"


BEGIN(Engine)

class ENGINE_DLL CEffect : public CGameObject
{

private:
	CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect(const CEffect& _Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT				Initialize_Prototype(const _tchar* _szFilePath); 
	virtual HRESULT				Initialize(void* _pArg); 
	virtual void				Priority_Update(_float _fTimeDelta);
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();


private:
	class CVIBuffer_Particle_Point* m_pParticleBufferCom = { nullptr };
	class CShader*					m_pShaderCom = { nullptr };
	class CTexture*					m_pTextureCom = { nullptr };

public:
	static	CEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;

};

END