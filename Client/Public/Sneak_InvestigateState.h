#pragma once
#include "State_Sneak.h"

BEGIN(Client)


//소리가 난 위치로 이동하는 상태
class CSneak_InvestigateState final : public CState_Sneak
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

private:
	void Determine_Direction();

private:
	_bool m_isTurn = { false };
	_bool m_isMove = { false };
	_float3 m_vDir = {};
	_float m_fAccTime = 0.f;
	_bool m_isRenew = { true };

public:
	static CSneak_InvestigateState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END