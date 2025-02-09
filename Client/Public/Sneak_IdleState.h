#pragma once
#include "State.h"

BEGIN(Client)

class CSneak_IdleState final : public CState
{
private:
	CSneak_IdleState();
	virtual ~CSneak_IdleState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	void Set_Sneak_StopTime(_float _fStopTime) 
	{
		m_fDelayTime = _fStopTime;
	}

private:
	_float m_fAccTime = { 0.f };

public:
	static CSneak_IdleState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END