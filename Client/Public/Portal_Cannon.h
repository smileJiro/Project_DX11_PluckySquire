#pragma once
#include "Portal.h"
#include "Interactable.h"

BEGIN(Engine)
class CEffect_System;
END

BEGIN(Client)
class CPortal_Cannon final : public CPortal, public virtual IInteractable
{

private:
	CPortal_Cannon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal_Cannon(const CPortal_Cannon& _Prototype);
	virtual ~CPortal_Cannon() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);

	virtual HRESULT			Setup_3D_Postion() override;

public:
	HRESULT					Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition) override;


	virtual void			Interact(CPlayer* _pUser);
	virtual _bool			Is_Interactable(CPlayer* _pUser);
	virtual _float			Get_Distance(COORDINATE _eCoord, CPlayer* _pUser);

	virtual void			Active_OnDisable() override;
	virtual void			Active_OnEnable() override;

protected:
	HRESULT					Ready_PartObjects(PORTAL_DESC* _pDesc);
	virtual void			On_InteractionStart(CPlayer* _pPlayer)override;
	virtual void			On_InteractionEnd(CPlayer* _pPlayer) override;


public:
	static CPortal_Cannon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END