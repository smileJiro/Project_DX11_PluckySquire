#include "stdafx.h"
#include "CollapseBlock.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CCollapseBlock::CCollapseBlock(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:C2DMapObject(_pDevice, _pContext)
{
}

CCollapseBlock::CCollapseBlock(const CCollapseBlock& _Prototype)
	:C2DMapObject(_Prototype)
{
}

HRESULT CCollapseBlock::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CCollapseBlock::Initialize(void* _pArg)
{
	// Save Desc

	// Add Desc

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	/* Save Origin Position*/
	XMStoreFloat2(&m_vOriginPostion, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
	/* Start State */
	m_eCurState = STATE::STATE_IDLE;

	return S_OK;
}

void CCollapseBlock::Priority_Update(_float _fTimeDelta)
{
	

	__super::Priority_Update(_fTimeDelta);
}

void CCollapseBlock::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CCollapseBlock::Late_Update(_float _fTimeDelta)
{
	State_Change();

	__super::Late_Update(_fTimeDelta);
}

HRESULT CCollapseBlock::Render()
{
#ifdef _DEBUG
    if(nullptr != m_p2DColliderComs[0])
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG
	return __super::Render();
}

void CCollapseBlock::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	_uint iCollisionGroupID = _pOtherCollider->Get_CollisionGroupID();

	if (OBJECT_GROUP::PLAYER == (OBJECT_GROUP)iCollisionGroupID)
	{
		if (STATE::STATE_IDLE == m_eCurState)
		{
			/* 첫 충돌 발생 시 + IDLE 상태인 경우 */

			m_eCurState = STATE::STATE_SHAKE;
		}
	}
}

void CCollapseBlock::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CCollapseBlock::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CCollapseBlock::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CCollapseBlock::STATE_IDLE:
		State_Change_Idle();
		break;
	case Client::CCollapseBlock::STATE_SHAKE:
		State_Change_Shake();
		break;
	case Client::CCollapseBlock::STATE_DROP:
		State_Change_Drop();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;

}

void CCollapseBlock::State_Change_Idle()
{
	m_pGravityCom->Set_Active(false);
	m_pGravityCom->Set_GravityOffset(0.0f);
}

void CCollapseBlock::State_Change_Shake()
{
	m_vShakeOffset = {};
	m_vShakeTime.y = 0.0f;
}

void CCollapseBlock::State_Change_Drop()
{
	/* 중력 활성화 */
	Event_SetActive(m_pGravityCom, true);
	m_pGravityCom->Change_State(CGravity::STATE_FALLDOWN);
	m_pGravityCom->Set_GravityAcc(0.0f); /* 현재 플레이어쪽에서 중력을 조작하다보니 우리가 외부에서 해야함 추후 변경예정. */
	m_vDropTime.y = 0.0f;
}

void CCollapseBlock::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CCollapseBlock::STATE_IDLE:
		Action_Idle(_fTimeDelta);
		break;
	case Client::CCollapseBlock::STATE_SHAKE:
		Action_Shake(_fTimeDelta);
		break;
	case Client::CCollapseBlock::STATE_DROP:
		Action_Drop(_fTimeDelta);
		break;
	default:
		break;
	}
}

void CCollapseBlock::Action_Idle(_float _fTimeDelta)
{
	Event_SetActive(m_pGravityCom, false);
}

void CCollapseBlock::Action_Shake(_float _fTimeDelta)
{
	/* ShakeTime이 종료되면, Drop 상태로 전환 */
	m_vShakeTime.y += _fTimeDelta;
	if (m_vShakeTime.x <= m_vShakeTime.y)
	{
		m_vShakeTime.y = 0.0f;
		m_eCurState = STATE::STATE_DROP;

		/* Shake 상태 종료 시, Position을 다시 Origin Position으로 복구 */
		Set_Position(XMVectorSetW(XMLoadFloat2(&m_vOriginPostion), 1.0f));
		return;
	}

	/* ShakeTime 중에는 Position Shaking 수행*/
	m_vShakeCycleTime.y += _fTimeDelta;
	if (m_vShakeCycleTime.x <= m_vShakeCycleTime.y)
	{
		m_vShakeCycleTime.y = 0.0f;
		/* 1. Offset Position을 구한다. */
		m_vShakeOffset.x = m_pGameInstance->Compute_Random(m_vShakeStrength.x * -1.0f, m_vShakeStrength.x);
		m_vShakeOffset.y = m_pGameInstance->Compute_Random(m_vShakeStrength.y * -1.0f, m_vShakeStrength.y);
		/* 2. Origin Position + Offset Position으로 FinalPosition을 구한다. */
		_vector vFinalPos = {};
		vFinalPos = XMLoadFloat2(&m_vOriginPostion) + XMLoadFloat2(&m_vShakeOffset);
		Set_Position(XMVectorSetW(vFinalPos, 1.0f));
	}
}

void CCollapseBlock::Action_Drop(_float _fTimeDelta)
{
	m_vDropTime.y += _fTimeDelta;
	if (m_vDropTime.x <= m_vDropTime.y)
	{
		/* Drop Time이 종료 되었다면, 다시 Object를 OriginPosition으로 이동시킨다. */
		m_vDropTime.y = 0.0f;

		m_eCurState = STATE::STATE_IDLE;
		/* Shake 상태 종료 시, Position을 다시 Origin Position으로 복구 */
		Set_Position(XMVectorSetW(XMLoadFloat2(&m_vOriginPostion), 1.0f));
	}
	else
	{
		/* Drop */
		/* Gravity가 업데이트 된다. */
	}
}

HRESULT CCollapseBlock::Ready_Components()
{
	/* Com_Collider */
	m_p2DColliderComs.resize(1);
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { 100.0f, 25.0f};
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 0.0f, 0.0f };
	AABBDesc.isBlock = true;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::BLOCKER;
	CCollider_AABB* pCollider = nullptr;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;

	/* Com_Gravity */
	CGravity::GRAVITY_DESC GravityDesc = {};
	GravityDesc.fGravity = 9.8f * 290.f;
	GravityDesc.vGravityDirection = { 0.0f, -1.0f, 0.0f };
	GravityDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Gravity"),
		TEXT("Com_Gravity"), reinterpret_cast<CComponent**>(&m_pGravityCom), &GravityDesc)))
		return E_FAIL;
	m_pGravityCom->Set_Active(false);

	return S_OK;
}

void CCollapseBlock::Active_OnEnable()
{
	__super::Active_OnEnable();
}

void CCollapseBlock::Active_OnDisable()
{
	__super::Active_OnDisable();
}

CCollapseBlock* CCollapseBlock::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCollapseBlock* pInstance = new CCollapseBlock(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CCollapseBlock ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCollapseBlock::Clone(void* _pArg)
{
	CCollapseBlock* pInstance = new CCollapseBlock(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollapseBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollapseBlock::Free()
{

	__super::Free();
}
