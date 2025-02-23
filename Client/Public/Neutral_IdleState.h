#pragma once
#include "State.h"

BEGIN(Client)

class CNeutral_IdleState final : public CState
{
private:
	CNeutral_IdleState();
	virtual ~CNeutral_IdleState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_float m_fAccTime = { 0.f };

public:
	static CNeutral_IdleState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END