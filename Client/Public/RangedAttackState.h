#pragma once
#include "State.h"

BEGIN(Client)


//공격 범위가 추적 범위보다 큰 원거리 공격 상태
class CRangedAttackState final : public CState
{
private:
	CRangedAttackState();
	virtual ~CRangedAttackState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CRangedAttackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END