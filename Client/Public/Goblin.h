#pragma once
#include "Monster.h"
class CGoblin final : public CMonster
{
public:
	enum Animation
	{
		ALERT,
		CHASE,
		CINE_LEAVES_BOOK_GT,
		DEATH,
		HIT,
		IDLE,
		PATROL,
		SLEEP,
		SLEEP_START,
		WAKE,
		LAST,
	};

private:
	CGoblin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGoblin(const CGoblin& _Prototype);
	virtual ~CGoblin() = default;

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

private:
	virtual	HRESULT					Ready_ActorDesc(void* _pArg);
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CGoblin* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

