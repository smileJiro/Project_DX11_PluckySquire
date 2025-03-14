#pragma once
#include "State_Sneak.h"

BEGIN(Client)


class CChase_WayState final : public CState_Sneak
{
private:
	CChase_WayState();
	virtual ~CChase_WayState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Set_WayPoint(SNEAKWAYPOINTINDEX _eWayIndex);

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_bool m_isTurn = { false };
	_bool m_isMove = { false };
	_bool m_isRenew = { true };
	_float3 m_vDir = {};
	_float m_fAccTime = { 0.f };

public:
	static CChase_WayState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END