#include "stdafx.h"
#include "Boss_TennisBall.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "ButterGrump_Shield.h"
#include "ButterGrump.h"

CBoss_TennisBall::CBoss_TennisBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_TennisBall::CBoss_TennisBall(const CBoss_TennisBall& _Prototype)
    : CProjectile_Monster(_Prototype)
{
}

HRESULT CBoss_TennisBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_TennisBall::Initialize(void* _pArg)
{
    BOSS_TENNISBALL_DESC* pDesc = static_cast<BOSS_TENNISBALL_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iObjectGroupID = OBJECT_GROUP::BOSS_PROJECTILE;

    m_pSpawner = pDesc->pSpawner;

    if (nullptr != m_pSpawner)
        Safe_AddRef(m_pSpawner);

	return S_OK;
}

void CBoss_TennisBall::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_TennisBall::Update(_float _fTimeDelta)
{
    if(false == m_isShoot)
    {
        Get_ControllerTransform()->Go_Straight(_fTimeDelta);
    }
    else
    {
		Get_ControllerTransform()->Go_Direction(Get_FinalPosition() - m_pTarget->Get_FinalPosition(), Get_ControllerTransform()->Get_SpeedPerSec() * 3.f, _fTimeDelta);
    }

    if(KEY_PRESSING(KEY::CTRL))
    {
        if (KEY_DOWN(KEY::NUMPAD1))
        {
            m_isShoot = true;
        }
    }

    __super::Update(_fTimeDelta);
}

void CBoss_TennisBall::Late_Update(_float _fTimeDelta)
{

  	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_TennisBall::Render()
{
    __super::Render();
    return S_OK;
}

void CBoss_TennisBall::Shoot()
{
    //회전
}

void CBoss_TennisBall::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if ((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
            XMVectorSetY(vRepulse, -1.f);
            Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), 1, vRepulse);
            //Event_KnockBack(static_cast<CCharacter*>(_My.pActorUserData->pOwner), vRepulse);
        }
    }

    if (OBJECT_GROUP::PLAYER_PROJECTILE & _Other.pActorUserData->iObjectGroup)
    {
        if(true == m_isShoot)
        {
            m_iHp -= 1;
            if (0 >= m_iHp)
            {
                m_isShoot = true;
            }
        }
    }

    if (OBJECT_GROUP::BOSS & _Other.pActorUserData->iObjectGroup)
    {
        if ((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeIndex)
        {
            CButterGrump_Shield* pShield = dynamic_cast<CButterGrump_Shield*>(_Other.pActorUserData->pOwner);
            if (nullptr != pShield)
            {
                pShield->Shield_Break(_My);
                Event_DeleteObject(this);
            }
        }
        
    }

    //if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
    //    Event_DeleteObject(this);
}

void CBoss_TennisBall::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CBoss_TennisBall::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

HRESULT CBoss_TennisBall::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "ENERGYBALL_Cleanup : NO PLAYER" << endl;
#endif // _DEBUG
        return S_OK;
    }

    if (true == m_pTarget->Is_Dead())
    {
        Safe_Release(m_pTarget);
        m_pTarget = nullptr;
    }

    return S_OK;
}

void CBoss_TennisBall::Active_OnEnable()
{
    __super::Active_OnEnable();

    m_iHp = 30;
    m_isShoot = false;
}

void CBoss_TennisBall::Active_OnDisable()
{
    if (nullptr != m_pSpawner)
    {
        m_pSpawner->Set_SpawnOrb(false);
    }

    __super::Active_OnDisable();
}

HRESULT CBoss_TennisBall::Ready_ActorDesc(void* _pArg)
{
    PROJECTILE_MONSTER_DESC* pDesc = static_cast<PROJECTILE_MONSTER_DESC*>(_pArg);

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc->pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc->FreezeRotation_XYZ[0] = true;
    ActorDesc->FreezeRotation_XYZ[1] = false;
    ActorDesc->FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc->FreezePosition_XYZ[0] = false;
    ActorDesc->FreezePosition_XYZ[1] = false;
    ActorDesc->FreezePosition_XYZ[2] = false;

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_SPHERE_DESC* ShapeDesc = new SHAPE_SPHERE_DESC;
    ShapeDesc->fRadius = 20.f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::BOSS_PROJECTILE;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::BOSS;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CBoss_TennisBall::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_TennisBall::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("TennisBall_01");
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(0.f, 0.7f, 0.f, 1.f));

    return S_OK;
}

CBoss_TennisBall* CBoss_TennisBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_TennisBall* pInstance = new CBoss_TennisBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_TennisBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_TennisBall::Clone(void* _pArg)
{
    CBoss_TennisBall* pInstance = new CBoss_TennisBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_TennisBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_TennisBall::Free()
{
    Safe_Release(m_pSpawner);

    __super::Free();
}
