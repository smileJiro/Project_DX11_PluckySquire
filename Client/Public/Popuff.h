#pragma once
#include "Monster.h"
BEGIN(Client)
class CPopuff final : public CMonster
{
public:
	enum Animation
	{
		ATTACK,
		DIE,
		IDLE,
		WALK,
		HIT,
		LAST,
	};

private:
	CPopuff(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPopuff(const CPopuff& _Prototype);
	virtual ~CPopuff() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Change_Animation() override;
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CPopuff* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END