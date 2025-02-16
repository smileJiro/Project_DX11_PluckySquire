#include "stdafx.h"
#include "Spawner.h"
#include "GameInstance.h"
#include "Pooling_Manager.h"


CSpawner::CSpawner(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CSpawner::CSpawner(const CSpawner& _Prototype)
	:CGameObject(_Prototype)
{
}

HRESULT CSpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpawner::Initialize(void* _pArg)
{
	SPAWNER_DESC* pDesc = static_cast<SPAWNER_DESC*>(_pArg);

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	/* Add To Layer Data */
	m_eCurLevelID = pDesc->eCurLevelID;
	m_eGameObjectPrototypeLevelID = pDesc->eGameObjectPrototypeLevelID;
	m_strLayerTag = pDesc->strLayerTag;
	m_tObjectCloneDesc = *pDesc->pObjectCloneDesc;
	 
	/* Spawner Data */
	m_vSpawnCycleTime.x = pDesc->fSpawnCycleTime;
	m_vSpawnCycleTime.y = 0.0f;
	m_iOneClycleSpawnCount = pDesc->iOneClycleSpawnCount;
	m_vSpawnPostion = pDesc->vSpawnPosition;

	/* Pooling Data */
	m_isPooling = pDesc->isPooling;
	if (true == m_isPooling)
	{
		m_ePoolingObjectStartCoord = pDesc->ePoolingObjectStartCoord;
		m_strPoolingTag = pDesc->strPoolingTag;
		/* 풀링 객체 등록 : 중복 등록해도 어짜피 풀링매니저에서 쳐낼거야 */
		if (FAILED(CPooling_Manager::GetInstance()->Register_PoolingObject(m_strPoolingTag,
			pDesc->tPoolingDesc,
			static_cast<CGameObject::GAMEOBJECT_DESC*>(pDesc->pObjectCloneDesc))))
		{
			/* 이미 등록된 객체라 실패했다면, Spawner에서 동적할당된 Desc 지워야함.*/
			m_isDeleteObjectDesc = true;
			pDesc = nullptr;
		}

	}
	else
	{
		/* 풀링 객체 아니여도 자기가 지워야함 */
		m_isDeleteObjectDesc = true;
	}

    return S_OK;
}

void CSpawner::Update(_float _fTimeDelta)
{
	m_vSpawnCycleTime.y += _fTimeDelta;
	if (m_vSpawnCycleTime.x <= m_vSpawnCycleTime.y)
	{
		m_vSpawnCycleTime.y = 0.0f;
		Spawn_Object();
	}
}

HRESULT CSpawner::Spawn_Object()
{
	if (true == m_isPooling)
	{
		CPooling_Manager::GetInstance()->Create_Object(m_strPoolingTag, m_ePoolingObjectStartCoord, &m_vSpawnPostion);
	}
	else
	{
		if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eGameObjectPrototypeLevelID, m_strLayerTag, m_eCurLevelID, m_strLayerTag, &m_tObjectCloneDesc)))
			return E_FAIL;
	}
    return S_OK;
}

void CSpawner::Active_OnEnable()
{
}

void CSpawner::Active_OnDisable()
{
}

CSpawner* CSpawner::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSpawner* pInstance = new CSpawner(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CSpawner");
		Safe_Release(pInstance);
	}
    return pInstance;
}

void CSpawner::Free()
{

	__super::Free();
}

CGameObject* CSpawner::Clone(void* _pArg)
{
	CSpawner* pInstance = new CSpawner(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CSpawner::Cleanup_DeadReferences()
{
	return S_OK;
}
