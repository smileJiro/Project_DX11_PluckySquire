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
	pDesc->strPartBodyModelTag = TEXT("Thrash");
	pDesc->iModelTagLevelID = LEVEL_STATIC;

    if (FAILED(Initialize(_pArg)))
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
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_DOWN);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_RIGHT);
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_UP);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_IDLE_RIGHT);
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
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_DOWN);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT);
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_UP);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT);
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
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_DOWN);
			break;
		case Client::CFriend::DIR_RIGHT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT);
			break;
		case Client::CFriend::DIR_UP:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_UP);
			break;
		case Client::CFriend::DIR_LEFT:
			Switch_PartAnim(PART_BODY, ANIM::THRASH_RUN_RIGHT);
			Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRightVector));
			break;
		}
	}
		break;
	case Client::CFriend::FRIEND_ATTACK:
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

}
