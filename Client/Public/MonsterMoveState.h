#pragma once
#include "State.h"

BEGIN(Client)


//일정 이동 상태
class CMonsterMoveState final : public CState
{
public:
	typedef struct tagMonsterMoveStateDesc : public STATEDESC
	{
		MONSTER_STATE eMoveNextState = { MONSTER_STATE::LAST };
	}MONSTERMOVE_DESC;

private:
	CMonsterMoveState();
	virtual ~CMonsterMoveState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;


private:
	MONSTER_STATE m_eMoveNextState = { MONSTER_STATE::LAST };

public:
	static CMonsterMoveState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END