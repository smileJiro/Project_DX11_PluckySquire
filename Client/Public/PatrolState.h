#pragma once
#include "State.h"

BEGIN(Client)


//어느정도 거리와 방향이 정해진 순찰 상태
class CPatrolState final : public CState
{
public:
	typedef struct tagPatrolBound {
		_float fMinX;
		_float fMaxX;
		_float fMinZ;
		_float fMaxZ;
		_float fMinDis;
		_float fMaxDis;
	}PATROLBOUND;

private:
	CPatrolState();
	virtual ~CPatrolState() = default;

public:
	void Set_Bound(_float3& _vPosition);

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	void PatrolMove(_float _fTimeDelta, _int _iDir);

private:
	_float m_fAccTime = { 0.f };
	_float m_fDelayTime = {};
	_float m_fAccDistance = { 0.f };
	_float m_fMoveDistance = {};
	_float m_fPatrolOffset = {};
	PATROLBOUND m_tPatrolBound = {};
	_int m_iPrevDir = {};
	_bool m_isDelay = { false };
	_bool m_isMove = { false };

public:
	static CPatrolState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END