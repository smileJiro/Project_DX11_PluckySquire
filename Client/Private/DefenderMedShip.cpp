#include "stdafx.h"
#include "DefenderMedShip.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Collider_Circle.h"
#include "Section_Manager.h"

CDefenderMedShip::CDefenderMedShip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CDefenderMonster(_pDevice, _pContext)
{
}

CDefenderMedShip::CDefenderMedShip(const CDefenderMedShip& _Prototype)
	: CDefenderMonster(_Prototype)
	, m_bUpSide ( _Prototype.m_bUpSide)
{
}

HRESULT CDefenderMedShip::Initialize_Prototype(_bool _bUpSide)
{
	m_bUpSide = _bUpSide;
	return S_OK;
}

HRESULT CDefenderMedShip::Initialize(void* _pArg)
{

	DEFENDER_MONSTER_DESC* pDesc = static_cast<DEFENDER_MONSTER_DESC*>(_pArg);
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;
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


HRESULT CDefenderMedShip::Ready_PartObjects()
{
	CModelObject::MODELOBJECT_DESC tModelDesc = {};
	tModelDesc.iObjectGroupID = OBJECT_GROUP::MONSTER;
	tModelDesc.iCurLevelID = m_iCurLevelID;
	tModelDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tModelDesc.eStartCoord = COORDINATE_2D;
	tModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc.isCoordChangeEnable = false;
	tModelDesc.strModelPrototypeTag_2D = TEXT("MedShip");
	tModelDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tModelDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tModelDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[0] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	if (nullptr == m_PartObjects[0])
		return E_FAIL;	return S_OK;
}

void CDefenderMedShip::Update(_float _fTimeDelta)
{
	m_fTimeAcc += _fTimeDelta;
	Move(XMVector3Normalize(m_pControllerTransform->Get_State(CTransform::STATE_RIGHT)) * m_fMoveSpeed ,_fTimeDelta);
	_vector vPosition = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	if (m_bUpSide)
		vPosition = XMVectorSetY(vPosition, m_fVerticalMoveRange * sinf(m_fTimeAcc * m_fVerticalMoveSpeed));
	else
		vPosition = XMVectorSetY(vPosition, m_fVerticalMoveRange * -sinf(m_fTimeAcc * m_fVerticalMoveSpeed));
	Set_Position(vPosition);


	__super::Update(_fTimeDelta);
	if (false == m_PartObjects[0]->Is_Active() && false == m_PartObjects[1]->Is_Active())
		Event_DeleteObject(this);

}

HRESULT CDefenderMedShip::Render()
{
#ifdef _DEBUG
	if (m_pBodyCollider->Is_Active())
		m_pBodyCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));


#endif // _DEBUG
	return __super::Render();
}

void CDefenderMedShip::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::PLAYER == _pOtherCollider->Get_CollisionGroupID()
		&& (_uint)COLLIDER2D_USE::COLLIDER2D_BODY == _pOtherCollider->Get_ColliderUse())
	{
		Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), m_tStat.iDamg, _vector{ 0.f, 0.f, 0.f });
		Set_Active(false);
	}
}

void CDefenderMedShip::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderMedShip::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderMedShip::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
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


CDefenderMedShip* CDefenderMedShip::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _bool _bUpSide)
{
	CDefenderMedShip* pInstance = new CDefenderMedShip(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_bUpSide)))
	{
		MSG_BOX("Failed to Created : DefenderMedShip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefenderMedShip::Clone(void* _pArg)
{
	CDefenderMedShip* pInstance = new CDefenderMedShip(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DefenderMedShip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefenderMedShip::Free()
{
	Safe_Release(m_pBodyCollider);
	__super::Free();
}
