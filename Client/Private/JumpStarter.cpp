#include "stdafx.h"
#include "JumpStarter.h"
#include "Player.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Actor_Dynamic.h"
#include "Section_Manager.h"

CJumpStarter::CJumpStarter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CJumpStarter::CJumpStarter(const CJumpStarter& _Prototype)
	:CGameObject(_Prototype)
{
}

HRESULT CJumpStarter::Initialize(void* _pArg)
{
	DRAGGABLE_DESC* pDesc = static_cast<DRAGGABLE_DESC*>(_pArg);

	m_eInteractID = INTERACT_ID::JUMP;
	pDesc->eStartCoord = COORDINATE_2D;
	m_eInteractKey = KEY::E;
	m_eInteractType = INTERACT_TYPE::NORMAL;


	if (COORDINATE_2D == pDesc->eStartCoord)
	{
		if (FAILED(__super::Initialize(pDesc)))
			return E_FAIL;
		m_p2DColliderComs.resize(3);

		CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
		CircleDesc.pOwner = this;
		CircleDesc.fRadius = 30.f;
		CircleDesc.vScale = { 1.0f, 1.0f };
		CircleDesc.vOffsetPosition = { pDesc->vBoxOffset.x,pDesc->vBoxOffset.y };
		CircleDesc.isBlock = false;
		CircleDesc.isTrigger = true;
		CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
			TEXT("Com_2DCollider_Circle"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[2]), &CircleDesc)))
			return E_FAIL;

		if (L"" != pDesc->strInitSectionTag)
			if (FAILED(SECTION_MGR->Add_GameObject_ToSectionLayer(pDesc->strInitSectionTag, this)))
				return E_FAIL;
	}



	return S_OK;
}

void CJumpStarter::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CJumpStarter::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CJumpStarter::Render()
{
	HRESULT hr = __super::Render();

#ifdef _DEBUG
	if (false == m_p2DColliderComs.empty())
	{
		for (auto pCollider : m_p2DColliderComs)
		{
			if (nullptr != pCollider)
				pCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
		}
	}
#endif // _DEBUG
	return hr;
}

void CJumpStarter::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::PLAYER == _pOtherCollider->Get_CollisionGroupID() && OBJECT_GROUP::BLOCKER == _pMyCollider->Get_CollisionGroupID())
	{
		m_bUserAround = true;
		m_bUserContact = true;
	}
}

void CJumpStarter::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CJumpStarter::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::PLAYER == _pOtherCollider->Get_CollisionGroupID() &&
		(OBJECT_GROUP::INTERACTION_OBEJCT == _pMyCollider->Get_CollisionGroupID()))
	{
		m_bUserAround = false;
		m_bUserContact = false;

		if (m_pDragger && m_pDragger == _pOtherObject)
		{
			m_pDragger->Set_InteractObject(nullptr);
			End_Interact(m_pDragger);
			m_pDragger = nullptr;
		}
	}
}

void CJumpStarter::Interact(CPlayer* _pUser)
{

}

_bool CJumpStarter::Is_Interactable(CPlayer* _pUser)
{
	if (_pUser->Is_CarryingObject())
		return false;
	//최초 붙잡기는 접촉해 있어야 함.
	//최초 붙잡기인지 판단 -> m_pDragger가 없는지?
	//접촉했는지 판단 -> m_bUserCOntact가 true?
	//이미 붙잡혀있었으면 m_bUserCOntact는 소용 없음. 
	//-> m_pDragger가 있으면 무적권 가능. 
	//플레이어가 이제 관심없어지면 m_pDragger를 nullptr로 만들어줘야 함.

	if (!(m_bUserAround && m_bUserContact))
		int a = 1;

	return m_bUserAround && m_bUserContact;
}

_float CJumpStarter::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];

}

void CJumpStarter::On_InteractionStart(CPlayer* _pPlayer)
{
}

void CJumpStarter::On_InteractionEnd(CPlayer* _pPlayer)
{
}

void CJumpStarter::Move(_fvector _vForce, _float _fTimeDelta)
{
}


CJumpStarter* CJumpStarter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CJumpStarter* pInstance = new CJumpStarter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : JumpStarter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJumpStarter::Clone(void* _pArg)
{
	CJumpStarter* pInstance = new CJumpStarter(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : JumpStarter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJumpStarter::Free()
{
	__super::Free();	
}

HRESULT CJumpStarter::Cleanup_DeadReferences()
{
	return E_NOTIMPL;
}
