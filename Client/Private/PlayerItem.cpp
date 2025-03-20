#include "stdafx.h"
#include "PlayerItem.h"

#include "GameInstance.h"
#include "Trigger_Manager.h"
#include "Effect_Manager.h"

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
 	m_fOriginScale = pDesc->tTransform3DDesc.vInitialScaling.x;
	pDesc->iActorType = (_uint)ACTOR_TYPE::DYNAMIC;
	pDesc->isTrigger = false;
	pDesc->eShapeType = (SHAPE_TYPE)SHAPE_TYPE::SPHERE;
	pDesc->fRadius = 0.8f;

	pDesc->iFillterMyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	pDesc->iFillterOtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MAPOBJECT;

	pDesc->iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
	m_szModelTag = pDesc->szModelTag;
	m_szItemTag = pDesc->szItemTag;
	m_iItemType = pDesc->iItemType;

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
	SphereDesc.fRadius = 1.f;
	ShapeData.pShapeDesc = &SphereDesc;

	m_pActorCom->Add_Shape(ShapeData);

	this->Register_EnterHandler([](_uint _iTriggerType, _int _iTriggerID, _wstring& _szEventTag) {
		Event_Trigger_Enter(_iTriggerType, _iTriggerID, _szEventTag);
		});

	//m_pControllerTransform->Set_Scale(4.f, 4.f, 4.f);
	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f));
	//m_pActorCom->Set_AngularVelocity({ 0.f, 0.f, XMConvertToRadians(45.f) });

	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Gravity(false);

	return S_OK;
}

void CPlayerItem::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CPlayerItem::Update(_float _fTimeDelta)
{
	if (m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 6.f))
	{
		if (false == m_isInFrustum)
		{
			// temp
			CEffect_Manager::GetInstance()->Active_Effect(TEXT("Guntlet1"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());
			m_isInFrustum = true;
		}
		
	}
	else
	{
		if (true == m_isInFrustum)
		{
			CEffect_Manager::GetInstance()->InActive_Effect(TEXT("Guntlet1"));
			m_isInFrustum = false;
		}
	}

	PxVec3 AnglularVelocity = static_cast<PxRigidDynamic*>(m_pActorCom->Get_RigidActor())->getAngularVelocity();

	m_pActorCom->Set_AngularVelocity({ 0.f, 1.f, 0.f });
	AnglularVelocity = static_cast<PxRigidDynamic*>(m_pActorCom->Get_RigidActor())->getAngularVelocity();
	
	if (static_cast<PxRigidDynamic*>(m_pActorCom->Get_RigidActor())->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC) {
		printf("Actor is Kinematic! AngularVelocity will not be applied.\n");
	}
	
	// Phyxis Manager Update 때문에 일단 Update 위로 올림
	Action_Mode(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CPlayerItem::Late_Update(_float _fTimeDelta)
{

	if(false == m_isFrustumCulling)
		m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_GEOMETRY, this);

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

void CPlayerItem::Change_Mode(_uint _iItemMode)
{
	m_iItemMode = _iItemMode;

	if (GETTING == m_iItemMode)
	{
		// 이펙트 팡
		CEffect_Manager::GetInstance()->Active_EffectID(TEXT("Guntlet1"), true, m_pControllerTransform->Get_WorldMatrix_Ptr(), 1);

		//CEffect_Manager::GetInstance()->Active_EffectPositionID(TEXT("Guntlet1"), true, Get_FinalPosition(), 1);
	}
}

HRESULT CPlayerItem::Ready_Components(PLAYERITEM_DESC* _pArg)
{
	// Shader
	if (FAILED(Add_Component(m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Model
	if (FAILED(Add_Component(LEVEL_STATIC, m_szModelTag,
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
	case WAIT:
		Action_Wait(_fTimeDelta);
		break;
	case DISAPPEAR:
		Action_Disappear(_fTimeDelta);
		break;
	}

	Check_PosY();
}

void CPlayerItem::Action_Getting(_float _fTimeDelta)
{
	// 1. Item 먹을 때 크기가 줄어듦
	if (false == m_isScaleDown) {

		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fScaleTime, _fTimeDelta, LERP);
		
		if (fRatio >= (1.f - EPSILON)) {
			m_fScaleTime.y = 0.f;
			m_isScaleDown = true;
			m_isRender = false;

			m_fOriginY = XMVectorGetY(m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
		}

		_float fScale = m_pGameInstance->Lerp(m_fOriginScale, 0.3f, fRatio);
		m_pControllerTransform->Set_Scale(fScale, fScale, fScale);
	}

	// 2. 크키가 줄어든 후 일정 시간 후 기존 위치보다 위에서 나타남
	else if (false == m_isFinishWait) {

		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fWaitTime, _fTimeDelta, LERP);

		if (fRatio >= (1.f - EPSILON)) {
			m_fWaitTime.y = 0.f;
			m_isFinishWait = true;
			m_isRender = true;
			m_isStop = false;

			// 위치 위로 수정
			m_pActorCom->Set_LinearVelocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), 3.f);
			m_pActorCom->Get_RigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		}
	}

	// 3. 위에서 나타남과 동시에 원래 크기로 다시 커지기 시작
	// 4. 이때 Wait 상태로 전환되며, 특정 동작(다이얼로그 끝)이 있을 때까지 대기하기 시작 -> Wait 상태로 넘어감
	else {
		
		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fScaleTime, _fTimeDelta, LERP);

		if (fRatio >= (1.f - EPSILON)) {
			m_fScaleTime.y = 0.f;
			m_isScaleDown = false;
			m_isFinishWait = false;

			m_iItemMode = WAIT;

			// 임시로 Trigger Manager에서 다 커지면 모든 동작을 끝내고 없애기 위해 넣었던 코드임 
			CTrigger_Manager::GetInstance()->On_End(m_szItemTag);
		}

		_float fScale = m_pGameInstance->Lerp(0.3f, m_fOriginScale, fRatio);
		m_pControllerTransform->Set_Scale(fScale, fScale, fScale);
	}
}

void CPlayerItem::Action_Wait(_float _fTimeDelta)
{
	// Action Disappear로 변하기 전까지 대기
}

void CPlayerItem::Action_Disappear(_float _fTimeDelta)
{
	// 마지막에? 사실 뭐 딱히 할 필요는 없음, 아예 삭제시켜 줘야 함
	this->Set_Active(false);
	m_isStop = false; 

	CEffect_Manager::GetInstance()->InActive_EffectID(TEXT("Guntlet1"), 0);

}

void CPlayerItem::Check_PosY()
{
	if (true == m_isStop)
		return;

	_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);
	_float fOffset = abs(m_fOriginY - XMVectorGetY(vPos));
	
	PxVec3 velocity = static_cast<PxRigidDynamic*>(m_pActorCom->Get_RigidActor())->getLinearVelocity();

	if (fOffset > 1.5f) {
		m_pActorCom->Set_LinearVelocity(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		m_isStop = true;
	}
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

CPlayerItem* CPlayerItem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
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
