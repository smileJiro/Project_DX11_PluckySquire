#pragma once
#include "Portal.h"
#include "Interactable.h"


BEGIN(Client)
class CPortal_Default final : public CPortal, public virtual IInteractable
{

private:
	CPortal_Default(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal_Default(const CPortal_Default& _Prototype);
	virtual ~CPortal_Default() = default;

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
	static CPortal_Default* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END