#include "stdafx.h"
#include "PlayerItem.h"

#include "GameInstance.h"

CPlayerItem::CPlayerItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CTriggerObject(_pDevice, _pContext)
{
}

CPlayerItem::CPlayerItem(const CPlayerItem& _Prototype)
	:CTriggerObject(_Prototype)
{
}

HRESULT CPlayerItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerItem::Initialize(void* _pArg)
{
	CPlayerItem::PLAYERITEM_DESC* pDesc = static_cast<CPlayerItem::PLAYERITEM_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

	pDesc->iActorType = (_uint)ACTOR_TYPE::DYNAMIC;
	pDesc->isTrigger = false;
	pDesc->eShapeType = (SHAPE_TYPE)SHAPE_TYPE::SPHERE;
	pDesc->fRadius = .5f;

	pDesc->iFillterMyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	pDesc->iFillterOtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;

	pDesc->iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
	m_szModelTag = pDesc->szModelTag;
	pDesc->vLocalPosOffset = { 0.f, 0.f, 0.f };
	pDesc->FreezeRotation[0] = true;
	pDesc->FreezeRotation[1] = false;
	pDesc->FreezeRotation[2] = false;

	if (FAILED(__super::Initialize(pDesc))) {
		MSG_BOX("PlayerItem Initialize Falied");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	// 충돌 감지용 구 (트리거)
	SHAPE_DATA ShapeData;

	ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	//ShapeData.iShapeUse = SHAPE_TRIGER;
	ShapeData.isTrigger = true;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.f, 0.f, 0.f)); //여기임
	SHAPE_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 0.5f;
	ShapeData.pShapeDesc = &SphereDesc;

	m_pActorCom->Add_Shape(ShapeData);

	this->Resister_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
		Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
		});

	m_pControllerTransform->Set_Scale(4.f, 4.f, 4.f);
	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f));
	//m_pActorCom->Set_AngularVelocity({ 0.f, 0.f, XMConvertToRadians(45.f) });

	return S_OK;
}

void CPlayerItem::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CPlayerItem::Update(_float _fTimeDelta)
{
	PxVec3 AnglularVelocity = static_cast<PxRigidDynamic*>(m_pActorCom->Get_RigidActor())->getAngularVelocity();

	m_pActorCom->Set_AngularVelocity({ 0.f, 1.f, 0.f });
	AnglularVelocity = static_cast<PxRigidDynamic*>(m_pActorCom->Get_RigidActor())->getAngularVelocity();
	
	if (static_cast<PxRigidDynamic*>(m_pActorCom->Get_RigidActor())->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC) {
		printf("Actor is Kinematic! AngularVelocity will not be applied.\n");
	}
	
	__super::Update(_fTimeDelta);
}

void CPlayerItem::Late_Update(_float _fTimeDelta)
{
	Action_Mode(_fTimeDelta);

	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_NONBLEND, this);

	__super::Late_Update(_fTimeDelta);
}

HRESULT CPlayerItem::Render()
{
	//m_pShaderCom->Begin((_uint)PASS_VTXMESH::DEFAULT);

	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;

	m_pModelCom->Render(m_pShaderCom, (_uint)PASS_VTXMESH::DEFAULT);

	return S_OK;
}

HRESULT CPlayerItem::Ready_Components(PLAYERITEM_DESC* _pArg)
{
	// Shader
	if (FAILED(Add_Component(m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Model
	if (FAILED(Add_Component(_pArg->iCurLevelID, m_szModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

void CPlayerItem::Action_Mode(_float _fTimeDelta)
{
	switch (m_iItemMode) {
	case DEFAULT:
		break;
	case GETTING:
		Action_Getting(_fTimeDelta);
		break;
	case DISAPPEAR:
		Action_Disappear(_fTimeDelta);
		break;
	}
}

void CPlayerItem::Action_Getting(_float _fTimeDelta)
{
}

void CPlayerItem::Action_Disappear(_float _fTimeDelta)
{
}

HRESULT CPlayerItem::Bind_ShaderResources_WVP()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pControllerTransform->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CTriggerObject* CPlayerItem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayerItem* pInstance = new CPlayerItem(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerItem::Clone(void* _pArg)
{
	CPlayerItem* pInstance = new CPlayerItem(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerItem::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
