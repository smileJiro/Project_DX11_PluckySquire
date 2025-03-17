#include "stdafx.h"
#include "Friend_Thrash.h"
#include "GameInstance.h"

CFriend_Thrash::CFriend_Thrash(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFriend(_pDevice, _pContext)
{
}

CFriend_Thrash::CFriend_Thrash(const CFriend& _Prototype)
    :CFriend(_Prototype)
{
}

HRESULT CFriend_Thrash::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFriend_Thrash::Initialize(void* _pArg)
{
	FRIEND_THRASH_DESC* pDesc = static_cast<FRIEND_THRASH_DESC*>(_pArg);

	// Add
	pDesc->strPartBodyModelTag = TEXT("Thrash");
	pDesc->iModelTagLevelID = LEVEL_STATIC;
	pDesc->strDialogueTag = TEXT("Friend_Thrash_");



    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CFriend_Thrash::Change_AnimIndex_CurDirection()
{
	_vector vRightVector = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
	switch (m_eCurState)
	{
	case Client::CFriend::FRIEND_IDLE:
	{
		switch (m_eDirection)
		{
		case Client::CFriend::DIR_DOWN:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_DOWN, true);
			m_eCurAnimIndex = ANIM::THRASH_IDLE_DOWN;
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_UP, true);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRightVector));
			break;
		}
	}
		break;
	case Client::CFriend::FRIEND_MOVE:
	{
		switch (m_eDirection)
		{
		case Client::CFriend::DIR_DOWN:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_DOWN, true);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_UP, true);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRightVector));
			break;
		}
	}
		break;
	case Client::CFriend::FRIEND_CHASE:
	{
		switch (m_eDirection)
		{
		case Client::CFriend::DIR_DOWN:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_DOWN, true);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_UP, true);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRightVector));
			break;
		}
	}
		break;
	case Client::CFriend::FRIEND_ATTACK:
	{
		switch (m_eDirection)
		{
		case Client::CFriend::DIR_DOWN:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_ATTACK_DOWN);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_ATTACK_RIGHT);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_ATTACK_UP);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_ATTACK_RIGHT);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRightVector));
			break;
		}
	}
		break;
	case Client::CFriend::FRIEND_HIT:
	{
		switch (m_eDirection)
		{
		case Client::CFriend::DIR_DOWN:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_HIT_DOWN);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_HIT_RIGHT);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_HIT_UP);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_HIT_RIGHT);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRightVector));
			break;
		}
	}
		break;
	case Client::CFriend::FRIEND_TALK:
		break;
	default:
		break;
	}
}

void CFriend_Thrash::Finished_DialogueAction()
{
}

void CFriend_Thrash::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	switch (m_eCurState)
	{
	case Client::CFriend::FRIEND_IDLE:
		break;
	case Client::CFriend::FRIEND_MOVE:
		break;
	case Client::CFriend::FRIEND_CHASE:
		break;
	case Client::CFriend::FRIEND_ATTACK:
		m_eCurState = FRIEND_IDLE;
		break;
	case Client::CFriend::FRIEND_TALK:
		break;
	case Client::CFriend::FRIEND_MOJAM:
		if (iAnimIdx == ANIM::THRASH_MOJAM_INTO)
		{
			Switch_PartAnim(PART_BODY, ANIM::THRASH_MOJAM_MOJAM, false);
		}
		else if(iAnimIdx == ANIM::THRASH_MOJAM_MOJAM)
		{
			m_eCurState = FRIEND_IDLE;
		}
		break;
	case Client::CFriend::FRIEND_HIT:
		m_eCurState = FRIEND_IDLE;
		break;
	case Client::CFriend::FRIEND_ANY:
		m_eCurState = FRIEND_IDLE;
		break;
	default:
		m_eCurState = FRIEND_IDLE;
		break;
	}

}

CFriend_Thrash* CFriend_Thrash::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFriend_Thrash* pInstance = new CFriend_Thrash(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CFriend_Thrash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFriend_Thrash::Clone(void* _pArg)
{
	CFriend_Thrash* pInstance = new CFriend_Thrash(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CFriend_Thrash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFriend_Thrash::Free()
{

	__super::Free();
}

void CFriend_Thrash::Switch_AnimIndex_State()
{
	switch (m_eCurState)
	{
	case Client::CFriend::FRIEND_IDLE:
	case Client::CFriend::FRIEND_MOVE:
	case Client::CFriend::FRIEND_CHASE:
	case Client::CFriend::FRIEND_ATTACK:
	case Client::CFriend::FRIEND_TALK:
	case Client::CFriend::FRIEND_HIT:
		Change_AnimIndex_CurDirection();
		break;
	case Client::CFriend::FRIEND_MOJAM:
		Switch_PartAnim(PART_BODY, ANIM::THRASH_MOJAM_INTO, false);
		break;

	default:
		break;
	}

}

void CFriend_Thrash::Switch_PartAnim(_uint _iPartIndex, _uint _iAnimIndex, _bool _isLoop)
{
	m_eCurAnimIndex = (ANIM)_iAnimIndex;

	__super::Switch_PartAnim(_iPartIndex, _iAnimIndex, _isLoop);
}
