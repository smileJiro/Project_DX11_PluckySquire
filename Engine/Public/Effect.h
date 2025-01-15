#pragma once
#include "GameObject.h"


BEGIN(Engine)

/* 
추후 수정 예정 내용
- FilePath를 통해서 파티클을 불러오는 건 Particle System에서 진행합니다.


*/

/* 하나의 파티클 종류만 나옵니다. */

class ENGINE_DLL CEffect : public CGameObject
{
public:
	typedef struct tagEffectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iProtoShaderLevel;
		const _tchar* szShaderTag = L"";
	} EFFECT_DESC;
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

private:
	HRESULT						Bind_ShaderResources();
	HRESULT						Ready_Components(const EFFECT_DESC* _pDesc);

public:
	static	CEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;

};

END