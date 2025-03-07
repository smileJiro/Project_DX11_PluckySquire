#pragma once
#include "Portal.h"

BEGIN(Engine)
class CEffect_System;
END

BEGIN(Client)
class CPortal_Immediately final : public CPortal
{

private:
	CPortal_Immediately(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal_Immediately(const CPortal_Immediately& _Prototype);
	virtual ~CPortal_Immediately() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);

	virtual void			On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;

public:
	HRESULT					Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition) override;

	virtual void			Active_OnDisable() override;
	virtual void			Active_OnEnable() override;

protected:
	HRESULT					Ready_PartObjects(PORTAL_DESC* _pDesc);

public:
	static CPortal_Immediately* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END