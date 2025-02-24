#pragma once
#include "State.h"

BEGIN(Client)


//y값 포함 추적 상태
class CChaseFlyState final : public CState
{
private:
	CChaseFlyState();
	virtual ~CChaseFlyState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CChaseFlyState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END