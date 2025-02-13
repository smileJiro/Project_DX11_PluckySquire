#pragma once
#include "ModelObject.h"
#include "Interactable.h"

BEGIN(Client)
class CPlayer;
class CCarriableObject :
	public CModelObject, public IInteractable
{
protected:
	explicit CCarriableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCarriableObject(const CCarriableObject& _Prototype);
	virtual ~CCarriableObject() = default;

public:
	virtual HRESULT			Render() override;

	HRESULT Initialize(void* _pArg);

public:
	// IInteractable을(를) 통해 상속됨
	virtual void Interact(CPlayer* _pUser) override;
	virtual _bool Is_Interactable(CPlayer* _pUser) override;
	virtual _float Get_Distance(CPlayer* _pUser) override;
	void Set_Kinematic(_bool _bKinematic);
public:
	HRESULT Set_Carrier(CPlayer* _pCarrier);
	void Throw(_fvector _vForce);
private:
	CPlayer* m_pCarrier = nullptr;
	CCollider* m_pBody2DColliderCom = nullptr;
public:
	static CCarriableObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END