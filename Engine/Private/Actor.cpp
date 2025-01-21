#include "Actor.h"
#include "GameInstance.h"
#include "ActorObject.h"

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
}

HRESULT CActor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActor::Initialize(void* _pArg)
{
    ACTOR_DESC* pDesc = static_cast<ACTOR_DESC*>(_pArg);
    if (nullptr == pDesc->pOwner)
        return E_FAIL;

    // Save Desc 
    m_pOwner = pDesc->pOwner;                       // 순환 참조로 인해 RefCount 관리 X
    if (pDesc->OffsetMatrix._44 != 0)                       // m_OffsetMatrix은 항등행렬로 초기화 된 상태임 Desc에 값을 채운경우에만, Offset 적용.
        m_OffsetMatrix = pDesc->OffsetMatrix;

    // Add Desc

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Shapes(pDesc->ShapeDatas)))
        return E_FAIL;

    if (FAILED(Ready_Actor(pDesc)))
        return E_FAIL;

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
}



HRESULT CActor::Ready_Actor(ACTOR_DESC* _pActorDesc)
{
    PxPhysics* pPhysic = m_pGameInstance->Get_Physics();
    PxTransform Transform = PxTransform();
    _vector vOwnerPos = m_pOwner->Get_Position();

    _vector vScale = {};
    _vector vPosition = {};
    _vector vQuat = {};
    XMMatrixDecompose(&vScale, &vQuat, &vPosition, XMLoadFloat4x4(&m_OffsetMatrix));
    vPosition += vOwnerPos;
    Transform.p = PxVec3(XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition));
    Transform.q = PxQuat(XMVectorGetX(vQuat), XMVectorGetY(vQuat), XMVectorGetZ(vQuat), XMVectorGetW(vQuat));
    /* 1. RigidBody Type Check */
    switch (m_eActorType)
    {
    case Engine::ACTOR_TYPE::STATIC:
        m_pActor = pPhysic->createRigidStatic(Transform);
        break;
    case Engine::ACTOR_TYPE::KINEMATIC:
    case Engine::ACTOR_TYPE::DYNAMIC:
        m_pActor = pPhysic->createRigidDynamic(Transform);
        static_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, _pActorDesc->FreezeRotation_XYZ[0]);
        static_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, _pActorDesc->FreezeRotation_XYZ[1]);
        static_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, _pActorDesc->FreezeRotation_XYZ[2]);
        static_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X,  _pActorDesc->FreezePosition_XYZ[0]);
        static_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y,  _pActorDesc->FreezePosition_XYZ[1]);
        static_cast<PxRigidDynamic*>(m_pActor)->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z,  _pActorDesc->FreezePosition_XYZ[2]);
        break;
    default:
        return E_FAIL;
    }

    if(ACTOR_TYPE::KINEMATIC == m_eActorType)
        static_cast<PxRigidDynamic*>(m_pActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

    return S_OK;
}

HRESULT CActor::Ready_Shapes(const vector<SHAPE_DATA>& ShapeDescs)
{
    PxPhysics* pPhysics = m_pGameInstance->Get_Physics();
    if (nullptr == pPhysics)
        return E_FAIL;

    /* Shape 생성 코드 시작. */
    PxMaterial* pShapeMaterial = m_pGameInstance->Get_Material(ACTOR_MATERIAL::DEFAULT);
    for (_uint i = 0; i < ShapeDescs.size(); ++i)
    {
        PxShapeFlags ShapeFlags = (PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE);

        /* IsTrigger Check */
        if (true == ShapeDescs[i].isTrigger)
        {
            ShapeFlags &= ~PxShapeFlag::eSIMULATION_SHAPE;
            ShapeFlags |= PxShapeFlag::eTRIGGER_SHAPE;
        }

        /* Material 생성 */
        if (true == ShapeDescs[i].isShapeMaterial)
        {
            PHYSX_RELEASE(pShapeMaterial);
            pShapeMaterial = m_pGameInstance->Get_Material(ShapeDescs[i].eMaterial);
        }


        /* 세부 Shape Geometry 생성 */
        PxShape* pShape = nullptr;
        switch (ShapeDescs[i].eShapeType)
        {
        case Engine::SHAPE_TYPE::BOX:
        {
            SHAPE_BOX_DESC* pDesc = static_cast<SHAPE_BOX_DESC*>(ShapeDescs[i].pShapeDesc);
            PxGeometry BoxGeometry = PxBoxGeometry(PxVec3(pDesc->vHalfExtents.x, pDesc->vHalfExtents.y, pDesc->vHalfExtents.z));
            pShape = pPhysics->createShape(BoxGeometry, *pShapeMaterial, ShapeFlags);
        }
            break;
        case Engine::SHAPE_TYPE::SPHERE:
        {
            SHAPE_SPHERE_DESC* pDesc = static_cast<SHAPE_SPHERE_DESC*>(ShapeDescs[i].pShapeDesc);
            PxSphereGeometry SphereGeometry = PxSphereGeometry(pDesc->fRadius);
            pShape = pPhysics->createShape(SphereGeometry, *pShapeMaterial, ShapeFlags);
        }
            break;
        case Engine::SHAPE_TYPE::CAPSULE:
        {
            SHAPE_CAPSULE_DESC* pDesc = static_cast<SHAPE_CAPSULE_DESC*>(ShapeDescs[i].pShapeDesc);
            PxCapsuleGeometry CapsuleGeometry = PxCapsuleGeometry(pDesc->fRadius, pDesc->fHalfHeight);
            pShape = pPhysics->createShape(CapsuleGeometry, *pShapeMaterial, ShapeFlags);
        }
            break;
        default:
            return E_FAIL;
        }
        pShape->setLocalPose(PxTransform(PxMat44((_float*)(&ShapeDescs[i].LocalOffsetMatrix))));
        m_pShapes.push_back(pShape);
    }

    return S_OK;
}

void CActor::Free()
{
    // 순환 참조로 인해 RefCount 관리하지 않는다.
    m_pOwner = nullptr;

    // PhysX Release
    PHYSX_RELEASE(m_pActor);

    __super::Free();
}
