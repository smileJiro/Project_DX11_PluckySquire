#include "stdafx.h"
#include "Npc_Companion.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"

#include "NPC_Violet.h"



CNPC_Companion::CNPC_Companion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CNPC(_pDevice, _pContext)
{
}

CNPC_Companion::CNPC_Companion(const CNPC_Companion& _Prototype)
	: CNPC(_Prototype)
{

}



HRESULT CNPC_Companion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Companion::Initialize(void* _pArg)
{

	CNPC::NPC_DESC* pDesc = static_cast<CNPC::NPC_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->iNumPartObjects = PART_END;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	Ready_Companion(TEXT("Layer_Companion"), pDesc);

	return S_OK;
}


HRESULT CNPC_Companion::Child_Initialize(void* _pArg)
{
	CNPC::NPC_DESC* pDesc = static_cast<CNPC::NPC_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->iNumPartObjects = PART_END;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}



void CNPC_Companion::Priority_Update(_float _fTimeDelta)
{
	

	__super::Priority_Update(_fTimeDelta);
}

void CNPC_Companion::Update(_float _fTimeDelta)
{
	for (auto& Child : m_vecCompanionNpc)
	{
		Child->Child_Update(_fTimeDelta);
	}

	__super::Update(_fTimeDelta);
}

void CNPC_Companion::Child_Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CNPC_Companion::Late_Update(_float _fTimeDelta)
{
	for (auto& Child : m_vecCompanionNpc)
	{
		Child->Child_LateUpdate(_fTimeDelta);
	}

	__super::Late_Update(_fTimeDelta);
}

void CNPC_Companion::Child_LateUpdate(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

_float CNPC_Companion::Cal_PlayerDistance()
{
	m_fPlayerDistance = m_pControllerTransform->Compute_Distance(m_pTargetObject->Get_FinalPosition());
	return m_fPlayerDistance;
}

_bool CNPC_Companion::Trace_Player(_float2 _vPlayerPos, _float2 vNPCPos)
{
	
	if (120.f >= m_fPlayerDistance)
	{
		return false;
	}

	_bool isMove = { false };
	float fdx = _vPlayerPos.x - vNPCPos.x;
	float fdy = _vPlayerPos.y - vNPCPos.y;

	float distance = sqrt(fdx * fdx + fdy * fdy);

	if (distance > 119.f)
	{ // 너무 가까워졌을 경우 이동 중단
		float fStep = m_f2DSpeed / 300.f * m_fPlayerDistance;
		if (distance < fStep)
		{
			// 목표까지의 거리가 step보다 작으면 도착 처리
			vNPCPos.x = _vPlayerPos.x;
			vNPCPos.y = _vPlayerPos.y;

			isMove = false;
		}
		else
		{
			vNPCPos.x += (fdx / distance) * fStep / 25.f;
			vNPCPos.y += (fdy / distance) * fStep / 25.f;
			
			isMove = true;
		}
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_State(CTransform::STATE_POSITION, _float4(vNPCPos.x, vNPCPos.y, 0.f, 1.f));

	}

	return isMove;

}


void CNPC_Companion::Delay_On()
{
	m_fAccTime = 0.f;
	m_isDelay = true;
}

void CNPC_Companion::Delay_Off()
{
	m_isDelay = false;
	m_fAccTime = 0.f;
}


HRESULT CNPC_Companion::Ready_Companion(const _wstring& _strLayerName, void* _pArg)
{
	COMPANIONDESC CompanionDesc = { };
	CompanionDesc.eStartCoord = COORDINATE_2D;
	CompanionDesc.isCoordChangeEnable = false;
	CompanionDesc.iNumPartObjects = PART_END;
	CompanionDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	CompanionDesc.tTransform2DDesc.fSpeedPerSec = static_cast<CPlayer*>(m_pTarget)->Get_MoveSpeed(COORDINATE_2D);
	CompanionDesc.iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	CompanionDesc.iMainIndex = 0;
	CompanionDesc.iSubIndex = 0;
	CompanionDesc.iCurLevelID = m_iCurLevelID;

	// 이걸 써야하나???
// 	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Companion"), 0);

	COMPANIONDESC* pDesc = static_cast<COMPANIONDESC*>(_pArg);
	m_vecCompanionNpc.reserve(3);

	CGameObject* pGameObject;
	// 찍찍이, 바이올렛, 쓰레쉬 생성 코드
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_NPC_Companion_Violet"), CompanionDesc.iCurLevelID, _strLayerName, &pGameObject, &CompanionDesc)))
		return E_FAIL;

	m_vecCompanionNpc.push_back(static_cast<CNPC_Violet*>(pGameObject));
	Safe_AddRef(pGameObject);


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_NPC_Companion_Thrash"), CompanionDesc.iCurLevelID, _strLayerName, &pGameObject, &CompanionDesc)))
		return E_FAIL;

	m_vecCompanionNpc.push_back(static_cast<CNPC_Violet*>(pGameObject));
	Safe_AddRef(pGameObject);



	for (_int i = 0; i < m_vecCompanionNpc.size(); ++i)
	{
		Set_TargetObject(i);
	}


	return S_OK;
}

void CNPC_Companion::Set_TargetObject(_int _index)
{
	if (0 == m_vecCompanionNpc.size())
		return;

	// 첫번째다? 플레이어를 따라가자.
	if (0 == _index)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pTarget);

		Set_PlayerObject(m_vecCompanionNpc[_index], pPlayer);
	} 

	// 두번째 세번째다?
	// 이전놈을 데리고 와야한다.
	else if (1 <= _index)
	{
		auto iter = m_vecCompanionNpc[_index - 1];
		Set_NPCObject(m_vecCompanionNpc[_index], iter);
	}
}







HRESULT CNPC_Companion::Render()
{
#ifdef _DEBUG

#endif // _DEBUG

	return S_OK;
}


CNPC_Companion* CNPC_Companion::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Companion* pInstance = new CNPC_Companion(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_Companion::Clone(void* _pArg)
{
	CNPC_Companion* pInstance = new CNPC_Companion(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_Companion Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_Companion::Free()
{
	//Safe_Release(m_pColliderCom);
	Safe_Release(m_pTargetObject);

	for (auto iter : m_vecCompanionNpc)
	{
		Safe_Release(iter);
	}
	m_vecCompanionNpc.clear();

	__super::Free();
}

HRESULT CNPC_Companion::Cleanup_DeadReferences()
{
	return S_OK;
}

