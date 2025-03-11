#include "stdafx.h"
#include "DefenderCapsule.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Effect2D_Manager.h"
#include "3DModel.h"
#include "Section_Manager.h"
#include "ScrollModelObject.h"

CDefenderCapsule::CDefenderCapsule(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CDefenderMonster(_pDevice, _pContext)
{
}

CDefenderCapsule::CDefenderCapsule(const CDefenderCapsule& _Prototype)
	: CDefenderMonster(_Prototype)
{
}

HRESULT CDefenderCapsule::Initialize(void* _pArg)
{
	m_fLifeTime = 9999.f;
	DEFENDER_CAPSULE_DESC* pDesc = static_cast<DEFENDER_CAPSULE_DESC*>(_pArg);
	m_iPersonCount = pDesc->iPersonCount;
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;
	pDesc->iNumPartObjects = 2;

	pDesc->_tStat.iHP = 20;
	pDesc->_tStat.iMaxHP = 20;
	pDesc->_tStat.iDamg = 0;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 36.f;
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

HRESULT CDefenderCapsule::Ready_PartObjects()
{
	CModelObject::MODELOBJECT_DESC tModelDesc = {};
	tModelDesc.eStartCoord = COORDINATE_2D;
	tModelDesc.iCurLevelID = m_iCurLevelID;
	tModelDesc.isCoordChangeEnable = false;
	tModelDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tModelDesc.strModelPrototypeTag_2D = TEXT("PersonCapsule");
	tModelDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tModelDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[DEFENDER_CAPSULE_PART_BODY] = m_pBody = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	if (nullptr == m_PartObjects[DEFENDER_CAPSULE_PART_BODY])
		return E_FAIL;
	Safe_AddRef(m_pBody);

	m_pBody->Register_OnAnimEndCallBack(bind(&CDefenderCapsule::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	m_pBody->Switch_Animation(SPHERE_APPEAR_FAST);

	CModelObject::MODELOBJECT_DESC tModelDesc2 = {};
	tModelDesc2.eStartCoord = COORDINATE_2D;
	tModelDesc2.iCurLevelID = m_iCurLevelID;
	tModelDesc2.isCoordChangeEnable = false;
	tModelDesc2.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tModelDesc2.strModelPrototypeTag_2D = TEXT("PersonBlinkerUI");
	tModelDesc2.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tModelDesc2.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tModelDesc2.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc2.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc2.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[DEFENDER_CAPSULE_PART_UI] = m_pUI = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc2));
	if (nullptr == m_PartObjects[DEFENDER_CAPSULE_PART_UI])
		return E_FAIL;
	Safe_AddRef(m_pUI);


	return S_OK;
}
void CDefenderCapsule::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

HRESULT CDefenderCapsule::Render()
{
#ifdef _DEBUG
	if (m_pBodyCollider->Is_Active())
		m_pBodyCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG
	return S_OK;
}


void CDefenderCapsule::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderCapsule::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
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

void CDefenderCapsule::On_Explode()
{
	m_pBody->Switch_Animation(SPHERE_BREAK);

	for (_uint i = 0; i < m_iPersonCount; i++)
	{
		CScrollModelObject::SCROLLMODELOBJ_DESC tDesc = {};
		tDesc.iCurLevelID = m_iCurLevelID;
		_vector vRandomOffset = { (_float)rand() / RAND_MAX *20.f, (_float)rand() / RAND_MAX * 20.f };
		XMStoreFloat3(&tDesc.tTransform2DDesc.vInitialPosition, Get_FinalPosition()+ vRandomOffset);
		CGameObject* pPerson = nullptr;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_DefenderPerson"), m_iCurLevelID, TEXT("Layer_Defender"), (CGameObject**)&pPerson, &tDesc)))
			return;
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, pPerson, SECTION_2D_PLAYMAP_OBJECT);
	}

}

void CDefenderCapsule::On_Spawned()
{
}

void CDefenderCapsule::On_Teleport()
{
}

void CDefenderCapsule::On_LifeTimeOut()
{
}

void CDefenderCapsule::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	switch (iAnimIdx)
	{
	case SPHERE_APPEAR_FAST:
		m_pBody->Switch_Animation(SPHERE_LOOP);
		break;

	case SPHERE_BREAK:
		Event_DeleteObject(this);
		break;
	}
}




CDefenderCapsule* CDefenderCapsule::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDefenderCapsule* pInstance = new CDefenderCapsule(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DefenderCapsule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefenderCapsule::Clone(void* _pArg)
{
	CDefenderCapsule* pInstance = new CDefenderCapsule(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DefenderCapsule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefenderCapsule::Free()
{
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pBody);
	Safe_Release(m_pUI);
	__super::Free();
}
