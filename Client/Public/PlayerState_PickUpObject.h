#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CCarriableObject;
class CPlayerState_PickUpObject :
    public CPlayerState
{
public:
    CPlayerState_PickUpObject(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;

    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;
    void Align();
private:
	CCarriableObject* m_pCarriableObject = nullptr;
    _bool m_bHeadUp = false;
    _float m_f2DAlignStartProgress = 0.05f;
    _float m_f2DAlignEndProgress = 0.185f;
    _float m_f2DHeadProgress = 0.778f;
    _float m_f3DAlignStartProgress = 0.145f;
    _float m_f3DAlignEndProgress = 0.26f;
    _float m_f3DHeadProgress = 0.43f;
    KEYFRAME m_tOriginalKeyFrame;
    KEYFRAME m_tPickupKeyFrame;
    KEYFRAME m_tCarryingKeyFrame;
};

END