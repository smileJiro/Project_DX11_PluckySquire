#pragma once
#include "State.h"

BEGIN(Client)


//어느정도 거리와 방향이 정해진 순찰 상태
class CSideScroll_PatrolState final : public CState
{
public:
	typedef struct tagPatrolBound {
		_float fMinX;
		_float fMaxX;
	}PATROLBOUND;

private:
	CSideScroll_PatrolState();
	virtual ~CSideScroll_PatrolState() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Initialize_SideScroll_PatrolBound(SIDESCROLL_PATROLBOUND _eSideScroll_Bound);

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	PATROLBOUND m_tPatrolBound = {};

public:
	static CSideScroll_PatrolState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END