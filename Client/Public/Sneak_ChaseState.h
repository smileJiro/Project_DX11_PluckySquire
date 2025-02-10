#pragma once
#include "State.h"

BEGIN(Client)


//공격 범위가 추적 범위보다 작고 이동값이 일정한 추적 상태
class CSneak_ChaseState final : public CState
{
private:
	CSneak_ChaseState();
	virtual ~CSneak_ChaseState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_bool m_isTurn = { false };
	_float3 m_vDir = {};

public:
	static CSneak_ChaseState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END