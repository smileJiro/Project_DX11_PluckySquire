#pragma once
#include "State.h"

BEGIN(Client)

class CFormationMoveState final : public CState
{
private:
	CFormationMoveState();
	virtual ~CFormationMoveState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	void Move();

private:
	_float m_fAccTime = { 0.f };
	_float3 m_vNextPos = {};
	_bool m_isTurn = { false };
	_bool m_isMove = { false };

public:
	static CFormationMoveState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END