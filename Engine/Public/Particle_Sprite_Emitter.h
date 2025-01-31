#pragma once
#include "Emitter.h"

BEGIN(Engine)


class ENGINE_DLL CParticle_Sprite_Emitter : public CEmitter
{

private:
	CParticle_Sprite_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CParticle_Sprite_Emitter(const CParticle_Sprite_Emitter& _Prototype);
	virtual ~CParticle_Sprite_Emitter() = default;

public:
	//HRESULT						Initialize_Prototype(const _tchar* _szFilePath);
	virtual HRESULT				Initialize_Prototype(const json& _jsonInfo) override;
	virtual HRESULT				Initialize(void* _pArg) override; 
	virtual void				Priority_Update(_float _fTimeDelta) override ;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override ;
	virtual HRESULT				Render() override;

public:
	virtual	void				Reset() override;

private:
	class CVIBuffer_Point_Particle* m_pParticleBufferCom = { nullptr };
	class CTexture*					m_pTextureCom = { nullptr };

private:
	_float							m_fAlphaDiscard = { 0.f };
	_float							m_fRGBDiscard = { 0.f };

private:
	HRESULT							Bind_ShaderResources();
	virtual HRESULT					Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc) override;

public:
	//static	CParticle_Sprite_Emitter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath);
	static	CParticle_Sprite_Emitter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonInfo);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;
	
	
#ifdef _DEBUG 
public:
	virtual void				Tool_Setting() override;
	virtual void				Tool_Update(_float _fTimeDelta) override;
	virtual HRESULT				Save(json& _jsonOut);
public:
	void						Set_Texture(class CTexture* _pTextureCom);

public:
	// DEBUG용 처음 만든 Sprite
	static	CParticle_Sprite_Emitter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);

#endif
};

END