#include "stdafx.h"
#include "Sneak_SentryTroop.h"
#include "GameInstance.h"
#include "Minigame_Sneak.h"
#include "ModelObject.h"

CSneak_SentryTroop::CSneak_SentryTroop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSneak_Troop(_pDevice, _pContext)
{
}

CSneak_SentryTroop::CSneak_SentryTroop(const CSneak_SentryTroop& _Prototype)
	: CSneak_Troop(_Prototype)
{
}

HRESULT CSneak_SentryTroop::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSneak_SentryTroop::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_eCurAction = SCOUT;
	m_iMaxDetectCount = m_iDetectCount;

	Switch_Animation_ByState();

	return S_OK;
}

void CSneak_SentryTroop::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CSneak_SentryTroop::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CSneak_SentryTroop::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSneak_SentryTroop::Render()
{
	return __super::Render();
}

void CSneak_SentryTroop::Action_Scout()
{
	// 4 8 16

	// 4 -> 8
	// 4 
	_int iPrev = m_iDetectCount;
	if (m_iDetectCount <= m_iMaxDetectCount * 0.25f)
	{
		m_iDetectCount = (_int)((_float)m_iMaxDetectCount * 0.5f);
	}
	else if (m_iDetectCount <= m_iMaxDetectCount * 0.5f)
	{
		m_iDetectCount = m_iMaxDetectCount;
	}
	else
	{
		m_iDetectCount = (_int)((_float)m_iMaxDetectCount * 0.25f);
	}
	
	if (nullptr != m_pSneakGameManager)
	{
		m_pSneakGameManager->DetectOff_Tiles(m_pDetected, iPrev, m_isRedDetect);
		m_pSneakGameManager->Detect_Tiles(m_pDetected, m_iDetectCount, m_iCurTileIndex, m_eCurDirection, m_isRedDetect);
	}

}

void CSneak_SentryTroop::GameStart()
{
	m_eCurDirection = m_eInitDirection;
	m_iCurTileIndex = m_iInitTileIndex;
	m_iTargetTileIndex = m_iInitTileIndex;
	m_eCurTurnDirection = m_eTurnDirection;

	_float2 vStartPosition = { 0.f, 0.f };
	if (nullptr != m_pSneakGameManager)
		vStartPosition = m_pSneakGameManager->Get_CurStageTilePosition(m_iCurTileIndex);

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vStartPosition.x, vStartPosition.y, 0.f, 1.f));

	m_eCurAction = SCOUT;

	Switch_Animation_ByState();

	m_iDetectCount = (_int)((_float)m_iMaxDetectCount * 0.25f);

	for (_int i = 0; i < m_iDetectCount; ++i)
		m_pDetected[i] = -1;

	if (nullptr != m_pSneakGameManager)
	{
		m_pSneakGameManager->Detect_Tiles(m_pDetected, m_iDetectCount, m_iCurTileIndex, m_eCurDirection, m_isRedDetect);
		m_pSneakGameManager->Reach_Destination(m_iCurTileIndex, m_iCurTileIndex);
	}

	Event_SetActive(this, true);
}

void CSneak_SentryTroop::Switch_Animation_ByState()
{
	if (nullptr == m_pBody)
		return;

	_vector vRight = { 1.f, 0.f, 0.f, 0.f };

	if (SCOUT == m_eCurAction)
	{
		m_pBody->Switch_Animation(S_IDLE_RIGHT);
	}
	else if (CATCH == m_eCurAction)
	{
		m_pBody->Switch_Animation(S_ALERT_RIGHT);
	}
	else if (FALL == m_eCurAction)
	{
		m_pBody->Switch_Animation(S_FALL);
	}

	if (F_DIRECTION::LEFT == m_eCurDirection)
		vRight = XMVectorSet(-1.f, 0.f, 0.f, 0.f);

	m_pBody->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, vRight);
}

HRESULT CSneak_SentryTroop::Ready_PartObjects()
{
	CModelObject::MODELOBJECT_DESC BodyDesc = {};

	BodyDesc.eStartCoord = COORDINATE_2D;
	BodyDesc.iCurLevelID = m_iCurLevelID;
	BodyDesc.isCoordChangeEnable = false;

	BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	BodyDesc.strModelPrototypeTag_2D = TEXT("Sneak_SentryTrooper");
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

	m_pBody->Set_AnimationLoop(COORDINATE_2D, S_ALERT_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, S_FALL, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, S_IDLE_RIGHT, true);

	return S_OK;
}

CSneak_SentryTroop* CSneak_SentryTroop::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_SentryTroop* pInstance = new CSneak_SentryTroop(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_SentryTroop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_SentryTroop::Clone(void* _pArg)
{
	CSneak_SentryTroop* pInstance = new CSneak_SentryTroop(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_Troop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_SentryTroop::Free()
{
	__super::Free();
}
