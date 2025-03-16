#include "stdafx.h"
#include "ButterGrump_Shield.h"
#include "Actor_Dynamic.h"
#include "3DModel.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "ButterGrump.h"
#include "Effect_Manager.h"


CButterGrump_Shield::CButterGrump_Shield(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFresnelModelObject(_pDevice, _pContext)
{
}


CButterGrump_Shield::CButterGrump_Shield(const CButterGrump_Shield& _Prototype)
    :CFresnelModelObject(_Prototype)
{
    m_pFresnelBuffer = _Prototype.m_pFresnelBuffer;
    Safe_AddRef(m_pFresnelBuffer);
    m_pColorBuffer = _Prototype.m_pColorBuffer;
    Safe_AddRef(m_pColorBuffer);
}

HRESULT CButterGrump_Shield::Initialize_Prototype()
{
    FRESNEL_INFO tBulletFresnelInfo = {};
    tBulletFresnelInfo.fBaseReflect = 0.19f;
    tBulletFresnelInfo.fExp = 1.99f;
    tBulletFresnelInfo.vColor = { 1.f, 1.f, 1.f, 2.f };
    tBulletFresnelInfo.fStrength = 1.f; // 안씀.
    m_pGameInstance->CreateConstBuffer(tBulletFresnelInfo, D3D11_USAGE_DEFAULT, &m_pFresnelBuffer);


    COLORS_INFO tColorsInfo = {};
    tColorsInfo.vDiffuseColor = _float4(0.25f, 0.4f, 0.8f, 0.15f);
    tColorsInfo.vBloomColor = _float4(0.12f, 0.22f, 0.5f, 0.69f);
    tColorsInfo.vSubColor = _float4(0.8f, 0.8f, 0.8f, 0.05f);
    tColorsInfo.vInnerColor = _float4(0.38f, 0.41f, 1.f, 1.f);

    m_pGameInstance->CreateConstBuffer(tColorsInfo, D3D11_USAGE_DEFAULT, &m_pColorBuffer);
    return S_OK;
}

HRESULT CButterGrump_Shield::Initialize(void* _pArg)
{
    //Part Sword
    BUTTERGRUMP_SHIELD_DESC* pDesc = static_cast<BUTTERGRUMP_SHIELD_DESC*>(_pArg);

    pDesc->isCoordChangeEnable = false;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->iObjectGroupID = OBJECT_GROUP::BOSS;

    pDesc->szDiffusePrototypeTag = L"Prototype_Component_Texture_BossShieldMain";
    pDesc->szNoisePrototypeTag = L"Prototype_Component_Texture_BossShieldNoise";

    m_pParent = pDesc->pParent;

    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);


    static_cast<C3DModel*>(Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(1.f, 1.f, 1.f, 1.f), true);
    //static_cast<C3DModel*>(Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(1.f, 1.f, 1.f, 0.5f), true);

    m_vDiffuseScaling = _float2(0.f, 1.f);
    m_vNoiseScaling = _float2(1.f, 3.f);


    return S_OK;
}

void CButterGrump_Shield::Update(_float _fTimeDelta)
{
    //list<CActorObject*>OnList;
    //PxSphereGeometry pxGeom;
    //Get_ActorCom()->Get_Shapes()[0]->getSphereGeometry(pxGeom);
    //
    //if (m_pGameInstance->Overlap(&pxGeom, Get_FinalPosition(), OnList))
    //{
    //    for (CActorObject* pActorObject : OnList)
    //    {
    //        if (OBJECT_GROUP::BOSS_PROJECTILE & pActorObject->Get_ObjectGroupID())
    //        {
    //            static_cast<CButterGrump*>(m_pParent)->Shield_Break();
    //        }
    //    }
    //}

    __super::Update(_fTimeDelta);
}

void CButterGrump_Shield::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CButterGrump_Shield::Render()
{
#ifdef _DEBUG
#endif
    if(Is_Active())
        return __super::Render();
    return S_OK;
}

void CButterGrump_Shield::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup
        && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse
        && (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
    {
        _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
        Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), m_pParent->Get_Stat().iDamg, vRepulse);
    }
}

void CButterGrump_Shield::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CButterGrump_Shield::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CButterGrump_Shield::Active_OnEnable()
{
    __super::Active_OnEnable();

    Get_ActorCom()->Set_AllShapeEnable(true);
}

void CButterGrump_Shield::Active_OnDisable()
{
    Get_ActorCom()->Set_AllShapeEnable(false);

    __super::Active_OnDisable();
}

void CButterGrump_Shield::Shield_Break(const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::BOSS_PROJECTILE & _Other.pActorUserData->iObjectGroup)
    {
        if(nullptr != m_pParent)
        {
            static_cast<CButterGrump*>(m_pParent)->Shield_Break();
        }

        CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("Shieldbreak"), true, Get_FinalPosition());
    }
}

HRESULT CButterGrump_Shield::Ready_ActorDesc(void* _pArg)
{
    CButterGrump_Shield::BUTTERGRUMP_SHIELD_DESC* pDesc = static_cast<CButterGrump_Shield::BUTTERGRUMP_SHIELD_DESC*>(_pArg);

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
    ShapeDesc->fRadius = 17.f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(ShapeDesc->fRadius * (-1.f), 0.f, 0.f)); // Shape의 LocalOffset을 행렬정보로 저장.
	//XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(8.0f, 0.f, -12.5f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::BOSS;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

CButterGrump_Shield* CButterGrump_Shield::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CButterGrump_Shield* pInstance = new CButterGrump_Shield(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : ButterGrump_Shield");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButterGrump_Shield::Clone(void* _pArg)
{
    CButterGrump_Shield* pInstance = new CButterGrump_Shield(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : ButterGrump_Shield");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButterGrump_Shield::Free()
{
    m_pParent = nullptr;

    __super::Free();
}
