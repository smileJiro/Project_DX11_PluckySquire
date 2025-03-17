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
	JUMP_STARTER_DESC* pDesc = static_cast<JUMP_STARTER_DESC*>(_pArg);

	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	pDesc->eStartCoord = COORDINATE_2D;
	m_eInteractID = INTERACT_ID::JUMP;
	m_eInteractKey = KEY::E;
	m_eInteractType = INTERACT_TYPE::NORMAL;
	m_eJumpMoveDir = pDesc->eJumpMoveDir;
	m_fTargetPos = pDesc->fTargetPos;
	m_fMoveMag = pDesc->fMoveMag;


	//switch (m_eJumpMoveDir)
	//{
	//case Client::F_DIRECTION::LEFT:
	//	m_fTargetPos.x *= -1.f;
	//	break;
	//case Client::F_DIRECTION::DOWN:
	//	m_fTargetPos.y *= -1.f;
	//	break;
	//}



	if (COORDINATE_2D == pDesc->eStartCoord)
	{
		if (FAILED(__super::Initialize(pDesc)))
			return E_FAIL;
		m_p2DColliderComs.resize(2);

		CCollider_AABB::COLLIDER_AABB_DESC tBoxDesc = {};
		tBoxDesc.pOwner = this;
		tBoxDesc.vExtents = { 1.f, 1.f};
		tBoxDesc.vScale = { 1.f, 1.f };
		tBoxDesc.vOffsetPosition = { pDesc->vBoxOffset.x, pDesc->vBoxOffset.y };
		tBoxDesc.isBlock = true;
		tBoxDesc.isTrigger = false;
		tBoxDesc.iCollisionGroupID = OBJECT_GROUP::BLOCKER;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_2DCollider_AABB"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &tBoxDesc)))
			return E_FAIL;
		
		 tBoxDesc = {};
		tBoxDesc.pOwner = this;
		tBoxDesc.vExtents = { 1.2f, 1.2f };
		tBoxDesc.vScale = { 1.f, 1.f };
		tBoxDesc.vOffsetPosition = { pDesc->vBoxOffset.x, pDesc->vBoxOffset.y };
		tBoxDesc.isBlock = false;
		tBoxDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_2DCollider_AABB_Trigger"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &tBoxDesc)))
			return E_FAIL;


		if (L"" != pDesc->strInitSectionTag)
			if (FAILED(SECTION_MGR->Add_GameObject_ToSectionLayer(pDesc->strInitSectionTag, this)))
				return E_FAIL;
	}

#ifdef _DEBUG
	if (false == m_p2DColliderComs.empty() && nullptr != m_p2DColliderComs[0])
		m_p2DColliderComs[0]->Set_DebugColor({ 1.f,1.f,0.f,1.f });
	if (false == m_p2DColliderComs.empty() && nullptr != m_p2DColliderComs[1])
		m_p2DColliderComs[1]->Set_DebugColor({ 1.f,0.f,1.f,1.f });
#endif // _DEBUG



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
	if (m_isBodyContact == false && _pMyCollider->Get_CollisionGroupID() == OBJECT_GROUP::BLOCKER)
	{
		m_isBodyContact = true;
	}
}

void CJumpStarter::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CJumpStarter::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (m_isBodyContact == true && _pMyCollider->Get_CollisionGroupID() == OBJECT_GROUP::BLOCKER)
	{
		m_isBodyContact = false;

	}
	if (m_isJump && _pMyCollider->Get_CollisionGroupID() == OBJECT_GROUP::INTERACTION_OBEJCT)
	{
		m_p2DColliderComs[0]->Set_Active(true);
		m_isJump = false;
	}
}

void CJumpStarter::Interact(CPlayer* _pUser)
{

	_vector vPlayerPos = _pUser->Get_FinalPosition() + XMVectorSet(m_fTargetPos.x, m_fTargetPos.y, 0.0f, 0.0f);
	AUTOMOVE_COMMAND AutoMove{};
	AutoMove.eType = AUTOMOVE_TYPE::MOVE_TO;
	AutoMove.fPostDelayTime = 0.0f;
	AutoMove.fPreDelayTime = 0.0f;
	AutoMove.fMoveSpeedMag = m_fMoveMag;
	AutoMove.iAnimIndex = (_uint)CPlayer::ANIM_STATE_2D::PLAYER_RUN_DOWN;
	AutoMove.vTarget = vPlayerPos;
	
	m_p2DColliderComs[0]->Set_Active(false);
	_pUser->Add_AutoMoveCommand(AutoMove);
	_pUser->Start_AutoMove(true);

	_pUser->Set_State(CPlayer::STATE::JUMP_UP);
	m_isJump = true;
}

_bool CJumpStarter::Is_Interactable(CPlayer* _pUser)
{
	if (true == m_isJump)
		return false;
	_vector vPos = Get_FinalPosition(COORDINATE_2D);
	_vector vTargetPos = _pUser->Get_FinalPosition(COORDINATE_2D);

	switch (m_eJumpMoveDir)
	{
	case Client::F_DIRECTION::LEFT:
		return m_isBodyContact && XMVectorGetX(vPos) < XMVectorGetX(vTargetPos);
		break;
	case Client::F_DIRECTION::RIGHT:
		return m_isBodyContact && XMVectorGetX(vPos) > XMVectorGetX(vTargetPos);
		break;
	case Client::F_DIRECTION::UP:
		return m_isBodyContact && XMVectorGetY(vPos) > XMVectorGetY(vTargetPos);
		break;
	case Client::F_DIRECTION::DOWN:
		return m_isBodyContact && XMVectorGetY(vPos) < XMVectorGetY(vTargetPos);
		break;
	default:
		break;
	}

	return false;
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
	return S_OK;
}
