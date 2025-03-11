#pragma once
#include "Monster.h"
BEGIN(Client)
class CSketchSpace_SpikeBall final : public CMonster
{
public:
	enum Animation2D
	{
		IDLE,
		ANIM2D_LAST
	};

	typedef struct tagSideScroll_Desc : public CMonster::MONSTER_DESC
	{
		SIDESCROLL_PATROLBOUND eSideScroll_Bound;
	}SIDESCROLLDESC;

private:
	CSketchSpace_SpikeBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSketchSpace_SpikeBall(const CSketchSpace_SpikeBall& _Prototype);
	virtual ~CSketchSpace_SpikeBall() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual void On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce) override;

public:
	virtual void			Enter_Section(const _wstring _strIncludeSectionName) override;

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

private:
	SIDESCROLL_PATROLBOUND m_eSideScroll_Bound = {};

public:
	static CSketchSpace_SpikeBall* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END