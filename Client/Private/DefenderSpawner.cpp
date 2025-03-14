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
	m_vSectionSize = m_pSection_Manager->Get_Section_RenderTarget_Size(m_strSectionName);
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

	for (auto& iter = m_SpawnList.begin(); iter != m_SpawnList.end(); )
	{
		if (iter->Is_PatternEnd())
		{
			if (iter->Is_Auto() )
			{
				if(iter->Is_CycleEnd())
					iter->Reset_Cycle();
			}
			else
			{
				iter = m_SpawnList.erase(iter);
				continue;
			}
		}
		iter++;
	}
	for (auto& tSpawn : m_SpawnList)
	{
		_bool bPaterrnStartDelayEndBefore = tSpawn.Is_PatternStartDelayEnd();
		tSpawn.Update(_fTimeDelta);
		if (tSpawn.Is_PatternStartDelayEnd())
		{
			if (false == bPaterrnStartDelayEndBefore && false == tSpawn.bAbsolutePosition)
			{
				tSpawn.vPosition = m_pPlayer->Get_FinalPosition();
				tSpawn.vPosition += _vector{ T_DIRECTION::LEFT == tSpawn.eDirection ? tSpawn.fPlayerDistance : -tSpawn.fPlayerDistance,0.f, 0.f };
				tSpawn.vPosition.m128_f32[1] = tSpawn.fHeight;
			}
			while (tSpawn.Is_UnitSpawnReady())
			{
				Spawn(tSpawn);
				tSpawn.Reset_Unit();
			}
		}
	}

	__super::Update(_fTimeDelta);
}

void CDefenderSpawner::Add_Spawn(SPAWN_DESC tDesc)
{
	//_vector v2DPosition;

	tDesc.fUnitTimeAcc = tDesc.fUnitDelay;
	m_SpawnList.push_back(tDesc);
}

void CDefenderSpawner::Delete_Pool()
{
	m_pPoolMgr->Delete_Pool(m_strPoolTag);
}

void CDefenderSpawner::Spawn_Single(T_DIRECTION _eDirection, _vector _vPos,_float _fMoveSpeed)
{

	_float3 vPos; XMStoreFloat3(&vPos, (_vPos));
	CDefenderMonster* pMonster = nullptr;

	m_pPoolMgr->Create_Object(m_strPoolTag,
		COORDINATE_2D,
		(CGameObject**)&pMonster,
		&vPos,
		(_float4*)nullptr,
		(_float3*)nullptr,
		&m_strSectionName);
	if (pMonster)
	{
		pMonster->Set_Direction(_eDirection);
		pMonster->Set_MoveSpeed(_fMoveSpeed);
		pMonster->On_Teleport();
	}
}

void CDefenderSpawner::Spawn(SPAWN_DESC& tDesc)
{

	switch (tDesc.ePattern)
	{
	case SPAWN_PATTERN_DOT :
		Spawn_Dot(tDesc);
		break;
	case SPAWN_PATTERN_ARROW :
		Spawn_Arrow(tDesc);
		break;
	case SPAWN_PATTERN_VERTICAL_UP:
		Spawn_Vertical(tDesc, true);
		break;
	case SPAWN_PATTERN_VERTICAL_DOWN:
		Spawn_Vertical(tDesc,false);
		break;
	case SPAWN_PATTERN_RANDOM :
		Spawn_Random(tDesc);
		break;
	default:
		break;
	}
	tDesc.iCurrentSpawnCount++;
	tDesc.fUnitTimeAcc = 0.f;
}

void CDefenderSpawner::Spawn_Dot(SPAWN_DESC& _tDesc)
{

	Spawn_Single(_tDesc.eDirection, _tDesc.vPosition, _tDesc.fMoveSpeed);
	
}

void CDefenderSpawner::Spawn_Arrow(SPAWN_DESC& _tDesc)
{
	_float fSeqXDistance = 30.f;
	_float fSeqYDistance = 30.f;


	_vector vPosition1 = _tDesc.vPosition;

	vPosition1 += _vector{
		(_int)_tDesc.iCurrentSpawnCount * (T_DIRECTION::LEFT == _tDesc.eDirection ? fSeqXDistance : -fSeqXDistance)
		, (_int)_tDesc.iCurrentSpawnCount * fSeqYDistance
		, 0.f };
	Spawn_Single(_tDesc.eDirection, vPosition1, _tDesc.fMoveSpeed);
	if (_tDesc.iCurrentSpawnCount > 0)
	{
		_vector vPosition2 = _tDesc.vPosition;
		vPosition2 += _vector{
			(_int)_tDesc.iCurrentSpawnCount * (T_DIRECTION::LEFT == _tDesc.eDirection ? fSeqXDistance : -fSeqXDistance)
			, (_int)_tDesc.iCurrentSpawnCount * -fSeqYDistance
			, 0.f };
		Spawn_Single(_tDesc.eDirection, vPosition2, _tDesc.fMoveSpeed);
	}
}

void CDefenderSpawner::Spawn_Vertical(SPAWN_DESC& _tDesc, _bool _bUp)
{
	_float fSeqYDistance = 30.f;
	_vector vPosition1 = _tDesc.vPosition;

	if(_bUp)
		vPosition1 += _vector{0.f, (_int)_tDesc.iCurrentSpawnCount * fSeqYDistance, 0.f };
	else
		vPosition1 -= _vector{0.f, (_int)_tDesc.iCurrentSpawnCount * fSeqYDistance, 0.f };
	Spawn_Single(_tDesc.eDirection, vPosition1, _tDesc.fMoveSpeed);
}

void CDefenderSpawner::Spawn_Random(SPAWN_DESC& _tDesc)
{
	_vector vPosition1 = _tDesc.vPosition;

	_float fRandomRange = 400.f;
	_float fRandomX = (_float)rand()/(_float)RAND_MAX * fRandomRange - fRandomRange*0.5f;
	_float fRandomY = (_float)rand() / (_float)RAND_MAX * fRandomRange - fRandomRange * 0.5f;
	vPosition1 += _vector{fRandomX, fRandomY, 0.f };
	Spawn_Single(_tDesc.eDirection, vPosition1, _tDesc.fMoveSpeed);
}

_vector CDefenderSpawner::Get_ScrolledPos(_vector _vPos)
{
	_float fDefaultWitdh = (m_vSectionSize.x * 0.5f);

	if (-fDefaultWitdh > _vPos.m128_f32[0])
	{
		_vPos = XMVectorSetX(_vPos, _vPos.m128_f32[0] + m_vSectionSize.x);
	}
	if (fDefaultWitdh < _vPos.m128_f32[0])
	{
		_vPos = XMVectorSetX(_vPos, _vPos.m128_f32[0] - m_vSectionSize.x);
	}
	return _vPos;
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

	Safe_Release(m_pPlayer);
	__super::Free();
}

HRESULT CDefenderSpawner::Cleanup_DeadReferences()
{
	return S_OK;
}


void SPAWN_DESC::Update(_float _fTimeDelta)
{
	if(false == Is_PatternStartDelayEnd())
	{
		fPatternStartTimeAcc += _fTimeDelta;

		return;
	}
	fUnitTimeAcc += _fTimeDelta;
	fUnitTimeAcc += _fTimeDelta;
	fCycleTimeAcc += _fTimeDelta;
}

