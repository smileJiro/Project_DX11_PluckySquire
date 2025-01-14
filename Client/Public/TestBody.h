#pragma once
#include "ModelObject.h"


BEGIN(Client)

class CTestBody final :  public CModelObject
{
public:
	typedef struct tagTestBodyDesc : public CModelObject::MODELOBJECT_DESC
	{

	}TESTBODY_DESC;
private:
	CTestBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestBody(const CTestBody& _Prototype);
	virtual ~CTestBody() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow() override;

private:
	HRESULT					Ready_Components();

public:
	static CTestBody*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
