#include "stdafx.h"
#include "Saw.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Character.h"

CSaw::CSaw(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CSaw::CSaw(const CSaw& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CSaw::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSaw::Initialize(void* _pArg)
{
    SAW_DESC* pDesc = static_cast<SAW_DESC*>(_pArg);
    // Save Desc

    // Add Desc
    pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("Saw"), TEXT("Prototype_Component_Shader_VtxPosTex"));
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;
    pDesc->isCoordChangeEnable = false;
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;	

	_vector vPos = Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat2(&m_vStartPos, vPos);

	vPos = XMVectorSetW(XMLoadFloat2(&m_vStartPos) + XMLoadFloat2(&m_vOffset), 1.0f);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
    return S_OK;
}

void CSaw::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CSaw::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::H))
	{
		m_eCurState = STATE_ATTACK;
	}
	if (KEY_DOWN(KEY::G))
	{
		m_eCurState = STATE_HIDE;
	}
	Action_State(_fTimeDelta);
	__super::Update(_fTimeDelta);
}

void CSaw::Late_Update(_float _fTimeDelta)
{
	State_Change();
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSaw::Render()
{
#ifdef _DEBUG
	if (m_p2DColliderComs[0]->Is_Active())
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif

	return __super::Render();
}

void CSaw::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID())
	{
		Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 1, XMVectorSet(0.0f, -500.f, 0.0f, 0.0f));
		m_iAttackCount++;
	}
}

void CSaw::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CSaw::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CSaw::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case CSaw::STATE_HIDE:
		State_Chage_Hide();
		break;
	case CSaw::STATE_ATTACK:
		State_Change_Attack();
		break;
	case CSaw::STATE_STOP:
		State_Change_Stop();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CSaw::State_Chage_Hide()
{
	Switch_Animation(ANIM::SAW_SPIN);
	End_StoppableRender();
}

void CSaw::State_Change_Attack()
{
	Switch_Animation(ANIM::SAW_SPIN);

	End_StoppableRender();
}

void CSaw::State_Change_Stop()
{
	Switch_Animation(ANIM::SAW_STOPPABLE);

	Start_StoppableRender();
}

void CSaw::Action_State(_float _fTimeDelta)
{

	switch (m_eCurState)
	{
	case CSaw::STATE_HIDE:
		Action_State_Hide(_fTimeDelta);
		break;
	case CSaw::STATE_ATTACK:
		Action_State_Attack(_fTimeDelta);
		break;
	case CSaw::STATE_STOP:
		Action_State_Stop(_fTimeDelta);
		break;
	default:
		break;
	}
}

void CSaw::Action_State_Hide(_float _fTimeDelta)
{
	/* x 움직임. */
	static _float fDirX = 1.0f;
	if (m_vMinMaxOffset_X.x > m_vOffset.x)
	{
		m_vOffset.x = m_vMinMaxOffset_X.x;
		fDirX *= -1.0f;
	}
	else if (m_vMinMaxOffset_X.y < m_vOffset.x)
	{
		m_vOffset.x = m_vMinMaxOffset_X.y;
		fDirX *= -1.0f;
	}
	m_vOffset.x += m_vMoveSpeed.x * fDirX * _fTimeDelta;
	/* y 움직임. */
	if (m_vMinMaxOffset_Y.y > m_vOffset.y)
	{
		m_vOffset.y += m_vMoveSpeed.y * _fTimeDelta;
	}
	else
	{
		m_vOffset.y = m_vMinMaxOffset_Y.y;
		m_eCurState = STATE_STOP;
	}


	_vector vPos = XMVectorSetW(XMLoadFloat2(&m_vStartPos) + XMLoadFloat2(&m_vOffset), 1.0f);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

void CSaw::Action_State_Attack(_float _fTimeDelta)
{
	/* x 움직임. */
	static _float fDirX = 1.0f;
	if (m_vMinMaxOffset_X.x > m_vOffset.x)
	{
		m_vOffset.x = m_vMinMaxOffset_X.x;
		fDirX *= -1.0f;
	}
	else if(m_vMinMaxOffset_X.y < m_vOffset.x)
	{
		m_vOffset.x = m_vMinMaxOffset_X.y;
		fDirX *= -1.0f;
	}
	m_vOffset.x += m_vMoveSpeed.x * fDirX * _fTimeDelta;
	/* y 움직임. */
	if (m_vMinMaxOffset_Y.x < m_vOffset.y)
	{
		m_vOffset.y -= m_vMoveSpeed.y * _fTimeDelta;
	}
	else
	{
		m_vOffset.y = m_vMinMaxOffset_Y.x;
	}


	_vector vPos = XMVectorSetW(XMLoadFloat2(&m_vStartPos) + XMLoadFloat2(&m_vOffset), 1.0f);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

void CSaw::Action_State_Stop(_float _fTimeDelta)
{
}

void CSaw::On_Stop()
{
	m_eCurState = STATE_STOP;
}

void CSaw::On_UnStop()
{
	m_eCurState = STATE_ATTACK;
}

HRESULT CSaw::Ready_Components(SAW_DESC* _pDesc)
{
	/* Teeth collider */
	m_p2DColliderComs.resize(1);

	CCollider_Circle::COLLIDER_CIRCLE_DESC Desc = {};
	Desc.pOwner = this;
	Desc.fRadius = 350.0f;
	Desc.vScale = { 1.0f, 1.0f };
	Desc.vOffsetPosition = { 0.0f, Desc.fRadius * 0.5f };
	Desc.isBlock = false;
	Desc.isTrigger = true;
	Desc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &Desc)))
		return E_FAIL;

	return S_OK;
}

CSaw* CSaw::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSaw* pInstance = new CSaw(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CSaw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSaw::Clone(void* _pArg)
{
	CSaw* pInstance = new CSaw(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CSaw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSaw::Free()
{

	__super::Free();
}
