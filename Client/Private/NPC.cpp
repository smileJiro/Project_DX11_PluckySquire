#include "stdafx.h"
#include "NPC.h"
#include "GameInstance.h"
#include "FSM.h"

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


	//플레이어 위치 가져오기
	//m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Player"), 0);
	//if (nullptr == m_pTarget)
	//{
	//#ifdef _DEBUG
	//	cout << "MONSTERINIT : NO PLAYER" << endl;
	//#endif // _DEBUG
	//	return S_OK;
	//}
	//
	//Safe_AddRef(m_pTarget);

	return S_OK;
}

void CNPC::Priority_Update(_float _fTimeDelta)
{
	CGameObject::Priority_Update_Component(_fTimeDelta); /* Component Priority_Update */
	__super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CNPC::Update(_float _fTimeDelta)
{
	CGameObject::Update_Component(_fTimeDelta); /* Component Update */
	__super::Update(_fTimeDelta); /* Part Object Update */
}

void CNPC::Late_Update(_float _fTimeDelta)
{
	CGameObject::Late_Update_Component(_fTimeDelta); /* Component Late_Update */
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

void CNPC::Change_Dir()
{
	//플레이어와의 각도를 구해 방향 전환 (시야각이 있을 때 기준)
	//_vector vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	//_vector vDir = XMVector3Normalize(m_pTarget->Get_Position() -> Get_Position());
	//_vector vLook = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//_float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))));
	//_float fResult = XMVectorGetX(XMVector3Cross(vUp, XMVector3Cross(vLook, vDir)));
	//if (0 > fResult)
	//{
	//	fAngle = 360 - fAngle;
	//}
	//
	//
	//if (315.f < fResult || 45.f >= fResult)
	//	Set_2D_Direction(F_DIRECTION::UP);
	//else if (45.f < fResult || 135.f >= fResult)
	//	Set_2D_Direction(F_DIRECTION::RIGHT);
	//else if (135.f < fResult || 225.f >= fResult)
	//	Set_2D_Direction(F_DIRECTION::DOWN);
	//else if (225.f < fResult || 315.f >= fResult)
	//	Set_2D_Direction(F_DIRECTION::LEFT);
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
		m_pFSM->CleanUp();
	}

	return S_OK;
}

void CNPC::Active_OnEnable()
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

void CNPC::Active_OnDisable()
{
	// 1. 몬스터 할거 하고


	// 2. PxActor 비활성화 
	CActorObject::Active_OnDisable();
}

void CNPC::Free()
{
	if (nullptr != m_pTarget)
		Safe_Release(m_pTarget);

	Safe_Release(m_pFSM);

	__super::Free();
}
