#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CBomb : public CCarriableObject
{
protected:
	explicit CBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBomb(const CBomb& _Prototype);
	virtual ~CBomb() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void		Priority_Update(_float _fTimeDelta) override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	
private:
	void Explode();

public:
	virtual HRESULT	 Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other) override;

	virtual void OnContact_Modify(const COLL_INFO& _0, const COLL_INFO& _1, CModifiableContacts& _ModifiableContacts, _bool _bIm0) override;

	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	_float m_fAccTime = { 0.f };
	_float m_fLifeTime = { 0.f };
	_float m_fExplodeTime = { 0.f };
	_bool m_isExplode = { false };

public:
	static CBomb* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END