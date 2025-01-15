#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSM.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CContainerObject{ Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//플레이어 위치 가져오기
	m_pPlayerTransform = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0)->Get_ControllerTransform();
	if (nullptr == m_pPlayerTransform)
		return E_FAIL;
	else
		Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CMonster::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
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

void CMonster::Attack(_float fTimeDelta)
{
	//if (m_PartObjects[PART_WEAPON] != nullptr && m_PartObjects[PART_WEAPON]->Get_UseColl())
	//	m_PartObjects[PART_WEAPON]->Set_Collider_Enable(true);
}

void CMonster::Free()
{
	if (nullptr != m_pPlayerTransform)
		Safe_Release(m_pPlayerTransform);

	Safe_Release(m_pFSM);

	__super::Free();
}
