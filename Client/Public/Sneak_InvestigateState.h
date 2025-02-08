#pragma once
#include "State.h"

BEGIN(Client)


//소리가 난 위치로 이동하는 상태
class CSneak_InvestigateState final : public CState
{
private:
	CSneak_InvestigateState();
	virtual ~CSneak_InvestigateState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CSneak_InvestigateState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END