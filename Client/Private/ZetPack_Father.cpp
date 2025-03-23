#include "stdafx.h"
#include "ZetPack_Father.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "Camera_Manager.h"
#include "Pooling_Manager.h"
#include "FatherGame.h"
#include "ZetPack_Child.h"
#include "PlayerData_Manager.h"

CZetPack_Father::CZetPack_Father(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CZetPack_Father::CZetPack_Father(const CZetPack_Father& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CZetPack_Father::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CZetPack_Father::Initialize(void* _pArg)
{
    ZETPACK_FATHER_DESC* pDesc = static_cast<ZETPACK_FATHER_DESC*>(_pArg);

	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("ZetPack_Father"), TEXT("Prototype_Component_Shader_VtxPosTex"));
	
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	
	/* Set Anim End */
	Register_OnAnimEndCallBack(bind(&CZetPack_Father::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	m_eCurState = STATE_NONRENDER;
	State_Change();
    return S_OK;
}

void CZetPack_Father::Priority_Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::K))
	{
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(-142.f, -75.f, 0.0f, 1.0f));
	}
	__super::Priority_Update(_fTimeDelta);
}

void CZetPack_Father::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CZetPack_Father::Late_Update(_float _fTimeDelta)
{
	State_Change();

	__super::Late_Update(_fTimeDelta);
}

HRESULT CZetPack_Father::Render()
{
#ifdef _DEBUG
	if (nullptr != m_p2DColliderComs[0])
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

	__super::Render();
    return S_OK;
}

void CZetPack_Father::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	_uint iCollisionGroupID = _pOtherCollider->Get_CollisionGroupID();

	if (OBJECT_GROUP::PLAYER == (OBJECT_GROUP)iCollisionGroupID)
	{
		if (false == m_isContactPlayer)
		{
			m_isContactPlayer = true;
			/* 1. 첫 충돌 시, ZetPack 만 특정위치로 보내고, ZetPack_Child의 대화를 재생 */
			CZetPack_Child* pChild = CFatherGame::GetInstance()->Get_ZetPack_Child();
			pChild->Set_CurState(CZetPack_Child::STATE_MAKEFATHER);

		}
	}
}

void CZetPack_Father::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CZetPack_Father::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CZetPack_Father::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CZetPack_Father::STATE_FIREENGINE:
		State_Change_State_FireEngine();
		break;
	case Client::CZetPack_Father::STATE_REASSEMBLE:
		State_Change_Reassemble();
		break;
	case Client::CZetPack_Father::STATE_HAPPY:
		State_Change_Happy();
		break;
	case Client::CZetPack_Father::STATE_IDLE:
		State_Change_Idle();
		break;
	case Client::CZetPack_Father::STATE_SAD:
		State_Change_Sad();
		break;
	case Client::CZetPack_Father::STATE_TALK:
		State_Change_Talk();
		break;
	case Client::CZetPack_Father::STATE_NONRENDER:
		State_Change_NoneRender();
		break;

	default:
		break;
	}
	m_ePreState = m_eCurState;
}

void CZetPack_Father::State_Change_State_FireEngine()
{
	START_SFX_DELAY(TEXT("A_sfx_jetpac_thrust_start-3"), 0.0f, g_SFXVolume * 1.2f, false);
	m_pControllerModel->Switch_Animation(STATE::STATE_FIREENGINE);
	_uint iCurZoomLevel = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CAMERA_TYPE::TARGET_2D)->Get_CurrentZoomLevel();
	CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::CAMERA_TYPE::TARGET_2D, 1.0f, iCurZoomLevel + 1, RATIO_TYPE::EASE_IN);
}

void CZetPack_Father::State_Change_Reassemble()
{
	START_SFX_DELAY(TEXT("A_sfx_zap_reconstruct"), 0.1f, g_SFXVolume * 1.4f, false);
	m_isRender = true;
	CCamera_Manager::GetInstance()->Change_CameraTarget(this);
	_uint iCurZoomLevel = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::CAMERA_TYPE::TARGET_2D)->Get_CurrentZoomLevel();
	CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::CAMERA_TYPE::TARGET_2D, 1.0f, iCurZoomLevel + 2, RATIO_TYPE::EASE_IN);
	m_pControllerModel->Switch_Animation(STATE::STATE_REASSEMBLE);
	
}

void CZetPack_Father::State_Change_Happy()
{
	m_pControllerModel->Switch_Animation(STATE::STATE_HAPPY);
}

void CZetPack_Father::State_Change_Idle()
{
	m_pControllerModel->Switch_Animation(STATE::STATE_IDLE);
}

void CZetPack_Father::State_Change_Sad()
{
	m_pControllerModel->Switch_Animation(STATE::STATE_SAD);
}

void CZetPack_Father::State_Change_Talk()
{
	m_pControllerModel->Switch_Animation(STATE::STATE_TALK);
	/* 2. Dialogue 재생 */
	_wstring str = TEXT("ZetPack_Father_");
	str += to_wstring(m_iDialogueIndex);
	CDialog_Manager::GetInstance()->Set_DialogId(str.c_str());

	/* 3. Player Input 막기. */
	//CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	//if (nullptr != pPlayer)
	//{
	//	pPlayer->Set_BlockPlayerInput(true);
	//	pPlayer->Set_2DDirection(E_DIRECTION::UP);
	//}

}

void CZetPack_Father::State_Change_NoneRender()
{
	m_isRender = false;
}


void CZetPack_Father::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CZetPack_Father::STATE_FIREENGINE:
		Action_State_State_FireEngine(_fTimeDelta);
		break;
	case Client::CZetPack_Father::STATE_REASSEMBLE:
		Action_State_Reassemble(_fTimeDelta);
		break;
	case Client::CZetPack_Father::STATE_HAPPY:
		Action_State_Happy(_fTimeDelta);
		break;
	case Client::CZetPack_Father::STATE_IDLE:
		Action_State_Idle(_fTimeDelta);
		break;
	case Client::CZetPack_Father::STATE_SAD:
		Action_State_Sad(_fTimeDelta);
		break;
	case Client::CZetPack_Father::STATE_TALK:
		Action_State_Talk(_fTimeDelta);
		break;
	case Client::CZetPack_Father::STATE_NONRENDER:
		Action_State_NoneRender(_fTimeDelta);
		break;
	default:
		break;
	}
}

void CZetPack_Father::Action_State_State_FireEngine(_float _fTimeDelta)
{
	
}

void CZetPack_Father::Action_State_Reassemble(_float _fTimeDelta)
{
}

void CZetPack_Father::Action_State_Happy(_float _fTimeDelta)
{

}

void CZetPack_Father::Action_State_Idle(_float _fTimeDelta)
{
}

void CZetPack_Father::Action_State_Sad(_float _fTimeDelta)
{
}

void CZetPack_Father::Action_State_Talk(_float _fTimeDelta)
{
	if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
	{
		Finished_DialogueAction();
	}
}

void CZetPack_Father::Action_State_NoneRender(_float _fTimeDelta)
{

}

void CZetPack_Father::Finished_DialogueAction()
{
	switch (m_iDialogueIndex)
	{
	case 0:
	{
		m_eCurState = STATE_FIREENGINE;
		++m_iDialogueIndex;
	}
		break;
	case 1:
	{
		CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
		assert(pPlayer);
		CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 1.0f);
		m_eCurState = STATE_IDLE;
		++m_iDialogueIndex;
	}
	break;
	default:
		break;
	}
}

void CZetPack_Father::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	switch (m_eCurState)
	{
	case Client::CZetPack_Father::STATE_FIREENGINE:
	{
		START_SFX_DELAY(TEXT("A_sfx_tank_fire_0"), 0.0f, g_SFXVolume * 1.2f, false);
		CCamera_Manager::GetInstance()->Start_ZoomIn();
		m_eCurState = STATE_TALK;
		// TODO:: 도장 튀어 나오게도 해야함. 컵 돌아가면서 
		CFatherGame::GetInstance()->Start_StopStampMoveWork();

		CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
		CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
		pPlayer->Set_Mode(CPlayer::PLAYER_MODE_SWORD);
		pZetPack_Child->ContactPlayer_End();
		pZetPack_Child->Set_CurState(CZetPack_Child::STATE_IDLE);
	}
		break;
	case Client::CZetPack_Father::STATE_REASSEMBLE:
		m_eCurState = STATE_HAPPY;
		//m_pControllerModel->Switch_Animation(STATE::STATE_HAPPY);
		break;
	case Client::CZetPack_Father::STATE_HAPPY:
		m_eCurState = STATE_TALK;
		break;
	case Client::CZetPack_Father::STATE_IDLE:
		break;
	case Client::CZetPack_Father::STATE_SAD:
		break;
	case Client::CZetPack_Father::STATE_TALK:
		break;
	default:
		break;
	}
}

HRESULT CZetPack_Father::Ready_Components(ZETPACK_FATHER_DESC* _pDesc)
{
	/* Com_Collider */
	m_p2DColliderComs.resize(1);
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { 10.0f, 100.0f };
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 200.0f, -50.0f };
	AABBDesc.isBlock = false;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::NPC_EVENT;
	CCollider_AABB* pCollider = nullptr;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

CZetPack_Father* CZetPack_Father::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CZetPack_Father* pInstance = new CZetPack_Father(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CZetPack_Father");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CZetPack_Father* CZetPack_Father::Clone(void* _pArg)
{
	CZetPack_Father* pInstance = new CZetPack_Father(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CZetPack_Father");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZetPack_Father::Free()
{

    __super::Free();
}
