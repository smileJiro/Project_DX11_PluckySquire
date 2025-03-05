#include "stdafx.h"
#include "DefenderPlayer.h"
#include "ModelObject.h"
#include "Controller_Transform.h"
#include "GameInstance.h"
#include "StateMachine.h"
#include "AnimEventGenerator.h"
#include "DefenderPlayerProjectile.h"
#include "Section_Manager.h"

CDefenderPlayer::CDefenderPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CDefenderPlayer::CDefenderPlayer(const CDefenderPlayer& _Prototype)
	: CCharacter(_Prototype)
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

	CDefenderPlayer::CHARACTER_DESC* pDesc = static_cast<CDefenderPlayer::CHARACTER_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;

	pDesc->iNumPartObjects = 1;
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

	return S_OK;
}

HRESULT CDefenderPlayer::Ready_PartObjects()
{

	/* Part Body */
	CModelObject::MODELOBJECT_DESC BodyDesc{};
	BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	BodyDesc.iCurLevelID = m_iCurLevelID;
	BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

	BodyDesc.iModelPrototypeLevelID_2D = LEVEL_CHAPTER_6;
	BodyDesc.strModelPrototypeTag_2D = TEXT("defenderplayer");
	BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[PART_BODY] = m_pBody =static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerBody"), &BodyDesc));
	if (nullptr == m_PartObjects[PART_BODY])
	{
		MSG_BOX("DefenderPlayer Body Creation Failed");
		return E_FAIL;
	}
	Safe_AddRef(m_pBody);
	return S_OK;
}
HRESULT CDefenderPlayer::Ready_Components()
{

	m_p2DColliderComs.resize(1);
	/*BODY 2D Collider */
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 20.f;
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
	Key_Input(_fTimeDelta);

	m_pCrossHair->Set_Position(_vector{ m_fCrossHairPosition, 0.f, 0.f });
	m_fShootTimeAcc += _fTimeDelta;
	__super::Update(_fTimeDelta);
}

void CDefenderPlayer::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CDefenderPlayer::Render()
{
	return __super::Render();
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

	CDefenderPlayerProjectile::DEFENDERPLAYER_PROJECTILE_DESC tProjectileDesc{};
	tProjectileDesc.tTransform2DDesc.vInitialPosition = _float3(XMVectorGetX(v2DPosition), XMVectorGetY(v2DPosition), 0.0f);
	tProjectileDesc.iCurLevelID = m_iCurLevelID;
	CDefenderPlayerProjectile* pProjectile = static_cast<CDefenderPlayerProjectile*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_DefenderPlayerProjectile"), &tProjectileDesc));
	m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Layer_PlayerSubs"), pProjectile);
	m_pSection_Manager->Add_GameObject_ToSectionLayer(m_pSection_Manager->Get_Cur_Section_Key(), pProjectile, SECTION_2D_PLAYMAP_OBJECT);
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
	Safe_Release(m_pBody2DColliderCom);
	__super::Free();
}
