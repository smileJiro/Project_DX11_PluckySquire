#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"
#include "FSM.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* _pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(_pArg);
	m_fChaseRange = pDesc->fChaseRange;
	m_fAttackRange = pDesc->fAttackRange;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	//플레이어 위치 가져오기
	CGameObject* pObject = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
	if (nullptr == pObject)
	{
	#ifdef _DEBUG
		cout << "MONSTER : NO PLAYER" << endl;
	#endif // _DEBUG
		return S_OK;
	}

	m_pPlayerTransform = pObject->Get_ControllerTransform();
	Safe_AddRef(m_pPlayerTransform);

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
