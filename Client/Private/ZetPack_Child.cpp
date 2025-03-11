#include "stdafx.h"
#include "ZetPack_Child.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"
#include "Camera_Manager.h"
#include "Pooling_Manager.h"
#include "FatherGame.h"
#include "ZetPack_Father.h"

CZetPack_Child::CZetPack_Child(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CZetPack_Child::CZetPack_Child(const CZetPack_Child& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CZetPack_Child::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CZetPack_Child::Initialize(void* _pArg)
{
    ZETPACK_CHILD_DESC* pDesc = static_cast<ZETPACK_CHILD_DESC*>(_pArg);
    if (nullptr == pDesc->pPlayer)
        return E_FAIL;
    m_pPlayer = pDesc->pPlayer;
    Safe_AddRef(m_pPlayer);

	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("ZetPack_Child"), TEXT("Prototype_Component_Shader_VtxPosTex"));
	
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	
	m_eCurState = STATE_IDLE;
	State_Change();
    return S_OK;
}

void CZetPack_Child::Priority_Update(_float _fTimeDelta)
{
	if (true == m_pPlayer->Is_Dead())
	{
		Safe_Release(m_pPlayer);
		m_pPlayer = nullptr;
	}

	if (KEY_DOWN(KEY::I))
	{
		m_isContactPlayer ^= 1;
	}
	__super::Priority_Update(_fTimeDelta);
}

void CZetPack_Child::Update(_float _fTimeDelta)
{
	Action_State(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CZetPack_Child::Late_Update(_float _fTimeDelta)
{
	State_Change();


	__super::Late_Update(_fTimeDelta);
}

HRESULT CZetPack_Child::Render()
{
#ifdef _DEBUG
	if (nullptr != m_p2DColliderComs[0])
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG

	__super::Render();
    return S_OK;
}

void CZetPack_Child::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	_uint iCollisionGroupID = _pOtherCollider->Get_CollisionGroupID();

	if (OBJECT_GROUP::PLAYER == (OBJECT_GROUP)iCollisionGroupID)
	{
		if (false == m_isContactPlayer)
		{
			/* 대화 시작 */
			m_eCurState = STATE_TALK;
		}
	}
}

void CZetPack_Child::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CZetPack_Child::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CZetPack_Child::State_Change()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case Client::CZetPack_Child::STATE_IDLE:
		State_Change_Idle();
		break;
	case Client::CZetPack_Child::STATE_TALK:
		State_Change_Talk();
		break;
	case Client::CZetPack_Child::STATE_CHASE:
		State_Change_Chase();
		break;
	case Client::CZetPack_Child::STATE_PORTALOUT:
		State_Change_PortalOut();
		break;
	case Client::CZetPack_Child::STATE_PORTALIN:
		State_Change_PortalIn();
		break;
	case Client::CZetPack_Child::STATE_MAKEFATHER:
		State_Change_MakeFather();
		break;
	default:
		break;
	}

	m_ePreState = m_eCurState;
}

void CZetPack_Child::State_Change_Idle()
{
}

void CZetPack_Child::State_Change_Talk()
{
	/* 1. 애니메이션 방향 체크 */
	if (STATE::STATE_MAKEFATHER == m_ePreState)
	{
		m_eDirection = DIR_DOWN;
		m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::TALK_DOWN);
	}
	else
	{
		Update_AnimationDirection();
	}


	/* 2. Dialogue 재생 */
	_wstring str = TEXT("ZetPack_Child_");
	str += to_wstring(m_iDialogueIndex);
	CDialog_Manager::GetInstance()->Set_DialogId(str.c_str());

	/* 3. Player Input 막기. */
	if (nullptr != m_pPlayer)
	{
		m_pPlayer->Set_BlockPlayerInput(true);
		m_pPlayer->Set_2DDirection(E_DIRECTION::UP);
	}

	/* 4. Change CameraTarget */
	CCamera_Manager::GetInstance()->Change_CameraTarget(this, 1.0f);
	_uint iZoomLevel = CCamera_Manager::GetInstance()->Get_CurrentCamera()->Get_CurrentZoomLevel();
	CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET_2D, 1.0f, iZoomLevel - 2, RATIO_TYPE::EASE_OUT);

}

void CZetPack_Child::State_Change_Chase()
{
}

void CZetPack_Child::State_Change_PortalOut()
{
	_wstring strSectionTag = Get_Include_Section_Name();
	CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(strSectionTag, this);
	if (CPlayer::PLAYER_MODE_ZETPACK != m_pPlayer->Get_PlayerMode())
	{
		m_pPlayer->Set_Mode(CPlayer::PLAYER_MODE_ZETPACK);
	}

	// 공통처리 Father Game에서의 플레이어의 상태를 추적하는 ZetPack_Child가 대부분의 조작을 담당한다 ui 켜고 끄고까지
	CFatherGame::GetInstance()->Set_Active_FatherParts_UIs(true);
}

void CZetPack_Child::State_Change_PortalIn()
{
	_wstring strPlayerSectionTag = m_pPlayer->Get_Include_Section_Name();
	CSection_2D* pSection = static_cast<CSection_2D*>(CSection_Manager::GetInstance()->Find_Section(strPlayerSectionTag));
	if (false == pSection->Is_Platformer())
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(strPlayerSectionTag, this);

		_vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
	}
	else
	{
		/* 만약 플랫포머라면, 포탈에 들어가지않는다. */
	}
	
	// 공통처리 Father Game에서의 플레이어의 상태를 추적하는 ZetPack_Child가 대부분의 조작을 담당한다 ui 켜고 끄고까지
	CFatherGame::GetInstance()->Set_Active_FatherParts_UIs(false);
}

void CZetPack_Child::State_Change_MakeFather()
{
	// 1. 미리 지정해둔 좌표로 이동하며, 아래를 본 상태에서 Talk State로 변경할 것이다.
	if (nullptr == m_pPlayer)
		return;

	m_pPlayer->Set_BlockPlayerInput(true);
	m_pPlayer->Set_2DDirection(E_DIRECTION::UP);
}

void CZetPack_Child::Action_State(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CZetPack_Child::STATE_IDLE:
		Action_State_Idle(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_TALK:
		Action_State_Talk(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_CHASE:
		Action_State_Chase(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_PORTALOUT:
		Action_State_PortalOut(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_PORTALIN:
		Action_State_PortalIn(_fTimeDelta);
		break;
	case Client::CZetPack_Child::STATE_MAKEFATHER:
		Action_State_MakeFather(_fTimeDelta);
		break;

	default:
		break;
	}
}

void CZetPack_Child::Action_State_Idle(_float _fTimeDelta)
{
	if (false == m_isContactPlayer)
		return;

	if (COORDINATE_3D == m_pPlayer->Get_CurCoord())
	{
		m_eCurState = STATE_PORTALOUT;
		return;
	}

	CTransform* pTransform2D = Get_ControllerTransform()->Get_Transform(COORDINATE_2D);
	_vector vPos = pTransform2D->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);
	_vector vDir = vPlayerPos - vPos;
	_float fDiff = XMVectorGetX(XMVector3Length(vDir));
	vDir /= fDiff;

	/* 0. 플레이어와의 거리를 기반으로 더 갈지 말지 판별 */
	if (m_fChaseInterval < fDiff)
	{
		m_eCurState = STATE_CHASE;
		return;
	}
}

void CZetPack_Child::Action_State_Talk(_float _fTimeDelta)
{

	if (false == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
	{
		Finished_DialogueAction();
	}

}

void CZetPack_Child::Action_State_Chase(_float _fTimeDelta)
{
	if (COORDINATE_3D == m_pPlayer->Get_CurCoord())
	{
		m_eCurState = STATE_PORTALOUT;
		return;
	}

	ChaseToTarget(_fTimeDelta);
}

void CZetPack_Child::Action_State_PortalOut(_float _fTimeDelta)
{
	if (COORDINATE_2D == m_pPlayer->Get_CurCoord())
	{
		m_eCurState = STATE_PORTALIN;
		return;
	}
}

void CZetPack_Child::Action_State_PortalIn(_float _fTimeDelta)
{
	m_eCurState = STATE_CHASE;
}

void CZetPack_Child::Action_State_MakeFather(_float _fTimeDelta)
{
	ChaseToTargetPosition(m_vMakeFatherPosition, _fTimeDelta);
}

void CZetPack_Child::Update_AnimationDirection()
{
	if (nullptr == m_pPlayer)
		return;

	CTransform* pTransform2D = Get_ControllerTransform()->Get_Transform(COORDINATE_2D);
	_vector vPos = pTransform2D->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);
	_vector vDir = vPlayerPos - vPos;
	_float fDiff = XMVectorGetX(XMVector3Length(vDir));
	vDir /= fDiff;

	/* 2. 외적을 통한 2D 애니메이션 방향 판별 */
	_vector vUpVector = { 0.0f, 1.0f, 0.0f, 0.0f };
	_vector vCrossResult = XMVector3Cross(vUpVector, vDir);
	_float fDot = XMVectorGetX(XMVector3Dot(vUpVector, vDir));

#pragma region Check AnimDirection

	/* 2. 외적의 결과 + 내적의 결과로 4방향 판별*/
	if (0.0f > XMVectorGetZ(vCrossResult))
	{
		/* 우측 방향에 플레이어 존재. */
		if (0.0f < fDot)
		{
			// 윗 방향 판별
			if (0.5f < fDot)
			{
				// 위로 본다.
				m_eDirection = DIR_UP;
			}
			else
			{
				// 우측으로 본다.
				m_eDirection = DIR_RIGHT;
			}
		}
		else
		{
			// 아래 방향 판별 
			if (-0.5f * -0.5f > fDot * fDot)
			{
				// 우측으로 본다.
				m_eDirection = DIR_RIGHT;
			}
			else
			{
				// 아래로 본다.
				m_eDirection = DIR_DOWN;
			}
		}


	}
	else
	{
		/* 좌측 방향에 플레이어 존재. */
		if (0.0f < fDot)
		{
			// 윗 방향 판별
			if (0.5f < fDot)
			{
				// 위로 본다.
				m_eDirection = DIR_UP;
			}
			else
			{
				// 좌측으로 본다.
				m_eDirection = DIR_LEFT;
			}
		}
		else
		{
			// 아래 방향 판별 
			if (-0.5f * -0.5f > fDot * fDot)
			{
				// 좌측으로 본다.
				m_eDirection = DIR_LEFT;
			}
			else
			{
				// 아래로 본다.
				m_eDirection = DIR_DOWN;
			}
		}
	}

	_vector vRightVector = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
	switch (m_eDirection)
	{
	case Client::CZetPack_Child::DIR_DOWN:
		if (STATE_TALK == m_eCurState)
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::TALK_DOWN);
		else
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::IDLE_DOWN);
		break;
	case Client::CZetPack_Child::DIR_RIGHT:
		if (STATE_TALK == m_eCurState)
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::TALK_RIGHT);
		else
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::IDLE_RIGHT);
		Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRightVector));
		break;
	case Client::CZetPack_Child::DIR_UP:
		if (STATE_TALK == m_eCurState)
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::TALK_UP);
		else
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::IDLE_UP);
		break;
	case Client::CZetPack_Child::DIR_LEFT:
		if (STATE_TALK == m_eCurState)
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::TALK_RIGHT);
		else
			m_pControllerModel->Switch_Animation((_uint)ANIMINDEX::IDLE_RIGHT);
		Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRightVector));
		break;
	default:
		break;
	}
#pragma endregion // Check AnimDirection

}

void CZetPack_Child::ChaseToTarget(_float _fTimeDelta)
{
	if (nullptr == m_pPlayer)
		return;

	CTransform* pTransform2D = Get_ControllerTransform()->Get_Transform(COORDINATE_2D);
	_vector vPos = pTransform2D->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = m_pPlayer->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION);
	_vector vDir = vPlayerPos - vPos;
	_float fDiff = XMVectorGetX(XMVector3Length(vDir));
	vDir /= fDiff;

	/* 0. 플레이어와의 거리를 기반으로 더 갈지 말지 판별 */
	if (m_fChaseInterval > fDiff)
	{
		m_eCurState = STATE_IDLE;
		return;
	}
	
	/* 1. Player 방향으로의 이동. */
	pTransform2D->Go_Direction(vDir, _fTimeDelta);

	/* 2. 방향에 맞게 애니메이션 변경 */
	Update_AnimationDirection();
}

void CZetPack_Child::ChaseToTargetPosition(_float2 _vTargetPosition, _float _fTimeDelta)
{
	CTransform* pTransform2D = Get_ControllerTransform()->Get_Transform(COORDINATE_2D);
	_vector vPos = pTransform2D->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = XMVectorSetW(XMLoadFloat2(&_vTargetPosition), 1.0f);
	_vector vDir = vTargetPos - vPos;
	_float fDiff = XMVectorGetX(XMVector3Length(vDir));
	vDir /= fDiff;

	/* 1. Player 방향으로의 이동. */
	pTransform2D->Go_Direction(vDir, _fTimeDelta);

	/* 2. 방향에 맞게 애니메이션 변경 */
	Update_AnimationDirection();

	/* 0. 플레이어와의 거리를 기반으로 더 갈지 말지 판별 */
	if (3.f > fDiff)
	{
		m_eCurState = STATE_TALK;
		return;
	}


}

void CZetPack_Child::Finished_DialogueAction()
{
	switch (m_iDialogueIndex)
	{
	case 0:
	{
		/* 3. Player Input 풀기. */
		if (nullptr != m_pPlayer)
		{
			m_pPlayer->Set_BlockPlayerInput(false);
			m_pPlayer->Set_2DDirection(E_DIRECTION::RIGHT);

			/* 4. Change CameraTarget */
			CCamera_Manager::GetInstance()->Change_CameraTarget(m_pPlayer, 1.0f);

			_uint iZoomLevel = CCamera_Manager::GetInstance()->Get_CurrentCamera()->Get_CurrentZoomLevel();
			CCamera_Manager::GetInstance()->Start_Zoom(CCamera_Manager::TARGET_2D, 1.0f, iZoomLevel + 2,RATIO_TYPE::EASE_OUT);


			Make_JusinBulb("../Bin/DataFiles/JusinBulb/Chapter6_FatherGame_ZetPack_BulbPos.json");
			
			m_isContactPlayer = true;
			m_eCurState = STATE_CHASE;


		}
	}
		break;
	case 9: // 아빠 조립 
	{
		CZetPack_Father* pFather = CFatherGame::GetInstance()->Get_ZetPack_Father();
		pFather->Set_CurState(CZetPack_Father::STATE_REASSEMBLE);
		m_eCurState = STATE_CHASE;
	}
	break;
	default:
		break;
	}
	++m_iDialogueIndex;
}

HRESULT CZetPack_Child::Make_JusinBulb(_string _strJsonPath)
{

	const std::string filePathLights = _strJsonPath;
	std::ifstream inputFile(filePathLights);
	if (!inputFile.is_open()) {
		throw std::runtime_error("파일을 열 수 없습니다: " + filePathLights);
		return E_FAIL;
	}
	json jsonBulbs;
	inputFile >> jsonBulbs;

	_uint iNumBulbs = (_uint)jsonBulbs.size();
	for (_uint i = 0; i < iNumBulbs; ++i)
	{
		/* 5. 전구 생성 */
		_float3 vPos = {};
		vPos.x = jsonBulbs[i]["vPos"]["x"];
		vPos.y = jsonBulbs[i]["vPos"]["y"];
		_wstring strCurSection = Get_Include_Section_Name();
		CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
	}

	inputFile.close();



	return S_OK;
}

HRESULT CZetPack_Child::Ready_Components(ZETPACK_CHILD_DESC* _pDesc)
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


CZetPack_Child* CZetPack_Child::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CZetPack_Child* pInstance = new CZetPack_Child(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CZetPack_Child");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CZetPack_Child* CZetPack_Child::Clone(void* _pArg)
{
	CZetPack_Child* pInstance = new CZetPack_Child(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CZetPack_Child");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZetPack_Child::Free()
{
    Safe_Release(m_pPlayer);

    __super::Free();
}
