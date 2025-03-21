#include "stdafx.h"
#include "DynamicCastleGate.h"
#include "ModelObject.h"
#include "Actor_Dynamic.h"
#include "GameInstance.h"
#include "Layer.h"
#include "3DMapSkspObject.h"

CDynamicCastleGate::CDynamicCastleGate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CContainerObject(_pDevice, _pContext)
{
}

CDynamicCastleGate::CDynamicCastleGate(const CDynamicCastleGate& _Prototype)
	:CContainerObject(_Prototype)
{
}

HRESULT CDynamicCastleGate::Initialize(void* _pArg)
{
	CContainerObject::CONTAINEROBJ_DESC* pDesc = static_cast<CContainerObject::CONTAINEROBJ_DESC*>(_pArg);
	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->isCoordChangeEnable = false;
	pDesc->iObjectGroupID = OBJECT_GROUP::DYNAMIC_OBJECT;
	pDesc->iNumPartObjects = CASTL_PART_LAST;
	pDesc->isCoordChangeEnable = false;

	pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
	CActor::ACTOR_DESC ActorDesc;
	pDesc->pActorDesc = &ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER;

	//XMStoreFloat4x4(&ActorDesc.ActorOffsetMatrix, XMMatrixTranslation(0.0f, -m_fGateHalfHeight, 0.f));
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = true;
	ActorDesc.FreezeRotation_XYZ[2] = true;
	ActorDesc.FreezePosition_XYZ[0] = true;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { m_fGateHalfWidth,m_fGateHalfHeight,m_fGateHalfThick };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::CASTLE_GATE;
	ShapeData.isTrigger = false;
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT|OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER;
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, m_fGateHalfHeight - 0.15f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);


	SHAPE_BOX_DESC BOXDesc2 = {};
	BOXDesc2.vHalfExtents={ 2.5f, 3.5f, m_fGateHalfThick};
	SHAPE_DATA ShapeData2 = {};
	ShapeData2.pShapeDesc = &BOXDesc2;
	ShapeData2.eShapeType = SHAPE_TYPE::BOX;
	ShapeData2.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
	ShapeData2.iShapeUse = (_uint)SHAPE_USE::SHAPE_FOOT;
	ShapeData2.isTrigger = false;
	XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0, m_fGateHalfHeight, 0));
	ShapeData2.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER;
	ActorDesc.ShapeDatas.push_back(ShapeData2);



	SHAPE_BOX_DESC BOXDesc3 = {};
	BOXDesc3.vHalfExtents = { m_fGateHalfWidth*0.5f,m_fGateHalfHeight*0.5f, m_fGateHalfThick };
	SHAPE_DATA ShapeData3 = {};
	ShapeData3.pShapeDesc = &BOXDesc3;
	ShapeData3.eShapeType = SHAPE_TYPE::BOX;
	ShapeData3.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
	ShapeData3.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
	ShapeData3.isTrigger = true;
	XMStoreFloat4x4(&ShapeData3.LocalOffsetMatrix, XMMatrixTranslation(0, m_fGateHalfHeight, 0));
	ShapeData3.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData3.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT;
	ActorDesc.ShapeDatas.push_back(ShapeData3);


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	//static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Rotation(_vector{ 0.f,1.f,0.f }, XMConvertToRadians(90.f));
	//static_cast<CActor_Dynamic*>(m_pActorCom)->Set_MassLocalPos(m_fMassLocal);
	m_pActorCom->Set_Mass(5.f);

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;
	

	_vector vForcDir = { 0.f,0.f,1.f };
	_float fForce = 1000.f;
	_vector vForce = vForcDir * fForce;
	m_vCollapseTorque = XMVector3Cross(vForce, { 0.f,m_fGateHalfHeight,0.f });

	return S_OK;
}

void CDynamicCastleGate::Update(_float _fTimeDelta)
{
#ifdef _DEBUG

#endif // _DEBUG
	if (KEY_DOWN(KEY::BACKSPACE))
		Collapse();
	Check_StateChange();
	if (COLLAPSING == m_eCurState)
	{
		m_fPushTimeAcc += _fTimeDelta;
		if (m_fPushTimeAcc < m_fPushTime)
		{
			CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
			_float3 vTorq; XMStoreFloat3(&vTorq, m_vCollapseTorque * _fTimeDelta);
			pDynamicActor->Add_Torque(vTorq);
			cout << "Push : "<< m_fPushTimeAcc << endl;
		}
	}
	else if (COLLAPSED == m_eCurState)
	{
	//	if(m_bCollapsed)
	}
	__super::Update(_fTimeDelta);
}

HRESULT CDynamicCastleGate::Render()
{
	return __super::Render();
}

void CDynamicCastleGate::OnContact_Modify(const COLL_INFO& _0, const COLL_INFO& _1, CModifiableContacts& _ModifiableContacts, _bool _bIm0)
{

	OBJECT_GROUP eOtherGroup = (OBJECT_GROUP)(_bIm0 ? _1.pActorUserData->iObjectGroup : _0.pActorUserData->iObjectGroup);
	SHAPE_USE eMyUse = (SHAPE_USE)(_bIm0 ? _0.pShapeUserData->iShapeUse : _1.pShapeUserData->iShapeUse);
	if (OBJECT_GROUP::PLAYER & eOtherGroup)
	{
		if (_bIm0)
		{
			_ModifiableContacts.Set_InvMassScale0(0.f); // Reduce mass effect on actor0
			_ModifiableContacts.Set_InvInertiaScale0(0.f); // Reduce rotation effect on actor0
		}
		else
		{
			_ModifiableContacts.Set_InvMassScale1(0.f); // Reduce mass effect on actor1
			_ModifiableContacts.Set_InvInertiaScale1(0.f); // Reduce rotation effect on actor1
		}
	}
	_uint iContactCount = _ModifiableContacts.Get_ContactCount();
	for (_uint i = 0; i < iContactCount; i++)
	{
		_ModifiableContacts.Set_Restitution(i, 0);
	}
	//else if ((OBJECT_GROUP::MAPOBJECT & eOtherGroup) && SHAPE_USE::SHAPE_BODY == eMyUse)
	//{

	//	_uint iContactCount = _ModifiableContacts.Get_ContactCount();
	//	for (_uint i = 0; i < iContactCount; i++)
	//	{
	//		_ModifiableContacts.Ignore(i);
	//		//if (0.9 >= _ModifiableContacts.Get_Normal(i).m128_f32[1])
	//		//else
	//		//	int a = 0;
	//	}
	//}
}

void CDynamicCastleGate::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{

}

void CDynamicCastleGate::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	OBJECT_GROUP eOtherGroup = (OBJECT_GROUP)_Other.pActorUserData->iObjectGroup;

	if (OBJECT_GROUP::MAPOBJECT & eOtherGroup
		&& (_uint)SHAPE_USE::SHAPE_TRIGER == _My.pShapeUserData->iShapeUse)
	{
		Set_State(COLLAPSED);
	}
}


        
void CDynamicCastleGate::Collapse()
{

	Set_State(COLLAPSING);

}

void CDynamicCastleGate::WorldMap_ReCapture()
{
	if(nullptr != m_PartObjects[CASTL_PART_SKSP])
	{ 
		static_cast<C3DMapSkspObject*>(m_PartObjects[CASTL_PART_SKSP])->Register_WorldCapture();
	}
}

HRESULT CDynamicCastleGate::Ready_PartObjects()
{
	//GATE
	CModelObject::MODELOBJECT_DESC tGateModelDesc{};

	tGateModelDesc.iCurLevelID = m_iCurLevelID;
	tGateModelDesc.eStartCoord = COORDINATE_3D;
	tGateModelDesc.isCoordChangeEnable = false;
	tGateModelDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
	tGateModelDesc.strModelPrototypeTag_3D = TEXT("Castle_Gate");
	tGateModelDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	tGateModelDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	tGateModelDesc.iRenderGroupID_3D = RG_3D;
	tGateModelDesc.iPriorityID_3D = PR3D_GEOMETRY;
	tGateModelDesc.tTransform3DDesc.vInitialRotation = {};
	tGateModelDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
	m_PartObjects[CASTL_PART_GATE] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tGateModelDesc));
	if (nullptr == m_PartObjects[CASTL_PART_GATE])
	{
		MSG_BOX("CASTL_PART_DOOR Creation Failed");
		return E_FAIL;
	}
	//m_PartObjects[CASTL_PART_DOOR]->Set_Position({ 0.f,0.225f,-0.225f });
	m_PartObjects[CASTL_PART_GATE]->Get_ControllerTransform()->Rotation(XMConvertToRadians(-90.f), _vector{ 1.f,0.f,0.f,0.f });
	

	CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_MapSkspObject"));
	if (nullptr == pLayer)
		return S_OK;
	list<CGameObject*> Objs = pLayer->Get_GameObjects();
	for (auto& pObj : Objs)
	{
		C3DMapSkspObject* pSksp = static_cast<C3DMapSkspObject*>(pObj);
		if(nullptr == pSksp) 
			continue;
		if (pSksp->Get_RenderSectionTag() == TEXT("Chapter4_SKSP_04"))
		{
			Add_PartObject(pSksp);
			Safe_AddRef(pSksp);
			pSksp->Set_ParentMatrix(COORDINATE_3D, m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
			pSksp->Set_Position(_vector{ 0.f,0.f,0.f });

		}
	}
	return S_OK;
}

void CDynamicCastleGate::Set_State(COLLAPSE_STATE _eState)
{
	m_eCurState = _eState;

}

void CDynamicCastleGate::Check_StateChange()
{
	if (m_ePrevState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CDynamicCastleGate::NONE:
			break;
		case Client::CDynamicCastleGate::COLLAPSING:
		{
			CActor_Dynamic* pDynamicActor = static_cast<CActor_Dynamic*>(m_pActorCom);
			pDynamicActor->Update(0);
			pDynamicActor->Set_Dynamic();
			pDynamicActor->Set_Gravity(true);
			pDynamicActor->Freeze_Rotation(false, true, true);
			pDynamicActor->Freeze_Position(true, false, false);

			_float3 vTorq; XMStoreFloat3(&vTorq, m_vCollapseTorque);
			//pDynamicActor->Add_Torque(vTorq);
			static_cast<CActor_Dynamic*>(m_pActorCom)->Set_MassLocalPos(m_fMassLocal);
		}
			break;
		case Client::CDynamicCastleGate::COLLAPSED:
		{
			static_cast<CActor_Dynamic*>(m_pActorCom)->Freeze_Position(true, false, true);
			static_cast<CActor_Dynamic*>(m_pActorCom)->Set_MassLocalPos({0.f,0.f,0.f});
			//m_pActorCom->Set_Mass(50.f
			static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Kinematic();


			_vector vPos = Get_FinalPosition();
			vPos += _vector{ 0.f,0.4f,0.f };
			_vector vLook = _vector{ 0.f,1.f,0.f };
			_vector vRight = _vector{ 1.f,0.f,0.f };
			_vector vUp = _vector{ 0.f,0.f,-1.f };
			// 2. 정규화 * Scale 후 값 세팅
			CTransform* pTransform = m_pControllerTransform->Get_Transform();
			_float3 vScale = pTransform-> Get_Scale();
			pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
			pTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
			pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
		}
			break;
		default:
			break;
		}
		m_ePrevState = m_eCurState;
	}
}

CDynamicCastleGate* CDynamicCastleGate::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDynamicCastleGate* pInstance = new CDynamicCastleGate(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DynamicCastleGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDynamicCastleGate::Clone(void* _pArg)
{
	CDynamicCastleGate* pInstance = new CDynamicCastleGate(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DynamicCastleGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicCastleGate::Free()
{
	__super::Free();
}
