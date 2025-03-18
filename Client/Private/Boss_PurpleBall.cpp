#include "stdafx.h"
#include "Boss_PurpleBall.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "FresnelModelObject.h"
#include "Effect_Manager.h"
#include "Player.h"

CBoss_PurpleBall::CBoss_PurpleBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_PurpleBall::CBoss_PurpleBall(const CBoss_PurpleBall& _Prototype)
    : CProjectile_Monster(_Prototype)
    , m_pFresnelBuffer(_Prototype.m_pFresnelBuffer)
    , m_pColorBuffer(_Prototype.m_pColorBuffer)
{
    Safe_AddRef(m_pFresnelBuffer);
    Safe_AddRef(m_pColorBuffer);
}

HRESULT CBoss_PurpleBall::Initialize_Prototype()
{
    FRESNEL_INFO tBulletFresnelInfo = {};
    tBulletFresnelInfo.fBaseReflect = -0.3f;
    tBulletFresnelInfo.fExp = 1.13f;
    tBulletFresnelInfo.vColor = { 1.f, 0.73f, 1.f, 1.f };
    tBulletFresnelInfo.fStrength = 1.f; // 안씀.
    m_pGameInstance->CreateConstBuffer(tBulletFresnelInfo, D3D11_USAGE_DEFAULT, &m_pFresnelBuffer);


    COLORS_INFO tColorsInfo = {};
    tColorsInfo.vDiffuseColor = _float4(0.96f, 0.25f, 0.94f, 1.08f);
    tColorsInfo.vBloomColor = _float4(0.21f, 0.15f, 0.46f, 0.49f);
    tColorsInfo.vSubColor = _float4(0.38f, 0.1f, 0.15f, 0.89f);
    tColorsInfo.vInnerColor = _float4(0.2f, 0.f, 0.23f, 1.f);
    m_pGameInstance->CreateConstBuffer(tColorsInfo, D3D11_USAGE_DEFAULT, &m_pColorBuffer);


	return S_OK;
}

HRESULT CBoss_PurpleBall::Initialize(void* _pArg)
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

void CBoss_PurpleBall::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_PurpleBall::Update(_float _fTimeDelta)
{

    /*_vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();
    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);*/
    m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CBoss_PurpleBall::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_PurpleBall::Render()
{
    __super::Render();
    return S_OK;
}

void CBoss_PurpleBall::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if ((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            if(false == static_cast<CPlayer*>(_Other.pActorUserData->pOwner)->Is_Invincible())
            {
                _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
                XMVectorSetY(vRepulse, -1.f);
                Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), 1, vRepulse);
                Event_DeleteObject(this);
                // Effect
                CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("PurpleHit"), true, m_pControllerTransform->Get_WorldMatrix());
            }
        }

    }
}

HRESULT CBoss_PurpleBall::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
        return S_OK;
    }

    if (true == m_pTarget->Is_Dead())
    {
        Safe_Release(m_pTarget);
        m_pTarget = nullptr;
    }

    return S_OK;
}

void CBoss_PurpleBall::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CBoss_PurpleBall::Active_OnDisable()
{
    __super::Active_OnDisable();
}

HRESULT CBoss_PurpleBall::Ready_ActorDesc(void* _pArg)
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
    ShapeDesc->fRadius = 1.f;

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
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER_PROJECTILE;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CBoss_PurpleBall::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_PurpleBall::Ready_PartObjects()
{
    CFresnelModelObject::FRESNEL_MODEL_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

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
    static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(0.625f, 0.125f, 0.9375f, 1.f), true);

    return S_OK;
}

CBoss_PurpleBall* CBoss_PurpleBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_PurpleBall* pInstance = new CBoss_PurpleBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_PurpleBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_PurpleBall::Clone(void* _pArg)
{
    CBoss_PurpleBall* pInstance = new CBoss_PurpleBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_PurpleBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_PurpleBall::Free()
{
    Safe_Release(m_pFresnelBuffer);
    Safe_Release(m_pColorBuffer);

    __super::Free();
}
