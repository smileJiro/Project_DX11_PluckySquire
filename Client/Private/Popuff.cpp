#include "stdafx.h"
#include "Popuff.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"

CPopuff::CPopuff(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CPopuff::CPopuff(const CPopuff& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CPopuff::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPopuff::Initialize(void* _pArg)
{
    CPopuff::MONSTER_DESC* pDesc = static_cast<CPopuff::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 2.f;
    pDesc->fAlert2DRange = 5.f;
    pDesc->fChase2DRange = 12.f;
    pDesc->fAttack2DRange = 2.f;

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

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, WALK, true);
    pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CPopuff::Animation_End, this, placeholders::_1, placeholders::_2));

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

    return S_OK;
}

void CPopuff::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CPopuff::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CPopuff::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CPopuff::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CPopuff::Change_Animation()
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
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(WALK);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACK);
            break;

        default:
            break;
        }
    }
}

void CPopuff::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CPopuff::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {

    case ATTACK:
        Set_AnimChangeable(true);
        break;

    default:
        break;
    }
}

HRESULT CPopuff::Ready_ActorDesc(void* _pArg)
{
    CPopuff::MONSTER_DESC* pDesc = static_cast<CPopuff::MONSTER_DESC*>(_pArg);

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
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE | OBJECT_GROUP::MONSTER;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CPopuff::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC FSMDesc;
    FSMDesc.fAlertRange = m_fAlertRange;
    FSMDesc.fChaseRange = m_fChaseRange;
    FSMDesc.fAttackRange = m_fAttackRange;
    FSMDesc.fAlert2DRange = m_fAlert2DRange;
    FSMDesc.fChase2DRange = m_fChase2DRange;
    FSMDesc.fAttack2DRange = m_fAttack2DRange;
    FSMDesc.pOwner = this;
    FSMDesc.iCurLevel = m_iCurLevelID;

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &FSMDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPopuff::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strModelPrototypeTag_3D = TEXT("Popuff_Rig");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);

    BodyDesc.tTransform3DDesc.fRotationPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_RotationPerSec();
    BodyDesc.tTransform3DDesc.fSpeedPerSec = Get_ControllerTransform()->Get_Transform(COORDINATE_3D)->Get_SpeedPerSec();

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CPopuff* CPopuff::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPopuff* pInstance = new CPopuff(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPopuff");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPopuff::Clone(void* _pArg)
{
    CPopuff* pInstance = new CPopuff(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPopuff");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPopuff::Free()
{

    __super::Free();
}
