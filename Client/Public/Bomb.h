#pragma once
#include "CarriableObject.h"
BEGIN(Engine)
class CEffect_System;
END
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

public:
	void Set_LifeTime(_float _fLifeTime)
	{
		m_fLifeTime = _fLifeTime;
	}
	void Set_Time_On();
	void Set_Time_Off();
	void Bomb_Shape_Enable(_bool _isEnable);

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

	virtual void On_PickUpStart(CPlayer* _pPalyer, _fmatrix _matPlayerOffset) override;

public:
	void Start_Parabola(_fvector _vStartPos, _fvector _vEndPos, _float _fParabolaTime);
	void Start_Parabola_3D(_fvector _vEndPos, _float _fLaunchAngleRadian, _float _fGravityMag = 9.81f * 3.0f);
private:
	void Action_Parabola(_float _fTimeDelta);
private:
	_bool m_isParabolaAction = false;
	_float3 m_vStartPos = {};
	_float3 m_vEndPos = {};
	_float2 m_vParabolaTime = { 1.0f, 0.0f };

private:
	_float m_fAccTime = { 0.f };
	_float m_fLifeTime = { 0.f };
	_float m_fExplodeTime = { 0.f };
	_bool m_isExplode = { false };
	_bool m_isOn = { true }; //심지에 불 붙은 상태

	class CEffect_System* m_pFuseEffect = { nullptr };

public:
	static CBomb* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END