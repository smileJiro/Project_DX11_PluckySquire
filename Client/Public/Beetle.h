#pragma once
#include "Monster.h"
class CBeetle final : public CMonster
{
public:
	enum Animation
	{
		ALERT,
		ATTACKSHORT,
		ATTACKRECOVERY,
		ATTACKSTRIKE,
		CONCERED,
		CONCERED_ATTACKREADY,
		DAMAGE,
		DIE,
		FLYLAND,
		FLY_LEFT,
		FLY_RIGHT,
		FLY_TAKEOFF,
		IDLE,
		RUN,
		WALK,
		CAUGHT,
		CAUGHTLOOP,
		DRAGGED,
		IDLE_STOP,
		PAUSE1,
		PAUSE2,
		STRAFE_BACK_FAST,
		STRAFE_BACK_SLOW,
		STRAFE_LEFT_FAST,
		TURN_LEFT,
		STRAFE_LEFT_SLOW,
		STRAFE_RIGHT_FAST,
		TURN_RIGHT,
		STRAFE_RIGHT_SLOW,
		WAIT1,
		WAIT2,
		LAST,
	};

private:
	CBeetle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBeetle(const CBeetle& _Prototype);
	virtual ~CBeetle() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void Change_Animation() override;
	void Alert_End(COORDINATE _eCoord, _uint iAnimIdx);
	void	Attack_End(COORDINATE _eCoord, _uint iAnimIdx);
	void	Attack_Recovery_End(COORDINATE _eCoord, _uint iAnimIdx);

private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();

public:
	static CBeetle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

