#include "stdafx.h"
#include "DefenderSmShip.h"
#include "ModelObject.h"
#include "GameInstance.h"

CDefenderSmShip::CDefenderSmShip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CDefenderMonster(_pDevice, _pContext)
{
}

CDefenderSmShip::CDefenderSmShip(const CDefenderSmShip& _Prototype)
	:CDefenderMonster(_Prototype)
{
}


HRESULT CDefenderSmShip::Initialize(void* _pArg)
{
	DEFENDER_MONSTER_DESC* pDesc = static_cast<DEFENDER_MONSTER_DESC*>(_pArg);
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;
	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = m_fMoveSpeed;
	pDesc->iNumPartObjects = 1;

	pDesc->_tStat.iHP = 1;
	pDesc->_tStat.iMaxHP = 1;
	pDesc->_tStat.iDamg = 1;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;


	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 10.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = false;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
	CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_pBodyCollider), &CircleDesc)))
		return E_FAIL;

	m_p2DColliderComs.push_back(m_pBodyCollider);
	Safe_AddRef(m_pBodyCollider);

	return S_OK;
}

void CDefenderSmShip::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	Move(XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_RIGHT)) * m_fMoveSpeed , _fTimeDelta);
}

void CDefenderSmShip::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::PLAYER & _pOtherCollider->Get_CollisionGroupID()
		&& (_uint)COLLIDER2D_USE::COLLIDER2D_BODY == _pOtherCollider->Get_ColliderUse())
	{
		Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), m_tStat.iDamg, _vector{ 0.f,0.f,0.f });
		Event_DeleteObject(this);
	}
}

void CDefenderSmShip::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	__super::On_Collision2D_Stay(_pMyCollider, _pOtherCollider, _pOtherObject);
}

void CDefenderSmShip::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	__super::On_Collision2D_Exit(_pMyCollider, _pOtherCollider, _pOtherObject);
}

void CDefenderSmShip::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
	__super::On_Hit(_pHitter, _iDamg, _vForce);

}

HRESULT CDefenderSmShip::Ready_PartObjects()
{
	CModelObject::MODELOBJECT_DESC tModelDesc = {};
	tModelDesc.eStartCoord = COORDINATE_2D;
	tModelDesc.iCurLevelID = m_iCurLevelID;
	tModelDesc.isCoordChangeEnable = false;
	tModelDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tModelDesc.strModelPrototypeTag_2D = TEXT("SmShip");
	tModelDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tModelDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[PART_BODY] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	if (nullptr == m_PartObjects[PART_BODY])
		return E_FAIL;


	return S_OK;
}

CDefenderSmShip* CDefenderSmShip::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDefenderSmShip* pInstance = new CDefenderSmShip(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DefenderSmShip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefenderSmShip::Clone(void* _pArg)
{
	CDefenderSmShip* pInstance = new CDefenderSmShip(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DefenderSmShip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefenderSmShip::Free()
{
	Safe_Release(m_pBodyCollider);
	__super::Free();
}
