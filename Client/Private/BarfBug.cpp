#include "stdafx.h"
#include "BarfBug.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Projectile_BarfBug.h"

CBarfBug::CBarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CBarfBug::CBarfBug(const CBarfBug& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CBarfBug::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBarfBug::Initialize(void* _pArg)
{
    CBarfBug::MONSTER_DESC* pDesc = static_cast<CBarfBug::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 3.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;

    //pDesc->fAlertRange = 5.f;
    //pDesc->fChaseRange = 12.f;
    //pDesc->fAttackRange = 10.f;
    //pDesc->fDelayTime = 1.f;
    //pDesc->fCoolTime = 3.f;

    pDesc->fAlertRange = 1.f;
    pDesc->fChaseRange = 5.f;
    pDesc->fAttackRange = 3.f;
    pDesc->fDelayTime = 1.f;
    pDesc->fCoolTime = 3.f;


    /* Create Test Actor (Desc를 채우는 함수니까. __super::Initialize() 전에 위치해야함. )*/
    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pFSM->Add_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::PATROL);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ALERT);
    m_pFSM->Add_State((_uint)MONSTER_STATE::CHASE);
    m_pFSM->Add_State((_uint)MONSTER_STATE::ATTACK);
    m_pFSM->Set_State((_uint)MONSTER_STATE::IDLE);
    m_pFSM->Set_PatrolBound();

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, WALK, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, BARF, true);
    pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CBarfBug::Animation_End, this, placeholders::_1, placeholders::_2));


    /*  Projectile  */
    Pooling_DESC Pooling_Desc;
    Pooling_Desc.iPrototypeLevelID = LEVEL_STATIC;
    Pooling_Desc.strLayerTag = TEXT("Layer_Monster");
    Pooling_Desc.strPrototypeTag = TEXT("Prototype_GameObject_Projectile_BarfBug");

    CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC* pProjDesc = new CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC;
    pProjDesc->fLifeTime = 5.f;
    pProjDesc->eStartCoord = COORDINATE_3D;
    pProjDesc->isCoordChangeEnable = false;
    pProjDesc->iNumPartObjects = PART_LAST;
    pProjDesc->iCurLevelID = m_iCurLevelID;

    pProjDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pProjDesc->tTransform2DDesc.fSpeedPerSec = 3.f;

    pProjDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pProjDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    CPooling_Manager::GetInstance()->Register_PoolingObject(TEXT("Pooling_Projectile_BarfBug"), Pooling_Desc, pProjDesc);


    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

   Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CBarfBug::Priority_Update(_float _fTimeDelta)
{
    if (true == m_isDelay)
    {
        m_fAccTime += _fTimeDelta;

        if (m_fDelayTime <= m_fAccTime)
        {
            Delay_Off();
            if (3 <= m_iAttackCount)
            {
                CoolTime_On();
            }
        }
    }

    if (true == m_isCool)
    {
        m_fAccTime += _fTimeDelta;

        if (m_fCoolTime <= m_fAccTime)
        {
            CoolTime_Off();
            m_iAttackCount = 0;
        }
    }

    __super::Priority_Update(_fTimeDelta); 
}

void CBarfBug::Update(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::NUMPAD1))
    {
        _int iCurCoord = (_int)Get_CurCoord();
        (_int)iCurCoord ^= 1;
        _float3 vNewPos = _float3(3.0f, 6.0f, 0.0f);
        Event_Change_Coordinate(this, (COORDINATE)iCurCoord, &vNewPos);
        //Change_Coordinate((COORDINATE)iCurCoord, _float3(0.0f, 0.0f, 0.0f));
    }

    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CBarfBug::Late_Update(_float _fTimeDelta)
{

    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CBarfBug::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CBarfBug::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    int a = 0;
}

void CBarfBug::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{

    int a = 0;
}

void CBarfBug::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
    int a = 0;
}

void CBarfBug::Change_Animation()
{
    if(m_iState != m_iPreState)
    {
        switch (MONSTER_STATE(m_iState))
        {
        case MONSTER_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::PATROL:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
            break;

        case MONSTER_STATE::ALERT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(BARF);
            break;

        default:
            break;
        }
    }
}

void CBarfBug::Attack(_float _fTimeDelta)
{
    if (false == m_isDelay && false == m_isCool)
    {
        _float3 vScale, vPosition;
        _float4 vRotation;
        if (false == m_pGameInstance->MatrixDecompose(&vScale, &vRotation, &vPosition, m_pControllerTransform->Get_WorldMatrix()))
            return;

        vPosition.y += vScale.y * 0.5f;

        CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_Projectile_BarfBug"),&vPosition, &vRotation);
        Delay_On();
        ++m_iAttackCount;
    }
}

void CBarfBug::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CBarfBug::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case ALERT:
        Set_AnimChangeable(true);
        break;

    case BARF:
        //딜레이 동안은 애니 전환 안됨. 따라서 상태 전환도 불가
        if (false == m_isDelay)
        {
            Set_AnimChangeable(true);
        }
        break;

    default:
        break;
    }
}

HRESULT CBarfBug::Ready_ActorDesc(void* _pArg)
{
    CBarfBug::MONSTER_DESC* pDesc = static_cast<CBarfBug::MONSTER_DESC*>(_pArg);
    
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
    ShapeDesc->fHalfHeight = 0.5f;
    ShapeDesc->fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::CAPSULE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(ShapeData);
    }

    return S_OK;
}

HRESULT CBarfBug::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC Desc;
    Desc.fAlertRange = m_fAlertRange;
    Desc.fChaseRange = m_fChaseRange;
    Desc.fAttackRange = m_fAttackRange;
    Desc.pOwner = this;

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &Desc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CBarfBug::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.strModelPrototypeTag_2D = TEXT("BarferBug");
    BodyDesc.strModelPrototypeTag_3D = TEXT("barfBug_Rig");
	BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBarfBug* CBarfBug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBarfBug* pInstance = new CBarfBug(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBarfBug::Clone(void* _pArg)
{
    CBarfBug* pInstance = new CBarfBug(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBarfBug::Free()
{

    __super::Free();
}
