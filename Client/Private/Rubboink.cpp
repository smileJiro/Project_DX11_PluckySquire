#include "stdafx.h"
#include "Rubboink.h"
#include "GameInstance.h"
#include "FSM.h"
#include "DetectionField.h"
#include "ModelObject.h"
#include "Effect_Manager.h"
#include "Effect2D_Manager.h"
#include "Section_Manager.h"
#include "Pooling_Manager.h"

CRubboink::CRubboink(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCharacter(_pDevice, _pContext)
{
}

CRubboink::CRubboink(const CRubboink& _Prototype)
    : CCharacter(_Prototype)
{
}

HRESULT CRubboink::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRubboink::Initialize(void* _pArg)
{
    CRubboink::CHARACTER_DESC* pDesc = static_cast<CRubboink::CHARACTER_DESC*>(_pArg);
    //pDesc->eStartCoord = COORDINATE_3D;
    //pDesc->isCoordChangeEnable = true;

    if (false == pDesc->isCoordChangeEnable)
    {
        if (COORDINATE_3D == pDesc->eStartCoord)
        {
            pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720.f);
            pDesc->tTransform3DDesc.fSpeedPerSec = 4.f;
        }
        else
        {
            pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
            pDesc->tTransform2DDesc.fSpeedPerSec = 80.f;
        }
    }
    else
    {
        pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(720.f);
        pDesc->tTransform3DDesc.fSpeedPerSec = 4.f;

        pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(360.f);
        pDesc->tTransform2DDesc.fSpeedPerSec = 80.f;
    }

    pDesc->_tStat.iHP = 5;
    pDesc->_tStat.iMaxHP = 5;
    pDesc->_tStat.iDamg = 1;


    /* Create Test Actor (Desc를 채우는 함수니까. __super::Initialize() 전에 위치해야함. )*/
    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;


    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);


    pModelObject->Register_OnAnimEndCallBack(bind(&CRubboink::Animation_End, this, placeholders::_1, placeholders::_2));

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CRubboink::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CRubboink::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CRubboink::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CRubboink::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG
    __super::Render();
#endif

    /* Font Render */

    return S_OK;
}

void CRubboink::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    if (COORDINATE_3D == Get_CurCoord())
    {
    //    switch ((CRubboink::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    //    {
    //    case ALERT:
    //        break;

    //    case HIT:
    //        pModelObject->Switch_Animation(WAKE);
    //        break;

    //    case WAKE:
    //        break;

    //    case DEATH:
    //        break;

    //    default:
    //        break;
    //    }
    //}

    //else if (COORDINATE_2D == Get_CurCoord())
    //{
    //    switch ((CRubboink::Animation2D)pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
    //    {
    //    case ALERT_DOWN:
    //    case ALERT_RIGHT:
    //    case ALERT_UP:
    //        break;

    //    case HIT_DOWN:
    //    case HIT_RIGHT:
    //    case HIT_UP:
    //        break;

    //    case DEATH_DOWN:
    //    case DEATH_RIGHT:
    //    case DEATH_UP:
    //        //Set_AnimChangeable(true);

    //        //CEffect2D_Manager::GetInstance()->Play_Effect(TEXT("Death_Burst"), CSection_Manager::GetInstance()->Get_Cur_Section_Key(), Get_ControllerTransform()->Get_WorldMatrix());

    //        ////확률로 전구 생성
    //        //if (2 == (_int)ceil(m_pGameInstance->Compute_Random(0.f, 3.f)))
    //        //{
    //        //    _float3 vPos; XMStoreFloat3(&vPos, Get_FinalPosition());
    //        //    _wstring strCurSection = CSection_Manager::GetInstance()->Get_Cur_Section_Key();
    //        //    CPooling_Manager::GetInstance()->Create_Object(TEXT("Pooling_2DBulb"), COORDINATE_2D, &vPos, nullptr, nullptr, &strCurSection);
    //        //}

    //        //Event_DeleteObject(this);
    //        break;

    //    default:
    //        break;
    //    }
    }
}

HRESULT CRubboink::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{

    return S_OK;
}

void CRubboink::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{


}

void CRubboink::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CRubboink::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CRubboink::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CRubboink::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Stay(_My, _Other);
}

void CRubboink::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    __super::OnTrigger_Exit(_My, _Other);
}

void CRubboink::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
}

void CRubboink::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CRubboink::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CRubboink::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    __super::On_Hit(_pHitter, _iDamg, _vForce);

    m_pGameInstance->Start_SFX(_wstring(L"A_sfx_goblin_hit_") + to_wstring(rand() % 5), 50.f);
}

HRESULT CRubboink::Ready_ActorDesc(void* _pArg)
{
    CRubboink::CHARACTER_DESC* pDesc = static_cast<CRubboink::CHARACTER_DESC*>(_pArg);

    //if (COORDINATE_2D == pDesc->eStartCoord)
    //    return S_OK;

    pDesc->eActorType = ACTOR_TYPE::DYNAMIC;
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
    ShapeDesc->fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::CHARACTER_FOOT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, ShapeDesc->fRadius, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    m_matQueryShapeOffset = ShapeData->LocalOffsetMatrix;


    //쿼리를 켜기 위한 트리거
    SHAPE_SPHERE_DESC* TriggerDesc = new SHAPE_SPHERE_DESC;
    TriggerDesc->fRadius = 0.6f; //pDesc->fAlertRange;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    ShapeData->pShapeDesc = TriggerDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    ShapeData->iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, TriggerDesc->fRadius * 0.5f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.
    ShapeData->FilterData.MyGroup = OBJECT_GROUP::RAY_TRIGGER;
    ShapeData->FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::INTERACTION_OBEJCT;

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);


    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER | OBJECT_GROUP::EXPLOSION | OBJECT_GROUP::DYNAMIC_OBJECT;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CRubboink::Ready_Components()
{

    if (false == Get_ControllerTransform()->Is_CoordChangeEnable() && COORDINATE_3D == Get_CurCoord())
        return S_OK;

    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = { 40.f };
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CRubboink::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    if (false == BodyDesc.isCoordChangeEnable)
    {
        if (COORDINATE_3D == Get_CurCoord())
        {
            BodyDesc.strModelPrototypeTag_3D = TEXT("goblin_Rig");
            BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

            BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

            BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
            BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

            BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
            BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();
        }

        else
        {
            BodyDesc.strModelPrototypeTag_2D = TEXT("Rubboink");
            BodyDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

            BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

            BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
            BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

            BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
            BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();
        }
    }
    else
    {
        BodyDesc.strModelPrototypeTag_3D = TEXT("goblin_Rig");
        BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;

        BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

        BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

        BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
        BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

        BodyDesc.strModelPrototypeTag_2D = TEXT("Rubboink");
        BodyDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

        BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

        BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
        BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

        BodyDesc.tTransform2DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_RotationPerSec();
        BodyDesc.tTransform2DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_SpeedPerSec();
    }

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CRubboink* CRubboink::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRubboink* pInstance = new CRubboink(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRubboink");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRubboink::Clone(void* _pArg)
{
    CRubboink* pInstance = new CRubboink(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CRubboink");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRubboink::Free()
{

    __super::Free();
}
