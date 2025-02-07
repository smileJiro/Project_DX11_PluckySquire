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

private:
	_float m_fAccTime = { 0.f };
	_float m_fDelayTime = {};

public:
	static CSneak_IdleState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END