#pragma once
#include "BossState.h"

BEGIN(Client)


class CBossHomingBallState final : public CBossState
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

public:
	static CBossHomingBallState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END