#include "stdafx.h"
#include "Projectile_SketchSpace_UFO.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CProjectile_SketchSpace_UFO::CProjectile_SketchSpace_UFO(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CProjectile_SketchSpace_UFO::CProjectile_SketchSpace_UFO(const CProjectile_SketchSpace_UFO& _Prototype)
	: CProjectile_Monster(_Prototype)
{
}

HRESULT CProjectile_SketchSpace_UFO::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_SketchSpace_UFO::Initialize(void* _pArg)
{
    PROJECTILE_BARFBUG_DESC* pDesc = static_cast<PROJECTILE_BARFBUG_DESC*>(_pArg);

    //투사체는 쓰는 객체가 Desc 넣어줌.
    pDesc->isCoordChangeEnable = false;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 400.f;

    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

    pDesc->fLifeTime = 5.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
    pModelObject->Set_AnimationLoop(COORDINATE_2D, PROJECTILE_LOOP, true);
    pModelObject->Set_Animation(Animation2D::PROJECTILE_LOOP);

    pModelObject->Register_OnAnimEndCallBack(bind(&CProjectile_SketchSpace_UFO::Animation_End, this, placeholders::_1, placeholders::_2));

	return S_OK;
}

void CProjectile_SketchSpace_UFO::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CProjectile_SketchSpace_UFO::Update(_float _fTimeDelta)
{
	if (COORDINATE_2D == Get_CurCoord())
    {
        if (false == m_isStop)
            m_pControllerTransform->Go_Up(_fTimeDelta);
    }

    __super::Update(_fTimeDelta);
}

void CProjectile_SketchSpace_UFO::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CProjectile_SketchSpace_UFO::Render()
{
#ifdef _DEBUG

#endif // _DEBUG

    __super::Render();
    return S_OK;
}

HRESULT CProjectile_SketchSpace_UFO::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == _eCoordinate)
    {
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(PROJECTILE_LOOP);
    }

    return S_OK;
}

void CProjectile_SketchSpace_UFO::Change_Animation()
{
}

void CProjectile_SketchSpace_UFO::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
    {
        CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
        switch (pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
        {
        case PROJECTILE_HIT:
            Event_DeleteObject(this);
            break;
        default:
            break;
        }
    }
}

void CProjectile_SketchSpace_UFO::On_Hit(CGameObject* _pHitter, _float _fDamg, _fvector _vForce)
{

}

void CProjectile_SketchSpace_UFO::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::MAPOBJECT & _pOtherObject->Get_ObjectGroupID() || OBJECT_GROUP::BLOCKER & _pOtherObject->Get_ObjectGroupID())
    {
        __super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);

        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(PROJECTILE_HIT);
    }
}

void CProjectile_SketchSpace_UFO::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CProjectile_SketchSpace_UFO::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CProjectile_SketchSpace_UFO::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CProjectile_SketchSpace_UFO::Active_OnDisable()
{
    __super::Active_OnDisable();
}

HRESULT CProjectile_SketchSpace_UFO::Ready_Components()
{
    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 10.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProjectile_SketchSpace_UFO::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strModelPrototypeTag_2D = TEXT("sketchspace_UFO_projectile");
    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CProjectile_SketchSpace_UFO* CProjectile_SketchSpace_UFO::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CProjectile_SketchSpace_UFO* pInstance = new CProjectile_SketchSpace_UFO(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProjectile_SketchSpace_UFO");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectile_SketchSpace_UFO::Clone(void* _pArg)
{
    CProjectile_SketchSpace_UFO* pInstance = new CProjectile_SketchSpace_UFO(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectile_SketchSpace_UFO");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProjectile_SketchSpace_UFO::Free()
{

    __super::Free();
}
