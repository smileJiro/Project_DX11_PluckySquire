#include "stdafx.h"
#include "Pip_Player.h"
#include "GameInstance.h"
#include "Minigame_Sneak.h"
#include "PlayerData_Manager.h"

CPip_Player::CPip_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPlayable(_pDevice, _pContext, PLAYABLE_ID::PIP)
{
}

CPip_Player::CPip_Player(const CPip_Player& _Prototype)
	: CPlayable(_Prototype)
{
}

HRESULT CPip_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CPip_Player::Initialize(void* _pArg)
{
	PIP_DESC* pDesc = static_cast<PIP_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;
	
	pDesc->iNumPartObjects = PIP_LAST;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform2DDesc.fSpeedPerSec = 100.f;
	
	// ?
	pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	
	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	static_cast<CModelObject*>(m_PartObjects[PIP_BODY])->Register_OnAnimEndCallBack(bind(&CDefenderPlayer::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	CPlayerData_Manager::GetInstance()->Register_Player(PLAYABLE_ID::PIP, this);

	return S_OK;
}

void CPip_Player::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CPip_Player::Update(_float _fTimeDelta)
{
	/*if (false == Is_PlayerInputBlocked())
		Key_Input()*/


	__super::Update(_fTimeDelta);
}

void CPip_Player::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CPip_Player::Ready_Components()
{
	return S_OK;
}

HRESULT CPip_Player::Ready_PartObjects()
{
	/* Part_Body */



	return S_OK;
}

CPip_Player* CPip_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPip_Player* pInstance = new CPip_Player(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPip_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPip_Player::Clone(void* _pArg)
{
	CPip_Player* pInstance = new CPip_Player(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CPip_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPip_Player::Free()
{
	Safe_Release(m_pBody);

	__super::Free();
}


