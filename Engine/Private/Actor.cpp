#include "Actor.h"
#include "GameInstance.h"
#include "ActorObject.h"
#include "ModelObject.h"

#include "DebugDraw.h"
CActor::CActor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ACTOR_TYPE _eActorType)
	: CComponent(_pDevice, _pContext)
	, m_eActorType(_eActorType)
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
}

CActor::CActor(const CActor& _Prototype)
	: CComponent(_Prototype)
	, m_OffsetMatrix(_Prototype.m_OffsetMatrix)
	, m_eActorType(_Prototype.m_eActorType)
{
#ifdef _DEBUG
	m_pInputLayout = _Prototype.m_pInputLayout;
	m_pBatch = _Prototype.m_pBatch;
	m_pEffect = _Prototype.m_pEffect;
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CActor::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	const void* pShaderByteCode = nullptr;
	size_t iShaderByteCodeLength = 0;

	/* m_pEffect 쉐이더의 옵션 자체를 Color 타입으로 변경해주어야 정상적인 동작을 한다. (기본 값은 Texture )*/
	m_pEffect->SetVertexColorEnabled(true);
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif // _DEBUG
	return S_OK;
}

HRESULT CActor::Initialize(void* _pArg)
{
	ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(_pArg);
	if (nullptr == pDesc->pOwner)
		return E_FAIL;

	// Save Desc 
	m_pOwner = pDesc->pOwner;                                   // 순환 참조로 인해 RefCount 관리 X
	if (pDesc->ActorOffsetMatrix._44 != 0)                       // m_OffsetMatrix은 항등행렬로 초기화 된 상태임 Desc에 값을 채운경우에만, Offset 적용.
		m_OffsetMatrix = pDesc->ActorOffsetMatrix;

	// Add Desc
	
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Actor(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Shapes(pDesc->ShapeDatas)))
		return E_FAIL;

	// Add User Data (Actor)
	ACTOR_USERDATA* pActorUserData = new ACTOR_USERDATA;
	pActorUserData->pOwner = m_pOwner;
	pActorUserData->iObjectGroup = pDesc->tFilterData.MyGroup;
	m_pActor->userData = pActorUserData;

	Setup_SimulationFiltering(pDesc->tFilterData.MyGroup, pDesc->tFilterData.OtherGroupMask, false);

	// Scene에 등록. (추후 곧바로 등록하지 않고 싶다면, 별도의 Desc 변수를 추가할 예정.)
	PxScene* pScene = m_pGameInstance->Get_Physx_Scene();
	pScene->addActor(*m_pActor);


	return S_OK;
}

void CActor::Priority_Update(_float _fTimeDelta)
{
}

void CActor::Update(_float _fTimeDelta)
{
}

void CActor::Late_Update(_float _fTimeDelta)
{
//#ifdef _DEBUG
//	m_pGameInstance->Add_DebugComponent(this);
//#endif // _DEBUG

}
#ifdef _DEBUG
HRESULT CActor::Render()
{
	if (false == m_isActive)
		return S_OK;

	if (nullptr == m_pOwner)
		return S_OK;
	/* dx9 처럼 W,V,P 행렬을 던져준다. */

    /* World Matrix를 Indentity로 던지는 이유 : 이미 각각의 CBounding* 가 소유한 BoundingDesc의 정점 자체를 이미 World까지 변환 할 것임. */
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
    m_pEffect->Apply(m_pContext);
    m_pContext->IASetInputLayout(m_pInputLayout);
    m_pBatch->Begin();
    for (auto& pShape : m_Shapes)
    {
        if (false == pShape->getFlags().isSet(PxShapeFlag::eTRIGGER_SHAPE))
            continue;

		PxGeometryType::Enum eType = pShape->getGeometryType();

		switch (eType)
		{
		case physx::PxGeometryType::eSPHERE:
		{
			_float fRadius = pShape->getGeometry().sphere().radius;
			_vector vOwnerPos = m_pOwner->Get_FinalPosition();
			PxVec3 vShapeOffsetPos = pShape->getLocalPose().p;
			_float3 vPosition = { vShapeOffsetPos.x, vShapeOffsetPos.y, vShapeOffsetPos.z };
			XMStoreFloat3(&vPosition, XMLoadFloat3(&vPosition) + vOwnerPos);
			BoundingSphere BoundingSphere(vPosition, fRadius);
			DX::Draw(m_pBatch, BoundingSphere, XMLoadFloat4(&m_vDebugColor));
		}
		break;
		case physx::PxGeometryType::ePLANE:
			break;
		case physx::PxGeometryType::eCAPSULE:
			break;
		case physx::PxGeometryType::eBOX:
		{
			PxVec3 vHalfExtents = pShape->getGeometry().box().halfExtents;
			PxQuat quat = pShape->getLocalPose().q;

			_float4 vQuat = { quat.x, quat.y, quat.z, quat.w };
			_vector vOwnerPos = m_pOwner->Get_FinalPosition();
			PxVec3 vShapeOffsetPos = pShape->getLocalPose().p;
			_float3 vPosition = { vShapeOffsetPos.x, vShapeOffsetPos.y, vShapeOffsetPos.z };
			XMStoreFloat3(&vPosition, XMLoadFloat3(&vPosition) + vOwnerPos);
			BoundingOrientedBox box(vPosition, _float3(vHalfExtents.x, vHalfExtents.y, vHalfExtents.z), vQuat);
			//BoundingBox BoundingBox(vPosition, _float3(vHalfExtents.x, vHalfExtents.y, vHalfExtents.z));
			DX::Draw(m_pBatch, box, XMLoadFloat4(&m_vDebugColor));
		}
		break;
		default:
			break;
		}
	}

	m_pBatch->End();
	return S_OK;
}
#endif // _DEBUG




HRESULT CActor::Ready_Actor(ACTOR_DESC* _pActorDesc)
{
	PxPhysics* pPhysic = m_pGameInstance->Get_Physics();
	PxTransform Transform = PxTransform();
	//_vector vOwnerPos = m_pOwner->Get_Position();
	_matrix OwnerWorldMatrix = m_pOwner->Get_WorldMatrix();
	_vector vScale = {};
	_vector vPosition = {};
	_vector vQuat = {};
	XMMatrixDecompose(&vScale, &vQuat, &vPosition, XMLoadFloat4x4(&m_OffsetMatrix) * OwnerWorldMatrix);
	//vPosition += vOwnerPos;
	Transform.p = PxVec3(XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition));
	Transform.q = PxQuat(XMVectorGetX(vQuat), XMVectorGetY(vQuat), XMVectorGetZ(vQuat), XMVectorGetW(vQuat));
	/* 1. RigidBody Type Check */
	switch (m_eActorType)
	{
	case Engine::ACTOR_TYPE::STATIC:
		m_pActor = pPhysic->createRigidStatic(Transform);
		static_cast<PxRigidStatic*>(m_pActor)->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		break;
	case Engine::ACTOR_TYPE::KINEMATIC:
		m_pActor = pPhysic->createRigidDynamic(Transform);
		static_cast<PxRigidDynamic*>(m_pActor)->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		static_cast<PxRigidDynamic*>(m_pActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		break;
	case Engine::ACTOR_TYPE::DYNAMIC:
	{
		PxRigidDynamic* pActor = pPhysic->createRigidDynamic(Transform);
		pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, _pActorDesc->FreezeRotation_XYZ[0]);
		pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, _pActorDesc->FreezeRotation_XYZ[1]);
		pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, _pActorDesc->FreezeRotation_XYZ[2]);
		pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, _pActorDesc->FreezePosition_XYZ[0]);
		pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, _pActorDesc->FreezePosition_XYZ[1]);
		pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, _pActorDesc->FreezePosition_XYZ[2]);
		pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
		//pActor->setSolverIterationCounts(8, 4);
		m_pActor = pActor;
	}
	

	break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

_float3 CActor::Get_GlobalPose()
{
	PxTransform CurTransform = m_pActor->getGlobalPose();

	return _float3(CurTransform.p.x, CurTransform.p.y, CurTransform.p.z);
}

void CActor::Set_GlobalPose(const _float3& _vPos)
{
	PxTransform CurTransform = m_pActor->getGlobalPose();
	CurTransform.p = { _vPos.x, _vPos.y , _vPos.z };
	m_pActor->setGlobalPose(CurTransform);
}

void CActor::Set_PxActorDisable()
{
	m_pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
}

void CActor::Set_PxActorEnable()
{
	m_pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
}

HRESULT CActor::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	// Actor 쪽 작업 방식.
	// 1. Actor 전환되고자 하느 
	if (COORDINATE_3D == _eCoordinate)
	{
		Set_Active(true);
	}
	else
	{
		Set_Active(false);
	}

	return S_OK;
}

HRESULT CActor::Add_Shape(const SHAPE_DATA& _ShapeData)
{
	PxPhysics* pPhysics = m_pGameInstance->Get_Physics();
	if (nullptr == pPhysics)
		return E_FAIL;

	PxShapeFlags ShapeFlags = _ShapeData.isTrigger ? PxShapeFlag::eTRIGGER_SHAPE : PxShapeFlag::eSIMULATION_SHAPE;

	if (true == _ShapeData.isSceneQuery)
		ShapeFlags |= PxShapeFlag::eSCENE_QUERY_SHAPE;

#ifdef _DEBUG
	if (true == _ShapeData.isVisual)
		ShapeFlags |= PxShapeFlag::eVISUALIZATION;
#endif // _DEBUG



	PxMaterial* pShapeMaterial = m_pGameInstance->Get_Material(_ShapeData.eMaterial);

	/* 세부 Shape Geometry 생성 */
	PxShape* pShape = nullptr;
	_float3 vScale;
	_float4 vQuat;
	_float3 vPosition;
	if (false == m_pGameInstance->MatrixDecompose(&vScale, &vQuat, &vPosition, XMLoadFloat4x4(&_ShapeData.LocalOffsetMatrix)))
		return E_FAIL;

	switch (_ShapeData.eShapeType)
	{
	case Engine::SHAPE_TYPE::BOX:
	{
		SHAPE_BOX_DESC* pDesc = static_cast<SHAPE_BOX_DESC*>(_ShapeData.pShapeDesc);
		PxBoxGeometry BoxGeometry = PxBoxGeometry(PxVec3(pDesc->vHalfExtents.x * vScale.x, pDesc->vHalfExtents.y * vScale.y, pDesc->vHalfExtents.z * vScale.z));
		pShape = pPhysics->createShape(BoxGeometry, *pShapeMaterial, true, ShapeFlags);
	}
	break;
	case Engine::SHAPE_TYPE::SPHERE:
	{
		SHAPE_SPHERE_DESC* pDesc = static_cast<SHAPE_SPHERE_DESC*>(_ShapeData.pShapeDesc);
		PxSphereGeometry SphereGeometry = PxSphereGeometry(pDesc->fRadius * vScale.x);
		pShape = pPhysics->createShape(SphereGeometry, *pShapeMaterial, true, ShapeFlags);
	}
	break;
	case Engine::SHAPE_TYPE::CAPSULE:
	{
		SHAPE_CAPSULE_DESC* pDesc = static_cast<SHAPE_CAPSULE_DESC*>(_ShapeData.pShapeDesc);
		PxCapsuleGeometry CapsuleGeometry = PxCapsuleGeometry(pDesc->fRadius * vScale.x, pDesc->fHalfHeight * vScale.x);

		pShape = pPhysics->createShape(CapsuleGeometry, *pShapeMaterial, true, ShapeFlags);
	}
	break;
	case Engine::SHAPE_TYPE::COOKING:
	{
		ShapeFlags &= ~PxShapeFlag::eVISUALIZATION; // 쿠킹 전용 쉐잎은렌더 데이터 생성 x 

		SHAPE_COOKING_DESC* pDesc = static_cast<SHAPE_COOKING_DESC*>(_ShapeData.pShapeDesc);

		if (nullptr == m_pOwner)
			return E_FAIL;

		CModelObject* pModelObj = dynamic_cast<CModelObject*>(m_pOwner);
		if (nullptr == pModelObj)
			return E_FAIL;

		CModel* _pModel = pModelObj->Get_Model(COORDINATE_3D);
		if (nullptr == _pModel)
			return E_FAIL;
		C3DModel* pModel = static_cast<C3DModel*>(_pModel);

		_uint iNumMeshes = pModel->Get_NumMeshes();



		for (_uint i = 0; i < iNumMeshes; i++)
		{

			if (false == pDesc->isLoad)
			{
#pragma region Cooking

				CMesh* pMesh = pModel->Get_Mesh(i);
				PxCooking* pCooking = m_pGameInstance->Get_Cooking();


#pragma region convex
				PxConvexMeshDesc  meshDesc;
				if (FAILED(pMesh->Cooking(meshDesc)))
					return E_FAIL;

				PxDefaultMemoryOutputStream writeBuffer;
				PxCookingParams params(pPhysics->getTolerancesScale());
				pCooking->setParams(params);


				if (!pCooking->cookConvexMesh(meshDesc, writeBuffer))
					return E_FAIL;

#pragma region Cooking Save
				if (pDesc->isSave)
				{

					ofstream file;

					file.open(pDesc->strFilePath, ios::binary);
					if (!file)
						return E_FAIL;

					_uint iMeshDataSize = writeBuffer.getSize();

					file.write((_char*)&iMeshDataSize, sizeof(_uint));
					file.write((_char*)writeBuffer.getData(), iMeshDataSize);
				}
#pragma endregion

				PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
				PxConvexMesh* pPxMesh = pPhysics->createConvexMesh(readBuffer);


				PxMeshScale mashScale(PxVec3(vScale.x, vScale.y, vScale.z));

				PxConvexMeshGeometry geometry(pPxMesh, mashScale);
				pShape = pPhysics->createShape(geometry, *pShapeMaterial, true, ShapeFlags);
#pragma endregion

			#pragma region Triangle

				//PxTriangleMeshDesc meshDesc;
				//if (FAILED(pMesh->Cooking(meshDesc)))
				//    return E_FAIL;
				//PxDefaultMemoryOutputStream writeBuffer;
				//PxCookingParams params(pPhysics->getTolerancesScale());
				//pCooking->setParams(params);

				//if (!pCooking->cookTriangleMesh(meshDesc, writeBuffer))
				//    return E_FAIL;

				//PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
				//PxTriangleMesh* pPxMesh = pPhysics->createTriangleMesh(readBuffer);
				//PxTriangleMeshGeometry geometry(pPxMesh);
				//pShape = pPhysics->createShape(geometry, *pShapeMaterial, true, ShapeFlags);
#pragma endregion
#pragma endregion
			}
			else
			{
				if (pModel->Has_CookingCollider())
				{
					PxDefaultMemoryInputData readBuffer((PxU8*)pModel->Get_CookingColliderData(), pModel->Get_CookingColliderDataLength());
					PxConvexMesh* pPxMesh = pPhysics->createConvexMesh(readBuffer);

					PxMeshScale mashScale(PxVec3(vScale.x, vScale.y, vScale.z));

					PxConvexMeshGeometry geometry(pPxMesh, mashScale);
					pShape = pPhysics->createShape(geometry, *pShapeMaterial, true, ShapeFlags);
				}
				else
				{
					ifstream  file;
					file.open(pDesc->strFilePath, ios::binary);
					if (!file)
						return E_FAIL;
					_uint meshDataSize;
					file.read(reinterpret_cast<_char*>(&meshDataSize), sizeof(_uint));
					vector<_char> meshData(meshDataSize);
					file.read(meshData.data(), meshDataSize);

					PxDefaultMemoryInputData readBuffer((PxU8*)meshData.data(), meshDataSize);

					PxConvexMesh* pPxMesh = pPhysics->createConvexMesh(readBuffer);

					PxMeshScale mashScale(PxVec3(vScale.x, vScale.y, vScale.z));

					PxConvexMeshGeometry geometry(pPxMesh, mashScale);
					pShape = pPhysics->createShape(geometry, *pShapeMaterial, true, ShapeFlags);
				}

			}

		}
	}
	break;
	default:
		return E_FAIL;
	}

	if (nullptr == pShape)
	{
		MSG_BOX("Failed Create pShape (CActor::Ready_Shape)");
		return E_FAIL;
	}


	pShape->setLocalPose(PxTransform(PxVec3(vPosition.x, vPosition.y, vPosition.z), PxQuat(vQuat.x, vQuat.y, vQuat.z, vQuat.w)));
	PxFilterData FilterData;
	FilterData.word0 = _ShapeData.FilterData.MyGroup;
	FilterData.word1 = _ShapeData.FilterData.OtherGroupMask;

	pShape->setSimulationFilterData(FilterData);
	//pShape->setContactOffset(1.f);
	//pShape->setRestOffset(0.1f);
	SHAPE_USERDATA* pShapeUserData = new SHAPE_USERDATA;
	pShapeUserData->iShapeInstanceID = m_pGameInstance->Create_ShapeID();
	pShapeUserData->iShapeUse = _ShapeData.iShapeUse;
	pShapeUserData->iShapeIndex = (_uint)m_Shapes.size();
	pShape->userData = pShapeUserData;
	m_pGameInstance->Add_ShapeUserData(pShapeUserData);

	m_pActor->attachShape(*pShape);
	m_Shapes.push_back(pShape);
	pShape->acquireReference(); // Add_Ref
	
	// 이건 생성 시점에 추가된 ref를 감소
	pShape->release();


	return S_OK;
}

HRESULT CActor::Delete_Shape(_int _iShapeIndex)
{
	if (m_Shapes.size() <= _iShapeIndex)
		return E_FAIL;

	if (nullptr == m_Shapes[_iShapeIndex])
		return E_FAIL;

	m_pActor->detachShape(*m_Shapes[_iShapeIndex]);

	m_Shapes[_iShapeIndex]->release();

	auto& iter = m_Shapes.begin();
	_int iIndexCount = 0;
	for (iter; iter != m_Shapes.end(); )
	{
		if (iIndexCount == _iShapeIndex)
		{
			m_Shapes.erase(iter);
			return S_OK;
		}
		else
		{
			++iter;
			++iIndexCount;
		}
	}

	return E_FAIL;
}

void CActor::Setup_SimulationFiltering(_uint _iMyGroup, _uint _iOtherGroupMask, _bool _isRunTime)
{
	PxScene* pScene = m_pGameInstance->Get_Physx_Scene(); // scene의 시뮬레이션 데이터를 reset해줘야함. 그래서 Scene 객체가 없다면 무의미.
	if (nullptr == pScene)
		return;

	PxFilterData FilterData;
	FilterData.word0 = _iMyGroup;
	FilterData.word1 = _iOtherGroupMask; // OhterGroupMask는 비트연산자를 통해 여러 그룹을 포함 가능.
	//FilterData.word2; // 필터셰이딩을 할때 필요한 데이터를 추가 저장이 가능.
	//FilterData.word3;
	PxU32 iNumShapes = m_pActor->getNbShapes();

	vector<PxShape*> pShapes;
	pShapes.resize(iNumShapes);
	m_pActor->getShapes(pShapes.data(), iNumShapes);

	for (auto& pShape : pShapes)
	{
		PxFilterData ShapeFilterData = pShape->getSimulationFilterData();
		if(0 == ShapeFilterData.word0 || 0 == ShapeFilterData.word1)
			pShape->setSimulationFilterData(FilterData);
		else
		{
			int a = 0;
		}
	}

	if (true == _isRunTime)
		pScene->resetFiltering(*m_pActor);
}

void CActor::Set_ActorOffsetMatrix(_fmatrix _ActorOffsetMatrix)
{
	// Offset 저장.
	XMStoreFloat4x4(&m_OffsetMatrix, _ActorOffsetMatrix);

	_matrix OwnerWorldMatrix = m_pOwner->Get_WorldMatrix();
	_matrix FinalMatrix = _ActorOffsetMatrix * OwnerWorldMatrix;
	_vector vScale = {};
	_vector vPosition = {};
	_vector vQuat = {};
	XMMatrixDecompose(&vScale, &vQuat, &vPosition, FinalMatrix);

	PxTransform Transform;
	Transform.p = PxVec3(XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition));
	Transform.q = PxQuat(XMVectorGetX(vQuat), XMVectorGetY(vQuat), XMVectorGetZ(vQuat), XMVectorGetW(vQuat));

	m_pActor->setGlobalPose(Transform);
}

HRESULT CActor::Set_ShapeLocalOffsetMatrix(_int _iShapeIndex, _fmatrix _ShapeLocalOffsetMatrix)
{
	if (m_Shapes.size() <= _iShapeIndex)
		return E_FAIL;

	// 1. 행렬에서 위치, 회전 추출
	XMVECTOR scale, rotationQuat, position;
	XMMatrixDecompose(&scale, &rotationQuat, &position, _ShapeLocalOffsetMatrix);

	// 2. DirectXMath 데이터를 PhysX 데이터로 변환
	PxVec3 pxPosition(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
	PxQuat pxRotation(XMVectorGetX(rotationQuat), XMVectorGetY(rotationQuat), XMVectorGetZ(rotationQuat), XMVectorGetW(rotationQuat));

	// 3. PxTransform 생성
	PxTransform newLocalPose(pxPosition, pxRotation);

	// 4. Shape의 Local Pose 설정
	m_Shapes[_iShapeIndex]->setLocalPose(newLocalPose);

	return S_OK;
}

HRESULT CActor::Set_ShapeLocalOffsetPosition(_int _iShapeIndex, const _float3& _vOffsetPos)
{
	if (m_Shapes.size() <= _iShapeIndex)
		return E_FAIL;

	PxTransform Transform = m_Shapes[_iShapeIndex]->getLocalPose();
	Transform.p = { _vOffsetPos.x, _vOffsetPos.y, _vOffsetPos.z };
	m_Shapes[_iShapeIndex]->setLocalPose(Transform);

	return S_OK;
}

HRESULT CActor::Set_ShapeLocalOffsetQuaternion(_int _iShapeIndex, const _float4& _vQuat)
{
	if (m_Shapes.size() <= _iShapeIndex)
		return E_FAIL;

	PxTransform Transform = m_Shapes[_iShapeIndex]->getLocalPose();
	Transform.q = PxQuat(_vQuat.x, _vQuat.y, _vQuat.z, _vQuat.w);

	m_Shapes[_iShapeIndex]->setLocalPose(Transform);

	return S_OK;
}

HRESULT CActor::Set_ShapeLocalOffsetPitchYawRoll(_int _iShapeIndex, const _float3& _vPitchYawRoll)
{
	if (m_Shapes.size() <= _iShapeIndex)
		return E_FAIL;

	PxTransform Transform = m_Shapes[_iShapeIndex]->getLocalPose();
	_vector vQuat = XMQuaternionRotationRollPitchYaw(_vPitchYawRoll.x, _vPitchYawRoll.y, _vPitchYawRoll.z);
	Transform.q = PxQuat(XMVectorGetX(vQuat), XMVectorGetY(vQuat), XMVectorGetZ(vQuat), XMVectorGetW(vQuat));

	m_Shapes[_iShapeIndex]->setLocalPose(Transform);

	return S_OK;
}

HRESULT CActor::Set_ShapeGeometry(_int _iShapeIndex, PxGeometryType::Enum _eType, SHAPE_DESC* _pDesc)
{
	if (m_Shapes.size() <= _iShapeIndex)
		return E_FAIL;

	PxGeometryType::Enum eType = m_Shapes[_iShapeIndex]->getGeometryType();

	if (eType != _eType)
	{
		MSG_BOX(" Shape Geometry Type이 틀렸어. >>> Set_ShapeScale()");
		return E_FAIL;
	}

	switch (_eType)
	{
	case physx::PxGeometryType::eSPHERE:
	{
		SHAPE_SPHERE_DESC* pSphereDesc = static_cast<SHAPE_SPHERE_DESC*>(_pDesc);
		PxSphereGeometry Geometry(pSphereDesc->fRadius);
		m_Shapes[_iShapeIndex]->setGeometry(Geometry);
	}
	break;
	case physx::PxGeometryType::eCAPSULE:
	{
		SHAPE_CAPSULE_DESC* pCapsuleDesc = static_cast<SHAPE_CAPSULE_DESC*>(_pDesc);
		PxCapsuleGeometry Geometry(pCapsuleDesc->fHalfHeight, pCapsuleDesc->fRadius);

		m_Shapes[_iShapeIndex]->setGeometry(Geometry);
	}
	break;
	case physx::PxGeometryType::eBOX:
	{
		SHAPE_BOX_DESC* pBoxDesc = static_cast<SHAPE_BOX_DESC*>(_pDesc);
		PxBoxGeometry Geometry(pBoxDesc->vHalfExtents.x, pBoxDesc->vHalfExtents.y, pBoxDesc->vHalfExtents.z);
		m_Shapes[_iShapeIndex]->setGeometry(Geometry);
	}
	break;
	default:
		MSG_BOX(" Sphere, Capsule, Box 만 크기를 변경할 수 있어. ");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CActor::Set_ShapeEnable(_int _iShapeIndex, _bool _isEnable)
{

	if (m_Shapes.size() <= _iShapeIndex)
		return E_FAIL;

	if (nullptr == m_Shapes[_iShapeIndex])
		return E_FAIL;

	if (true == _isEnable)
		m_pActor->attachShape(*m_Shapes[_iShapeIndex]);
	else
		m_pActor->detachShape(*m_Shapes[_iShapeIndex]);

	return S_OK;
}

HRESULT CActor::Set_AllShapeEnable(_bool _isEnable)
{
	if (true == _isEnable)
	{
		for (auto& pShape : m_Shapes)
			m_pActor->attachShape(*pShape);
	}
	else
	{
		for (auto& pShape : m_Shapes)
			m_pActor->detachShape(*pShape);
	}

	return S_OK;
}

void CActor::Set_ShapeRayCastFlag(_bool _isRayCast)
{
	for (auto& pShape : m_Shapes)
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, _isRayCast);

}

void CActor::Set_MassLocalPos(_float3 _vPosition)
{	//무게중심 이동
	PxTransform newCOM(PxVec3(_vPosition.x, _vPosition.y, _vPosition.z), PxQuat(PxIdentity));
	static_cast<PxRigidDynamic*>(m_pActor)->setCMassLocalPose(newCOM);
}

void CActor::Set_Mass(_float _vValue)
{
	static_cast<PxRigidDynamic*>(m_pActor)->setMass(_vValue);

}



void CActor::Active_OnEnable()
{
	m_pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
}

void CActor::Active_OnDisable()
{
	m_pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
}

HRESULT CActor::Ready_Shapes(const vector<SHAPE_DATA>& _ShapeDatas)
{
	PxPhysics* pPhysics = m_pGameInstance->Get_Physics();
	if (nullptr == pPhysics)
		return E_FAIL;
	
	for (auto& ShapeData : _ShapeDatas)
	{
		Add_Shape(ShapeData);
	}

#pragma region old
	//    /* Shape 생성 코드 시작. */
//    PxMaterial* pShapeMaterial = m_pGameInstance->Get_Material(ACTOR_MATERIAL::DEFAULT);
//    for (_uint i = 0; i < _ShapeDatas.size(); ++i)
//    {
//        PxShapeFlags ShapeFlags = (PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE);
//
//        /* IsTrigger Check */
//        if (true == _ShapeDatas[i].isTrigger)
//        {
//            ShapeFlags.clear(PxShapeFlag::eSIMULATION_SHAPE);
//            ShapeFlags.set(PxShapeFlag::eTRIGGER_SHAPE);
//        }
//
//        /* Material 생성 */
//        pShapeMaterial = m_pGameInstance->Get_Material(_ShapeDatas[i].eMaterial);
//
//
//        /* 세부 Shape Geometry 생성 */
//        PxShape* pShape = nullptr;
//        switch (_ShapeDatas[i].eShapeType)
//        {
//        case Engine::SHAPE_TYPE::BOX:
//        {
//            SHAPE_BOX_DESC* pDesc = static_cast<SHAPE_BOX_DESC*>(_ShapeDatas[i].pShapeDesc);
//            PxBoxGeometry BoxGeometry = PxBoxGeometry(PxVec3(pDesc->vHalfExtents.x, pDesc->vHalfExtents.y, pDesc->vHalfExtents.z));
//            pShape = pPhysics->createShape(BoxGeometry, *pShapeMaterial, true, ShapeFlags);
//        }
//            break;
//        case Engine::SHAPE_TYPE::SPHERE:
//        {
//            SHAPE_SPHERE_DESC* pDesc = static_cast<SHAPE_SPHERE_DESC*>(_ShapeDatas[i].pShapeDesc);
//            PxSphereGeometry SphereGeometry = PxSphereGeometry(pDesc->fRadius);
//            pShape = pPhysics->createShape(SphereGeometry, *pShapeMaterial, true, ShapeFlags);
//        }
//            break;
//        case Engine::SHAPE_TYPE::CAPSULE:
//        {
//            SHAPE_CAPSULE_DESC* pDesc = static_cast<SHAPE_CAPSULE_DESC*>(_ShapeDatas[i].pShapeDesc);
//            PxCapsuleGeometry CapsuleGeometry = PxCapsuleGeometry(pDesc->fRadius, pDesc->fHalfHeight);
//            pShape = pPhysics->createShape(CapsuleGeometry, *pShapeMaterial, true, ShapeFlags);
//        }
//            break;
//        default:
//            return E_FAIL;
//        }
//
//        if (nullptr == pShape)
//        {
//            MSG_BOX("Failed Create pShape (CActor::Ready_Shape)");
//            return E_FAIL;
//        }
//
//        pShape->setLocalPose(PxTransform(PxMat44((_float*)(&_ShapeDatas[i].LocalOffsetMatrix))));
//        //pShape->setContactOffset(0.05f);
//        //pShape->setRestOffset(0.01f);
//        SHAPE_USERDATA* pShapeUserData = new SHAPE_USERDATA;
//        pShapeUserData->iShapeInstanceID = m_pGameInstance->Create_ShapeID();
//        pShapeUserData->iShapeUse = _ShapeDatas[i].iShapeUse;
//        pShape->userData = pShapeUserData;
//        m_pGameInstance->Add_ShapeUserData(pShapeUserData);
//        m_pActor->attachShape(*pShape);
//#ifdef _DEBUG
//        if (true == _ShapeDatas[i].isTrigger)
//        {
//            m_pTriggerShapes.push_back(pShape);
//            pShape->acquireReference();
//        }
//#endif // _DEBUG
//        pShape->release();
//    }

#pragma endregion // old


	return S_OK;
}

void CActor::Free()
{
#ifdef _DEBUG

	Safe_Release(m_pInputLayout);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

#endif // _DEBUG

	for (auto& pPxShape : m_Shapes)
	{
		pPxShape->release();
	}

	if (nullptr != m_pActor)
	{
		// 순환 참조로 인해 RefCount 관리하지 않는다.
		ACTOR_USERDATA* pActorUserData = reinterpret_cast<ACTOR_USERDATA*>(m_pActor->userData);
		pActorUserData->pOwner = nullptr;
		delete m_pActor->userData;
		m_pActor->userData = nullptr;
		m_pOwner = nullptr;
		m_pGameInstance->Get_Physx_Scene()->removeActor(*m_pActor);
		m_pActor->release();
		m_pActor = nullptr;
	}
	else
	{
		int a = 0;
	}

	__super::Free();
}
