#pragma once
#include "BossState.h"

BEGIN(Client)


class CBossMoveState final : public CBossState
{
private:
	CBossMoveState();
	virtual ~CBossMoveState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CBossMoveState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END