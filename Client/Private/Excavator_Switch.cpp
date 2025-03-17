#include "stdafx.h"
#include "Excavator_Switch.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "PlayerData_Manager.h"

CExcavator_Switch::CExcavator_Switch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CExcavator_Switch::CExcavator_Switch(const CExcavator_Switch& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CExcavator_Switch::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CExcavator_Switch::Initialize(void* _pArg)
{
	SWITCH_DESC* pDesc = static_cast<SWITCH_DESC*>(_pArg);

	// Save Desc
	
	// Add Desc 
	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("Excavator_Switch"), TEXT("Prototype_Component_Shader_VtxPosTex"));
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	pDesc->isCoordChangeEnable = false;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_eInteractType = INTERACT_TYPE::CHARGE;
	m_fInteractChargeTime = 1.5f;
	m_eInteractID = INTERACT_ID::EXCAVATOR_SWITCH;

	m_eCurState = STATE_LOCK;
	State_Change();
    return S_OK;
}

void CExcavator_Switch::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CExcavator_Switch::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);
	__super::Update(_fTimeDelta);
}

void CExcavator_Switch::Late_Update(_float _fTimeDelta)
{
	State_Change();
	__super::Late_Update(_fTimeDelta);
}

HRESULT CExcavator_Switch::Render()
{
	if (m_p2DColliderComs[0]->Is_Active())
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	return __super::Render();
}

void CExcavator_Switch::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CExcavator_Switch::STATE_LOCK:
		State_Change_Lock();
		break;
	case Client::CExcavator_Switch::STATE_OPEN:
		State_Change_Open();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CExcavator_Switch::State_Change_Lock()
{
	
}

void CExcavator_Switch::State_Change_Open()
{
	Start_FadeAlphaOut(0.5f);
}

void CExcavator_Switch::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CExcavator_Switch::STATE_LOCK:
		Action_State_Lock(_fTimeDelta);
		break;
	case Client::CExcavator_Switch::STATE_OPEN:
		Action_State_Open(_fTimeDelta);
		break;
	case Client::CExcavator_Switch::STATE_LAST:
		break;
	default:
		break;
	}
}

void CExcavator_Switch::Action_State_Lock(_float _fTimeDelta)
{

}


void CExcavator_Switch::Action_State_Open(_float _fTimeDelta)
{
	if (false == Is_Active())
		return;

	if (m_vFadeAlpha.y / m_vFadeAlpha.x >= 1.0f - EPSILON)
	{
		/* Fade Out이 종료되었다는 체크 */
		Event_SetActive(this, false);
	}
}

HRESULT CExcavator_Switch::Ready_Components(SWITCH_DESC* _pDesc)
{
	m_p2DColliderComs.resize(1);
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 30.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius * 0.5f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = false;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;

    return S_OK;
}

void CExcavator_Switch::On_InteractionStart(CPlayer* _pPlayer)
{
	/* 1. player 당기기 상태로 변경 loop 상태를 바꿔? */
	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	pPlayer->Set_State(CPlayer::PULL);
}

void CExcavator_Switch::Interact(CPlayer* _pPlayer)
{
	/* 2. 뒤로 굴러넘이지기 state로 변경. */
	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	pPlayer->Set_State(CPlayer::BACKROLL);

	m_eCurState = STATE_OPEN;
}

CExcavator_Switch* CExcavator_Switch::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CExcavator_Switch* pInstance = new CExcavator_Switch(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CExcavator_Switch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExcavator_Switch::Clone(void* _pArg)
{
	CExcavator_Switch* pInstance = new CExcavator_Switch(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CExcavator_Switch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExcavator_Switch::Free()
{

	__super::Free();
}

_bool CExcavator_Switch::Is_Interactable(CPlayer* _pUser)
{
	return true;
}

_float CExcavator_Switch::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}
