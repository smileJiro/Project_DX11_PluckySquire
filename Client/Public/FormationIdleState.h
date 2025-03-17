#pragma once
#include "State.h"

BEGIN(Client)

class CFormationIdleState final : public CState
{
private:
	CFormationIdleState();
	virtual ~CFormationIdleState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CFormationIdleState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END