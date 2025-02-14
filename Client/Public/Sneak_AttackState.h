#pragma once
#include "State_Sneak.h"

BEGIN(Client)


//공격 범위가 인식 범위보다 큰 원거리 공격 상태
class CSneak_AttackState final : public CState_Sneak
{
private:
	CSneak_AttackState();
	virtual ~CSneak_AttackState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	void After_Attack();

public:
	static CSneak_AttackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END