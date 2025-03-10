#include "stdafx.h"
#include "DefenderSpawner.h"
#include "DefenderSmShip.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "DefenderPlayer.h"


CDefenderSpawner::CDefenderSpawner(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CDefenderSpawner::CDefenderSpawner(const CDefenderSpawner& _Prototype)
	:CGameObject(_Prototype)
{
}

HRESULT CDefenderSpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDefenderSpawner::Initialize(void* _pArg)
{
	m_pSection_Manager = CSection_Manager::GetInstance();
	m_pPoolMgr = CPooling_Manager::GetInstance();

	DEFENDER_SPAWNER_DESC* pDesc = static_cast<DEFENDER_SPAWNER_DESC*>(_pArg);
	m_strSectionName = pDesc->strSectionName;
	m_strPoolTag = pDesc->strPoolTag;
	m_pPlayer = pDesc->pPlayer;
	Safe_AddRef(m_pPlayer);
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	//XMStoreFloat4(&m_vLeftShootQuaternion, XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(180.f)));
	//XMStoreFloat4(&m_vRightShootQuaternion, XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f));

	return S_OK;
}

void CDefenderSpawner::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	for (auto& tSpawn : m_SpawnList)
	{
		tSpawn.Update(_fTimeDelta);

	}
	for (auto& tSpawn : m_SpawnList)
	{
		if (tSpawn.Is_SpawnEnd())
		{
			if (tSpawn.Is_Auto())
				tSpawn.fCycleTimeAcc = 0.f;

		}
	}
}

void CDefenderSpawner::Add_Spawn(SPAWN_DESC tDesc)
{
	//_vector v2DPosition;
	if (false == tDesc.bAbsolutePosition)
	{
		tDesc.vPosition = m_pPlayer->Get_FinalPosition();
		tDesc.vPosition += _vector{ T_DIRECTION::LEFT == tDesc.eDirection ? tDesc.fPlayerDistance : -tDesc.fPlayerDistance, tDesc.fHeight, 0.f };
	}

	m_SpawnList.push_back(tDesc);
}

void CDefenderSpawner::Spawn_Single(T_DIRECTION _eDirection, _vector _vPos)
{
	_float3 vPos; XMStoreFloat3(&vPos, _vPos);
	CDefenderMonster* pMonster = nullptr;
	m_pPoolMgr->Create_Object(m_strPoolTag,
		COORDINATE_2D,
		(CGameObject**)&pMonster,
		&vPos,
		(_float4*)nullptr,
		(_float3*)nullptr,
		&m_strSectionName);
	if (pMonster)
		pMonster->Set_Direction(_eDirection);
}




CDefenderSpawner* CDefenderSpawner::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDefenderSpawner* pInstance = new CDefenderSpawner(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DefenderSpawner");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CDefenderSpawner::Clone(void* _pArg)
{
	CDefenderSpawner* pInstance = new CDefenderSpawner(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DefenderSpawner");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CDefenderSpawner::Free()
{
	Safe_Release(m_pPoolMgr);
	Safe_Release(m_pPlayer);
	__super::Free();
}

HRESULT CDefenderSpawner::Cleanup_DeadReferences()
{
	return S_OK;
}


void SPAWN_DESC::Update(_float _fTimeDelta)
{
	fPatternTimeAcc += _fTimeDelta;
	fPatternTimeAcc += _fTimeDelta;
	fCycleTimeAcc += _fTimeDelta;

	//if (fAutoCycleTime < 0.f)
	//{
	//	if (Is_SpawnEnd())
	//		return;
	//	if (fPatternTimeAcc >= fPatternTime)
	//	{
	//		Spawn_Single(_pPool, _strPoolTag, _strSectionName, eDirection, vPosition);
	//		fPatternTimeAcc = 0.f;
	//	}
	//}

	//if (fCycleTimeAcc >= fAutoCycleTime)
	//{
	//	fCycleTimeAcc = 0.f;
	//	iCurrentSpawnCount = 0;
	//}
	//if (fPatternTimeAcc >= fPatternTime)
	//{
	//	fPatternTimeAcc = 0.f;
	//	iCurrentSpawnCount++;
	//}
}

