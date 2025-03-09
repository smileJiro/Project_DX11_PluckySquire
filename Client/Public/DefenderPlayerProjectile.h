#pragma once
#include "ScrollModelObject.h"

BEGIN(Client)
class CDefenderPlayerProjectile :
    public CScrollModelObject
{
public:
	typedef struct tagDefenderPlayerProjectileDesc : public CScrollModelObject::SCROLLMODELOBJ_DESC
	{

	}DEFENDERPLAYER_PROJECTILE_DESC;

private:
	CDefenderPlayerProjectile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderPlayerProjectile(const CDefenderPlayerProjectile& _Prototype);
	virtual ~CDefenderPlayerProjectile() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void		Priority_Update(_float _fTimeDelta) override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

private:

	_float m_fLifeTime = 2.f;
	_float m_fLifeTimeAcc = 0.f;
public:
	static CDefenderPlayerProjectile* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END