#pragma once
#include "State.h"

BEGIN(Client)


//처음 인식 상태 (있다면 애니메이션 재생)
class CAlertState final : public CState
{
private:
	CAlertState();
	virtual ~CAlertState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	_bool m_isTurn = { false };

public:
	static CAlertState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END