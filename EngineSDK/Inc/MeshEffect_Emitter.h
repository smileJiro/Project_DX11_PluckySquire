#pragma once
#include "Emitter.h"

BEGIN(Engine)

class CEffectModel;
class CTexture;

class ENGINE_DLL CMeshEffect_Emitter : public CEmitter
{
public:
	enum TEXTURE_TYPE { MASK, NOISE, TEXTURE_END };

private:
	CMeshEffect_Emitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMeshEffect_Emitter(const CMeshEffect_Emitter& _Prototype);
	virtual ~CMeshEffect_Emitter() = default;

public:
	HRESULT						Initialize_Prototype(const json& _jsonInfo);
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual	void				Reset() override;

private:
	CEffectModel* m_pEffectModelCom = { nullptr };

	vector<class CTexture*>	m_Textures;

private:
	_float3		m_vColor = {};
	_float		m_fAlpha = {};
private:
	HRESULT							Bind_ShaderResources();
	virtual HRESULT					Ready_Components(const PARTICLE_EMITTER_DESC* _pDesc) override;
	HRESULT							Load_TextureInfo(const json& _jsonInfo);

public:
	static	CMeshEffect_Emitter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonInfo);

	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;

#ifdef _DEBUG
public:
	virtual void				Tool_Setting() override;
	virtual void				Tool_Update(_float _fTimeDelta) override;
	virtual HRESULT				Save(json& _jsonOut);

public:
	void						Set_Texture(class CTexture* _pTextureCom, _uint _iType);

private:
	_string m_strModelPath;
	_float4x4 m_PreTransformMatrix = {};
public:
// DEBUG용 처음 만든 Mesh
	static	CMeshEffect_Emitter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
#endif
};

END

BEGIN(Engine)
NLOHMANN_JSON_SERIALIZE_ENUM(CMeshEffect_Emitter::TEXTURE_TYPE, {
{CMeshEffect_Emitter::TEXTURE_TYPE::MASK, "MASK"},
{CMeshEffect_Emitter::TEXTURE_TYPE::NOISE, "NOISE"},

	});
END