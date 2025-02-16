#include "stdafx.h"
#include "State.h"
#include "PlayerState_IntoPortal.h"
#include "Portal.h" 
#include "Player.h"
#include "Actor_Dynamic.h"

CPlayerState_JumpToPortal::CPlayerState_JumpToPortal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMPTO_PORTAL)
{
}


void CPlayerState_JumpToPortal::Update(_float _fTimeDelta)
{
}

void CPlayerState_JumpToPortal::Enter()
{
	m_pPortal = (m_pOwner->Get_CurrentPortal());
    assert(m_pPortal);
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
    _vector vPortalPos = m_pPortal->Get_ControllerTransform()->Get_Transform(eCoord)->Get_State(CTransform::STATE_POSITION);
    if (COORDINATE_3D == eCoord)
    {
        static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Start_ParabolicTo(vPortalPos, XMConvertToRadians(45.f));
	}
    else
    {

    }
}

void CPlayerState_JumpToPortal::Exit()
{
}

void CPlayerState_JumpToPortal::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{
		if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKEXIT_RIGHT == iAnimIdx
            || (_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKEXIT_UP == iAnimIdx
            || (_uint)CPlayer::ANIM_STATE_2D::PLAYER_BOOKEXIT_DOWN == iAnimIdx)
		{
            m_pPortal->Use_Portal(m_pOwner);
		}
	}
}

void CPlayerState_JumpToPortal::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    switch (eShapeUse)
    {
    case Client::SHAPE_USE::SHAPE_TRIGER:
    {
        if (m_pPortal == _Other.pActorUserData->pOwner)
        {
            m_pPortal->Use_Portal(m_pOwner);
        }
        break;
    }
    }
}


