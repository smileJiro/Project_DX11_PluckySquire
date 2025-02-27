#pragma once
#include "ModelObject.h"
#include "Interactable.h"
BEGIN(Client)
class CPalmDecal :
	public CModelObject, public IInteractable
{
private:
	explicit CPalmDecal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPalmDecal(const CPalmDecal& _Prototype);
	virtual ~CPalmDecal() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Late_Update(_float _fTimeDelta) override;

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	void On_Placed(_fvector _v2DPos, _fvector _v2DDir);
public:
	void Interact(CPlayer* _pUser) override;
	_bool Is_Interactable(CPlayer* _pUser) override;
	_float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
public:
	static CPalmDecal* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;


};

END