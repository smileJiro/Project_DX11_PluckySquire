#pragma once
#include "ModelObject.h"


BEGIN(Client)

class CBeetleBody final :  public CModelObject
{
public:
	typedef struct tagBeetleBodyDesc : public CModelObject::MODELOBJECT_DESC
	{

	}BEETLEBODY_DESC;
private:
	CBeetleBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBeetleBody(const CBeetleBody& _Prototype);
	virtual ~CBeetleBody() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	virtual HRESULT			Render_Shadow() override;

private:
	HRESULT					Ready_Components();

public:
	static CBeetleBody*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
