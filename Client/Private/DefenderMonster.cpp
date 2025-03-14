#include "stdafx.h"
#include "DefenderMonster.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Section_Manager.h"
#include "Effect2D_Manager.h"
#include "ModelObject.h"
#include "2DModel.h"

CDefenderMonster::CDefenderMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCharacter(_pDevice, _pContext)
{
}

CDefenderMonster::CDefenderMonster(const CDefenderMonster& _Prototype)
	:CCharacter(_Prototype)
{
}

HRESULT CDefenderMonster::Initialize(void* _pArg)
{
	DEFENDER_MONSTER_DESC* pDesc = static_cast<DEFENDER_MONSTER_DESC*>(_pArg);
	m_eTDirection = pDesc->eTDirection;

	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;


	pDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	pDesc->eActorType = ACTOR_TYPE::LAST;
	pDesc->pActorDesc = nullptr;


	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	Set_Direction(m_eTDirection);


	return S_OK;
}

void CDefenderMonster::Update(_float _fTimeDelta)
{
	if (false == m_bSpawned  && m_pTeleportFX
		&& static_cast<C2DModel*>(m_pTeleportFX->Get_Model(COORDINATE_2D))->Get_Animation(m_iFXTeleportInIdx)->Get_Progress() >= 0.5f)
	{
		Safe_Release(m_pTeleportFX);
		m_pTeleportFX = nullptr;
		m_bSpawned = true;
		m_PartObjects[PART_BODY]->Set_Active(true);

		On_Spawned();
	}
	m_fLifeTimeAcc += _fTimeDelta;
	if (m_fLifeTimeAcc >= m_fLifeTime)
	{
		Event_DeleteObject(this);
		m_fLifeTimeAcc = 0.f;
		On_LifeTimeOut();
	}
	__super::Update(_fTimeDelta);
}



void CDefenderMonster::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if(OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID())
	{
		Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), m_tStat.iDamg, _vector{ 0.f,0.f,0.f });
	}
}

void CDefenderMonster::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderMonster::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderMonster::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
	m_tStat.iHP -= _iDamg;
	COORDINATE eCoord = Get_CurCoord();
	if (m_tStat.iHP <= 0)
	{
		Event_DeleteObject(this);
		On_Explode();
		m_tStat.iHP = m_tStat.iMaxHP;
		return;
	}
}

void CDefenderMonster::On_Explode()
{
}

void CDefenderMonster::On_Spawned()
{
	m_bSpawned = true;
	m_PartObjects[PART_BODY]->Set_Active(true);
	m_pBodyCollider->Set_Active(true);
}

void CDefenderMonster::On_Teleport()
{
	m_bSpawned = false;
	m_pBodyCollider->Set_Active(false);
	m_PartObjects[PART_BODY]->Set_Active(false);
	if (m_pTeleportFX)
		Safe_Release(m_pTeleportFX);
	_matrix mat = Get_FinalWorldMatrix();
	mat.r[3] = Get_ScrolledPosition(mat.r[3]);
	CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("DefTeleport"), Get_Include_Section_Name()
		, mat, 0.f
		, m_iFXTeleportInIdx, false, 0.f, SECTION_2D_PLAYMAP_EFFECT, (CGameObject**)&m_pTeleportFX);
	Safe_AddRef(m_pTeleportFX);
	static_cast<C2DModel*>(m_pTeleportFX->Get_Model(COORDINATE_2D))->Get_Animation(m_iFXTeleportInIdx)->Reset();
}

void CDefenderMonster::On_LifeTimeOut()
{
	_matrix mat = Get_FinalWorldMatrix();
	mat.r[3] = Get_ScrolledPosition(mat.r[3]);
	CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("DefTeleport"), Get_Include_Section_Name()
		, mat, 0.f
		, m_iFXTeleportOutIdx, false, 0.f, SECTION_2D_PLAYMAP_EFFECT);
}

void CDefenderMonster::Set_Direction(T_DIRECTION _eDirection)
{
	m_eTDirection = _eDirection;
	switch (_eDirection)
	{
	case Client::T_DIRECTION::LEFT:
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
		break;
	}
	case Client::T_DIRECTION::RIGHT:
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
		break;
	}
	default:
		break;
	}
}

CDefenderMonster* CDefenderMonster::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDefenderMonster* pInstance = new CDefenderMonster(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DefenderMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefenderMonster::Clone(void* _pArg)
{
	CDefenderMonster* pInstance = new CDefenderMonster(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DefenderMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefenderMonster::Free()
{
	Safe_Release(m_pTeleportFX);
	__super::Free();
}
