#include "stdafx.h"
#include "NPC.h"
#include "GameInstance.h"
#include "FSM.h"
#include "UI_Manager.h"
#include "StateMachine.h"

CNPC::CNPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CNPC::CNPC(const CNPC& _Prototype)
	: CCharacter(_Prototype)
{
}

HRESULT CNPC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC::Initialize(void* _pArg)
{
	NPC_DESC* pDesc = static_cast<NPC_DESC*>(_pArg);

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_iMainIndex = pDesc->iMainIndex;
	m_iSubIndex = pDesc->iSubIndex; 
	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Player"), 0);
	wsprintf(m_strDialogueIndex, pDesc->strDialogueIndex);
	wsprintf(m_strCurSecion, pDesc->strLocateSection);

	Safe_AddRef(m_pTarget);

	return S_OK;
}

void CNPC::Priority_Update(_float _fTimeDelta)
{
	CGameObject::Priority_Update_Component(_fTimeDelta); /* Component Priority_Update */
	__super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CNPC::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta); /* Part Object Update */
}

void CNPC::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CNPC::Render()
{

	return S_OK;
}

void CNPC::Attack()
{
}

HRESULT CNPC::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;

	if (COORDINATE_2D == Get_CurCoord())
		Set_2D_Direction(F_DIRECTION::DOWN);

	return S_OK;
}

void CNPC::Throw_Dialogue()
{

	Uimgr->Set_DialogId(m_strDialogueIndex, m_strCurSecion);

	_float4 vPos = {};

	if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
	{
		vPos = _float4(m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0], m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1], 0.f, 1.f);
	}
	else if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
	{
		vPos = _float4(m_pControllerTransform->Get_Transform(COORDINATE_3D)->Get_State(CTransform::STATE_POSITION).m128_f32[0],
			m_pControllerTransform->Get_Transform(COORDINATE_3D)->Get_State(CTransform::STATE_POSITION).m128_f32[1],
			m_pControllerTransform->Get_Transform(COORDINATE_3D)->Get_State(CTransform::STATE_POSITION).m128_f32[2],
			1.f);
	}

	Uimgr->Set_DialoguePos(vPos);
	Uimgr->Set_DisplayDialogue(true);
	Uimgr->Set_PortraitRender(true);


}

void CNPC::Set_2D_Direction(F_DIRECTION _eDir)
{
	m_e2DDirection = _eDir;
	if (F_DIRECTION::LEFT == m_e2DDirection)
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
	}
	else if (F_DIRECTION::RIGHT == m_e2DDirection)
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		m_pControllerTransform->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
	}
}

HRESULT CNPC::Cleanup_DeadReferences()
{
	if (FAILED(__super::Cleanup_DeadReferences()))
		return E_FAIL;

	if (nullptr == m_pTarget)
	{
#ifdef _DEBUG
		cout << "MONSTER : NO PLAYER" << endl;
#endif // _DEBUG
		return S_OK;
	}

	if (true == m_pTarget->Is_Dead())
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;

	}

	return S_OK;
}

void CNPC::Active_OnEnable()
{
	// 1. PxActor 활성화 (활성화 시점에는 먼저 켜고)
	CActorObject::Active_OnEnable();



	// 2. 몬스터 할거 하고
//	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_Player"), 0);
//	if (nullptr == m_pTarget)
//	{
//#ifdef _DEBUG
//		cout << "MONSTERINIT : NO PLAYER" << endl;
//#endif // _DEBUG
//		return;
//	}
//
//	Safe_AddRef(m_pTarget);


}

void CNPC::Active_OnDisable()
{
	// 1. 몬스터 할거 하고


	// 2. PxActor 비활성화 
	CActorObject::Active_OnDisable();
}

void CNPC::Free()
{
	for (auto& pGameObject : m_pNpcObject) // 태웅 추가
		Safe_Release(pGameObject);
	m_pNpcObject.clear();

	if (nullptr != m_pTarget)
		Safe_Release(m_pTarget);

	Safe_Release(m_pAnimEventGenerator);

	Safe_Release(m_p2DNpcCollider); // 태웅 추가
	Safe_Release(m_pStateMachine); // 태웅 추가



	__super::Free();
}
