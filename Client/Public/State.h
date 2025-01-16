#pragma once
#include "Base.h"

BEGIN(Engine)
class CController_Transform;
END

BEGIN(Client)

class CFSM;
class CMonster;

class CState abstract : public CBase
{
public:
	typedef struct tagStateDesc
	{
		_float fChaseRange;
		_float fAttackRange;
	}STATEDESC;

protected:
	CState();
	virtual ~CState() = default;

public:
	void Set_Owner(CMonster* _pOwner);
	void Set_FSM(CFSM* _pFSM)
	{
		m_pFSM = _pFSM;
		Safe_AddRef(m_pFSM);
	}

public:
	virtual HRESULT Initialize(void* _pArg);

public:
	//state 처음 들어갈 때
	virtual void State_Enter() = 0;
	//현재 상태에 대해 매 프레임마다 불리는 함수
	virtual void State_Update(_float _fTimeDelta) = 0;
	//state 전환시 빠져나가면서 불림
	virtual void State_Exit() = 0;

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	CController_Transform* m_pTargetTransform = { nullptr };
	//상태를 가지는 몬스터
	CMonster* m_pOwner = { nullptr };
	CFSM* m_pFSM = { nullptr };

	_float	m_fChaseRange = {};
	_float	m_fAttackRange = {};

public:
	virtual void Free() override;
};

END