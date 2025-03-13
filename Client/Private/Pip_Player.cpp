#include "stdafx.h"
#include "Pip_Player.h"
#include "GameInstance.h"
#include "Minigame_Sneak.h"
#include "PlayerData_Manager.h"
#include "ModelObject.h"

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
	// TEMP
	pDesc->tTransform2DDesc.vInitialPosition = { -1050.f, 180.f, 0.f };

	// ?
	pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	
	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	static_cast<CModelObject*>(m_PartObjects[PIP_BODY])->Register_OnAnimEndCallBack(bind(&CPip_Player::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	CPlayerData_Manager::GetInstance()->Register_Player(PLAYABLE_ID::PIP, this);


	Switch_Animation_ByState();

	m_pSneakGameManager = CMinigame_Sneak::GetInstance();
	return S_OK;
}

void CPip_Player::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CPip_Player::Update(_float _fTimeDelta)
{
	if (false == Is_PlayerInputBlocked())
		Key_Input(_fTimeDelta);

	Do_Action(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CPip_Player::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CPip_Player::Render()
{
	return __super::Render();
}

void CPip_Player::Start_Stage(_float2 _vPosition)
{
	m_iCurTileIndex = 0;
	m_iTargetTileIndex = 0;

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(_vPosition.x, _vPosition.y, 0.f, 1.f));

	m_eCurAction = IDLE;
	m_eCurDirection = F_DIRECTION::RIGHT;
	m_eInputDirection = F_DIRECTION::F_DIR_LAST;

	Switch_Animation_ByState();
}


void CPip_Player::FadeIn(_float _fTime)
{
	m_pBody->Start_FadeAlphaIn(_fTime);
}

void CPip_Player::Action_Move(_int _iTileIndex, _float2 _vPosition)
{
	m_eCurAction = MOVE;
	m_eCurDirection = m_eInputDirection;
	m_eInputDirection = F_DIRECTION::F_DIR_LAST;
	
	m_vTargetPosition = _vPosition;
	m_iTargetTileIndex = _iTileIndex;

	Switch_Animation_ByState();
}


void CPip_Player::Action_None()
{
	// 이전에 IDLE로 동일
	if (IDLE == m_eCurAction && (F_DIRECTION::F_DIR_LAST == m_eInputDirection/* || m_eCurDirection == m_eInputDirection*/))
		return;

	m_eCurAction = IDLE;
	m_eCurDirection = m_eInputDirection;
	m_eInputDirection = F_DIRECTION::F_DIR_LAST;

	Switch_Animation_ByState();
}

void CPip_Player::Action_Flip()
{
	m_eCurAction = FLIP;
	if (F_DIRECTION::F_DIR_LAST != m_eInputDirection)
		m_eCurDirection = m_eInputDirection;
	m_eInputDirection = F_DIRECTION::F_DIR_LAST;

	Switch_Animation_ByState();
}

void CPip_Player::Action_Caught()
{
	m_eCurAction = CAUGHT;
	m_eInputDirection = F_DIRECTION::F_DIR_LAST;

	m_pBody->Start_FadeAlphaOut(0.5f);

	Switch_Animation_ByState();
}

void CPip_Player::Action_Victory()
{
	m_eCurAction = VICTORY;
	Switch_Animation_ByState();

	m_pGameInstance->Start_SFX(TEXT("A_sfx_C9_TheTraiterousRodent"), 50.f);
}

void CPip_Player::Switch_Animation_ByState()
{
	if (nullptr == m_pBody)
		return;

	_vector vRight = { 1.f, 0.f, 0.f, 0.f };

	if (IDLE == m_eCurAction)
	{
		if (F_DIRECTION::UP == m_eCurDirection)
			m_pBody->Switch_Animation(IDLE_UP);
		else if (F_DIRECTION::DOWN == m_eCurDirection)
		{
			_float fRandom = m_pGameInstance->Compute_Random_Normal();
			if (0.5f < fRandom)
				m_pBody->Switch_Animation(IDLE_DOWN);
			else
				m_pBody->Switch_Animation(DANCE_DOWN);
		}
		else
			m_pBody->Switch_Animation(IDLE_RIGHT);
	}
	else if (MOVE == m_eCurAction)
	{
		if (F_DIRECTION::UP == m_eCurDirection)
			m_pBody->Switch_Animation(MOVE_UP);
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			m_pBody->Switch_Animation(MOVE_DOWN);
		else
			m_pBody->Switch_Animation(MOVE_RIGHT);
	}
	else if (CAUGHT == m_eCurAction)
	{
		if (F_DIRECTION::UP == m_eCurDirection)
			m_pBody->Switch_Animation(CAUGHT_UP);
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			m_pBody->Switch_Animation(CAUGHT_DOWN);
		else
			m_pBody->Switch_Animation(CAUGHT_RIGHT);
	}
	else if (FLIP == m_eCurAction)
	{
		if (F_DIRECTION::UP == m_eCurDirection)
			m_pBody->Switch_Animation(FLIP_UP);
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			m_pBody->Switch_Animation(FLIP_DOWN);
		else
			m_pBody->Switch_Animation(FLIP_RIGHT);
	}
	else if (VICTORY == m_eCurAction)
	{
		m_eCurDirection = F_DIRECTION::RIGHT;
		m_pBody->Switch_Animation(ANIM_VICTORY);
	}

	if (F_DIRECTION::LEFT == m_eCurDirection)
		vRight = XMVectorSet(-1.f, 0.f, 0.f, 0.f);

	m_pBody->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, vRight);
}


void CPip_Player::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (FLIP == m_eCurAction)
	{
		m_eCurAction = IDLE;	
		Switch_Animation_ByState();
	}
	else if (VICTORY == m_eCurAction)
	{
		m_eCurAction = IDLE;
		Switch_Animation_ByState();
	}
}

void CPip_Player::Do_Action(_float _fTimeDelta)
{
	switch (m_eCurAction)
	{
	case MOVE:
		Dir_Move(_fTimeDelta);
		break;
	case IDLE:
		break;
	case TURN:
		break;
	case CAUGHT:
		break;
	case FLIP:
		break;
	}
}

void CPip_Player::Dir_Move(_float _fTimeDelta)
{
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	switch (m_eCurDirection)
	{
	case F_DIRECTION::LEFT:
		vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
		break;
	case F_DIRECTION::RIGHT:
		vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		break;
	case F_DIRECTION::UP:
		vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		break;
	case F_DIRECTION::DOWN:
		vDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		break;
	default:
		break;
	}

	Move(vDir * 1000.f, _fTimeDelta);

	_vector vPosition = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vTarget = XMVectorSetW(XMLoadFloat2(&m_vTargetPosition), 1.f);

	if (10.f > XMVectorGetX(XMVector3Length(vPosition - vTarget)))
	{
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vTarget);
		m_eCurAction = IDLE;

		Switch_Animation_ByState();

		m_pSneakGameManager->Reach_Destination(m_iCurTileIndex, m_iTargetTileIndex);
		m_iCurTileIndex = m_iTargetTileIndex;
	}

}

void CPip_Player::Key_Input(_float _fTimeDelta)
{
	if (false == (m_pSneakGameManager->Is_InputTime(_fTimeDelta)) || F_DIRECTION::F_DIR_LAST != m_eInputDirection)/* || MOVE == m_eCurAction*/
		return;

	if (KEY_DOWN(KEY::W))
	{
		m_eInputDirection = F_DIRECTION::UP;
	}
	else if (KEY_DOWN(KEY::S))
	{
		m_eInputDirection = F_DIRECTION::DOWN;
	}
	else if (KEY_DOWN(KEY::A))
	{
		m_eInputDirection = F_DIRECTION::LEFT;
	}
	else if (KEY_DOWN(KEY::D))
	{
		m_eInputDirection = F_DIRECTION::RIGHT;

	}
}

HRESULT CPip_Player::Ready_Components()
{
	return S_OK;
}

HRESULT CPip_Player::Ready_PartObjects()
{
	/* Part_Body */
	CModelObject::MODELOBJECT_DESC BodyDesc = {};

	BodyDesc.eStartCoord = COORDINATE_2D;
	BodyDesc.iCurLevelID = m_iCurLevelID;
	BodyDesc.isCoordChangeEnable = false;

	BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	BodyDesc.strModelPrototypeTag_2D = TEXT("Sneak_Pip");
	BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[PART_BODY] = m_pBody = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
	if (nullptr == m_PartObjects[PART_BODY])
	{
		MSG_BOX("PipPlayer Body Creation Failed");
		return E_FAIL;
	}
	Safe_AddRef(m_pBody);

	m_pBody->Set_AnimationLoop(COORDINATE_2D, IDLE_RIGHT, true);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, IDLE_DOWN, true);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, IDLE_UP, true);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, DANCE_DOWN, true);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, MOVE_DOWN, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, MOVE_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, MOVE_UP, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, ANIM_VICTORY, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_DOWN, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_UP, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, CAUGHT_DOWN, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, CAUGHT_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, CAUGHT_UP, false);

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


