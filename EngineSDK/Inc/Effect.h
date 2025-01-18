#pragma once
#include "GameObject.h"


BEGIN(Engine)

/* 
추후 수정 예정 내용
- FilePath를 통해서 파티클을 불러오는 건 Particle System에서 진행합니다.


*/

/* 하나의 파티클 종류만 나옵니다. */

class ENGINE_DLL CSprite_Particle : public CGameObject
{
public:
	typedef struct tagEffectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iProtoShaderLevel;
		const _tchar* szShaderTag = L"";
	} EFFECT_DESC;
private:
	CSprite_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSprite_Particle(const CEffect& _Prototype);
	virtual ~CSprite_Particle() = default;

public:
	virtual HRESULT				Initialize_Prototype(const _tchar* _szFilePath); 
	virtual HRESULT				Initialize(void* _pArg); 
	virtual void				Priority_Update(_float _fTimeDelta);
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();


private:
	class CVIBuffer_Point_Particle* m_pParticleBufferCom = { nullptr };
	class CShader*					m_pShaderCom = { nullptr };
	class CTexture*					m_pTextureCom = { nullptr };

private:
	HRESULT						Bind_ShaderResources();
	HRESULT						Ready_Components(const EFFECT_DESC* _pDesc);

public:
	static	CSprite_Particle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;

};

END