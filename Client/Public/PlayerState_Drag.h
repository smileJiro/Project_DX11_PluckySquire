#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CDraggableObject;
class CPlayerState_Drag :
    public CPlayerState
{
public:
    CPlayerState_Drag(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;


private:
	_float m_fDragMoveSpeed = 0.f;
	CDraggableObject* m_pDraggableObject = nullptr;
};

END