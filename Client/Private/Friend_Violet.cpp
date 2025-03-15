#include "stdafx.h"
#include "Friend_Violet.h"
#include "GameInstance.h"

CFriend_Violet::CFriend_Violet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFriend(_pDevice, _pContext)
{
}

CFriend_Violet::CFriend_Violet(const CFriend& _Prototype)
    :CFriend(_Prototype)
{
}

HRESULT CFriend_Violet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFriend_Violet::Initialize(void* _pArg)
{
	FRIEND_VIOLET_DESC* pDesc = static_cast<FRIEND_VIOLET_DESC*>(_pArg);

	// Add
	pDesc->strPartBodyModelTag = TEXT("Violet");
	pDesc->iModelTagLevelID = LEVEL_STATIC;
	pDesc->strDialogueTag = TEXT("Friend_Violet_");



    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CFriend_Violet::Change_AnimIndex_CurDirection()
{
	_vector vRightVector = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
	switch (m_eCurState)
	{
	case Client::CFriend::FRIEND_IDLE:
	{
		switch (m_eDirection)
		{
		case Client::CFriend::DIR_DOWN:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_IDLE_DOWN, true);
			m_eCurAnimIndex = ANIM::VIOLET_IDLE_DOWN;
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_IDLE_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_IDLE_UP, true);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_IDLE_RIGHT, true);
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
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_DOWN, true);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_UP, true);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_RIGHT, true);
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
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_DOWN, true);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_RIGHT, true);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_UP, true);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_RUN_RIGHT, true);
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
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_ATTACK_DOWN);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_ATTACK_RIGHT);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_ATTACK_UP);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_ATTACK_RIGHT);
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
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_HIT_DOWN);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_HIT_RIGHT);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_HIT_UP);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::VIOLET_HIT_RIGHT);
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

void CFriend_Violet::Finished_DialogueAction()
{
}

void CFriend_Violet::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
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
		m_eCurState = FRIEND_IDLE;
		break;
	case Client::CFriend::FRIEND_HIT:
		m_eCurState = FRIEND_IDLE;
		break;
	default:
		break;
	}

}

CFriend_Violet* CFriend_Violet::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFriend_Violet* pInstance = new CFriend_Violet(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CFriend_Violet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFriend_Violet::Clone(void* _pArg)
{
	CFriend_Violet* pInstance = new CFriend_Violet(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CFriend_Violet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFriend_Violet::Free()
{

	__super::Free();
}

void CFriend_Violet::Switch_AnimIndex_State()
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
		break;

	default:
		break;
	}

}

void CFriend_Violet::Switch_PartAnim(_uint _iPartIndex, _uint _iAnimIndex, _bool _isLoop)
{
	m_eCurAnimIndex = (ANIM)_iAnimIndex;

	__super::Switch_PartAnim(_iPartIndex, _iAnimIndex, _isLoop);
}
