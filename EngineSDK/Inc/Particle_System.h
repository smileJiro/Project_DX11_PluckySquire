#pragma once
#include "GameObject.h"
#include "Particle_Emitter.h"

BEGIN(Engine)


class ENGINE_DLL CParticle_System : public CGameObject
{
public:
	typedef struct tagParticleSystemDesc : public CGameObject::GAMEOBJECT_DESC
	{
		vector<_uint> EmitterShaderLevels;
		vector<const _tchar*> ShaderTags;
	} PARTICLE_SYSTEM_DESC;

private:
	CParticle_System(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CParticle_System(const CParticle_System& _Prototype);
	virtual ~CParticle_System() = default;

public:
	HRESULT						Initialize_Prototype(const _tchar* _szFilePath);
	virtual	HRESULT				Initialize_Prototype();
	HRESULT						Initialize(void* _pArg, const CParticle_System* _pPrototype);
	HRESULT						Initialize();
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;


private:
	vector<class CParticle_Emitter*> m_ParticleEmitters;

public:
	static	CParticle_System* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;

#ifdef _DEBUG
public:
	HRESULT						Add_NewEmitter(CParticle_Emitter::PARTICLE_TYPE _eType, void* _pArg);
	void						Tool_Update(_float _fTimeDelta);

public:
	void						Set_Texture(class CTexture* _pTextureCom);

public:
	static	CParticle_System* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); /* 툴전용의 파티클 생성 코드입니다, 어떤 Emitter도 만들지 않습니다.*/
private:
	_char						m_szInputTexturePath[MAX_PATH] = "../Bin/Resources/Textures/Effects/";
	_int						m_iInputNumInstances = { 0 };
	CParticle_Emitter*			m_pNowItem = { nullptr };
private:
	//void						Tool_Make();
	void						Tool_ShowList();
#endif
};

END

BEGIN(Engine)
NLOHMANN_JSON_SERIALIZE_ENUM(CParticle_Emitter::PARTICLE_TYPE, {
{CParticle_Emitter::PARTICLE_TYPE::SPRITE, "SPRITE"},
{CParticle_Emitter::PARTICLE_TYPE::MESH, "MESH"},
	});
END