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


    void Switch_To_PushAnimation(COORDINATE _eCoord );
    void Switch_To_PullAnimation(COORDINATE _eCoord, _vector _vMoveDir);

private:
	_float m_fDragMoveSpeed = 0.f;
	CDraggableObject* m_pDraggableObject = nullptr;

	_float m_fAnimTransitionTime = 0.2f;
	_float m_fAnimTransitionTimeAcc = 0.0f;

    _float m_fPushThreshold = 0.3f;
    _vector m_vHoldOffset = { 0.f,0.f,0.f };
    F_DIRECTION m_eOldDragDirection = F_DIRECTION::F_DIR_LAST;
	_uint m_iAdditionalShapeIndex = 0;
};

END