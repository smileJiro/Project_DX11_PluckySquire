#include "stdafx.h"
#include "Mug_Alien.h"
#include "GameInstance.h"
#include "Dialog_Manager.h"
#include "Camera_Manager.h"
#include "PlayerData_Manager.h"
#include "Section_Manager.h"

CMug_Alien::CMug_Alien(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CMug_Alien::CMug_Alien(const CMug_Alien& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CMug_Alien::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMug_Alien::Initialize(void* _pArg)
{
	MUG_ALIEN_DESC* pDesc = static_cast<MUG_ALIEN_DESC*>(_pArg);
	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("Mug_Allien_v02"), TEXT("Prototype_Component_Shader_VtxPosTex"));

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_eCurState = STATE_IDLE;
	State_Change();
    return S_OK;
}

void CMug_Alien::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CMug_Alien::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CMug_Alien::Late_Update(_float _fTimeDelta)
{
	State_Change();

	__super::Late_Update(_fTimeDelta);
}

HRESULT CMug_Alien::Render()
{
#ifdef _DEBUG
	if (nullptr != m_p2DColliderComs[0])
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

	if (FAILED(__super::Render()))
		return E_FAIL;

    return S_OK;
}

void CMug_Alien::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	_uint iCollisionGroupID = _pOtherCollider->Get_CollisionGroupID();

	if (OBJECT_GROUP::PLAYER == (OBJECT_GROUP)iCollisionGroupID)
	{
		/* 대화 시작 */
		m_eCurState = STATE_TALK;
	}
}

void CMug_Alien::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMug_Alien::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CMug_Alien::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CMug_Alien::STATE_IDLE:
		State_Change_Idle();
		break;
	case Client::CMug_Alien::STATE_TALK:
		State_Change_Talk();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CMug_Alien::State_Change_Idle()
{
	m_pControllerModel->Switch_Animation((_uint)STATE::STATE_IDLE);
}

void CMug_Alien::State_Change_Talk()
{
	m_pControllerModel->Switch_Animation((_uint)STATE::STATE_TALK);

	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	if(nullptr != pPlayer)
	{
		pPlayer->Set_2DDirection(E_DIRECTION::UP);
	}

	/* 2. Dialogue 재생 */
	_wstring str = TEXT("Mug_Alien_");
	str += to_wstring(m_iDialogueIndex);
	CDialog_Manager::GetInstance()->Set_DialogId(str.c_str());

	/* 4. Change CameraTarget */
	CCamera_Manager::GetInstance()->Change_CameraTarget(this, 1.0f);
	_uint iZoomLevel = CCamera_Manager::GetInstance()->Get_CurrentCamera()->Get_CurrentZoomLevel();
	CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET_2D, 1.0f, iZoomLevel - 2, RATIO_TYPE::EASE_OUT);
}

void CMug_Alien::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CMug_Alien::STATE_IDLE:
		Action_State_Idle(_fTimeDelta);
		break;
	case Client::CMug_Alien::STATE_TALK:
		Action_State_Talk(_fTimeDelta);
		break;
	default:
		break;
	}
}

void CMug_Alien::Action_State_Idle(_float _fTimeDelta)
{
}

void CMug_Alien::Action_State_Talk(_float _fTimeDelta)
{
	if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
	{
		Finished_DialogueAction();
	}
}

void CMug_Alien::Finished_DialogueAction()
{
	switch (m_iDialogueIndex)
	{
	case 0:
	{
		CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
		/* 3. Player Input 풀기. */
		if (nullptr != pPlayer)
		{
			pPlayer->Set_2DDirection(E_DIRECTION::RIGHT);

			/* 1. Change CameraTarget */
			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 1.0f);

			/* 2. Change Zoom Level */
			_uint iZoomLevel = CCamera_Manager::GetInstance()->Get_CurrentCamera()->Get_CurrentZoomLevel();
			CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET_2D, 1.0f, iZoomLevel + 2, RATIO_TYPE::EASE_OUT);

			/* 3. Active True FatherParts Prop */

			m_eCurState = STATE_IDLE;
			++m_iDialogueIndex;
		}
	}
	break;
	default:
		break;
	}
}

HRESULT CMug_Alien::Ready_Components(MUG_ALIEN_DESC* _pDesc)
{
	/* Com_Collider */
	m_p2DColliderComs.resize(1);
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { 10.0f, 100.0f };
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 0.0f, 0.0f };
	AABBDesc.isBlock = false;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::NPC_EVENT;
	CCollider_AABB* pCollider = nullptr;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;

    return S_OK;
}

CMug_Alien* CMug_Alien::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMug_Alien* pInstance = new CMug_Alien(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CMug_Alien");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMug_Alien::Clone(void* _pArg)
{
	CMug_Alien* pInstance = new CMug_Alien(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CMug_Alien");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMug_Alien::Free()
{

	__super::Free();
}
