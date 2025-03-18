#include "stdafx.h"
#include "Formation_Manager.h"
#include "Client_Function.h"
#include "GameInstance.h"
#include "PlayerData_Manager.h"
#include "Event_Manager.h"
#include "Formation.h"
#include "Monster.h"


IMPLEMENT_SINGLETON(CFormation_Manager)

CFormation_Manager::CFormation_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFormation_Manager::Initialize()
{
	if (FAILED(Ready_Chapter8_Formation()))
		return E_FAIL;

	return S_OK;
}

void CFormation_Manager::Update(_float _fTimeDelta)
{
	for (auto pFormation : m_Formations)
	{
		pFormation->Update(_fTimeDelta);
	}
}

void CFormation_Manager::Initialize_Formation_PatrolPoints(_uint _iIndex, CFormation* _pFormation)
{
	if (nullptr == _pFormation)
		return;

	switch (Formation_PatrolPoint(_iIndex))
	{
	case Client::CFormation_Manager::FORMATION1:
		_pFormation->Add_Formation_PatrolPoints(_float3(-104.f, 65.37f, 20.13f));
		_pFormation->Add_Formation_PatrolPoints(_float3(-100.f, 65.37f, 20.13f));
		//_pFormation->Add_Formation_PatrolPoints(_float3(34.f, 24.37f, 4.f));
		//_pFormation->Add_Formation_PatrolPoints(_float3(38.f, 24.37f, 4.f));
		break;
	case Client::CFormation_Manager::FORMATION2:
		_pFormation->Add_Formation_PatrolPoints(_float3(34.f, 24.37f, 10.5f));
		_pFormation->Add_Formation_PatrolPoints(_float3(38.f, 24.37f, 10.5f));
		break;
	default:
		break;
	}
}

void CFormation_Manager::Register_Formation(CFormation* _pFormation)
{
	if (nullptr == _pFormation)
		return;

	m_Formations.push_back(_pFormation);
	Initialize_Formation_PatrolPoints(m_Formations.size() - 1, _pFormation);
}

_bool CFormation_Manager::Add_To_Formation(CMonster* _pMember, CFormation** _pFormation)
{
	//포메이션들에 빈 위치 있는지 확인해서 넣어줌. (위치 측정 없이 일단 순서대로 탐색해서 넣어봄)
	for (auto pFormation : m_Formations)
	{
		if (true == pFormation->Has_EmptySlot())
		{
			pFormation->Add_To_Formation(_pMember, _pFormation);
			return true;
		}
	}

	return false;
}

HRESULT CFormation_Manager::Ready_Chapter8_Formation()
{
	_wstring strLayerTag = TEXT("Layer_Formation");

	CFormation* pFormation = nullptr;

	CFormation::FORMATIONDESC FormationDesc;
	FormationDesc.iCurLevelID = LEVEL_CHAPTER_8;
	FormationDesc.eStartCoord = COORDINATE_3D;
	FormationDesc.isCoordChangeEnable = false;
	//FormationDesc.tTransform3DDesc.vInitialPosition = _float3(36.f, 24.37f, 4.f);
	FormationDesc.tTransform3DDesc.vInitialPosition = _float3(-102.f, 65.37f, 20.13f);
	FormationDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
	FormationDesc.tTransform3DDesc.fSpeedPerSec = 4.f;

	FormationDesc.strMemberPrototypeTag = TEXT("Prototype_GameObject_Spear_Soldier");
	FormationDesc.strMemberLayerTag = TEXT("Layer_Monster_Locker");
	FormationDesc.fDelayTime = 2.f;

	//테스트
	FormationDesc.iRow = 1;
	FormationDesc.iColumn = 1;

	pFormation = static_cast<CFormation*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_Formation"), &FormationDesc));
	if (nullptr == pFormation)
		return E_FAIL;

	Register_Formation(pFormation);


	//FormationDesc.tTransform3DDesc.vInitialPosition = _float3(36.f, 24.37f, 10.5f);

	//pFormation = static_cast<CFormation*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_CHAPTER_8, TEXT("Prototype_GameObject_Formation"), &FormationDesc));
	//if (nullptr == pFormation)
	//	return E_FAIL;

	//Register_Formation(pFormation);

	return S_OK;
}


CFormation_Manager* CFormation_Manager::Create()
{
	CFormation_Manager* pInstance = new CFormation_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFormation_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFormation_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (CFormation* pFormation : m_Formations)
	{
		Safe_Release(pFormation);
	}
	m_Formations.clear();

	__super::Free();
}
