#pragma once
#include "State.h"

BEGIN(Client)


class CBossHomingBallState final : public CState
{
private:
	CBossHomingBallState();
	virtual ~CBossHomingBallState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_float m_fDelayTime = {};
	_float m_fAccTime = { 0.f };

public:
	static CBossHomingBallState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END