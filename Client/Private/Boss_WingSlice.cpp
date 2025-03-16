#include "stdafx.h"
#include "Boss_WingSlice.h"
#include "FresnelModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "Effect_Trail.h"
#include "Effect_Manager.h"

CBoss_WingSlice::CBoss_WingSlice(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_WingSlice::CBoss_WingSlice(const CBoss_WingSlice& _Prototype)
    : CProjectile_Monster(_Prototype)
    , m_pColorBuffer(_Prototype.m_pColorBuffer)
    , m_pFresnelBuffer(_Prototype.m_pFresnelBuffer)
{
    Safe_AddRef(m_pColorBuffer);
    Safe_AddRef(m_pFresnelBuffer);
}

HRESULT CBoss_WingSlice::Initialize_Prototype()
{
    FRESNEL_INFO tBulletFresnelInfo = {};
    tBulletFresnelInfo.fBaseReflect = 0.18f;
    tBulletFresnelInfo.fExp = 2.9f;
    tBulletFresnelInfo.vColor = { 0.66f, 1.f, 0.f, 1.f };
    tBulletFresnelInfo.fStrength = 1.f; // 안씀.
    m_pGameInstance->CreateConstBuffer(tBulletFresnelInfo, D3D11_USAGE_DEFAULT, &m_pFresnelBuffer);


    COLORS_INFO tColorsInfo = {};
    tColorsInfo.vDiffuseColor = _float4(0.97f, 0.55f, 0.12f, 1.f);
    tColorsInfo.vBloomColor = _float4(0.08f, 0.32f, 0.21f, 0.2f);
    tColorsInfo.vSubColor = _float4(0.27f, 1.f, 0.6f, 0.1f);
    tColorsInfo.vInnerColor = _float4(0.7f, 0.7f, 0.7f, 1.f);

    m_pGameInstance->CreateConstBuffer(tColorsInfo, D3D11_USAGE_DEFAULT, &m_pColorBuffer);

	return S_OK;
}

HRESULT CBoss_WingSlice::Initialize(void* _pArg)
{
    PROJECTILE_MONSTER_DESC* pDesc = static_cast<PROJECTILE_MONSTER_DESC*>(_pArg);

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

	return S_OK;
}

void CBoss_WingSlice::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_WingSlice::Update(_float _fTimeDelta)
{

    /*_vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();
    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);*/
    m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CBoss_WingSlice::Late_Update(_float _fTimeDelta)
{

  	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_WingSlice::Render()
{
    __super::Render();
    return S_OK;
}

void CBoss_WingSlice::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Enter(_My, _Other);
}

void CBoss_WingSlice::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CBoss_WingSlice::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

HRESULT CBoss_WingSlice::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "WingSlice_Cleanup : NO PLAYER" << endl;
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

void CBoss_WingSlice::Active_OnEnable()
{
    if (nullptr != m_pTrailEffect)
        m_pTrailEffect->Delete_Effect();

    CEffect_Manager::GetInstance()->Active_Effect(TEXT("WingSliceDust"), true, m_pControllerTransform->Get_WorldMatrix_Ptr());

    __super::Active_OnEnable();
}

void CBoss_WingSlice::Active_OnDisable()
{
    CEffect_Manager::GetInstance()->Stop_Spawn(TEXT("WingSliceDust"), 0.25f);

    __super::Active_OnDisable();
}

HRESULT CBoss_WingSlice::Ready_ActorDesc(void* _pArg)
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
    SHAPE_CAPSULE_DESC* ShapeDesc = new SHAPE_CAPSULE_DESC;
    ShapeDesc->fRadius = 2.f;
    ShapeDesc->fHalfHeight = 20.f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

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

HRESULT CBoss_WingSlice::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_WingSlice::Ready_PartObjects()
{
    CFresnelModelObject::FRESNEL_MODEL_DESC BodyDesc{};

    m_PartObjects.resize(WINGSLICE_PART_END);

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sonic_03");
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::NOISEFRESNEL;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_PARTICLE;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(20.f, 20.f, 20.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    BodyDesc.pFresnelBuffer = m_pFresnelBuffer;
    BodyDesc.pColorBuffer = m_pColorBuffer;
    BodyDesc.szDiffusePrototypeTag = L"Prototype_Component_Texture_BossWingAttack";
    BodyDesc.vDiffuseScaling = { 1.f, 1.f };
    BodyDesc.vNoiseScaling = { 0.f, 0.f };

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_FresnelModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_UseAlbedoMap(0, false, true);
    static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(1.f, 1.f, 1.f, 1.f), true);

    //m_PartObjects[PART_BODY]->Get_ControllerTransform()->RotationXYZ(_float3(45.f, 0.f, -45.f));
    m_PartObjects[PART_BODY]->Get_ControllerTransform()->RotationQuaternion(_float3(XMConvertToRadians(0.f), XMConvertToRadians(90.f),XMConvertToRadians(0.f)));


  /*  CEffect_System::EFFECT_SYSTEM_DESC EffectSystemDesc = {};
    EffectSystemDesc.iCurLevelID = m_iCurLevelID;
    EffectSystemDesc.isCoordChangeEnable = false;
    EffectSystemDesc.iSpriteShaderLevel = LEVEL_STATIC;
    EffectSystemDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
    EffectSystemDesc.iEffectShaderLevel = LEVEL_STATIC;
    EffectSystemDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
    EffectSystemDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";

    m_PartObjects[WINGSLICE_PART_EFFECT] = m_pDustEffect = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("WingSliceDust.json"), &EffectSystemDesc));
    if (nullptr != m_pDustEffect)
    {
        m_pDustEffect->Set_ParentMatrix(COORDINATE_3D, m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
    }

    Safe_AddRef(m_pDustEffect);

    if (nullptr != m_pDustEffect)
        m_pDustEffect->Active_All();*/

    CEffect_Trail::EFFECTTRAIL_DESC TrailDesc = {};
    TrailDesc.eStartCoord = COORDINATE_3D;
    TrailDesc.iCurLevelID = m_iCurLevelID;
    TrailDesc.isCoordChangeEnable = false;
    TrailDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr();
    TrailDesc.fLength = 40.f;
    TrailDesc.vAddPoint = _float3(0.f, 20.f, -1.5f);
    TrailDesc.vColor = _float4(0.2f, 1.f, 0.6f, 1.f);
    TrailDesc.szTextureTag = L"Prototype_Component_Texture_WingSlam";
    TrailDesc.iTextureLevel = m_iCurLevelID;
    TrailDesc.szBufferTag = L"Prototype_Component_VIBuffer_Trail64";
    TrailDesc.fAddTime = 0.03f;
    TrailDesc.fTrailLifeTime = 0.4f;
    TrailDesc.eTrailType = CEffect_Trail::FOLLOW_TRAIL;

    m_PartObjects[WINGSLICE_PART_TRAIL] = m_pTrailEffect = static_cast<CEffect_Trail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_EffectTrail"), &TrailDesc));

    if (nullptr != m_pTrailEffect)
    {
        m_pTrailEffect->Get_ControllerTransform()->Rotation(XMConvertToRadians(90.f), XMVectorSet(0.f, 0.f, 1.f, 0.f));
        m_pTrailEffect->Set_AddUpdate(true);
    }

    Safe_AddRef(m_pTrailEffect);

    return S_OK;
}

CBoss_WingSlice* CBoss_WingSlice::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_WingSlice* pInstance = new CBoss_WingSlice(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_WingSlice");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_WingSlice::Clone(void* _pArg)
{
    CBoss_WingSlice* pInstance = new CBoss_WingSlice(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_WingSlice");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_WingSlice::Free()
{
    Safe_Release(m_pColorBuffer);
    Safe_Release(m_pFresnelBuffer);
    Safe_Release(m_pTrailEffect);

    __super::Free();
}
