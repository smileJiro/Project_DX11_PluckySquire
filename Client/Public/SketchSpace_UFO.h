#pragma once
#include "Monster.h"
BEGIN(Client)
class CSketchSpace_UFO final : public CMonster
{
public:
	enum Animation2D
	{
		ATTACK,
		DIE,
		HIT,
		IDLE,
		ANIM2D_LAST
	};

	typedef struct tagSideScroll_Desc : public CMonster::MONSTER_DESC
	{
		SIDESCROLL_PATROLBOUND eSideScroll_Bound;
	}SIDESCROLLDESC;

private:
	CSketchSpace_UFO(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSketchSpace_UFO(const CSketchSpace_UFO& _Prototype);
	virtual ~CSketchSpace_UFO() = default;

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

	virtual void Monster_Move(_fvector _vDirection) override;

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
	_float3		m_vCenter = {};
	_float		m_fAngle = { 0.f };
	SIDESCROLL_PATROLBOUND m_eSideScroll_Bound = {};

public:
	static CSketchSpace_UFO* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END