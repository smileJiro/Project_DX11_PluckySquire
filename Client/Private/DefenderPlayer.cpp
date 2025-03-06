#include "stdafx.h"
#include "DefenderPlayer.h"
#include "ModelObject.h"
#include "Controller_Transform.h"
#include "GameInstance.h"
#include "StateMachine.h"
#include "AnimEventGenerator.h"
#include "DefenderPlayerProjectile.h"
#include "Section_Manager.h"
#include "PlayerData_Manager.h"

CDefenderPlayer::CDefenderPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPlayable(_pDevice, _pContext,PLAYALBE_ID::DEFENDER)
{
}

CDefenderPlayer::CDefenderPlayer(const CDefenderPlayer& _Prototype)
	: CPlayable(_Prototype)
{
}

HRESULT CDefenderPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CDefenderPlayer::Initialize(void* _pArg)
{
	m_pSection_Manager = CSection_Manager::GetInstance();

	CDefenderPlayer::DEFENDERPLAYER_DESC* pDesc = static_cast<DEFENDERPLAYER_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;
	m_pOriginalPlayer = pDesc->pOriginalPlayer;
	Safe_AddRef(m_pOriginalPlayer);

	pDesc->iNumPartObjects = DEFENDER_PART_LAST;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 500.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720);
	pDesc->tTransform3DDesc.fSpeedPerSec = 8.f;

	pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CDefenderPlayer::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	CPlayerData_Manager::GetInstance()->Register_Player(PLAYALBE_ID::DEFENDER, this);
	return S_OK;
}

HRESULT CDefenderPlayer::Ready_PartObjects()
{

	/* Part Body */
	CModelObject::MODELOBJECT_DESC BodyDesc{};
	BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	BodyDesc.iCurLevelID = m_iCurLevelID;
	BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

	BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	BodyDesc.strModelPrototypeTag_2D = TEXT("defenderplayer");
	BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[PART_BODY] = m_pBody = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
	if (nullptr == m_PartObjects[PART_BODY])
	{
		MSG_BOX("DefenderPlayer Body Creation Failed");
		return E_FAIL;
	}
	Safe_AddRef(m_pBody);

	/* Part CrossHair*/
	CModelObject::MODELOBJECT_DESC tCrossHairDesc{};
	tCrossHairDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	tCrossHairDesc.iCurLevelID = m_iCurLevelID;
	tCrossHairDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

	tCrossHairDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tCrossHairDesc.strModelPrototypeTag_2D = TEXT("DefenderCrosshair_02_Sprite");
	tCrossHairDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tCrossHairDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tCrossHairDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tCrossHairDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, m_fCrossHairYOffset, 0.0f);
	tCrossHairDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[DEFENDER_PART_CROSSHAIR] = m_pCrossHair = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tCrossHairDesc));
	if (nullptr == m_PartObjects[DEFENDER_PART_CROSSHAIR])
	{
		MSG_BOX("DefenderPlayer Body Creation Failed");
		return E_FAIL;
	}
	Safe_AddRef(m_pCrossHair);
	m_pCrossHair->Set_Active(false);
	return S_OK;
}
HRESULT CDefenderPlayer::Ready_Components()
{

	m_p2DColliderComs.resize(1);
	/*BODY 2D Collider */
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 40.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius * 0.5f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = false;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::PLAYER;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;
	m_pBody2DColliderCom = m_p2DColliderComs[0];
	Safe_AddRef(m_pBody2DColliderCom);

	return S_OK;
}

void CDefenderPlayer::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CDefenderPlayer::Update(_float _fTimeDelta)
{
	if(false == Is_PlayerInputBlocked())
		Key_Input(_fTimeDelta);

	m_pCrossHair->Set_Position(_vector{ m_fCrossHairPosition, m_fCrossHairYOffset, 0.f });
	Set_Direction(m_fCrossHairPosition >= 0.f ? T_DIRECTION::RIGHT : T_DIRECTION::LEFT);

	m_fShootTimeAcc += _fTimeDelta;
	__super::Update(_fTimeDelta);
}

void CDefenderPlayer::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CDefenderPlayer::Render()
{
#ifdef _DEBUG
	if (m_p2DColliderComs[0]->Is_Active())
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG
	return __super::Render();
}

void CDefenderPlayer::Set_Direction(T_DIRECTION _eTDir)
{
	m_eTDirection = _eTDir;
	CController_Transform* pBopdyTransform = m_pBody->Get_ControllerTransform();
	_vector vRight = pBopdyTransform->Get_State(CTransform::STATE_RIGHT);
	switch (m_eTDirection)
	{
	case Client::T_DIRECTION::LEFT:
		pBopdyTransform->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
		break;
	case Client::T_DIRECTION::RIGHT:
		pBopdyTransform->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
		break;
	case Client::T_DIRECTION::NONE:
	case Client::T_DIRECTION::T_DIR_LAST:
	default:
		break;
	}

}

void CDefenderPlayer::Key_Input(_float _fTimeDelta)
{
	_vector vMoveDir = {};
	if (KEY_PRESSING(KEY::W))
		vMoveDir += _vector{ 0.f, 1.f, 0.f,0.f };
	if (KEY_PRESSING(KEY::A))
		vMoveDir+= _vector{ -1.f, 0.f, 0.f,0.f };
	if (KEY_PRESSING(KEY::S))
		vMoveDir+= _vector{ 0.f, -1.f, 0.f,0.f };
	if (KEY_PRESSING(KEY::D))
		vMoveDir+= _vector{ 1.f, 0.f, 0.f,0.f };
	
	vMoveDir = XMVector2Normalize(vMoveDir);
	Move(vMoveDir * m_fMoveSpeed, _fTimeDelta);

	m_fCrossHairPosition += (_float)m_pGameInstance->GetDIMouseMove(MOUSE_AXIS::X) * _fTimeDelta * m_fCrossHairMoveSpeed;
	m_fCrossHairPosition = max(-m_fCrossHairMoveRange, min(m_fCrossHairMoveRange, m_fCrossHairPosition));

	if (MOUSE_PRESSING(MOUSE_KEY::LB))
	{
		if (m_fShootTimeAcc >= m_fShootDelay)
		{
			Shoot();
			m_fShootTimeAcc = 0.f;
		}
	}
}

void CDefenderPlayer::Start_Game()
{
	m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_TRANSFORM_IN);

	CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
	CPlayer* pNormalPlayer = pPDM->Get_NormalPlayer_Ptr();

	pNormalPlayer->Set_Active(false);
	Set_Active(true);
	Set_BlockPlayerInput(true);
}



void CDefenderPlayer::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if ((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_TRANSFORM_IN == iAnimIdx)
	{
		m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_IDLE);
		m_pCrossHair->Set_Active(true);
		CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
		pPDM->Set_CurrentPlayer(PLAYALBE_ID::DEFENDER);

	}
	else if ((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_SHOT == iAnimIdx)
	{
		m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_IDLE);
	}
}

void CDefenderPlayer::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CDefenderPlayer::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CDefenderPlayer::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CDefenderPlayer::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CDefenderPlayer::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CDefenderPlayer::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CDefenderPlayer::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPlayer::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPlayer::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPlayer::Shoot()
{
	m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_SHOT);

	_vector v2DPosition = Get_FinalPosition();
	v2DPosition += m_fBarrelOffset;
	CDefenderPlayerProjectile::DEFENDERPLAYER_PROJECTILE_DESC tProjectileDesc{};
	tProjectileDesc.eStartCoord = COORDINATE_2D;
	tProjectileDesc.tTransform2DDesc.vInitialPosition = _float3(XMVectorGetX(v2DPosition), XMVectorGetY(v2DPosition), 0.0f);
	tProjectileDesc.iCurLevelID = m_iCurLevelID;
	tProjectileDesc._eTDirection = m_eTDirection;
	CDefenderPlayerProjectile* pProjectile = static_cast<CDefenderPlayerProjectile*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_DefenderPlayerProjectile"), &tProjectileDesc));
	m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Layer_PlayerSubs"), pProjectile);
	m_pSection_Manager->Add_GameObject_ToSectionLayer(m_strSectionName, pProjectile, SECTION_2D_PLAYMAP_OBJECT);
}

CDefenderPlayer* CDefenderPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDefenderPlayer* pInstance = new CDefenderPlayer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefenderPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefenderPlayer::Clone(void* _pArg)
{
	CDefenderPlayer* pInstance = new CDefenderPlayer(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CDefenderPlayer::Free()
{
	Safe_Release(m_pBody);
	Safe_Release(m_pCrossHair);
	Safe_Release(m_pBody2DColliderCom);
	Safe_Release(m_pOriginalPlayer);
	__super::Free();
}
