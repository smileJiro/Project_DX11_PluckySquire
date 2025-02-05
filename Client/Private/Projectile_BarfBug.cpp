#include "stdafx.h"
#include "Projectile_BarfBug.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"

CProjectile_BarfBug::CProjectile_BarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CProjectile_BarfBug::CProjectile_BarfBug(const CProjectile_BarfBug& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CProjectile_BarfBug::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_BarfBug::Initialize(void* _pArg)
{
    PROJECTILE_BARFBUG_DESC* pDesc = static_cast<PROJECTILE_BARFBUG_DESC*>(_pArg);

    //투사체는 쓰는 객체가 Desc 넣어줌.
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = true;
    pDesc->iNumPartObjects = PART_LAST;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 1000.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;


    pDesc->fLifeTime = 5.f;
    m_fLifeTime = pDesc->fLifeTime;


    if (FAILED(Ready_ActorDesc(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    pModelObject->Set_AnimationLoop(COORDINATE_2D, PROJECTILE, true);


    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

	return S_OK;
}

void CProjectile_BarfBug::Priority_Update(_float _fTimeDelta)
{
    m_fAccTime += _fTimeDelta;

    __super::Priority_Update(_fTimeDelta);
}

void CProjectile_BarfBug::Update(_float _fTimeDelta)
{
	if (false == Is_Dead() && m_fLifeTime <= m_fAccTime)
    {
        m_fAccTime = 0.f;
        Event_DeleteObject(this);
    }

	if (COORDINATE_2D == Get_CurCoord())
    {
        _vector vPos = m_pControllerTransform->Get_State(CTransform::STATE::STATE_POSITION);
        _vector vLook = m_pControllerTransform->Get_State(CTransform::STATE::STATE_UP);
        _vector vFinalPos = vPos + XMVector3Normalize(vLook) * m_pControllerTransform->Get_SpeedPerSec() * _fTimeDelta;

        m_pControllerTransform->Set_State(CTransform::STATE::STATE_POSITION, vFinalPos);
    }

    else if (COORDINATE_3D == Get_CurCoord())
        m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CProjectile_BarfBug::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CProjectile_BarfBug::Render()
{
    __super::Render();
    return S_OK;
}

HRESULT CProjectile_BarfBug::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == Get_CurCoord())
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(PROJECTILE);

    return S_OK;
}

void CProjectile_BarfBug::Change_Animation()
{
}

void CProjectile_BarfBug::Active_OnEnable()
{
}

void CProjectile_BarfBug::Active_OnDisable()
{
    m_pControllerTransform->Set_WorldMatrix(XMMatrixIdentity());
    m_fAccTime = 0.f;
}

HRESULT CProjectile_BarfBug::Ready_ActorDesc(void* _pArg)
{
    CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC* pDesc = static_cast<CProjectile_BarfBug::PROJECTILE_BARFBUG_DESC*>(_pArg);

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
    ShapeDesc->fRadius = 0.5f;

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA* ShapeData = new SHAPE_DATA;
    ShapeData->pShapeDesc = ShapeDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
    ShapeData->eShapeType = SHAPE_TYPE::SPHERE;     // Shape의 형태.
    ShapeData->eMaterial = ACTOR_MATERIAL::DEFAULT; // PxMaterial(정지마찰계수, 동적마찰계수, 반발계수), >> 사전에 정의해둔 Material이 아닌 Custom Material을 사용하고자한다면, Custom 선택 후 CustomMaterial에 값을 채울 것.
    ShapeData->isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
    XMStoreFloat4x4(&ShapeData->LocalOffsetMatrix, XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.f, 0.0f)); // Shape의 LocalOffset을 행렬정보로 저장.

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc->ShapeDatas.push_back(*ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc->tFilterData.MyGroup = OBJECT_GROUP::MONSTER_PROJECTILE;
    ActorDesc->tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::PLAYER_PROJECTILE;

    /* Actor Component Finished */
    pDesc->pActorDesc = ActorDesc;

    /* Shapedata 할당해제 */
    Safe_Delete(ShapeData);

    return S_OK;
}

HRESULT CProjectile_BarfBug::Ready_Components()
{
    return S_OK;
}

HRESULT CProjectile_BarfBug::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_2D = TEXT("barferbug_projectile");
    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::TEST_PROJECTILE;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(0.5f, 0.5f, 0.5f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_NONBLEND;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CProjectile_BarfBug* CProjectile_BarfBug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CProjectile_BarfBug* pInstance = new CProjectile_BarfBug(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProjectile_BarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectile_BarfBug::Clone(void* _pArg)
{
    CProjectile_BarfBug* pInstance = new CProjectile_BarfBug(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectile_BarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProjectile_BarfBug::Free()
{

    __super::Free();
}
