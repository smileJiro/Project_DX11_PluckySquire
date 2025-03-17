#pragma once
#include "State.h"

BEGIN(Client)

class CFormationBackState final : public CState
{
private:
	CFormationBackState();
	virtual ~CFormationBackState() = default;

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
	_float m_fOriginSpeed = { 0.f };

public:
	static CFormationBackState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END