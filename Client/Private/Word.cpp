#include "stdafx.h"
#include "Word.h"
#include "Player.h"
#include "Section_Manager.h"    

#include "Collider_Circle.h"
#include "Actor_Dynamic.h"
#include "Controller_Model.h"
#include "Model.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

CWord::CWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCarriableObject(_pDevice, _pContext)
{
}

CWord::CWord(const CWord& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CWord::Initialize(void* _pArg)
{
	WORD_DESC* pDesc = static_cast<WORD_DESC*>(_pArg);
	//2.23 머리위에 왔을 때의 위치, 회전 등 상태에 대한 설정 추가. -김지완-
	pDesc->eCrriableObjId = WORD;
	//pDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,0.f };
	pDesc->vHeadUpOffset3D = { 0.f,1.5f,0.f };
	XMStoreFloat4x4(&m_matHeadUpMatrix[COORDINATE_2D], XMMatrixTranslation(0.f, pDesc->fHeadUpHeight2D, 0.f));

	_matrix matHeadUpRotationMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vHeadUpRoolPitchYaw3D)));
	XMStoreFloat4x4(&m_matHeadUpMatrix[COORDINATE_3D],
		matHeadUpRotationMatrix
		* XMMatrixTranslation(pDesc->vHeadUpOffset3D.x, pDesc->vHeadUpOffset3D.y, pDesc->vHeadUpOffset3D.z));


	m_iRenderGroupID_3D = RG_3D;
	m_iPriorityID_3D = PR3D_GEOMETRY;

	m_pWordTexture = pDesc->pSRV;
	m_fSize = pDesc->fSize;
	m_eWordType = pDesc->eType;
	m_strText = pDesc->strText;
	m_fSize.x *= 0.9f;
	m_fSize.y *= 0.9f;
	pDesc->Build_2D_Transform({ 0.f,0.f }, m_fSize, 100.f);
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = true;
	CActor::ACTOR_DESC ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;
	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { 0.3f,0.3f,0.3f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	SHAPE_SPHERE_DESC ShapeDesc2 = {};
	ShapeDesc2.fRadius = 0.25f;
	SHAPE_DATA ShapeData2;
	ShapeData2.pShapeDesc = &ShapeDesc2;
	ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData2.isTrigger = false;
	ShapeData2.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER_TRIGGER;
	XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData2);


	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER_TRIGGER | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;
	pDesc->pActorDesc = &ActorDesc;
	pDesc->eActorType = ACTOR_TYPE::DYNAMIC;

	m_eCarriableId = WORD;

	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	m_eInteractID = INTERACT_ID::CARRIABLE;

	if (FAILED(CActorObject::Initialize(pDesc)))
		return E_FAIL;

	m_p2DColliderComs.resize(1);
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 1.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;
	m_pBody2DColliderCom = m_p2DColliderComs[0];
	Safe_AddRef(m_pBody2DColliderCom);


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pControllerTransform->Get_Transform(COORDINATE_3D)->Rotation(XMConvertToRadians(90.f), XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pControllerTransform->Get_Transform(COORDINATE_3D)->Set_Scale({ 0.7f,0.3f,0.7f });

	return S_OK;
}


HRESULT CWord::Render()
{
	COORDINATE eCurCoord = Get_CurCoord();/*
	_matrix = XMMatrixScaling(m_fSize.x, m_fSize.y, 1.f);*/
	
	if (FAILED(m_pShaderComs[eCurCoord]->Bind_Matrix("g_WorldMatrix", Get_FinalWorldMatrix_Ptr(eCurCoord))))
		return E_FAIL;

	if (FAILED(m_pShaderComs[eCurCoord]->Bind_SRV("g_DiffuseTexture", m_pWordTexture)))
		return E_FAIL;

	switch (eCurCoord)
	{
	case Engine::COORDINATE_2D:
	{
		m_pShaderComs[COORDINATE_2D]->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	}
	break;
	case Engine::COORDINATE_3D:
	{
		if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		m_pShaderComs[COORDINATE_2D]->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);

	}
	break;
	}


	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();
#ifdef _DEBUG
	if(!m_p2DColliderComs.empty())
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG
	return S_OK;
}



void CWord::Update(_float _fTimeDelta)
{
	m_pControllerTransform->Get_Transform(COORDINATE_3D)->Rotation(XMConvertToRadians(90.f), XMVectorSet(1.f, 0.f, 0.f, 0.f));
	Execute_WordMode_Action(_fTimeDelta);
	__super::Update(_fTimeDelta);
}

void CWord::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
	
}

HRESULT CWord::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (Is_Carrying())
	{
		Set_Position({ 0,0,0 });
		Get_ControllerTransform()->Rotation(0, _vector{ 0,1,0 });
		Set_SocketMatrix(_eCoordinate, &Get_HeadUpMatrix(_eCoordinate));
		Set_ParentMatrix(_eCoordinate, m_pCarrier->Get_ControllerTransform()->Get_WorldMatrix_Ptr(_eCoordinate));

		//m_pControllerModel->Get_Model(COORDINATE_3D)->Set_Active(COORDINATE_3D == _eCoordinate);
		//m_pControllerModel->Get_Model(COORDINATE_2D)->Set_Active(COORDINATE_2D == _eCoordinate);
	}
	if (FAILED(CActorObject::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;
	return S_OK;

}


void CWord::Execute_WordMode_Action(_float _fTimeDelta)
{
	switch (m_eWordMode)
	{
		//case Client::CWord::WORD_MODE_ENTER_SETUP:
		//
		//	_vector vTargetPos = XMLoadFloat3(&m_fContainorPosition);
		//	_vector vPos = Get_FinalPosition(COORDINATE_2D);
		//	if (true == m_pControllerTransform->MoveToTarget(vTargetPos, _fTimeDelta))
		//		m_isActionComplete = true;
  //			break;

		case Client::CWord::WORD_MODE_EXIT_SETUP:
		{
			if (0.f > m_fOutputSecond)
			{
				Set_WordMode();
			}
			else
			{
				_vector vTargetDir = XMLoadFloat3(&m_fOutputDir);
				m_fOutputSecond -= _fTimeDelta;
				m_pControllerTransform->Go_Direction(vTargetDir, m_fOutputSecond * 1000.f ,_fTimeDelta);
			}

			break;
		}
		case Client::CWord::WORD_MODE_SETUP:
		case Client::CWord::WORD_MODE_LAND:
		default:
			break;
	}

}

HRESULT CWord::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;


	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CWord::On_LayDownEnd(_fmatrix _matWoroverride)
{
	m_bLaydown = true;
}

void CWord::On_PickUpStart(CPlayer* _pPalyer, _fmatrix _matPlayerOffset)
{
	m_bLaydown = false;
}

void CWord::On_Land()
{
	m_bLaydown = true;
}


void CWord::Interact(CPlayer* _pUser)
{
	//_pUser->Set_CarryingObject(this);
	_pUser->CarryObject(this);
}

_bool CWord::Is_Interactable(CPlayer* _pUser)
{
	return false == _pUser->Is_CarryingObject();
}

_float CWord::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}



CWord* CWord::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWord* pInstance = new CWord(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWord::Clone(void* _pArg)
{
	CWord* pInstance = new CWord(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWord::Free()
{
	Safe_Release(m_pWordTexture);
	Safe_Release(m_pVIBufferCom);
	__super::Free();
}
