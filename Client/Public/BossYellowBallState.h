#pragma once
#include "BossState.h"

BEGIN(Client)


class CBossYellowBallState final : public CBossState
{
private:
	CBossYellowBallState();
	virtual ~CBossYellowBallState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CBossYellowBallState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END