#pragma once
#include "State.h"

BEGIN(Client)


//공격 범위가 추적 범위보다 작은 근접공격 상태
class CMeleeAttackState final : public CState
{
private:
	CMeleeAttackState();
	virtual ~CMeleeAttackState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CMeleeAttackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END