#include "stdafx.h"
#include "LightningBolt.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CLightningBolt::CLightningBolt(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CLightningBolt::CLightningBolt(const CLightningBolt& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CLightningBolt::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightningBolt::Initialize(void* _pArg)
{
    LIGHTNINGBOLT_DESC* pDesc = static_cast<LIGHTNINGBOLT_DESC*>(_pArg);

    //투사체는 쓰는 객체가 Desc 넣어줌.
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iNumPartObjects = PART_LAST;

    pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pDesc->tTransform2DDesc.fSpeedPerSec = 400.f;

    pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;

    pDesc->fLifeTime = 5.f;
    m_fLifeTime = pDesc->fLifeTime;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);

    pModelObject->Register_OnAnimEndCallBack(bind(&CLightningBolt::Animation_End, this, placeholders::_1, placeholders::_2));

    /* Actor Desc 채울 때 쓴 데이터 할당해제 */

    for (_uint i = 0; i < pDesc->pActorDesc->ShapeDatas.size(); i++)
    {
        Safe_Delete(pDesc->pActorDesc->ShapeDatas[i].pShapeDesc);
    }

    Safe_Delete(pDesc->pActorDesc);

	return S_OK;
}

void CLightningBolt::Priority_Update(_float _fTimeDelta)
{
    m_fAccTime += _fTimeDelta;

    __super::Priority_Update(_fTimeDelta);
}

void CLightningBolt::Update(_float _fTimeDelta)
{
	if (false == Is_Dead() && m_fLifeTime <= m_fAccTime)
    {
        m_fAccTime = 0.f;
        Event_DeleteObject(this);
    }

    if (true == Is_Dead())
    {
        int a = 10;
    }

	//if (COORDINATE_2D == Get_CurCoord())
 //   {
 //       if(false == m_isStop)
 //           m_pControllerTransform->Go_Up(_fTimeDelta);
 //   }

    __super::Update(_fTimeDelta);
}

void CLightningBolt::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CLightningBolt::Render()
{
#ifdef _DEBUG
    if (COORDINATE_2D == Get_CurCoord())
    {
        for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
        {
            m_p2DColliderComs[i]->Render();
        }
    }
#endif // _DEBUG

    __super::Render();
    return S_OK;
}

HRESULT CLightningBolt::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
    if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
        return E_FAIL;

    if (COORDINATE_2D == _eCoordinate)
    {
        static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Set_Animation(LIGHTNING_BOLT);

        CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);
    }
    else
        CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);

    return S_OK;
}

void CLightningBolt::Change_Animation()
{
}

void CLightningBolt::Animation_End(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
    {
        CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
        switch (pModelObject->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
        {
        default:
            break;
        }
    }
}

void CLightningBolt::On_Hit(CGameObject* _pHitter, _float _fDamg)
{

}

void CLightningBolt::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
    if (OBJECT_GROUP::PLAYER & _Other.pActorUserData->iObjectGroup)
    {
        if((_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
        {
            Event_Hit(this, _Other.pActorUserData->pOwner, 1.f);
            _vector vRepulse = 10.f * XMVector3Normalize(XMVectorSetY(_Other.pActorUserData->pOwner->Get_FinalPosition() - Get_FinalPosition(), 0.f));
            XMVectorSetY(vRepulse, -1.f);
            Event_KnockBack(static_cast<CCharacter*>(_My.pActorUserData->pOwner), vRepulse);
            Event_DeleteObject(this);
        }

    }

    //if (OBJECT_GROUP::MAPOBJECT & _Other.pActorUserData->iObjectGroup)
    //    Event_DeleteObject(this);
}

void CLightningBolt::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CLightningBolt::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CLightningBolt::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID())
    {
        Event_Hit(this, _pOtherObject, 1.f);
        m_isStop = true;
    }
}

void CLightningBolt::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CLightningBolt::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CLightningBolt::Active_OnEnable()
{
    __super::Active_OnEnable();
 
    m_isStop = false;
	//if (COORDINATE_3D == Get_CurCoord())
 //       m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, true);
}

void CLightningBolt::Active_OnDisable()
{
    m_pControllerTransform->Set_WorldMatrix(XMMatrixIdentity());
    m_fAccTime = 0.f;
    m_isStop = false;

    CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);

    //if (COORDINATE_3D == Get_CurCoord())
	   // m_pActorCom->Set_ShapeEnable((_int)SHAPE_USE::SHAPE_BODY, false);
    __super::Active_OnDisable();
}

HRESULT CLightningBolt::Ready_Components()
{
    /* 2D Collider */
    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
    CircleDesc.pOwner = this;
    CircleDesc.fRadius = 30.f;
    CircleDesc.vScale = { 1.0f, 1.0f };
    CircleDesc.vOffsetPosition = { 0.f, 0.f };
    CircleDesc.isBlock = false;
    CircleDesc.iCollisionGroupID = OBJECT_GROUP::MONSTER_PROJECTILE;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLightningBolt::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strModelPrototypeTag_2D = TEXT("barferbug_projectile");
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

CLightningBolt* CLightningBolt::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLightningBolt* pInstance = new CLightningBolt(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CLightningBolt");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLightningBolt::Clone(void* _pArg)
{
    CLightningBolt* pInstance = new CLightningBolt(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CLightningBolt");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLightningBolt::Free()
{

    __super::Free();
}
