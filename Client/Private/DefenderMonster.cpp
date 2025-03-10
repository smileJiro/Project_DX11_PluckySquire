#include "stdafx.h"
#include "DefenderMonster.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Section_Manager.h"

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
	m_fLifeTimeAcc += _fTimeDelta;
	if (m_fLifeTimeAcc >= m_fLifeTime)
	{
		Event_DeleteObject(this);
		m_fLifeTimeAcc = 0.f;
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
		m_tStat.iHP = m_tStat.iMaxHP;
		return;
	}
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
	__super::Free();
}
