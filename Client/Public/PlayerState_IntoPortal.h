#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPortal;
class CPlayerState_JumpToPortal :
    public CPlayerState
{
public:
    CPlayerState_JumpToPortal(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

    virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
    _bool Check_ExceedPortal(_vector vPos);
private:
    CPortal* m_pPortal = nullptr;
	_vector m_vPortalPos = {};
	_vector m_vPlayerStartPos = {};
	_float m_f3DDistanceThreshold = 0.15f;
    _float m_f3DJumpRadianMin =XMConvertToRadians(45.f);
    _float m_f3DJumpRadianMax =XMConvertToRadians(89.f);

};

END