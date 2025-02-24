#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerState_LaydownObject :
    public CPlayerState
{
public:
    CPlayerState_LaydownObject(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;

    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
    void Lay();

private:
    CCarriableObject* m_pCarriableObject = nullptr;

    _float m_f3DLayStartProgress = 0.44f;
    _float m_f3DFloorPrgress = 0.28f;
    _float m_f2DLayStartProgress = 0.778f;
    _float m_f2DFloorPrgress = 0.185f;
	_float m_fLerpStartTime = 0.f;
	_float m_fLerpEndTime = 0.f;
	_float m_fTimeAcc = 0.f;
    _bool m_bLayed= false;
    KEYFRAME m_tCarryingKeyFrame;
    KEYFRAME m_tLayDownFrame;
};

END