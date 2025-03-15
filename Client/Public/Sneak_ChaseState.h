#pragma once
#include "State_Sneak.h"

BEGIN(Client)


class CSneak_ChaseState final : public CState_Sneak
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
	_bool m_isMove = { false };
	_bool m_isRenew = { true };
	_float3 m_vDir = {};
	_float m_fAccTime = { 0.f };

public:
	static CSneak_ChaseState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END