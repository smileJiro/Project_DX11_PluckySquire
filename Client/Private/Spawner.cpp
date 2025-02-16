#include "stdafx.h"
#include "Spawner.h"
#include "GameInstance.h"

CSpawner::CSpawner(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
}

HRESULT CSpawner::Initialize(SPAWNER_DESC* _pDesc)
{
	m_pObjecClonetDesc = _pDesc->pObjectCloneDesc;
	m_vSpawnCycleTime.x = _pDesc->fSpawnCycleTime;
	m_iOneClycleSpawnCount = _pDesc->iOneClycleSpawnCount;
	m_isPooling = _pDesc->isPooling;

	if (true == m_isPooling)
	{
		/* 풀링 객체 등록 */

		///* Pooling Test */
		//Pooling_DESC Pooling_Desc;
		//Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
		//Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
		//Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Beetle");
		//CBeetle::MONSTER_DESC* pDesc = new CBeetle::MONSTER_DESC;
		//pDesc->iCurLevelID = m_eLevelID;
		//CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_TestBeetle"), Pooling_Desc, pDesc);

	}
    return S_OK;
}

HRESULT CSpawner::Update(_float _fTimeDelta)
{
	m_vSpawnCycleTime.y += _fTimeDelta;
	if (m_vSpawnCycleTime.x <= m_vSpawnCycleTime.y)
	{
		Spawn_Object();
	}

    return S_OK;
}

HRESULT CSpawner::Spawn_Object()
{
	if (true == m_isPooling)
	{

	}
    return S_OK;
}

void CSpawner::Active_OnEnable()
{
}

void CSpawner::Active_OnDisable()
{
}

CSpawner* CSpawner::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SPAWNER_DESC* _pDesc)
{
	CSpawner* pInstance = new CSpawner(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Create Failed : CSpawner");
		Safe_Release(pInstance);
	}


    return pInstance;
}

void CSpawner::Free()
{
	/* Spawner는 반드시 ObjectManager, PrototypeManager, SectionManager보다 우선하여 삭제되어야한다. */
	Safe_Delete(m_pObjecClonetDesc);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
