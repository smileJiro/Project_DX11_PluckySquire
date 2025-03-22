#include "stdafx.h"
#include "Sneak_Troop.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Minigame_Sneak.h"

CSneak_Troop::CSneak_Troop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CSneak_Troop::CSneak_Troop(const CSneak_Troop& _Prototype)
	: CCharacter(_Prototype)
{
}

HRESULT CSneak_Troop::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSneak_Troop::Initialize(void* _pArg)
{
	SNEAK_TROOP_DESC* pDesc = static_cast<SNEAK_TROOP_DESC*>(_pArg);

	pDesc->iNumPartObjects = TROOP_LAST;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform2DDesc.fSpeedPerSec = 100.f;

	m_pSneakGameManager = CMinigame_Sneak::GetInstance();
	
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;
	m_eInitDirection = m_eCurDirection = pDesc->_eCurDirection;
	m_iTargetTileIndex = m_iInitTileIndex = m_iCurTileIndex = pDesc->_iTileIndex;
	m_isMoveable = pDesc->_isMoveable;
	m_eCurTurnDirection = m_eTurnDirection = pDesc->_eTurnDirection;
	m_eSecondTurnDirection = pDesc->_eSecondTurnDirection;

	// 처음 위치는 밖에서 정해주자. 
	// _float2 vInitPosition = m_pSneakGameManager->Get_TilePosition()

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	static_cast<CModelObject*>(m_PartObjects[TROOP_BODY])->Register_OnAnimEndCallBack(bind(&CSneak_Troop::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	m_eCurAction = IDLE;
	
	// Detect Tile.
	m_iDetectCount = pDesc->_iDetectCount;
	if (0 < m_iDetectCount)
		m_pDetected = new _int[m_iDetectCount];
	for (_int i = 0; i < m_iDetectCount; ++i)
		m_pDetected[i] = -1;

	m_isRedDetect = pDesc->_isRedDetect;

	Switch_Animation_ByState();
	
	return S_OK;
}

void CSneak_Troop::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CSneak_Troop::Update(_float _fTimeDelta)
{
	Do_Action(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CSneak_Troop::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSneak_Troop::Render()
{
	if (nullptr != m_pSneakGameManager)
	{
		if (m_pSneakGameManager->Is_StartGame())
			return __super::Render();
		else
			return S_OK;
	}
	else
		return __super::Render();
}

_int CSneak_Troop::Predict_Route() const
{
	// 이미 죽은 애는 경로에서 제외되어야 한다.
	if (FALL == m_eCurAction)
		return -1;
	// 이동하지 않는 애는 굳이 경로 계산 안해도 된다.
	if (false == m_isMoveable )
		return m_iCurTileIndex;

	// 이동하는 친구.
	else
	{
		if (nullptr != m_pSneakGameManager)
		{;
			_int iNextTile = m_pSneakGameManager->Get_NextTile(m_iCurTileIndex, m_eCurDirection);

			// 이동 불가 판단. (없는 타일 & 현재 이동 불가능 타일)
			if (0 > iNextTile || false == m_pSneakGameManager->Is_MoveableTile(iNextTile))
			{
				return m_iCurTileIndex;
			}
			else
			{
				return iNextTile;
			}
		}
	}

	return m_iCurTileIndex;
}

void CSneak_Troop::Update_Detection()
{
	if (FALL == m_eCurAction)
		return;

	m_pSneakGameManager->Detect_Tiles(m_pDetected, m_iDetectCount, m_iCurTileIndex, m_eCurDirection, m_isRedDetect);
}

void CSneak_Troop::Action_Move(_int _iTileIndex, _float2 _vPosition)
{
	if (FALL == m_eCurAction)
		return;

	m_eCurAction = MOVE;
	m_vTargetPosition = _vPosition;
	m_iTargetTileIndex = _iTileIndex;

	Switch_Animation_ByState();
}

void CSneak_Troop::Action_Turn()
{
	if (FALL == m_eCurAction)
		return;

	// 회전하지 않는 친구.
	if (F_DIRECTION::F_DIR_LAST == m_eCurTurnDirection)
		return;
	
	// 설정될 방향
	F_DIRECTION eTargetDirection = F_DIRECTION::F_DIR_LAST;

	// 하드코딩 아쉽네.
	if (F_DIRECTION::LEFT == m_eCurTurnDirection)
	{
		if (F_DIRECTION::RIGHT == m_eCurDirection)
			eTargetDirection = F_DIRECTION::UP;
		else if (F_DIRECTION::UP == m_eCurDirection)
			eTargetDirection = F_DIRECTION::LEFT;
		else if (F_DIRECTION::LEFT == m_eCurDirection)
			eTargetDirection = F_DIRECTION::DOWN;
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			eTargetDirection = F_DIRECTION::RIGHT;
	}
	else if (F_DIRECTION::RIGHT == m_eCurTurnDirection)
	{
		if (F_DIRECTION::RIGHT == m_eCurDirection)
			eTargetDirection = F_DIRECTION::DOWN;
		else if (F_DIRECTION::UP == m_eCurDirection)
			eTargetDirection = F_DIRECTION::RIGHT;
		else if (F_DIRECTION::LEFT == m_eCurDirection)
			eTargetDirection = F_DIRECTION::UP;
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			eTargetDirection = F_DIRECTION::LEFT;
	}
	else if (F_DIRECTION::DOWN == m_eCurTurnDirection || F_DIRECTION::UP == m_eCurTurnDirection)
	{
		if (F_DIRECTION::RIGHT == m_eCurDirection)
			eTargetDirection = F_DIRECTION::LEFT;
		else if (F_DIRECTION::UP == m_eCurDirection)
			eTargetDirection = F_DIRECTION::DOWN;
		else if (F_DIRECTION::LEFT == m_eCurDirection)
			eTargetDirection = F_DIRECTION::RIGHT;
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			eTargetDirection = F_DIRECTION::UP;
	}

	if (nullptr != m_pSneakGameManager)
	{
		_int iTurnTile = m_pSneakGameManager->Get_NextTile(m_iCurTileIndex, eTargetDirection);
		// Turn 했을 때 아무것도 없을 때.
		if (-1 == iTurnTile)
		{
			// Second Turn Direction이 있을 때
			if (F_DIRECTION::F_DIR_LAST != m_eSecondTurnDirection)
			{
				if (m_eCurTurnDirection == m_eTurnDirection)
					eTargetDirection = m_eSecondTurnDirection;
				else
					eTargetDirection = m_eTurnDirection;


				m_eCurTurnDirection = eTargetDirection;
				Action_Turn();
				return;
			}

			else
			{
				if (F_DIRECTION::RIGHT == eTargetDirection)
					eTargetDirection = F_DIRECTION::LEFT;
				else if (F_DIRECTION::UP == eTargetDirection)
					eTargetDirection = F_DIRECTION::DOWN;
				else if (F_DIRECTION::LEFT == eTargetDirection)
					eTargetDirection = F_DIRECTION::RIGHT;
				else if (F_DIRECTION::DOWN == eTargetDirection)
					eTargetDirection = F_DIRECTION::UP;
			}
		}
	}

	Switch_TurnAnimation(eTargetDirection);
	m_eCurDirection = eTargetDirection;
	m_pGameInstance->Start_SFX_Delay(_wstring(L"A_sfx_enemy_rotate-") + to_wstring(rand() % 4), 0.1f, 15.f);

}

void CSneak_Troop::Action_Fall()
{
	if (false == m_isActive || FALL == m_eCurAction)
		return; 

	m_eCurDirection = F_DIRECTION::RIGHT;
	m_eCurAction = FALL;

	if (nullptr != m_pSneakGameManager)
		m_pSneakGameManager->DetectOff_Tiles(m_pDetected, m_iDetectCount, m_isRedDetect);

	m_pGameInstance->Start_SFX(_wstring(L"A_sfx_FallInPit-") + to_wstring(rand() % 3), 50.f);

	Switch_Animation_ByState();
}

void CSneak_Troop::Action_Catch()
{
	// Detection 안에 있으면 잡는다.
	if (nullptr != m_pSneakGameManager)
	{
		_int iPlayerIndex = m_pSneakGameManager->Get_PlayerTile();
		if (-1 == iPlayerIndex)
			return;

		for (_int i = 0; i < m_iDetectCount; ++i)
		{
			// 잡았다!
			if (iPlayerIndex == m_pDetected[i])
			{
				m_eCurAction = CATCH;
				Switch_Animation_ByState();
				m_pSneakGameManager->GameOver();
				m_pGameInstance->Start_SFX(TEXT("A_sfx_spotted"), 50.f);
				
			}
		}
	}



}

void CSneak_Troop::Action_Scout()
{
}

void CSneak_Troop::GameStart()
{
	m_eCurDirection = m_eInitDirection;
	m_iCurTileIndex = m_iInitTileIndex;
	m_iTargetTileIndex = m_iInitTileIndex;
	m_eCurTurnDirection = m_eTurnDirection;

	_float2 vStartPosition = { 0.f, 0.f };
	if (nullptr != m_pSneakGameManager)
		vStartPosition = m_pSneakGameManager->Get_CurStageTilePosition(m_iCurTileIndex);
	
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vStartPosition.x, vStartPosition.y, 0.f, 1.f));

	m_eCurAction = IDLE;

	Switch_Animation_ByState();

	for (_int i = 0; i < m_iDetectCount; ++i)
		m_pDetected[i] = -1;

	if (nullptr != m_pSneakGameManager)
	{
		m_pSneakGameManager->Detect_Tiles(m_pDetected, m_iDetectCount, m_iCurTileIndex, m_eCurDirection);
		m_pSneakGameManager->Reach_Destination(m_iCurTileIndex, m_iCurTileIndex);
	}

	Event_SetActive(this, true);
}

void CSneak_Troop::FadeIn(_float _fTimeDelta)
{
	if (nullptr != m_pBody)
		m_pBody->Start_FadeAlphaIn(_fTimeDelta);
}

void CSneak_Troop::FadeOut(_float _fTimeDelta)
{
	if (nullptr != m_pBody)
		m_pBody->Start_FadeAlphaOut(_fTimeDelta);

}

void CSneak_Troop::Switch_TurnAnimation(F_DIRECTION _eTargetDirection)
{
	if (nullptr == m_pBody)
		return;

	_bool	isReverse = { false };
	TROOP_ANIM eAnim = FLIP_UD;

	if (F_DIRECTION::LEFT == m_eCurDirection)
	{
		if (F_DIRECTION::RIGHT == _eTargetDirection)
		{
			eAnim = FLIP_LR;
		}
		else if (F_DIRECTION::UP == _eTargetDirection)
		{
			eAnim = FLIP_LU;
		}
		else if (F_DIRECTION::LEFT == _eTargetDirection)
		{
			return;
		}
		else if (F_DIRECTION::DOWN == _eTargetDirection)
		{
			eAnim = FLIP_LD;
		}
	}
	else if (F_DIRECTION::RIGHT == m_eCurDirection)
	{
		if (F_DIRECTION::RIGHT == _eTargetDirection)
		{
			return;
		}
		else if (F_DIRECTION::UP == _eTargetDirection)
		{
			eAnim = FLIP_RU;
		}
		else if (F_DIRECTION::LEFT == _eTargetDirection)
		{
			eAnim = FLIP_LR;
			isReverse = true;
		}
		else if (F_DIRECTION::DOWN == _eTargetDirection)
		{
			eAnim = FLIP_RD;
		}
	}
	else if (F_DIRECTION::UP == m_eCurDirection)
	{
		if (F_DIRECTION::RIGHT == _eTargetDirection)
		{
			eAnim = FLIP_UR;
		}
		else if (F_DIRECTION::UP == _eTargetDirection)
		{
			return;
		}
		else if (F_DIRECTION::LEFT == _eTargetDirection)
		{
			eAnim = FLIP_LU;
			isReverse = true;
		}
		else if (F_DIRECTION::DOWN == _eTargetDirection)
		{
			eAnim = FLIP_UD;
		}
	}
	else if (F_DIRECTION::DOWN == m_eCurDirection)
	{
		if (F_DIRECTION::RIGHT == _eTargetDirection)
		{
			eAnim = FLIP_DR;
		}
		else if (F_DIRECTION::UP == _eTargetDirection)
		{
			eAnim = FLIP_DU;
		}
		else if (F_DIRECTION::LEFT == _eTargetDirection)
		{
			eAnim = FLIP_LD;
			isReverse = true;
		}
		else if (F_DIRECTION::DOWN == _eTargetDirection)
		{
			return;
		}
	}

	m_pBody->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.0f, 0.f, 0.f, 0.f));
	m_pBody->Set_ReverseAnimation(isReverse);
	m_pBody->Switch_Animation(eAnim);


	m_eCurAction = TURN;
}

void CSneak_Troop::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (TURN == m_eCurAction)
	{
		if (nullptr == m_pBody)
			return;
		
		m_eCurAction = IDLE;
		Switch_Animation_ByState();

		// Tile Detction;
		if (nullptr != m_pSneakGameManager)
			m_pSneakGameManager->Detect_Tiles(m_pDetected, m_iDetectCount, m_iCurTileIndex, m_eCurDirection);

	}
	else if (FALL == m_eCurAction)
	{
		Event_SetActive(this, false);
	}
}

void CSneak_Troop::Switch_Animation_ByState()
{
	if (nullptr == m_pBody)
		return;

	m_pBody->Set_ReverseAnimation(false);
	_vector vRight = { 1.f, 0.f, 0.f, 0.f };

	if (IDLE == m_eCurAction)
	{
		if (F_DIRECTION::UP == m_eCurDirection)
			m_pBody->Switch_Animation(IDLE_UP);
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			m_pBody->Switch_Animation(IDLE_DOWN);
		else
			m_pBody->Switch_Animation(IDLE_RIGHT);
	}
	else if (MOVE == m_eCurAction)
	{
		if (F_DIRECTION::UP == m_eCurDirection)
			m_pBody->Switch_Animation(MOVE_UP);
		else if(F_DIRECTION::DOWN == m_eCurDirection)
			m_pBody->Switch_Animation(MOVE_DOWN);
		else
			m_pBody->Switch_Animation(MOVE_RIGHT);
	}
	else if (CATCH == m_eCurAction)
	{
		if (F_DIRECTION::UP == m_eCurDirection)
			m_pBody->Switch_Animation(ALERT_UP);
		else if (F_DIRECTION::DOWN == m_eCurDirection)
			m_pBody->Switch_Animation(ALERT_DOWN);
		else
			m_pBody->Switch_Animation(ALERT_RIGHT);		
	}
	else if (FALL == m_eCurAction)
	{
		m_pBody->Switch_Animation(FALL_DOWN);
	}

	if (F_DIRECTION::LEFT == m_eCurDirection)
		vRight = XMVectorSet(-1.f, 0.f, 0.f, 0.f);

	m_pBody->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, vRight);
}

void CSneak_Troop::Do_Action(_float _fTimeDelta)
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
	case CATCH:
		break;
	case FALL:
		break;
	case SCOUT:
		break;
	}
}

void CSneak_Troop::Dir_Move(_float _fTimeDelta)
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

		// Tile Detction;
		if (nullptr != m_pSneakGameManager)
			m_pSneakGameManager->Detect_Tiles(m_pDetected, m_iDetectCount, m_iCurTileIndex, m_eCurDirection);
	}
}

HRESULT CSneak_Troop::Ready_PartObjects()
{
	CModelObject::MODELOBJECT_DESC BodyDesc = {};

	BodyDesc.eStartCoord = COORDINATE_2D;
	BodyDesc.iCurLevelID = m_iCurLevelID;
	BodyDesc.isCoordChangeEnable = false;

	BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	BodyDesc.strModelPrototypeTag_2D = TEXT("Sneak_Trooper");
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
	m_pBody->Set_AnimationLoop(COORDINATE_2D, MOVE_DOWN, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, MOVE_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, MOVE_UP, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_UD, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_LR, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_DU, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, ALERT_DOWN, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, ALERT_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, ALERT_UP, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FALL_DOWN, false);

	return S_OK;
}

CSneak_Troop* CSneak_Troop::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_Troop* pInstance = new CSneak_Troop(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_Troop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_Troop::Clone(void* _pArg)
{
	CSneak_Troop* pInstance = new CSneak_Troop(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_Troop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_Troop::Free()
{
	Safe_Release(m_pBody);
	Safe_Delete_Array(m_pDetected);

	__super::Free();
}
