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

	/* Set Anim End */
	Register_OnAnimEndCallBack(bind(&CPortalLocker::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	return S_OK;
}

void CPortalLocker::Priority_Update(_float _fTimeDelta)
{
	if (true == m_pTargetPortal->Is_Dead())
	{
		Safe_Release(m_pTargetPortal);
		m_pTargetPortal = nullptr;
	}

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
	m_pControllerModel->Switch_Animation((_uint)m_ePortalLockerType + (_uint)m_eCurState);
}

void CPortalLocker::State_Change_Open()
{
	m_pControllerModel->Switch_Animation((_uint)m_ePortalLockerType + (_uint)m_eCurState);
}

void CPortalLocker::State_Change_Dead()
{
	Start_FadeAlphaOut();
}

void CPortalLocker::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CPortalLocker::STATE_LOCK:
		Action_State_Lock(_fTimeDelta);
		break;
	case Client::CPortalLocker::STATE_OPEN:
		Action_State_Open(_fTimeDelta);
		break;
	case Client::CPortalLocker::STATE_DEAD:
		Action_State_Dead(_fTimeDelta);
		break;

	default:
		break;
	}
}

void CPortalLocker::Action_State_Lock(_float _fTimeDelta)
{
	if (nullptr == m_pTargetPortal)
		return;

	if (true == m_pTargetPortal->Is_Active())
	{
		Event_SetActive(m_pTargetPortal, false);
	}
}

void CPortalLocker::Action_State_Open(_float _fTimeDelta)
{
	if (nullptr == m_pTargetPortal)
		return;
}

void CPortalLocker::Action_State_Dead(_float _fTimeDelta)
{
	//Event_SetActive(m_pTargetPortal, true);
	if (FADEALPHA_DEFAULT == m_eFadeAlphaState)
	{
		if (m_vFadeAlpha.y == 1.0f)
		{
			/* Fade Out이 종료되었다는 체크 */
			Event_DeleteObject(this);
			if (false == m_pTargetPortal->Is_Active())
			{
				m_pTargetPortal->Set_FirstActive(true);
				//Event_SetActive(m_pTargetPortal, true);
			}
		}
	}

	

}

void CPortalLocker::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	switch (_iAnimIdx)
	{
	case 0: /* 보라색 + Lock */
		break;
	case 1: /* 보라색 + Open */
	{
		m_eCurState = STATE_DEAD;
	}
		break;
	case 2: /* 노란색 + Lock */
		break;
	case 3: /* 노란색 + Open */
	{
		m_eCurState = STATE_DEAD;
	}
		break;
	case 4: /* Effect */
		break;
	default:
		break;
	}
}

CPortalLocker* CPortalLocker::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPortalLocker* pInstance = new CPortalLocker(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created Failed : CPortalLocker");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CPortalLocker* CPortalLocker::Clone(void* _pArg)
{
	CPortalLocker* pInstance = new CPortalLocker(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone Failed : CPortalLocker");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPortalLocker::Free()
{
	Safe_Release(m_pTargetPortal);

	__super::Free();
}
