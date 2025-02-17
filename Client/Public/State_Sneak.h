#pragma once
#include "State.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)
class CFSM;
class CMonster;

class CState_Sneak abstract : public CState
{
public:
	typedef struct tagStateSneakDesc : STATEDESC
	{
		SNEAKWAYPOINTINDEX eWayIndex = SNEAKWAYPOINTINDEX::LAST;
	}SNEAKSTATEDESC;

	struct Node
	{
		_float3 vPosition;
		vector<_uint>Neighbors;
	};

	struct compare
	{
		bool operator()(pair<_float, pair<_uint, _uint>>a, pair<_float, pair<_uint, _uint>>b)
		{
			return a.first > b.first;
		}
	};

protected:
	CState_Sneak();
	virtual ~CState_Sneak() = default;

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

	

protected:
	//잠입 모드가 아니어도 필요할 것으로 보임. 일단 잠입상태에 둠
	HRESULT Initialize_WayPoints(SNEAKWAYPOINTINDEX _eWayIndex);
	void Initialize_PatrolPoints(SNEAKWAYPOINTINDEX _iWayIndex);
	void Determine_NextDirection(_fvector& _vDestination, _float3* _vDirection);
	void Determine_BackDirection(_fvector& _vDestination, _float3* _vDirection);

public:
	virtual HRESULT CleanUp();

protected:
	_float3 m_vSneakPos = {};
	SNEAKWAYPOINTINDEX m_eWayIndex = {};
	vector<Node> m_WayPoints = {};
	map<pair<_uint, _uint>, _float> m_Weights;
	vector<_uint> m_Ways;

	vector<_uint> m_PatrolWays = {};

	_uint m_iCurWayIndex = 0;
	_bool m_isOnWay = false;
	_bool m_isPathFind = false;	//true면 길찾기 수행

public:
	virtual void Free() override;
};

END