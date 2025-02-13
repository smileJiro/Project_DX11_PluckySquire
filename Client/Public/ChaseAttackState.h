#pragma once
#include "State.h"

BEGIN(Client)


//공격 없이 추적으로 공격이 종료
class CChaseAttackState final : public CState
{
private:
	CChaseAttackState();
	virtual ~CChaseAttackState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CChaseAttackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END