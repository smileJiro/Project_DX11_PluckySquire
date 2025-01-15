#pragma once
#include "State.h"

BEGIN(Client)

class CIdleState final : public CState
{
private:
	CIdleState();
	virtual ~CIdleState() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	//virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CIdleState* Create();
	//virtual CIdleState* Clone(void* _pArg);

public:
	virtual void Free() override;
};

END