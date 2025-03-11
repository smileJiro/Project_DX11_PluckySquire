#pragma once
#include "State.h"

BEGIN(Client)


//어느정도 거리와 방향이 정해진 순찰 상태
class CSideScroll_AttackState final : public CState
{
private:
	CSideScroll_AttackState();
	virtual ~CSideScroll_AttackState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CSideScroll_AttackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END