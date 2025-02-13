#pragma once
#include "State.h"

BEGIN(Client)


//다음 동작에 공격이 없는 대기 상태
class CStandbyNoneAttackState final : public CState
{
private:
	CStandbyNoneAttackState();
	virtual ~CStandbyNoneAttackState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_float m_fAccTime = { 0.f };
	_bool m_isDelay = { false };
	_bool m_isCool = { false };

public:
	static CStandbyNoneAttackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END