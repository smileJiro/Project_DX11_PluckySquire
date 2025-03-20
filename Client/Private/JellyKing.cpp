#include "stdafx.h"
#include "JellyKing.h"
#include "GameInstance.h"

#include "Dialog_Manager.h"
#include "Camera_Manager.h"
#include "PlayerData_Manager.h"
#include "Section_Manager.h"
#include "FatherPart_Prop.h"
#include "FatherGame.h"
#include "CandleGame.h"

CJellyKing::CJellyKing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CJellyKing::CJellyKing(const CJellyKing& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CJellyKing::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CJellyKing::Initialize(void* _pArg)
{
	JELLYKING_DESC* pDesc = static_cast<JELLYKING_DESC*>(_pArg);
	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("JellyKing"), TEXT("Prototype_Component_Shader_VtxPosTex"));
	
	/* 3D Actor */
	pDesc->eActorType = ACTOR_TYPE::STATIC;
	CActor::ACTOR_DESC ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER;

	SHAPE_SPHERE_DESC SphereDesc{};
	SphereDesc.fRadius = 13.0f;

	SHAPE_DATA ShapeData = {};
	ShapeData.pShapeDesc = &SphereDesc;
	ShapeData.isTrigger = true;
	ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	ActorDesc.ShapeDatas.push_back(ShapeData);
	pDesc->pActorDesc = &ActorDesc;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	/* Trigger Actor Set GlobalPose */
	_vector vPos = m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);
	vPos = CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(TEXT("Chapter6_SKSP_06"), _float2(XMVectorGetX(vPos), XMVectorGetY(vPos)));

	m_pActorCom->Set_GlobalPose(_float3(XMVectorGetX(vPos) + 2.0f, XMVectorGetY(vPos), XMVectorGetZ(vPos) + 2.0f));
	m_pActorCom->Set_Active(true);

	/* Set Anim End */
	Register_OnAnimEndCallBack(bind(&CJellyKing::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	m_eCurState = STATE_IDLE;
	State_Change();
    return S_OK;
}

void CJellyKing::Priority_Update(_float _fTimeDelta)
{
	//if (KEY_DOWN(KEY::K))
	//{
	//	Set_Position(XMVectorSet(314, -54,0,1));
	//}

	if (nullptr != m_pCandleGame)
	{
		if (true == m_isCandleGame)
		{

			if (true == m_pCandleGame->Is_ClearGame())
			{
				m_eCurState = STATE_CHEER;
				m_isCandleGame = false;
			}
		}
	}


	__super::Priority_Update(_fTimeDelta);
}

void CJellyKing::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CJellyKing::Late_Update(_float _fTimeDelta)
{
	State_Change();


	__super::Late_Update(_fTimeDelta);
}

HRESULT CJellyKing::Render()
{
#ifdef _DEBUG
	if (nullptr != m_p2DColliderComs[0])
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

	if (FAILED(__super::Render()))
		return E_FAIL;

    return S_OK;
}

void CJellyKing::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	_uint iCollisionGroupID = _pOtherCollider->Get_CollisionGroupID();

	if (OBJECT_GROUP::PLAYER == (OBJECT_GROUP)iCollisionGroupID)
	{
		///* 대화 시작 */
		//if (m_iDialogueIndex < 1)
		//{
		//	m_eCurState = STATE_TALK;
		//}
	}
}

void CJellyKing::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	/* Event 실행 : 조건 (jellyking dialogueindex 가 0 일때만.)*/
	if (_My.pActorUserData->iObjectGroup == OBJECT_GROUP::PLAYER)
	{
		if (0 == m_iDialogueIndex)
		{
			m_eCurState = STATE_TALK;
		}
	}

}

void CJellyKing::Active_OnEnable()
{
	// actor 켜거나 끄지 않는다.	
}

void CJellyKing::Active_OnDisable()
{
	// actor 켜거나 끄지 않는다.
}


void CJellyKing::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CJellyKing::STATE_CHEER_OLD:
		break;
	case Client::CJellyKing::STATE_CHEER:
		State_Change_Cheer();
		break;
	case Client::CJellyKing::STATE_CRY:
		break;
	case Client::CJellyKing::STATE_GETATTENTION_1:
		State_Change_GetAttention1();
		break;
	case Client::CJellyKing::STATE_GETATTENTION_2:
		State_Change_GetAttention2();
		break;
	case Client::CJellyKing::STATE_IDLE:
		State_Change_Idle();
		break;
	case Client::CJellyKing::STATE_TALK:
		State_Change_Talk();
		break;
	case Client::CJellyKing::STATE_PANIC:
		break;
	default:
		break;
	}


	m_ePreState = m_eCurState;
}

void CJellyKing::State_Change_CheerOld()
{
}

void CJellyKing::State_Change_Cheer()
{
	m_pControllerModel->Switch_Animation((_uint)STATE::STATE_CHEER);

	CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

	CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET);
	CCamera_Manager::GetInstance()->Change_CameraTarget(this, 1.0f);
	_uint iZoomLevel = CCamera_Manager::GetInstance()->Get_CurrentCamera()->Get_CurrentZoomLevel();
	CCamera_Manager::GetInstance()->Start_Turn_ArmVector(CCamera_Manager::TARGET, 1.0f, XMVector3Normalize(XMVectorSet(0.0f, 0.2f, -1.0f, 0.0f)), RATIO_TYPE::LERP);
	CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 1.0f, iZoomLevel - 2, RATIO_TYPE::EASE_OUT);
}

void CJellyKing::State_Change_Cry()
{
}

void CJellyKing::State_Change_GetAttention1()
{
}

void CJellyKing::State_Change_GetAttention2()
{
}

void CJellyKing::State_Change_Idle()
{
	m_pControllerModel->Switch_Animation((_uint)STATE::STATE_IDLE);
}

void CJellyKing::State_Change_Talk()
{
	m_pControllerModel->Switch_Animation((_uint)STATE::STATE_TALK);

	/* 1. Player Input 막기 */
	CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
	if(nullptr != pPlayer)
	{
		pPlayer->Set_BlockPlayerInput(true);
		pPlayer->Set_2DDirection(E_DIRECTION::UP);
	}

	/* 2. Dialogue 재생 */
	_wstring str = TEXT("JellyKing_");
	str += to_wstring(m_iDialogueIndex);
	CDialog_Manager::GetInstance()->Set_DialogId(str.c_str());

	/* 3. Change CameraTarget */
	if (1 != m_iDialogueIndex)
	{
		CCamera_Manager::GetInstance()->Set_ResetData(CCamera_Manager::TARGET);

		CCamera_Manager::GetInstance()->Change_CameraType(CCamera_Manager::TARGET);
		CCamera_Manager::GetInstance()->Change_CameraTarget(this, 1.0f);
		_uint iZoomLevel = CCamera_Manager::GetInstance()->Get_CurrentCamera()->Get_CurrentZoomLevel();
		CCamera_Manager::GetInstance()->Start_Turn_ArmVector(CCamera_Manager::TARGET, 1.0f, XMVector3Normalize(XMVectorSet(0.0f, 0.2f, -1.0f, 0.0f)), RATIO_TYPE::LERP);
		CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET, 1.0f, iZoomLevel - 2, RATIO_TYPE::EASE_OUT);
	}

}

void CJellyKing::State_Change_Panic()
{
}

void CJellyKing::Action_State(_float _fTimeDelta)
{

	switch (m_eCurState)
	{
	case Client::CJellyKing::STATE_CHEER_OLD:
		break;
	case Client::CJellyKing::STATE_CHEER:
		Action_State_Cheer(_fTimeDelta);
		break;
	case Client::CJellyKing::STATE_CRY:
		break;
	case Client::CJellyKing::STATE_GETATTENTION_1:
		Action_State_GetAttention1(_fTimeDelta);
		break;
	case Client::CJellyKing::STATE_GETATTENTION_2:
		Action_State_GetAttention2(_fTimeDelta);
		break;
	case Client::CJellyKing::STATE_IDLE:
		Action_State_Idle(_fTimeDelta);
		break;
	case Client::CJellyKing::STATE_TALK:
		Action_State_Talk(_fTimeDelta);
		break;
	case Client::CJellyKing::STATE_PANIC:
		break;
	default:
		break;
	}

}

void CJellyKing::Action_State_CheerOld(_float _fTimeDelta)
{
}

void CJellyKing::Action_State_Cheer(_float _fTimeDelta)
{
}

void CJellyKing::Action_State_Cry(_float _fTimeDelta)
{
}

void CJellyKing::Action_State_GetAttention1(_float _fTimeDelta)
{
}

void CJellyKing::Action_State_GetAttention2(_float _fTimeDelta)
{
}

void CJellyKing::Action_State_Idle(_float _fTimeDelta)
{
}

void CJellyKing::Action_State_Talk(_float _fTimeDelta)
{
	if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
	{
		Finished_DialogueAction();
	}
}

void CJellyKing::Action_State_Panic(_float _fTimeDelta)
{
}

void CJellyKing::Finished_DialogueAction()
{
	switch (m_iDialogueIndex)
	{
	case 0: /* 대화 후 캔들게임 시작 */
	{
		CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
		/* 3. Player Input 풀기. */
		if (nullptr != pPlayer)
		{
			pPlayer->Set_BlockPlayerInput(false);

			/* 1. Change CameraTarget */
			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 1.0f);

			/* 2. Change Zoom Level */
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 1.0f);
			
			/* 3. Start CandleGame */
			/* Test Candle */
			CCandleGame::CANDLEGAME_DESC CandleGameDesc;
			CandleGameDesc.iCurLevelID = LEVEL_CHAPTER_6;
			CandleGameDesc.Build_3D_Transform(_float3(0.0f, 0.0f, 0.0f));
			CandleGameDesc.eStartCoord = COORDINATE_3D;
			CandleGameDesc.isCoordChangeEnable = false;

			// (1). 감옥 양초
			CandleGameDesc.CandlePositions.push_back(_float3(74.f, 1.5f, -2.f));
			// (2). 우측 상단
			CandleGameDesc.CandlePositions.push_back(_float3(77.f, 23.9f, 35.6f));
			// (3). 창문 근처
			CandleGameDesc.CandlePositions.push_back(_float3(43.9f, 36.1f, 29.f));
			// (4). 낙사 구간
			CandleGameDesc.CandlePositions.push_back(_float3(45.f, 2.43f, -31.3f));
			// (5). 꼭대기
			CandleGameDesc.CandlePositions.push_back(_float3(69.68f, 16.64f, -19.4f));

			CGameObject* pGameObject = nullptr;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_CandleGame"), LEVEL_CHAPTER_6, TEXT("Layer_CandleGame"), &pGameObject, &CandleGameDesc)))
			{
				++m_iDialogueIndex;
				return;
			}
			m_pCandleGame = static_cast<CCandleGame*>(pGameObject);
			Safe_AddRef(m_pCandleGame);
			m_isCandleGame = true;

			m_eCurState = STATE_IDLE;


		}
	}
	break;
	case 1: /* 캔들게임 클리어 후 대화 끝나고. */
	{
		CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
		/* 3. Player Input 풀기. */
		if (nullptr != pPlayer)
		{
			pPlayer->Set_BlockPlayerInput(false);

			/* 1. Change CameraTarget */
			CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer, 1.0f);

			/* 2. Change Zoom Level */
			CCamera_Manager::GetInstance()->Start_ResetArm_To_SettingPoint(CCamera_Manager::TARGET, 1.0f);


			// 부품 생성 
			CFatherPart_Prop::FATHERPART_PROP_DESC Desc{};
			Desc.iCurLevelID = m_iCurLevelID;
			Desc.iFatherPartID = CFatherGame::FATHER_PART::FATHER_WING;
			Desc.Build_2D_Transform(_float2(150.f, -160.f), _float2(200.0f, 200.0f));
			CGameObject* pGameObject = nullptr;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_FatherPart_Prop"), LEVEL_CHAPTER_6, TEXT("Layer_FatherPart_Prop"), &pGameObject, &Desc)))
				assert(nullptr);

			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, pGameObject, SECTION_2D_PLAYMAP_TRIGGER);

			/* 캔들게임 종료 및 ui 끄기 */
			m_pCandleGame->OnOffCandleUIs(false);
			Safe_Release(m_pCandleGame);
			m_pCandleGame = nullptr;


			m_eCurState = STATE_IDLE;
		}
	}
	break;
	case 2: /* 최종 대화 끝나고. 파트 윙 생성 */
	{

	}
	break;
	default:
		break;
	}

	++m_iDialogueIndex;
}

HRESULT CJellyKing::Ready_Components(JELLYKING_DESC* _pDesc)
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

void CJellyKing::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	switch (m_eCurState)
	{
	case Client::CJellyKing::STATE_CHEER_OLD:
		break;
	case Client::CJellyKing::STATE_CHEER:
		m_eCurState = STATE_TALK;
		break;
	case Client::CJellyKing::STATE_CRY:
		break;
	case Client::CJellyKing::STATE_GETATTENTION_1:
		break;
	case Client::CJellyKing::STATE_GETATTENTION_2:
		break;
	case Client::CJellyKing::STATE_IDLE:
		break;
	case Client::CJellyKing::STATE_TALK:
		break;
	case Client::CJellyKing::STATE_PANIC:
		break;
	default:
		break;
	}
}

CJellyKing* CJellyKing::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CJellyKing* pInstance = new CJellyKing(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CJellyKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJellyKing::Clone(void* _pArg)
{
	CJellyKing* pInstance = new CJellyKing(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CJellyKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJellyKing::Free()
{
	Safe_Release(m_pCandleGame);
	__super::Free();
}
