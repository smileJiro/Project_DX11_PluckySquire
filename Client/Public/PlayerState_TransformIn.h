#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_TransformIn :
    public CPlayerState
{
public:
    CPlayerState_TransformIn(CPlayer* _pOwner, CZip_C8* _pZip);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:
    _float m_fAttachkProgress = 0.72f;
    _bool m_bAttach = false;
    class CZip_C8* m_pZip = nullptr;
};

END