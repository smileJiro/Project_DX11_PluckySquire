#pragma once
#include "PlayerState.h"
BEGIN(Client)

class CPlayerState_GetItem :
    public CPlayerState
{
public:
    CPlayerState_GetItem(CPlayer* _pOwner);
    CPlayerState_GetItem(CPlayer* _pOwner, PLAYER_2D_ITEM_ID _eItemID);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx) override;
	void On_FXAnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);
private:

	CModelObject* m_pFX = nullptr;
	CModelObject* m_pItemImg = nullptr;
    _vector m_vItemOffset = { 100.f,100.f };
	PLAYER_2D_ITEM_ID m_eItemID = PLAYER_2D_ITEM_ID::LAST;
};

END