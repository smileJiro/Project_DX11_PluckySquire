#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSM.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& _Prototype)
	: CCharacter(_Prototype)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* _pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(_pArg);
	m_fAlertRange = pDesc->fAlertRange;
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;
	m_fDelayTime = pDesc->fDelayTime;
	m_fCoolTime = pDesc->fCoolTime;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	//플레이어 위치 가져오기
	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Player"), 0);
	if (nullptr == m_pTarget)
	{
	#ifdef _DEBUG
		cout << "MONSTERINIT : NO PLAYER" << endl;
	#endif // _DEBUG
		return S_OK;
	}

	Safe_AddRef(m_pTarget);

	return S_OK;
}

void CMonster::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CMonster::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CMonster::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

_float CMonster::Calculate_Distance()
{
	//return XMVectorGetX(XMVector3Length((m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransform->Get_State(CTransform::STATE_POSITION))));
	return 0.f;
}

void CMonster::Attack(_float _fTimeDelta)
{
}

HRESULT CMonster::Cleanup_DeadReferences()
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
		m_pFSM->CleanUp();
	}

	return S_OK;
}

void CMonster::Active_OnEnable()
{
	// 1. PxActor 활성화 (활성화 시점에는 먼저 켜고)
	CActorObject::Active_OnEnable();



	// 2. 몬스터 할거 하고
//	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
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

void CMonster::Active_OnDisable()
{
	// 1. 몬스터 할거 하고


	// 2. PxActor 비활성화 
	CActorObject::Active_OnDisable();
}

void CMonster::Free()
{
	if (nullptr != m_pTarget)
		Safe_Release(m_pTarget);

	Safe_Release(m_pFSM);

	__super::Free();
}
