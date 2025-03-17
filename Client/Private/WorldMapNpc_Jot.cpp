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
	{
		__super::Render();
		DisplayLocationName();
	}

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
	__super::Free();
}

HRESULT CWorldMapNpc_Jot::Cleanup_DeadReferences()
{
	return S_OK;
}

HRESULT CWorldMapNpc_Jot::DisplayLocationName()
{


	_float2 vMidPoint = { RTSIZE_BOOK2D_X / 2.f, RTSIZE_BOOK2D_Y / 2.f };

	_float2 vCalPos = { 0.f, 0.f };
	_float2 vTextPos = { 0.f, 0.f };

	switch ((CWorldMapNPC::MOVEPOS)m_iCurIndex)
	{
	case CWorldMapNPC::POS_HONEYBEE:
	{
		DisplayHoneyBee(vMidPoint);
	}
	break;

	case CWorldMapNPC::POS_TOWER:
	{
		DisplayHoneyBee(vMidPoint);
		vTextPos = _float2(-272.f, 188.1f);
		wsprintf(m_strLocationName, L"½ÃÀÇ ´Ë");
		//m_strLocationName = TEXT("½ÃÀÇ ´Ë");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName, _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
	}
	break;

	case CWorldMapNPC::POS_SWAMPEND:
	{
		DisplayHoneyBee(vMidPoint);
		vTextPos = _float2(-272.f, 188.1f);
		wsprintf(m_strLocationName, L"½ÃÀÇ ´Ë");
		//m_strLocationName = TEXT("½ÃÀÇ ´Ë");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName, _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;

		vTextPos = _float2(99.1f, 70.5f);
		wsprintf(m_strLocationName, L"¾Æ¸£Æ¼¾Æ");
		//m_strLocationName = TEXT("¾Æ¸£Æ¼¾Æ");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font30"), m_strLocationName, _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
	}
	break;

	case CWorldMapNPC::POS_ATRIA:
	{
		DisplayHoneyBee(vMidPoint);
		vTextPos = _float2(-272.f, 188.1f);
		wsprintf(m_strLocationName, L"½ÃÀÇ ´Ë");
		//m_strLocationName = TEXT("½ÃÀÇ ´Ë");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName, _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;

		vTextPos = _float2(99.1f, 70.5f);
		wsprintf(m_strLocationName, L"¾Æ¸£Æ¼¾Æ");
		//m_strLocationName = TEXT("¾Æ¸£Æ¼¾Æ");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font30"), m_strLocationName, _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;

		vTextPos = _float2(282.9f, -200.f);
		wsprintf(m_strLocationName, L"Æ®¶ó¸£±× »ê");
		//m_strLocationName = TEXT("Æ®¶ó¸£±× »ê");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font24"), m_strLocationName, _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
	}
	break;

	}

	return S_OK;
}

HRESULT CWorldMapNpc_Jot::DisplayHoneyBee(_float2 _MidPoint)
{
	_float2 Middle = _MidPoint;

	_float2 vText = { 0.f, 0.f };
	_float2 vCal = { 0.f, 0.f };

	vText = _float2(-507.f, -206.f);
	wsprintf(m_strLocationName, L"¸Ô¹°½£");
	//m_strLocationName = TEXT("¸Ô¹°½£");

	vCal.x = Middle.x + vText.x;
	vCal.y = Middle.y - vText.y;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName, _float2((vCal.x), (vCal.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	vText = _float2(-474.f, -66.f);
	wsprintf(m_strLocationName, L"¹ú²Ü ºÀ¿ì¸®");
	//m_strLocationName = TEXT("¹ú²Ü ºÀ¿ì¸®");

	vCal.x = _MidPoint.x + vText.x;
	vCal.y = _MidPoint.y - vText.y;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName, _float2((vCal.x), (vCal.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	vText = _float2(-560.f, 175.9f);
	wsprintf(m_strLocationName, L"Ã¥ÀÇ Å¾");
	//m_strLocationName = TEXT("Ã¥ÀÇ Å¾");

	vCal.x = _MidPoint.x + vText.x;
	vCal.y = _MidPoint.y - vText.y;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName, _float2((vCal.x), (vCal.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	return S_OK;
}