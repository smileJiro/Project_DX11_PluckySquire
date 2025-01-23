#pragma once
#include "State.h"

BEGIN(Client)


class CBossEnergyBallState final : public CState
{
private:
	CBossEnergyBallState();
	virtual ~CBossEnergyBallState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	void Delay_On()
	{
		m_isDelay = true;
	}
	void Delay_Off()
	{
		m_isDelay = false;
		m_fAccTime = 0.f;
	}

private:
	_float m_fDelayTime = {};
	_float m_fAccTime = { 0.f };
	_uint	 m_iAttackCount = { 0 };
	_bool m_isDelay = { false };

public:
	static CBossEnergyBallState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END