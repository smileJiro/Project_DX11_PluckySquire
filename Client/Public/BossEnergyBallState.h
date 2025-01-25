#pragma once
#include "BossState.h"

BEGIN(Client)


class CBossEnergyBallState final : public CBossState
{
private:
	CBossEnergyBallState();
	virtual ~CBossEnergyBallState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CBossEnergyBallState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END