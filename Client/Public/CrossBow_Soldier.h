#pragma once
#include "Monster.h"
BEGIN(Client)
class CCrossBow_Soldier final : public CMonster
{
public:
	enum Animation
	{
		CROSSBOW_ALERT,
		CROSSBOW_DEATH_ALT,
		CROSSBOW_DEATH,
		CROSSBOW_HIT_FWDS,
		CROSSBOW_IDLE,
		CROSSBOW_SHOOT,
		CROSSBOW_SHOOT_IDLE,
		CROSSBOW_SHOOT_RECOVERY,
		CROSSBOW_STRAFE_BWD,
		CROSSBOW_STRAFE_FWD,
		CROSSBOW_STRAFE_LEFT,
		CROSSBOW_STRAFE_RIGHT,
		CROSSBOW_WALK,
		LAST,
	};

private:
	CCrossBow_Soldier(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCrossBow_Soldier(const CCrossBow_Soldier& _Prototype);
	virtual ~CCrossBow_Soldier() = default;

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
	static CCrossBow_Soldier* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END