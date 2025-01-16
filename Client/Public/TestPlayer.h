#pragma once
#include "ContainerObject.h"

BEGIN(Client)

class CTestPlayer final : public CContainerObject
{
public:
	enum ANIM_STATE
	{

	};
private:
	CTestPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestPlayer(const CTestPlayer& _Prototype);
	virtual ~CTestPlayer() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	void					Key_Input(_float _fTimeDelta);

private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();

private:
	_uint itmpIdx = 0;
public:
	static CTestPlayer*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;

};
END
