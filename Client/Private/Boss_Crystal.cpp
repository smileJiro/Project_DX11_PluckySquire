#include "stdafx.h"
#include "Boss_Crystal.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "Pooling_Manager.h"
#include "ButterGrump.h"
#include "Effect_Manager.h"

CBoss_Crystal::CBoss_Crystal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_Crystal::CBoss_Crystal(const CBoss_Crystal& _Prototype)
    : CProjectile_Monster(_Prototype)
{
}

HRESULT CBoss_Crystal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Crystal::Initialize(void* _pArg)
{
    BOSS_CRYSTAL_DESC* pDesc = static_cast<BOSS_CRYSTAL_DESC*>(_pArg);


    pDesc->_tStat.iMaxHP = 10;
    pDesc->_tStat.iHP = 10;
    pDesc->_tStat.iDamg = 1;

    if(FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;


    m_pSpawner = pDesc->pSpawner;

    if (nullptr != m_pSpawner)
        Safe_AddRef(m_pSpawner);

	return S_OK;
}

void CBoss_Crystal::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_Crystal::Update(_float _fTimeDelta)
{

    /*_vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();
    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);*/
    m_pControllerTransform->Go_Straight(_fTimeDelta);


    if (KEY_PRESSING(KEY::CTRL))
    {
        if (KEY_DOWN(KEY::NUMPAD1))
        {
            Event_DeleteObject(this);
        }
    }

    __super::Update(_fTimeDelta);
}

void CBoss_Crystal::Late_Update(_float _fTimeDelta)
{

  	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_Crystal::Render()
{
    __super::Render();
    return S_OK;
}

void CBoss_Crystal::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
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
}

void CBoss_Crystal::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CBoss_Crystal::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CBoss_Crystal::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    m_tStat.iHP -= _iDamg;
    if (0 >= m_tStat.iHP && false == Is_Dead())
    {
        CEffect_Manager::GetInstance()->Active_EffectMatrix(TEXT("Meteorite"), true, m_pControllerTransform->Get_WorldMatrix());

        Event_DeleteObject(this);
    }
}

HRESULT CBoss_Crystal::Cleanup_DeadReferences()
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

void CBoss_Crystal::Active_OnEnable()
{
    __super::Active_OnEnable();

    m_tStat.iHP = m_tStat.iMaxHP;
}

void CBoss_Crystal::Active_OnDisable()
{
    if(false == m_isTimeOver)
    {
        _float3 vPosition;
        _float4 vRotation;
        m_pGameInstance->MatrixDecompose(nullptr, &vRotation, &vPosition, Get_FinalWorldMatrix());

        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Boss_TennisBall"), COORDINATE_3D, &vPosition, &vRotation);
    }

    else
    {
        if (nullptr != m_pSpawner)
        {
            m_pSpawner->Set_SpawnOrb(false);
        }
    }

    __super::Active_OnDisable();
}

HRESULT CBoss_Crystal::Ready_ActorDesc(void* _pArg)
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
    ShapeDesc->fRadius = 2.f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::BOSS_PROJECTILE;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CBoss_Crystal::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_Crystal::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Crystal_01_LowPoly");
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(2.f, 2.f, 2.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBoss_Crystal* CBoss_Crystal::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_Crystal* pInstance = new CBoss_Crystal(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_Crystal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_Crystal::Clone(void* _pArg)
{
    CBoss_Crystal* pInstance = new CBoss_Crystal(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_Crystal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Crystal::Free()
{
    Safe_Release(m_pSpawner);

    __super::Free();
}
