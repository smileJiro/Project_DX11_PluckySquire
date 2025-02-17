#pragma once
#include "State.h"

BEGIN(Client)

class CSideScroll_HitState final : public CState
{
private:
	CSideScroll_HitState();
	virtual ~CSideScroll_HitState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_float m_fAccTime = { 0.f };

public:
	static CSideScroll_HitState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END