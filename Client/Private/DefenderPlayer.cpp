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
#include "Pooling_Manager.h"
#include "Minigame_Defender.h"
#include "DefenderPerson.h"

CDefenderPlayer::CDefenderPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPlayable(_pDevice, _pContext,PLAYABLE_ID::DEFENDER)
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
	m_pPoolMgr = CPooling_Manager::GetInstance();
	CDefenderPlayer::DEFENDERPLAYER_DESC* pDesc = static_cast<DEFENDERPLAYER_DESC*>(_pArg);

	m_pMinigame = pDesc->pMinigame;
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
	CPlayerData_Manager::GetInstance()->Register_Player(PLAYABLE_ID::DEFENDER, this);



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
HRESULT CDefenderPlayer::Ready_BulletPool()
{
	Pooling_DESC Pooling_Desc;
	Pooling_Desc.iPrototypeLevelID = m_iCurLevelID;
	Pooling_Desc.strLayerTag = TEXT("Layer_PlayerProjectiles");
	Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_DefenderPlayerProjectile");
	Pooling_Desc.eSection2DRenderGroup = SECTION_2D_PLAYMAP_OBJECT;

	CDefenderPlayerProjectile::DEFENDERPLAYER_PROJECTILE_DESC* pBulletDesc = new CDefenderPlayerProjectile::DEFENDERPLAYER_PROJECTILE_DESC;
	pBulletDesc->iCurLevelID = m_iCurLevelID;
	pBulletDesc->vSectionSize = m_pSection_Manager->Get_Section_RenderTarget_Size(m_strSectionName);

	CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Projectile_DefenderPlayerBullet"), Pooling_Desc, pBulletDesc);

	XMStoreFloat4(&m_vLeftShootQuaternion, XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(180.f)));
	XMStoreFloat4(& m_vRightShootQuaternion, XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f));

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
	CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
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

void CDefenderPlayer::Start_Transform()
{

	if (FAILED(Ready_BulletPool()))
		return ;
	m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_TRANSFORM_IN);
	Set_BlockPlayerInput(true);
}



void CDefenderPlayer::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if ((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_TRANSFORM_IN == iAnimIdx)
	{
		m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_IDLE);
		m_pCrossHair->Set_Active(true);
		CPlayerData_Manager* pPDM = CPlayerData_Manager::GetInstance();
		pPDM->Set_CurrentPlayer(PLAYABLE_ID::DEFENDER);
		Set_BlockPlayerInput(false);
		m_pMinigame->Start_Gamde();
	}
	else if ((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_SHOT == iAnimIdx)
	{
		m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_IDLE);
	}
	else if ((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_DEATH == iAnimIdx)
	{
		m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_IDLE);
		m_pMinigame->Restart_Game();
	}
}

void CDefenderPlayer::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

	if (OBJECT_GROUP::GIMMICK_OBJECT & _pOtherCollider->Get_CollisionGroupID()
		&& (_uint)COLLIDER2D_USE::COLLIDER2D_BODY == _pOtherCollider->Get_ColliderUse())
	{
		CDefenderPerson* pPerson = dynamic_cast<CDefenderPerson*>(_pOtherObject);
		if (nullptr == pPerson)
			return;
	
		for (auto& pFollower : m_Followers)
			if (pFollower == pPerson)
				return;

		CGameObject* pFollowObj = nullptr;
		if (m_Followers.size() > 0)
			pFollowObj = m_Followers.back();
		else
			pFollowObj = this;

		pPerson->Set_FollowObject(pFollowObj);
		m_Followers.push_back(pPerson);
		Safe_AddRef(pPerson);
	}

}

void CDefenderPlayer::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPlayer::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPlayer::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
	m_tStat.iHP -= _iDamg;

	if (m_tStat.iHP <= 0)
	{
		m_tStat.iHP = 0;
		Set_BlockPlayerInput(true);
		m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_DEATH);
		return;
	}

}

void CDefenderPlayer::Shoot()
{
	m_pBody->Switch_Animation((_uint)ANIM_STATE_CYBERJOT2D::CYBER2D_SHOT);

	_float3 v2DPosition;
	XMStoreFloat3(&v2DPosition, Get_FinalPosition() + m_fBarrelOffset);

	m_pPoolMgr->Create_Object(TEXT("Pooling_Projectile_DefenderPlayerBullet"), 
		COORDINATE_2D, 
		&v2DPosition,
		T_DIRECTION::RIGHT == m_eTDirection ? &m_vRightShootQuaternion : &m_vLeftShootQuaternion,
		(_float3*)nullptr,
		&m_strSectionName);

}

void CDefenderPlayer::Remove_Follower(CDefenderPerson* _pPerson)
{
	m_Followers.remove(_pPerson);
	Safe_Release(_pPerson);
	CGameObject* pFollowObj = this;
	for (auto& pFollower : m_Followers)
	{
		pFollower->Set_FollowObject(pFollowObj);
		pFollowObj = pFollower;
	}
}

void CDefenderPlayer::Recover()
{
	m_tStat.iHP = m_tStat.iMaxHP;
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
	for (auto& pFollower : m_Followers)
	{
		Safe_Release(pFollower);
	}
	m_Followers.clear();
	__super::Free();
}

HRESULT CDefenderPlayer::Cleanup_DeadReferences()
{
	__super::Cleanup_DeadReferences();
	for (auto& pFollower : m_Followers)
	{
		if (pFollower->Is_Dead())
		{
			m_Followers.remove(pFollower);
			Safe_Release(pFollower);
		}
	}
	return S_OK;
}
