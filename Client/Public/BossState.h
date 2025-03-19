#pragma once
#include "State.h"

BEGIN(Client)


class CBossState abstract : public CState
{
protected:
	CBossState();
	virtual ~CBossState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter()=0;
	virtual void State_Update(_float _fTimeDelta)=0;
	virtual void State_Exit()=0;

protected:
	void Delay_On()
	{
		m_isDelay = true;
	}
	void Delay_Off()
	{
		m_isDelay = false;
		m_fAccTime = 0.f;
	}

protected:
	_float m_fAccTime = { 0.f };
	_uint	 m_iAttackCount = { 0 };
	_bool m_isDelay = { false };
	_uint m_iNumAttack = { 0 };

public:
	virtual void Free() override;
};

END