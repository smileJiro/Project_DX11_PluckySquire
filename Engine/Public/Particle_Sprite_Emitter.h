#pragma once
#include "Particle_Emitter.h"


BEGIN(Engine)


class ENGINE_DLL CParticle_Sprite_Emitter : public CParticle_Emitter
{

private:
	CParticle_Sprite_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CParticle_Sprite_Emitter(const CParticle_Sprite_Emitter& _Prototype);
	virtual ~CParticle_Sprite_Emitter() = default;

public:
	HRESULT						Initialize_Prototype(const _tchar* _szFilePath); 
	virtual HRESULT				Initialize(void* _pArg) override ; 
	virtual void				Priority_Update(_float _fTimeDelta) override ;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override ;
	virtual HRESULT				Render() override;


private:
	class CVIBuffer_Point_Particle* m_pParticleBufferCom = { nullptr };
	class CTexture*					m_pTextureCom = { nullptr };

private:
	HRESULT							Bind_ShaderResources();
	virtual HRESULT					Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc) override;

public:
	static	CParticle_Sprite_Emitter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;

};

END