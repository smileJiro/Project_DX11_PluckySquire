#pragma once
#include "State.h"

BEGIN(Client)


//블록커 넘는 점프 상태
class CMonsterJumpState final : public CState
{
public:
	typedef struct tagMonsterJumpStateDesc : public STATEDESC
	{
	}MONSTERMOVE_DESC;

private:
	CMonsterJumpState();
	virtual ~CMonsterJumpState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

public:
	static CMonsterJumpState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END