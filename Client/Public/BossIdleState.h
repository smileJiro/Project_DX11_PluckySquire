#pragma once
#include "State.h"

BEGIN(Client)


class CBossIdleState final : public CState
{
private:
	CBossIdleState();
	virtual ~CBossIdleState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_float m_fCoolTime = { 0.f };
	_float m_fAccTime = { 0.f };

public:
	static CBossIdleState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END