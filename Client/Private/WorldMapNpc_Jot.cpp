#include "stdafx.h"
#include "WorldMapNpc_Jot.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"
#include "RabbitLunch.h"



CWorldMapNpc_Jot::CWorldMapNpc_Jot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CWorldMapNpc_Jot::CWorldMapNpc_Jot(const CWorldMapNpc_Jot& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CWorldMapNpc_Jot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWorldMapNpc_Jot::Initialize(void* _pArg)
{
	WORLDMAP_PART_JOT_DESC* pDesc = static_cast<WORLDMAP_PART_JOT_DESC*>(_pArg);
	
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strModelPrototypeTag_2D = TEXT("WorldMap_NPC");
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, -40.0f, 0.0f);
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.6f, 1.6f, 1.f);
	pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;

	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 300.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;
	
	m_pParent = pDesc->pParent;
	Safe_AddRef(m_pParent);

	pDesc->pParentMatrices[COORDINATE_2D] = m_pParent->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_2D);


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CWorldMapNpc_Jot::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWorldMapNpc_Jot::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CWorldMapNpc_Jot::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CWorldMapNpc_Jot::Render()
{
	if (Is_Active())
		return __super::Render();

	return S_OK;
}

void CWorldMapNpc_Jot::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

}

void CWorldMapNpc_Jot::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	
}

void CWorldMapNpc_Jot::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	
}



void CWorldMapNpc_Jot::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	
}


void CWorldMapNpc_Jot::Interact(CPlayer* _pUser)
{

}

_bool CWorldMapNpc_Jot::Is_Interactable(CPlayer* _pUser)
{
	return false;
}

_float CWorldMapNpc_Jot::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return 0.f;
}








CWorldMapNpc_Jot* CWorldMapNpc_Jot::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWorldMapNpc_Jot* pInstance = new CWorldMapNpc_Jot(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWorldMapNpc_Jot::Clone(void* _pArg)
{
	CWorldMapNpc_Jot* pInstance = new CWorldMapNpc_Jot(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CWorldMapNpc_Jot Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWorldMapNpc_Jot::Free()
{
	Safe_Release(m_pParent);
	__super::Free();
}

HRESULT CWorldMapNpc_Jot::Cleanup_DeadReferences()
{
	return S_OK;
}
