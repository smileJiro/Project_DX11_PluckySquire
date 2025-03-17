#include "stdafx.h"
#include "Spawner.h"
#include "GameInstance.h"
#include "Pooling_Manager.h"
#include "FallingRock.h"


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
	m_eSpawnerMode = pDesc->eMode;
	m_fRandomStartPos = pDesc->fRandomStartPos;
	m_fRandomEndPos = pDesc->fRandomEndPos;
	m_fIntaval = pDesc->fIntaval;
	m_eGameObjectPrototypeLevelID = pDesc->eGameObjectPrototypeLevelID;
	m_strLayerTag = pDesc->strLayerTag;
	m_pObjectCloneDesc = pDesc->pObjectCloneDesc;
	 
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
	if (SPAWNER_RAMDOM == m_eSpawnerMode)
	{
		static _float2 vPreSpawnPos = {};
		
		_bool isIntarvalCheck = false;
		_float2 vRandomPos = {};

#ifdef _DEBUG
		_uint iLoopCnt = 0;
#endif // _DEBUG
		do
		{
#ifdef _DEBUG
			iLoopCnt++;
#endif
			vRandomPos.x = m_pGameInstance->Compute_Random(m_fRandomStartPos.x, m_fRandomEndPos.x);
			vRandomPos.y = m_pGameInstance->Compute_Random(m_fRandomStartPos.y, m_fRandomEndPos.y);
			isIntarvalCheck = m_fIntaval.x < fabs(vPreSpawnPos.x - vRandomPos.x);/* && m_fIntaval.y <= fabs(vPreSpawnPos.y - vRandomPos.y)*/;
			#ifdef _DEBUG
			if (iLoopCnt > 200)
				assert(false);
			#endif // _DEBUG
		} while (false == isIntarvalCheck);

		vPreSpawnPos = vRandomPos;
		m_vSpawnPostion = _float3(vRandomPos.x, vRandomPos.y, 0.f);
	
	}
	if (true == m_isPooling)
	{
		CPooling_Manager::GetInstance()->Create_Object(m_strPoolingTag, m_ePoolingObjectStartCoord, &m_vSpawnPostion);
	}
	else
	{
		if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_eGameObjectPrototypeLevelID, m_strLayerTag, m_eCurLevelID, m_strLayerTag, m_pObjectCloneDesc)))
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
	if (false == m_isPooling)
		Safe_Delete(m_pObjectCloneDesc);
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
