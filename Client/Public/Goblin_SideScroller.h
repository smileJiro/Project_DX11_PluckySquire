#pragma once
#include "Monster.h"
BEGIN(Client)
class CGoblin_SideScroller final : public CMonster
{
public:
	enum Animation2D
	{
		ALERT,
		ALERT_EYES,
		CHASE,
		CHASE_EYES,
		DEATH,
		DEATH_EYES,
		HIT,
		HIT_EYES,
		IDLE,
		IDLE_EYES,
		PATROL,
		PATROL_EYES,
		ANIM2D_LAST
	};

private:
	CGoblin_SideScroller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGoblin_SideScroller(const CGoblin_SideScroller& _Prototype);
	virtual ~CGoblin_SideScroller() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Attack() override;
	virtual void Change_Animation() override;
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);

public:
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual void On_Hit(CGameObject* _pHitter, _int _iDamg) override;

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CGoblin_SideScroller* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END