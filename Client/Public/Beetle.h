#pragma once
#include "Monster.h"
class CBeetle final : public CMonster
{
public:
	enum Animation
	{
		Alert,
		AttackShort,
		AttackRecovery,
		AttackStrike,
		Concered,
		Concered_AttackReady,
		Damage,
		Death,
		FlyLand,
		Fly_Left,
		Fly_Right,
		Fly_Takeoff,
		Idle,
		Run,
		Walk,
		Caught,
		CaughtLoop,
		Dragged,
		Idle_Stop,
		Pause1,
		Pause2,
		Strafe_Back_Fast,
		Strafe_Back_Slow,
		Strafe_Left_Fast,
		Turn_Left,
		Strafe_Left_Slow,
		Strafe_Right_Fast,
		Turn_Right,
		Strafe_Right_Slow,
		Wait1,
		Wait2,
		End,
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

private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();

public:
	static CBeetle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

