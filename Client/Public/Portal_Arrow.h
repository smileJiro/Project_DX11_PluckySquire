#pragma once
#include "Portal.h"

BEGIN(Engine)
class CEffect_System;
END

BEGIN(Client)
class CPortal_Arrow final : public CPortal
{

private:
	CPortal_Arrow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal_Arrow(const CPortal_Arrow& _Prototype);
	virtual ~CPortal_Arrow() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);
	virtual	void			Update(_float _fTimeDelta) override;
	virtual void			On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;

public:
	HRESULT					Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition) override;

	virtual void			Active_OnDisable() override;
	virtual void			Active_OnEnable() override;

protected:
	HRESULT					Ready_PartObjects(PORTAL_DESC* _pDesc);
	HRESULT					Ready_Components(PORTAL_DESC* _pDesc) override;

public:
	static CPortal_Arrow* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END