#pragma once
#include "State.h"

BEGIN(Client)


class CBomb_AlertState final : public CState
{
private:
	CBomb_AlertState();
	virtual ~CBomb_AlertState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CBomb_AlertState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END