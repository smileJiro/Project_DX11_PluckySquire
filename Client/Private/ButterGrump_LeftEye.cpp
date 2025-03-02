#include "stdafx.h"
#include "ButterGrump_LeftEye.h"
#include "Actor_Dynamic.h"
#include "3DModel.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Monster.h"

CButterGrump_LeftEye::CButterGrump_LeftEye(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject(_pDevice, _pContext)
{
}


CButterGrump_LeftEye::CButterGrump_LeftEye(const CButterGrump_LeftEye& _Prototype)
    :CPartObject(_Prototype)
{

}

HRESULT CButterGrump_LeftEye::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CButterGrump_LeftEye::Initialize(void* _pArg)
{
    //Part Sword
    BUTTERGRUMP_LEFTEYE_DESC* pDesc = static_cast<BUTTERGRUMP_LEFTEYE_DESC*>(_pArg);

    pDesc->isCoordChangeEnable = false;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->iObjectGroupID = OBJECT_GROUP::BOSS;

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

    return S_OK;
}

void CButterGrump_LeftEye::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CButterGrump_LeftEye::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CButterGrump_LeftEye::Render()
{
#ifdef _DEBUG
#endif
    if(Is_Active())
        return __super::Render();
    return S_OK;
}

void CButterGrump_LeftEye::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup
        && (_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse
        && (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
    {
        _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
        Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), m_pParent->Get_Stat().iDamg, vRepulse);
    }

}

void CButterGrump_LeftEye::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CButterGrump_LeftEye::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CButterGrump_LeftEye::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CButterGrump_LeftEye::Active_OnDisable()
{
    __super::Active_OnDisable();
}

HRESULT CButterGrump_LeftEye::Ready_ActorDesc(void* _pArg)
{
    CButterGrump_LeftEye::BUTTERGRUMP_LEFTEYE_DESC* pDesc = static_cast<CButterGrump_LeftEye::BUTTERGRUMP_LEFTEYE_DESC*>(_pArg);

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
    ShapeDesc->fRadius = 3.f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = true;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

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

CButterGrump_LeftEye* CButterGrump_LeftEye::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CButterGrump_LeftEye* pInstance = new CButterGrump_LeftEye(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : ButterGrump_LeftEye");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButterGrump_LeftEye::Clone(void* _pArg)
{
    CButterGrump_LeftEye* pInstance = new CButterGrump_LeftEye(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : ButterGrump_LeftEye");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButterGrump_LeftEye::Free()
{
    m_pParent = nullptr;

    __super::Free();
}
