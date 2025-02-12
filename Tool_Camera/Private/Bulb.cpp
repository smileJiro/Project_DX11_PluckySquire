#include "stdafx.h"
#include "Bulb.h"

#include "GameInstance.h"

CBulb::CBulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CTriggerObject(_pDevice, _pContext)
{
}

CBulb::CBulb(const CBulb& _Prototype)
	: CTriggerObject(_Prototype)
{
}

HRESULT CBulb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBulb::Initialize(void* _pArg)
{
	CBulb::BULB_DESC* pDesc = static_cast<CBulb::BULB_DESC*>(_pArg);
	
	pDesc->isCoordChangeEnable = false;

	if (COORDINATE_3D == pDesc->eStartCoord)
		Initialize_3D_Object(pDesc);

	if (FAILED(__super::Initialize(pDesc))) {
		MSG_BOX("Bulb Initialize Falied");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (COORDINATE_3D == pDesc->eStartCoord)
		Add_Shape();

	// Event_Get_Bulb 만들기
	//this->Resister_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
	//	Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
	//	});
	m_pActorCom->Get_RigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	return S_OK;
}

void CBulb::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CBulb::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CBulb::Late_Update(_float _fTimeDelta)
{
	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_GEOMETRY, this);

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBulb::Render()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;

	m_pModelCom->Render(m_pShaderCom, (_uint)PASS_VTXMESH::DEFAULT);

	return __super::Render();
}

HRESULT CBulb::Initialize_3D_Object(BULB_DESC* _pDesc)
{
	_pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
	_pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

	_pDesc->iActorType = (_uint)ACTOR_TYPE::DYNAMIC;
	_pDesc->isTrigger = false;
	_pDesc->eShapeType = (SHAPE_TYPE)SHAPE_TYPE::SPHERE;
	_pDesc->fRadius = .5f;

	_pDesc->iFillterMyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	_pDesc->iFillterOtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;

	_pDesc->iTriggerType = (_uint)TRIGGER_TYPE::DEFAULT_TRIGGER;

	_pDesc->vLocalPosOffset = { 0.f, 0.f, 0.f };
	_pDesc->FreezeRotation[0] = true;
	_pDesc->FreezeRotation[1] = true;
	_pDesc->FreezeRotation[2] = true;

	return S_OK;
}

HRESULT CBulb::Initialize_2D_Object(BULB_DESC* _pDesc)
{
	return S_OK;
}

void CBulb::Add_Shape()
{
	// 충돌 감지용 구 (트리거)
	SHAPE_DATA ShapeData;

	ShapeData.eShapeType = SHAPE_TYPE::SPHERE;
	//ShapeData.iShapeUse = SHAPE_TRIGER;
	ShapeData.isTrigger = true;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.f, 0.f, 0.f)); //여기임
	SHAPE_SPHERE_DESC SphereDesc = {};
	SphereDesc.fRadius = 1.f;
	ShapeData.pShapeDesc = &SphereDesc;

	m_pActorCom->Add_Shape(ShapeData);
}

HRESULT CBulb::Ready_Components(BULB_DESC* _pArg)
{
	// Shader
	if (FAILED(Add_Component(m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Model
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("LightbulbPickup_01"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBulb::Bind_ShaderResources_WVP()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pControllerTransform->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	_float4x4 ViewMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMat = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMat)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMat)))
		return E_FAIL;

	return S_OK;
}

CBulb* CBulb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBulb* pInstance = new CBulb(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBulb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBulb::Clone(void* _pArg)
{
	CBulb* pInstance = new CBulb(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CBulb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBulb::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
