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
	HRESULT Initialize(void* _pArg);

	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual HRESULT	 Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

public:
	// IInteractable을(를) 통해 상속됨
	virtual void Interact(CPlayer* _pUser) override;
	virtual _bool Is_Interactable(CPlayer* _pUser) override;
	virtual _float Get_Distance(CPlayer* _pUser) override;
public:
	_bool Is_Carrying() { return nullptr != m_pCarrier; }
	HRESULT Set_Carrier(CPlayer* _pCarrier);
	void Throw(_fvector _vForce);
	void Set_Kinematic(_bool _bKinematic);
private:
	CPlayer* m_pCarrier = nullptr;
	CCollider* m_pBody2DColliderCom = nullptr;
	_float m_f2DFloorDistance = 0;
	_vector m_v2DThrowHorizeForce = { 0.f, 0.f };
	_vector m_v2DGroundPosition = { 0.f, 0.f };
	_float m_f2DUpForce = 0.f;
	_bool m_b2DOnGround= false;
	_bool m_bThrowing = false;

	//실험용
	_float m_fRestitution = 0.5f;
public:
	static CCarriableObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END