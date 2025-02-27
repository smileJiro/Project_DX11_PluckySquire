#pragma once
#include "ModelObject.h"
#include "Interactable.h"
BEGIN(Client)
class CPalmMarker :
	public CModelObject, public IInteractable
{
private:
	explicit CPalmMarker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPalmMarker(const CPalmMarker& _Prototype);
	virtual ~CPalmMarker() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void Interact(CPlayer* _pUser) override;
	virtual _bool Is_Interactable(CPlayer* _pUser) override;
	virtual _float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
private:

public:
	static CPalmMarker* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END