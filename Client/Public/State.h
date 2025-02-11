#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)
class CFSM;
class CMonster;

class CState abstract : public CBase
{
public:
	typedef struct tagStateDesc
	{
		_float fAlertRange;
		_float fChaseRange;
		_float fAttackRange;
		_float fAlert2DRange;
		_float fChase2DRange;
		_float fAttack2DRange;
		_float fDelayTime;
		_float fCoolTime;
		_int iCurLevel;
	}STATEDESC;

protected:
	CState();
	virtual ~CState() = default;

public:
	void Set_Owner(CMonster* _pOwner);
	void Set_FSM(CFSM* _pFSM)
	{
		m_pFSM = _pFSM;
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

	//FSM에 상태가 끝나고 전환가능한지 알려줌
	_bool IsTransition_Ready() {};

public:
	HRESULT CleanUp();

protected:
	_float Get_CurCoordRange(MONSTER_STATE _eState);
	_bool Check_Target3D(_bool _isSneak = false);
	void	Set_Sneak_InvestigatePos(_fvector _vPosition);
	void Determine_AvoidDirection(_fvector& _vDestination, _float3* _vDirection);

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	CGameObject* m_pTarget = { nullptr };
	//상태를 가지는 몬스터
	CMonster* m_pOwner = { nullptr };
	CFSM* m_pFSM = { nullptr };
	_int m_iCurLevel = { LEVEL_END };

	_float	m_fAlertRange = {};
	_float	m_fChaseRange = {};
	_float	m_fAttackRange = {};
	_float	m_fAlert2DRange = {};
	_float	m_fChase2DRange = {};
	_float	m_fAttack2DRange = {};
	_float	m_fDelayTime = {};
	_float	m_fCoolTime = {};
	_float3 m_vSneakPos = {};
	_float m_fRayDis = { 5.f };

public:
	virtual void Free() override;
};

END