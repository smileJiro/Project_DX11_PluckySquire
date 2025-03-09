#include "stdafx.h"
#include "DefenderSpawner.h"
#include "DefenderSmShip.h"
#include "GameInstance.h"
#include "Section_Manager.h"



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
	Safe_AddRef(m_pSection_Manager);
	m_pPoolMgr = CPooling_Manager::GetInstance();
	Safe_AddRef(m_pPoolMgr);

	DEFENDER_SPAWNER_DESC* pDesc = static_cast<DEFENDER_SPAWNER_DESC*>(_pArg);
	m_strSectionName = pDesc->strSectionName;
	m_strPoolTag = pDesc->strPoolTag;
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
}

void CDefenderSpawner::Spawn(SPAWN_PATTERN _ePattern, T_DIRECTION _eDirection, _float _fTimeStep)
{
	switch (_ePattern)
	{
	case Client::CDefenderSpawner::PATTERN_DOT:
		Spawn_Dot(_eDirection);
		break;
	case Client::CDefenderSpawner::PATTERN_DOT_SEQUENCE:
		Spawn_Dot_Sequence(_eDirection, _fTimeStep);
		break;
	case Client::CDefenderSpawner::PATTERN_ARROW:
		break;
	case Client::CDefenderSpawner::PATTERN_VERTICAL:
		break;
	case Client::CDefenderSpawner::PATTERN_RANDOM:
		break;
	default:
		break;
	}
}

void CDefenderSpawner::Spawn_Dot(T_DIRECTION _eDirection, _vector _vPosOffset)
{
	_float3 v2DPosition;
	XMStoreFloat3(&v2DPosition, Get_FinalPosition() + _vPosOffset);

	CDefenderMonster* pMonster = nullptr;
	m_pPoolMgr->Create_Object(m_strPoolTag,
		COORDINATE_2D,
		(CGameObject**)&pMonster,
		&v2DPosition,
		(_float4*)nullptr,
		(_float3*)nullptr,
		&m_strSectionName);
	if(pMonster)
		pMonster->Set_Direction(_eDirection);
}

void CDefenderSpawner::Spawn_Dot_Sequence(T_DIRECTION _eDirection, _float _fTimeStep, _vector _vPosOffset)
{
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
	Safe_Release(m_pSection_Manager);
	Safe_Release(m_pPoolMgr);
	__super::Free();
}

HRESULT CDefenderSpawner::Cleanup_DeadReferences()
{
	return S_OK;
}
