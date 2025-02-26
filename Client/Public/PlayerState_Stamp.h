#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_Stamp :
    public CPlayerState
{
	enum STAMP_STATE
	{
		STAMP_START,
		STAMP_KEEP,
		STAMP_SMASH,
		STAMP_STATE_LAST
	};
public:
    CPlayerState_Stamp(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
	virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
	STAMP_STATE m_eStampState = STAMP_START;
	_float m_fStampSpeed = 0.f;
	_float m_fRotateSpeed = 180.f;
	_float m_fStampingRange = 0.85f;

};

END