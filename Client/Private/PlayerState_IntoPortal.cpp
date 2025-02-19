#include "stdafx.h"
#include "State.h"
#include "PlayerState_IntoPortal.h"
#include "Portal.h" 
#include "Player.h"
#include "Actor_Dynamic.h"
#include "GameInstance.h"

CPlayerState_JumpToPortal::CPlayerState_JumpToPortal(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::JUMPTO_PORTAL)
{
}


void CPlayerState_JumpToPortal::Update(_float _fTimeDelta)
{
    if (m_bPortaled)
        return;
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	NORMAL_DIRECTION eNormal;
    if (COORDINATE_2D == eCoord)
    {
        _float fProgress = m_pOwner->Get_AnimProgress();
        if (fProgress >= 0.86f)
        {
            m_pPortal->Use_Portal(m_pOwner,&eNormal);
            m_pOwner->Set_PortalNormal(eNormal);
            m_bPortaled = true;
            return;
        }
	}
    else
    {
        if(m_pPortal->Get_Distance(eCoord, m_pOwner) <= m_f3DDistanceThreshold)
		{
			m_pPortal->Use_Portal(m_pOwner, &eNormal);
            m_pOwner->Set_PortalNormal(eNormal);
			m_bPortaled = true;
			return;
		}
    }

}

void CPlayerState_JumpToPortal::Enter()
{
	m_pPortal = (m_pOwner->Get_CurrentPortal());
    assert(m_pPortal);
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
        m_vPortalPos = m_pPortal->Get_ControllerTransform()->Get_Transform(eCoord)->Get_State(CTransform::STATE_POSITION);
    _float fYDIff   = XMVectorGetY(m_vPortalPos) - XMVectorGetY(m_pOwner->Get_FinalPosition());
	_float fXZDiff = XMVectorGetX(XMVector3Length(XMVectorSetY(m_vPortalPos, 0.f) - XMVectorSetY(m_pOwner->Get_FinalPosition(), 0.f)));
	_float fYRadian = max(m_f3DJumpRadianMin, atan2f(fYDIff, fXZDiff)) + XMConvertToRadians(10.f);

    if (COORDINATE_3D == eCoord)
    {
        static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_FOOT,false);
        static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Start_ParabolicTo(m_vPortalPos, fYRadian);
	}
    else
    {

    }

    m_pGameInstance->Start_SFX_Delay(_wstring(L"A_sfx_jot_vocal_portal_jump-") + to_wstring(rand() % 7), 0.15f, 50.f);
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
            NORMAL_DIRECTION eNormal;
            m_pPortal->Use_Portal(m_pOwner, &eNormal);
			m_pOwner->Set_PortalNormal(eNormal);
		}
	}
}

void CPlayerState_JumpToPortal::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    //if (m_bPortaled)
    //    return;
    //SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
    //switch (eShapeUse)
    //{
    //case Client::SHAPE_USE::SHAPE_TRIGER:
    //{
    //    if (m_pPortal == _Other.pActorUserData->pOwner)
    //    {
    //        m_pPortal->Use_Portal(m_pOwner);
    //        m_bPortaled = true;
    //    }
    //    break;
    //}
    //}
}


