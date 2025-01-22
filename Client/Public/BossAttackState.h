#pragma once
#include "State.h"

BEGIN(Client)


class CBossAttackState final : public CState
{
private:
	CBossAttackState();
	virtual ~CBossAttackState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CBossAttackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END