#pragma once
#include "Monster.h"
BEGIN(Client)
class CSoldier_Bomb final : public CMonster
{
public:
	enum Animation
	{
		ALERT = 13,
		CHASE = 15,
		BLOCK_HOLD_IMPACT,
		BLOCK_HOLD_LOOP,
		BLOCK_HOLD_UP,
		DASH_ATTACK_LOOP,
		DASH_ATTACK_RECOVERY,
		DASH_ATTACK_STARTUP,
		DEATH_01 = 22,
		DEATH_02_EDIT,
		FAST_WALK,
		HIT_BACK,
		HIT_FRONT,
		IDLE,
		SALUTE,
		SHAKE_HEAD,
		STRAFE_BWD,
		STRAFE_LEFT_01,
		STRAFE_LEFT_02,
		STRAFE_RIGHT_01,
		STRAFE_RIGHT_02,
		STRETCH_01,
		STRETCH_02,
		WALK,
		BOMB_LOOP,
		BOMB_OUT,
		BOMB_THROW,
		ARREST,
		LAST,
	};

private:
	CSoldier_Bomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSoldier_Bomb(const CSoldier_Bomb& _Prototype);
	virtual ~CSoldier_Bomb() = default;

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
	static CSoldier_Bomb* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END