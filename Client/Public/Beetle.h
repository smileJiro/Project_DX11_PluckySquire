#pragma once
#include "Monster.h"
class CBeetle final : public CMonster
{
public:


private:
	CBeetle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBeetle(const CBeetle& _Prototype);
	virtual ~CBeetle() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();

public:
	static CBeetle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

