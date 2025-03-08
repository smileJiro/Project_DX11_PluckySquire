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

	// TEMP
	//pDesc->tTransform2DDesc.vInitialPosition = { -1050.f, 180.f, 0.f };

	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;

	static_cast<CModelObject*>(m_PartObjects[TROOP_BODY])->Register_OnAnimEndCallBack(bind(&CSneak_Troop::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	

	m_eCurDirection = pDesc->_eCurDirection;

	Switch_Animation_ByState();
	
	m_pSneakGameManager = CMinigame_Sneak::GetInstance();
	return S_OK;
}

void CSneak_Troop::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CSneak_Troop::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CSneak_Troop::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSneak_Troop::Render()
{
	return __super::Render();
}

void CSneak_Troop::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}

void CSneak_Troop::Switch_Animation_ByState()
{

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
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_DOWN, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FLIP_UP, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, ALERT_DOWN, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, ALERT_RIGHT, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, ALERT_UP, false);
	m_pBody->Set_AnimationLoop(COORDINATE_2D, FALL, false);

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

	__super::Free();
}
