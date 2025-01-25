#include "stdafx.h"
#include "Rat.h"
#include "GameInstance.h"
#include "FSM.h"
#include "ModelObject.h"

CRat::CRat(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CRat::CRat(const CRat& _Prototype)
    : CMonster(_Prototype)
{
}

HRESULT CRat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRat::Initialize(void* _pArg)
{
    CRat::MONSTER_DESC* pDesc = static_cast<CRat::MONSTER_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_END;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 6.f;

    pDesc->fAlertRange = 5.f;
    pDesc->fChaseRange = 12.f;
    pDesc->fAttackRange = 2.f;

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

    //pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, IDLE, true);
    //pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_3D, RUN, true);
    //pModelObject->Set_Animation(IDLE);

    pModelObject->Register_OnAnimEndCallBack(bind(&CRat::Animation_End, this, placeholders::_1, placeholders::_2));

    return S_OK;
}

void CRat::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CRat::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta); /* Part Object Update */
}

void CRat::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CRat::Render()
{
    /* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */

#ifdef _DEBUG

#endif // _DEBUG

    /* Font Render */

    return S_OK;
}

void CRat::Change_Animation()
{
    /*if(m_iState != m_iPreState)
    {
        switch (MONSTER_STATE(m_iState))
        {
        case MONSTER_STATE::IDLE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(IDLE);
            break;

        case MONSTER_STATE::ALERT:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ALERT);
            break;

        case MONSTER_STATE::CHASE:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(RUN);
            break;

        case MONSTER_STATE::ATTACK:
            static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(ATTACKSTRIKE);
            break;

        default:
            break;
        }
    }*/
}

void CRat::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    /*CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    switch ((CRat::Animation)pModelObject->Get_Model(COORDINATE_3D)->Get_CurrentAnimIndex())
    {
    case ALERT:
        Set_AnimChangeable(true);
        break;
        
    case ATTACKSTRIKE:
        pModelObject->Switch_Animation(ATTACKRECOVERY);
        break;

    case ATTACKRECOVERY:
        pModelObject->Switch_Animation(ATTACKSTRIKE);
        break;

    default:
        break;
    }*/
}

HRESULT CRat::Ready_ActorDesc(void* _pArg)
{
    CRat::MONSTER_DESC* pDesc = static_cast<CRat::MONSTER_DESC*>(_pArg);

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
    ShapeData->isShapeMaterial = true;               // Material을 별도로 지정할 것인지, Default Material을 사용할 것인지.
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

HRESULT CRat::Ready_Components()
{
    /* Com_FSM */
    CFSM::FSMDESC Desc;
    Desc.fAlertRange = m_fAlertRange;
    Desc.fChaseRange = m_fChaseRange;
    Desc.fAttackRange = m_fAttackRange;

    if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM), &Desc)))
        return E_FAIL;
    m_pFSM->Set_Owner(this);

    return S_OK;
}

HRESULT CRat::Ready_PartObjects()
{
    /* Part Body */
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("Rat_Rig");
	BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CRat* CRat::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRat* pInstance = new CRat(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRat");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRat::Clone(void* _pArg)
{
    CRat* pInstance = new CRat(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CRat");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRat::Free()
{

    __super::Free();
}
