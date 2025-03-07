#include "stdafx.h"
#include "PortalLocker.h"
#include "GameInstance.h"
#include "Portal.h"
#include "Section_Manager.h"

CPortalLocker::CPortalLocker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CPortalLocker::CPortalLocker(const CPortalLocker& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CPortalLocker::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CPortalLocker::Initialize(void* _pArg)
{
	PORTALLOCKER_DESC* pDesc = static_cast<PORTALLOCKER_DESC*>(_pArg);

	// Save
	m_pTargetPortal = pDesc->pTargetPortal;
	if (nullptr == m_pTargetPortal)
		return E_FAIL;
	Safe_AddRef(m_pTargetPortal);
	m_ePortalLockerType = pDesc->ePortalLockerType;

	// Add
	_vector vPortalPos = m_pTargetPortal->Get_FinalPosition();
	pDesc->Build_2D_Transform(_float2(XMVectorGetX(vPortalPos), XMVectorGetY(vPortalPos)));

	pDesc->isCoordChangeEnable = false;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
	pDesc->strModelPrototypeTag_2D = TEXT("PortalLocker");
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;	

	if(FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pDesc->strSectionKey, this, SECTION_2D_PLAYMAP_OBJECT)))
		return E_FAIL;

	m_eCurState = STATE_LOCK;
	State_Change();
	//m_pControllerModel->Switch_Animation((_uint)m_ePortalLockerType + (_uint)m_eCurState);
	return S_OK;
}

void CPortalLocker::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CPortalLocker::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CPortalLocker::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

void CPortalLocker::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CPortalLocker::STATE_LOCK:
		State_Change_Lock();
		break;
	case Client::CPortalLocker::STATE_OPEN:
		State_Change_Open();
		break;
	case Client::CPortalLocker::STATE_DEAD:
		State_Change_Dead();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CPortalLocker::State_Change_Lock()
{
}

void CPortalLocker::State_Change_Open()
{
}

void CPortalLocker::State_Change_Dead()
{
}

void CPortalLocker::Action_State(_float _fTimeDelta)
{
}

void CPortalLocker::Action_State_Lock(_float _fTimeDelta)
{
}

void CPortalLocker::Action_State_Open(_float _fTimeDelta)
{
}

void CPortalLocker::Action_State_Dead(_float _fTimeDelta)
{
}

CPortalLocker* CPortalLocker::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CPortalLocker* CPortalLocker::Clone(void* _pArg)
{
	return nullptr;
}

void CPortalLocker::Free()
{
}
