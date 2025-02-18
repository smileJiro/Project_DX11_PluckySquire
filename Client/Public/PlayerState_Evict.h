#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_Evict :
    public CPlayerState
{
public:
    enum EVICT_STATE
    {
		GRABBED,
		SMUSHLOOP,
        BOOKOUT,
		EVICT_LAST
    };
public:

    CPlayerState_Evict(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
	EVICT_STATE m_eCurState = GRABBED;
	_float m_fSmushTime = 1.5f;
	_float m_fSmushTimeAcc = 0.f;
};

END