#include "stdafx.h"
#include "Bulb.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Manager.h"

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

	if (COORDINATE_3D == pDesc->eStartCoord) {
		Add_Shape();
		static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f));
	}

	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Gravity(false);

	return S_OK;
}

void CBulb::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CBulb::Update(_float _fTimeDelta)
{
	Default_Move(_fTimeDelta);
	Sticking(_fTimeDelta);

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
	_pDesc->isTrigger = true;
	_pDesc->eShapeType = (SHAPE_TYPE)SHAPE_TYPE::SPHERE;
	_pDesc->fRadius = .5f;

	_pDesc->iFillterMyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	_pDesc->iFillterOtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;

	_pDesc->iTriggerType = (_uint)TRIGGER_TYPE::DEFAULT_TRIGGER;

	_pDesc->vLocalPosOffset = { 0.f, 0.f, 0.f };
	_pDesc->FreezeRotation[0] = true;
	_pDesc->FreezeRotation[1] = true;
	_pDesc->FreezeRotation[2] = false;
	_pDesc->iShapeUse = SHAPE_BODY;

	return S_OK;
}

HRESULT CBulb::Initialize_2D_Object(BULB_DESC* _pDesc)
{
	return S_OK;
}

void CBulb::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	if (OBJECT_GROUP::PLAYER != _Other.pActorUserData->pOwner->Get_ObjectGroupID() || 
		(_uint)SHAPE_USE::SHAPE_BODY !=_Other.pShapeUserData->iShapeUse)
		return;

	switch (_My.pShapeUserData->iShapeUse)
	{
	case BULB_SHAPE_USE::SHAPE_BODY:
	{

		Event_Get_Bulb(COORDINATE_3D);
		Event_DeleteObject(this);
		static_cast<CActor_Dynamic*>(m_pActorCom)->Set_LinearVelocity(XMVectorZero());
		m_isSticking = false;

		CEffect_Manager::GetInstance()->Active_Effect(TEXT("Bulb"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());

	}
		break;
	case BULB_SHAPE_USE::SHAPE_STICKING:
	{
		m_pTargetWorld = _Other.pActorUserData->pOwner->Get_ControllerTransform()->Get_WorldMatrix_Ptr();
		m_isSticking = true;
	}
		break;
	}
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
	SphereDesc.fRadius = 2.f;
	ShapeData.pShapeDesc = &SphereDesc;
	ShapeData.iShapeUse = SHAPE_STICKING;

	m_pActorCom->Add_Shape(ShapeData);
}

void CBulb::Sticking(_float _fTimeDelta)
{
	if (false == m_isSticking)
		return;

	_vector vTargetPos;
	memcpy(&vTargetPos, m_pTargetWorld->m[3], sizeof(_float4));
	
	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 0.6f);
	_vector vPos = Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);

	vPos = XMVectorLerp(vPos, vTargetPos, 0.3f);

	Get_ActorCom()->Set_GlobalPose({ XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) });
}

void CBulb::Default_Move(_float _fTimeDelta)
{
	if (true == m_isSticking)
		return;

	m_fBulbTime.y += _fTimeDelta;
	_float fRatio = m_fBulbTime.y / m_fBulbTime.x;
	if (fRatio >= (1.f - EPSILON)) {
		m_eBulbState = (BULB_STATE)(m_eBulbState ^ BULB_ALL);
		m_fBulbTime.y = 0.f;
		return;
	}

	switch (m_eBulbState) {
	case BULB_UP:
	{
		m_pActorCom->Set_LinearVelocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.35f);
	}
		break;
	case BULB_DOWN:
	{
		m_pActorCom->Set_LinearVelocity(XMVectorSet(0.f, -1.f, 0.f, 0.f), 0.35f);

	}
		break;
	}
	



	//_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	//
	//if (XMVectorGetY(vPos) < m_fMin) {	
	//	static_cast<CActor_Dynamic*>(m_pActorCom)->Add_Impulse({ 0.f, 0.6f, 0.f });
	//}
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
