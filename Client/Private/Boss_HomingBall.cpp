#include "stdafx.h"
#include "Boss_HomingBall.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "FresnelModelObject.h"

CBoss_HomingBall::CBoss_HomingBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_HomingBall::CBoss_HomingBall(const CBoss_HomingBall& _Prototype)
	: CProjectile_Monster(_Prototype)
    , m_pFresnelBuffer(_Prototype.m_pFresnelBuffer)
    , m_pColorBuffer(_Prototype.m_pColorBuffer)
{
    Safe_AddRef(m_pFresnelBuffer);
    Safe_AddRef(m_pColorBuffer);
}

HRESULT CBoss_HomingBall::Initialize_Prototype()
{
    FRESNEL_INFO tBulletFresnelInfo = {};
    tBulletFresnelInfo.fBaseReflect = 1.24f;
    tBulletFresnelInfo.fExp = -1.45f;
    tBulletFresnelInfo.vColor = { 0.66f, 0.327f, 0.22f, 1.f };
    tBulletFresnelInfo.fStrength = 1.f; // 안씀.
    m_pGameInstance->CreateConstBuffer(tBulletFresnelInfo, D3D11_USAGE_DEFAULT, &m_pFresnelBuffer);


    COLORS_INFO tColorsInfo = {};
    tColorsInfo.vDiffuseColor = _float4(1.f, 0.24f, 0.83f, 1.f);
    tColorsInfo.vBloomColor = _float4(0.49f, 0.17f, 0.0f, 0.38f);
    tColorsInfo.vSubColor = _float4(0.64f, 0.0f, 0.24f, 1.f);
    tColorsInfo.vInnerColor = _float4(0.665f, 0.005f, 0.f, 1.f);

    m_pGameInstance->CreateConstBuffer(tColorsInfo, D3D11_USAGE_DEFAULT, &m_pColorBuffer);


    return S_OK;
}

HRESULT CBoss_HomingBall::Initialize(void* _pArg)
{
    PROJECTILE_MONSTER_DESC* pDesc = static_cast<PROJECTILE_MONSTER_DESC*>(_pArg);

    m_fOriginSpeed = pDesc->tTransform3DDesc.fSpeedPerSec;
    pDesc->iNumPartObjects = PART_HOMING_END;

    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }
    Safe_Delete(pDesc->pActorDesc);


    m_fMinDistance = 10.f;

	return S_OK;
}

void CBoss_HomingBall::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_HomingBall::Update(_float _fTimeDelta)
{
    Homing(_fTimeDelta);

     __super::Update(_fTimeDelta);
}

void CBoss_HomingBall::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_HomingBall::Render()
{
    __super::Render();
    return S_OK;
}

void CBoss_HomingBall::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Enter(_My, _Other);
}

void CBoss_HomingBall::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CBoss_HomingBall::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

HRESULT CBoss_HomingBall::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "HOMINGBALL_Cleanup : NO PLAYER" << endl;
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

void CBoss_HomingBall::Active_OnEnable()
{
    __super::Active_OnEnable();

    if (nullptr != m_pEffectSystem)
    {
        m_pEffectSystem->Set_SpawnMatrix(m_pControllerTransform->Get_WorldMatrix_Ptr());
        m_pEffectSystem->Active_All(true);
    }
}

void CBoss_HomingBall::Active_OnDisable()
{
    m_pControllerTransform->Set_SpeedPerSec(m_fOriginSpeed);
    m_isHoming = false;
    XMStoreFloat3(&m_vDir, XMVectorZero());

    if (nullptr != m_pEffectSystem)
        m_pEffectSystem->Inactive_All();

    __super::Active_OnDisable();
}

void CBoss_HomingBall::Homing(_float _fTimeDelta)
{
    _vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();

    //if (false == m_isHoming)
    //{
    //    if (2.f <= m_fAccTime)
    //    {
    //        m_isHoming = true;
    //        m_pControllerTransform->Set_SpeedPerSec(m_fOriginSpeed * 3.f);
    //    }
    //    m_pControllerTransform->Go_Straight(_fTimeDelta);
    //}
    //else
    //{
    // 
    //    m_pControllerTransform->Set_AutoRotationYDirection(vDir);
    //    m_pControllerTransform->Update_AutoRotation(_fTimeDelta);
    //    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);
    //}

    if (false == m_isHoming)
    {
        _vector vLook = Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);
        if (1.f <= m_fAccTime)
        {
            m_isHoming = true;
            m_pControllerTransform->Set_SpeedPerSec(m_fOriginSpeed * 4.f);
            XMStoreFloat3(&m_vDir, XMVector3Normalize(vDir));
        }
    }
    else
    {
        _float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMLoadFloat3(&m_vDir)));
        //거리가 가깝지 않고 뒤에 있지 않을 때 방향전환
        if (m_fMinDistance < XMVectorGetX(XMVector3Length(vDir)) && 0.f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMLoadFloat3(&m_vDir))))
        {
            XMStoreFloat3(&m_vDir, XMVector3Normalize(vDir));
        }

        _vector vLook = Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK);
		_float fScaleZ = Get_ControllerTransform()->Get_Scale().z;
		Get_ControllerTransform()->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(XMVectorLerp(XMVector3Normalize(vLook), XMLoadFloat3(&m_vDir), 0.2f)) * fScaleZ);
    }

    m_pControllerTransform->Go_Straight(_fTimeDelta);
}

HRESULT CBoss_HomingBall::Ready_ActorDesc(void* _pArg)
{
    PROJECTILE_MONSTER_DESC* pDesc = static_cast<PROJECTILE_MONSTER_DESC*>(_pArg);

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    CActor::ACTOR_DESC* ActorDesc = new CActor::ACTOR_DESC;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc->pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc->FreezeRotation_XYZ[0] = false;
    ActorDesc->FreezeRotation_XYZ[1] = false;
    ActorDesc->FreezeRotation_XYZ[2] = false;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc->FreezePosition_XYZ[0] = false;
    ActorDesc->FreezePosition_XYZ[1] = false;
    ActorDesc->FreezePosition_XYZ[2] = false;

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
    ShapeDesc->fRadius = 2.f;
    ShapeDesc->fHalfHeight = 2.f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    //XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(ShapeDesc->fHalfHeight, ShapeDesc->fRadius + ShapeDesc->fHalfHeight, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
	XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 0.f, 0.f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER_PROJECTILE;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CBoss_HomingBall::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_HomingBall::Ready_PartObjects()
{
    CFresnelModelObject::FRESNEL_MODEL_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();


    //테스트용, 원 게임에서는 구체인데 일단 불릿 모델 써봄
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::NOISEFRESNEL;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_PARTICLE;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    BodyDesc.pFresnelBuffer = m_pFresnelBuffer;
    BodyDesc.pColorBuffer = m_pColorBuffer;
    BodyDesc.szDiffusePrototypeTag = L"Prototype_Component_Texture_BossProjectileMain1";
    BodyDesc.szNoisePrototypeTag = L"Prototype_Component_Texture_BossProjectileNoise1";
    BodyDesc.vDiffuseScaling = { 1.f, 1.f };
    BodyDesc.vNoiseScaling = { 3.f, 2.f };

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_FresnelModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_UseAlbedoMap(0, false, true);
    static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(1.f, 0.f, 0.f, 1.f), true);

    // Effect 생성.
    CEffect_System::EFFECT_SYSTEM_DESC EffectDesc = {};
    EffectDesc.eStartCoord = COORDINATE_3D;
    EffectDesc.isCoordChangeEnable = false;
    EffectDesc.iSpriteShaderLevel = LEVEL_STATIC;
    EffectDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
    EffectDesc.iEffectShaderLevel = LEVEL_STATIC;
    EffectDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
    EffectDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";

    m_PartObjects[PART_EFFECT] = m_pEffectSystem = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("HomingBallDust.json"), &EffectDesc));
    if (nullptr != m_pEffectSystem)
    {
        m_pEffectSystem->Set_SpawnMatrix(m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
        m_pEffectSystem->Active_All(true);
    }
    Safe_AddRef(m_pEffectSystem);
    
    

    return S_OK;
}

CBoss_HomingBall* CBoss_HomingBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_HomingBall* pInstance = new CBoss_HomingBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_HomingBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_HomingBall::Clone(void* _pArg)
{
    CBoss_HomingBall* pInstance = new CBoss_HomingBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_HomingBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_HomingBall::Free()
{
    Safe_Release(m_pEffectSystem);

    Safe_Release(m_pColorBuffer);
    Safe_Release(m_pFresnelBuffer);

    __super::Free();
}
