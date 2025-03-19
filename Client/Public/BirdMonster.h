#pragma once
#include "Monster.h"
BEGIN(Client)
class CBirdMonster final : public CMonster
{
public:
	enum Animation
	{
		ALERT,
		ALERT_EDIT,
		ATTACK,
		DIE,
		HIT,
		IDLE,
		LAST,
	};

private:
	CBirdMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBirdMonster(const CBirdMonster& _Prototype);
	virtual ~CBirdMonster() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void	On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce) override;


public:
	virtual void Change_Animation() override;
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);
	virtual void Attack() override;

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CBirdMonster* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END