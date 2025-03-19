#include "stdafx.h"
#include "FatherPart_Prop.h"
#include "GameInstance.h"
#include "FatherGame.h"
#include "Section_Manager.h"
#include "PlayerData_Manager.h"

CFatherPart_Prop::CFatherPart_Prop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CFatherPart_Prop::CFatherPart_Prop(const CFatherPart_Prop& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CFatherPart_Prop::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFatherPart_Prop::Initialize(void* _pArg)
{
	FATHERPART_PROP_DESC* pDesc = static_cast<FATHERPART_PROP_DESC*>(_pArg);

	m_iFatherPart = pDesc->iFatherPartID;
	switch ((CFatherGame::FATHER_PART)pDesc->iFatherPartID)
	{
	case CFatherGame::FATHER_PART::FATHER_BODY:
		pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("FatherParts_Prop_Body"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::DEFAULT);
		break;
	case CFatherGame::FATHER_PART::FATHER_WING:
		pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("FatherParts_Prop_Wing"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::DEFAULT);
		break;
	case CFatherGame::FATHER_PART::FATER_HEAD:
		pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("FatherParts_Prop_Head"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::DEFAULT);
		break;
	default:
		break;
	}
	
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_eCurState = STATE_IDLE;
	State_Change();
    return S_OK;
}

void CFatherPart_Prop::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CFatherPart_Prop::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::K))
	{
		Set_Scale(_float3(100.f, 100.f, 0.0f));
	}
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CFatherPart_Prop::Late_Update(_float _fTimeDelta)
{
	State_Change();

	__super::Late_Update(_fTimeDelta);
}

HRESULT CFatherPart_Prop::Render()
{
#ifdef _DEBUG
	if (nullptr != m_p2DColliderComs[0])
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

	if (FAILED(__super::Render()))
		return E_FAIL;
    return S_OK;
}

void CFatherPart_Prop::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	_uint iCollisionGroupID = _pOtherCollider->Get_CollisionGroupID();

	if (OBJECT_GROUP::PLAYER == (OBJECT_GROUP)iCollisionGroupID)
	{
		m_eCurState = STATE::STATE_PICKUP;
	}
}

CFatherPart_Prop* CFatherPart_Prop::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFatherPart_Prop* pInstance = new CFatherPart_Prop(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CFatherPart_Prop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFatherPart_Prop::Clone(void* _pArg)
{
	CFatherPart_Prop* pInstance = new CFatherPart_Prop(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CFatherPart_Prop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFatherPart_Prop::Free()
{

	__super::Free();
}

HRESULT CFatherPart_Prop::Cleanup_DeadReferences()
{
    return S_OK;
}

void CFatherPart_Prop::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CFatherPart_Prop::STATE_IDLE:
		State_Change_Idle();
		break;
	case Client::CFatherPart_Prop::STATE_PICKUP:
		State_Change_Pickup();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CFatherPart_Prop::State_Change_Idle()
{
}

void CFatherPart_Prop::State_Change_Pickup()
{
	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	assert(pPlayer);

	switch (m_iFatherPart)
	{
	case CFatherGame::FATHER_PART::FATHER_BODY:
		pPlayer->Acquire_Item(PLAYER_2D_ITEM_ID::FATHER_BODY);
		break;
	case CFatherGame::FATHER_PART::FATHER_WING:
		pPlayer->Acquire_Item(PLAYER_2D_ITEM_ID::FATHER_WING);
		break;
	case CFatherGame::FATHER_PART::FATER_HEAD:
		pPlayer->Acquire_Item(PLAYER_2D_ITEM_ID::FATER_HEAD);
		break;
	default:
		break;
	}

	//CFatherGame::GetInstance()->Pickup_FatherPart((CFatherGame::FATHER_PART)m_iFatherPart);
	Event_DeleteObject(this);
}

void CFatherPart_Prop::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CFatherPart_Prop::STATE_IDLE:
		Action_State_Idle(_fTimeDelta);
		break;
	case Client::CFatherPart_Prop::STATE_PICKUP:
		Action_State_Pickup(_fTimeDelta);
		break;
	default:
		break;
	}
}

void CFatherPart_Prop::Action_State_Idle(_float _fTimeDelta)
{
}

void CFatherPart_Prop::Action_State_Pickup(_float _fTimeDelta)
{
}

HRESULT CFatherPart_Prop::Ready_Components(FATHERPART_PROP_DESC* _pDesc)
{
	/* Com_Collider */
	m_p2DColliderComs.resize(1);
	CCollider_Circle::COLLIDER_CIRCLE_DESC Desc = {};
	Desc.pOwner = this;
	Desc.fRadius = 0.5f;
	Desc.vScale = { 1.0f, 1.0f };
	Desc.vOffsetPosition = { 0.0f, 0.0f };
	Desc.isBlock = false;
	Desc.iCollisionGroupID = OBJECT_GROUP::NPC_EVENT;
	CCollider_Circle* pCollider = nullptr;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &Desc)))
		return E_FAIL;

	return S_OK;
}
