#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CEffect : public CGameObject
{
public:
	typedef struct tagEffectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		// TODO 1 : Shader, Texture, Buffer Tag
		// TODO 2 : Shader, Texture, Buffer Level 

	} EFFECT_DESC;
private:
	CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect(const CEffect& _Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT				Initialize_Prototype(); 
	virtual HRESULT				Initialize(void* _pArg); 
	virtual void				Priority_Update(_float _fTimeDelta);
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

public:
	static	CEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;

};

END