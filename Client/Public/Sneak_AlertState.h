#pragma once
#include "State.h"

BEGIN(Client)


//플레이어가 시야 내에 들어와서 인식 (애니메이션 재생)
class CSneak_AlertState final : public CState
{
private:
	CSneak_AlertState();
	virtual ~CSneak_AlertState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CSneak_AlertState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END