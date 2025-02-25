#pragma once
#include "Emitter.h"

BEGIN(Engine)


class ENGINE_DLL CParticle_Sprite_Emitter : public CEmitter
{
public:
	enum EFFECT_SHADERPASS { DEFAULT, DEFAULT_BLOOM, 
		ROTATION_BILLBOARD, ROTATION_BILLBOARD_BLOOM,
		VELOCITY_BILLBOARD, VELOCITY_BILLBOARD_BLOOM,
		SUBCOLORBLOOM,		DEFAULT_DISSOLVE, VELOCITY_BILLBOARD_SUBCOLORBLOOM,
		SUBCOLORBLOOM_DISSOLVE
	
	};

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
	class CTexture*					m_pMaskTextureCom = { nullptr };
	class CTexture*					m_pDissolveTextureCom = { nullptr };

private:
	//_float							m_fAlphaDiscard = { 0.f };
	//_float							m_fRGBDiscard = { 0.f };
	//_float							m_fBloomThreshold = { 0.f };
	

private:
	virtual void					Update_Emitter(_float _fTimeDelta) override;


	HRESULT							Bind_ShaderResources();
	HRESULT							Bind_ShaderValue_ByPass();
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
	void						Set_Texture(class CTexture* _pTextureCom, _uint _iTextureIndex);

public:
	// DEBUG용 처음 만든 Sprite
	static	CParticle_Sprite_Emitter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);

#endif
};

END

BEGIN(Engine)
NLOHMANN_JSON_SERIALIZE_ENUM(CParticle_Sprite_Emitter::EFFECT_SHADERPASS, {
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::DEFAULT, "DEFAULT"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::DEFAULT_BLOOM, "DEFAULT_BLOOM"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::SUBCOLORBLOOM, "SUBCOLOR_BLOOM"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::VELOCITY_BILLBOARD_SUBCOLORBLOOM, "VELOCITY_BILLBOARD_SUBCOLORBLOOM"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::ROTATION_BILLBOARD, "ROTATION_BILLBOARD"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::ROTATION_BILLBOARD_BLOOM, "ROTATION_BILLBOARD_BLOOM"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::VELOCITY_BILLBOARD, "VELOCITY_BILLBOARD"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::VELOCITY_BILLBOARD_BLOOM, "VELOCITY_BILLBOARD_BLOOM"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::DEFAULT_DISSOLVE, "DEFAULT_DISSOLVE"},
{CParticle_Sprite_Emitter::EFFECT_SHADERPASS::SUBCOLORBLOOM_DISSOLVE, "SUBCOLORBLOOM_DISSOLVE"},

	});
END