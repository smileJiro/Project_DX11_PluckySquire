#pragma once
#include "State.h"

BEGIN(Client)

class CChaseWalkState final : public CState
{
private:
	CChaseWalkState();
	virtual ~CChaseWalkState() = default;

public:
	virtual HRESULT Initialize() override;
	//virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CChaseWalkState* Create();
	//virtual CChaseWalkState* Clone(void* _pArg);

public:
	virtual void Free() override;
};

END